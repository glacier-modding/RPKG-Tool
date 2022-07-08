#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include "temp.h"
#include <map>
#include <chrono>
#include <sstream>
#include <regex>
#include <filesystem>
#include <string_view>

void rpkg_function::dev_extract_temp_from(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    initialize_enum_map_h2();
    initialize_enum_map_h3();

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

        timing_string = ss.str();

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        //LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        //LOG("Loading Hash List: Done");

        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path == "C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg")
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    std::map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    recursive_temp_loader(rpkg_index, it->second, 3, parents_map, temps_indexes, 0, 0, 0);

                    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
                    int stringstream_length = 80;

                    timing_string = "Found " + util::uint32_t_to_string(temps.size()) + " TEMP/TBLU recursively linked file(s).\n\nLoading recursive TEMP/TBLU file data: 0% done";

                    percent_progress = 0;

                    std::string message = "Found " + util::uint32_t_to_string(temps.size()) + " TEMP/TBLU recursively linked file(s).\n\nLoading recursive TEMP/TBLU file data: ";

                    for (uint64_t t = 0; t < temps.size(); t++)
                    {
                        //if (((t * (uint64_t)100000) / (uint64_t)temps.size()) % (uint64_t)100 == 0 && t > 0)
                        if (t > 0)
                        {
                            stringstream_length = console::update_console(message, temps.size(), t, start_time, stringstream_length);
                        }

                        if (temps.at(t).tblu_return_value == TEMP_TBLU_FOUND)
                        {
                            temps.at(t).load_data();
                        }
                    }

                    timing_string = "Found " + util::uint32_t_to_string(temps.size()) + " TEMP/TBLU recursively linked file(s).\n\nLoading recursive TEMP/TBLU file data: 100% done";

                    percent_progress = 100;

                    if (temps.at(0).tblu_return_value == TEMP_TBLU_FOUND)
                    {
                        task_multiple_status = TASK_SUCCESSFUL;
                    }
                    else
                    {
                        task_multiple_status = temps.at(0).tblu_return_value;
                    }

                    task_single_status = TASK_SUCCESSFUL;
                }
            }
        }
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}