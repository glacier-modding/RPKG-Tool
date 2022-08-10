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
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <set>

void rpkg_function::extract_entity_to_qn(std::string& input_path, std::string& filter, std::string& output_path)
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
            rpkg_function::import_rpkg(input_path, false);
        }

        std::filesystem::path base_folder_path = input_path;

        std::string parent_path = base_folder_path.parent_path().string();

        rpkg_function::import_rpkg_files_in_folder(parent_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = "Extracting entity (TEMP/TBLU) to QN (QuickEntity) JSON...";

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(true);
            LOG("Loading Hash List: Done");
        }

        std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t temp_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_hash_value);

            if (rpkg_index != UINT32_MAX)
            {
                std::unordered_map<uint64_t, uint64_t>::iterator it6 = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it6 != rpkgs.at(rpkg_index).hash_map.end())
                {
                    temp temp_temp(rpkg_index, it6->second, 3);

                    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

                    temp_temp.load_data();

                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    std::string message = "Converting entity (TEMP/TBLU) via ResourceTool to RT JSON...";

                    std::stringstream ss;

                    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

                    LOG(ss.str());

                    std::string temp_output_path = "";

                    bool output_path_is_dir = false;

                    if (output_path.length() > 5)
                    {
                        if (util::to_lower_case(output_path).substr(output_path.length() - 5) == ".json")
                        {
                            temp_output_path = output_path;
                        }
                        else
                        {
                            output_path_is_dir = true;
                        }
                    }
                    else
                    {
                        output_path_is_dir = true;
                    }

                    if (output_path_is_dir)
                    {
                        if (output_path != "")
                        {
                            file::create_directories(output_path);
                        }

                        temp_output_path = file::output_path_append(filters.at(f) + ".entity.json", output_path);
                    }

                    start_time = std::chrono::high_resolution_clock::now();

                    temp_temp.rt_json_to_qn_json();

                    end_time = std::chrono::high_resolution_clock::now();

                    message = "Converting entity (TEMP/TBLU) from RT JSON to QN (QuickEntity) JSON...";

                    ss.str(std::string());

                    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

                    LOG(ss.str());

                    temp_temp.write_qn_json_to_file(temp_output_path);
                }
            }
        }

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}