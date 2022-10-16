#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "text.h"
#include "thirdparty/directxtex/DirectXTex.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <regex>
#include <filesystem>

void rpkg_function::extract_text_from(std::string& input_path, const std::string& filter, const std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    //task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path))
    {
        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path, true);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        //LOG("Loading Hash List...");

        //generic_function::load_hash_list(false);

        //LOG("Loading Hash List: Done");

        //std::vector<std::string>().swap(prim_asset_file_names);

        const std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t text_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            for (uint64_t i = 0; i < rpkgs.size(); i++)
            {
                uint64_t rpkg_index = i;

                if (rpkgs.at(i).rpkg_file_path == input_path)
                {
                    std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(text_hash_value);

                    if (it != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        text temp_text(rpkg_index, it->second);

                        std::string text_output_dir = file::output_path_append("TEXT\\" + rpkgs.at(rpkg_index).rpkg_file_name, output_path);

                        file::create_directories(text_output_dir);

                        std::string tga_path = text_output_dir + "\\" + util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + ".tga";

                        std::string meta_path = text_output_dir + "\\" + "metas";

                        file::create_directories(meta_path);

                        if (temp_text.texd_found)
                        {
                            temp_text.save_texd_to_tga(tga_path);
                        }
                        else
                        {
                            temp_text.save_text_to_tga(tga_path);
                        }

                        temp_text.generate_tga_meta_files(meta_path);
                        
                        //temp_text.load_from_tga(tga_path);
                    }
                }
            }
        }

        task_single_status = TASK_SUCCESSFUL;
        //task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}