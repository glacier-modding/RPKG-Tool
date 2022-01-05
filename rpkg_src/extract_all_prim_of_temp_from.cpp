#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "gltf.h"
#include "obj.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <set>

void rpkg_function::extract_all_prim_of_temp_from(std::string& input_path, std::string& filter, std::string& output_path, int type)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

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

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        timing_string = "Extracting linked PRIMs To GLB File(s)...";

        LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        //std::vector<std::string>().swap(prim_asset_file_names);

        std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t temp_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            for (uint64_t i = 0; i < rpkgs.size(); i++)
            {
                uint64_t rpkg_index = i;

                if (rpkgs.at(i).rpkg_file_path == input_path)
                {
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                    if (it != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        std::string temp_hash_file_name = rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name;

                        if (rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type == "TEMP")
                        {
                            temp temp_temp(rpkg_index, it->second);

                            for (uint64_t j = 0; j < temp_temp.prim_depends_file_name.size(); j++)
                            {
                                rpkg_function::extract_prim_from(rpkgs.at(temp_temp.prim_depends_rpkg_index.at(j).at(temp_temp.prim_depends_rpkg_index_index.at(j))).rpkg_file_path, temp_temp.prim_depends_file_name.at(j), output_path, type, true);
                            }
                        }
                    }
                }
            }
        }

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}