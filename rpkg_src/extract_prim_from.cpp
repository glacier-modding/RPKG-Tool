#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "gltf.h"
#include "obj.h"
#include "prim.h"
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

void rpkg_function::extract_prim_from(std::string& input_path, std::string& filter, std::string& output_path, int type, bool rotate)
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
                            if (type == GLB_SINGLE)
                            {
                                std::string asset_file_name = "";

                                if (output_path == "")
                                {
                                    asset_file_name = std::filesystem::current_path().generic_string() + "/" + rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + ".glb";
                                }
                                else
                                {
                                    asset_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + ".glb", output_path);
                                }

                                file::create_directories(output_path);

                                prim_asset_file_names.push_back(asset_file_name);

                                std::string meta_path = output_path + "\\" + "metas";

                                file::create_directories(meta_path);

                                temp_prim.extract_meta(meta_path);

                                std::string hash_meta_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name, meta_path);

                                rpkg_function::extract_hash_meta(rpkg_index, it->second, hash_meta_file_name);

                                gltf::output_to_single_file(temp_prim.asset3ds_data, asset_file_name, type, rotate);
                            }
                            else if (type == GLTF_SINGLE)
                            {
                                std::string asset_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + ".gltf", output_path);

                                prim_asset_file_names.push_back(asset_file_name);

                                temp_prim.extract_meta(output_path);

                                gltf::output_to_single_file(temp_prim.asset3ds_data, asset_file_name, type, rotate);
                            }
                            else if (type == OBJ_SINGLE)
                            {
                                std::string asset_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + ".obj", output_path);

                                prim_asset_file_names.push_back(asset_file_name);

                                //temp_prim.extract_meta(output_path);

                                obj::output_to_single_file(temp_prim.asset3ds_data, asset_file_name);
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