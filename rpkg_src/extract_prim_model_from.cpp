#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "gltf.h"
#include "obj.h"
#include "prim.h"
#include "mati.h"
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

void rpkg_function::extract_prim_model_from(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    //task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    //else
    //{
        //input_path = file::parse_input_folder_path(input_path);
    //}

    if (file::path_exists(input_path) && input_path_is_rpkg_file)
    {
        /*if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path);
        }*/

        std::filesystem::path base_folder_path = input_path;

        std::string parent_path = base_folder_path.parent_path().string();

        rpkg_function::import_rpkg_files_in_folder(parent_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        //timing_string = "Calculating Entity World Coordinate Data And Extracting Associated PRIMs to glTFs...";

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

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

                        prim temp_prim(rpkg_index, it->second);

                        if (temp_prim.asset3ds_data.vertexes.size() > 0 && temp_prim.success)
                        {
                            std::string asset_file_name = "";

                            if (output_path == "")
                            {
                                output_path = std::filesystem::current_path().generic_string();

                                std::string to_replace = "/";
                                std::string replace_with = "\\";

                                util::replace_all_string_in_string(output_path, to_replace, replace_with);
                            }

                            std::string prim_output_path = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + "\\PRIM\\" + rpkgs.at(rpkg_index).rpkg_file_name, output_path);

                            std::string text_output_path = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name, output_path);

                            asset_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + ".glb", prim_output_path);

                            file::create_directories(prim_output_path);

                            file::create_directories(text_output_path);

                            prim_asset_file_names.push_back(asset_file_name);

                            std::string meta_path = prim_output_path + "\\" + "metas";

                            file::create_directories(meta_path);

                            temp_prim.extract_meta(meta_path);

                            std::string hash_meta_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name, meta_path);

                            rpkg_function::extract_hash_meta(rpkg_index, it->second, hash_meta_file_name);

                            gltf::output_to_single_file(temp_prim.asset3ds_data, asset_file_name, GLB_SINGLE, true);

                            for (uint64_t j = 0; j < temp_prim.mati_depends_file_name.size(); j++)
                            {
                                mati temp_mati(temp_prim.mati_depends_rpkg_index.at(j).at(temp_prim.mati_depends_rpkg_index_index.at(j)), temp_prim.mati_depends_hash_index.at(j).at(temp_prim.mati_depends_hash_index_index.at(j)));

                                for (uint64_t k = 0; k < temp_mati.text_depends_file_name.size(); k++)
                                {
                                    rpkg_function::extract_text_from(temp_mati.text_depends_in_rpkgs.at(k).at(temp_mati.text_depends_in_rpkgs_index.at(k)), temp_mati.text_depends_file_name.at(k), text_output_path);
                                }
                            }
                        }
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