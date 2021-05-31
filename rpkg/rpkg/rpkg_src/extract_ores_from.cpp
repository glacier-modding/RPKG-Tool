#include "rpkg_function.h"
#include "file.h"
#include "util.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <regex>

void rpkg_function::extract_ores_from(std::string& input_path, std::string& filter, std::string& output_path)
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
        LOG("Loading Hash List...");

        generic_function::load_hash_list();

        LOG("Loading Hash List: Done");

        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        file::create_directories(file::output_path_append("ORES", output_path));

        std::vector<std::string> filters = util::parse_input_filter(filter);

        bool extract_single_hash = false;

        if (filters.size() == 1)
        {
            std::string legal_chars = "0123456789ABCDEF";

            bool is_a_hash = false;

            if (filters.at(0).length() == 16)
            {
                is_a_hash = true;

                for (int i = 0; i < 16; i++)
                {
                    bool is_legal = false;

                    for (int j = 0; j < 16; j++)
                    {
                        if (filters.at(0)[i] == legal_chars[j])
                        {
                            is_legal = true;
                        }
                    }

                    if (!is_legal)
                    {
                        is_a_hash = false;
                    }
                }
            }

            if (is_a_hash)
            {
                extract_single_hash = true;
            }
        }

        timing_string = "Extracting ORES to IOI Paths...";

        LOG("Extracting ORES to IOI Paths...");

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (uint64_t h = 0; h < hash_list_hash_strings.size(); h++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Extracting ORES to IOI Paths" << std::string(period_count, '.');

                timing_string = ss.str() + std::string((80 - ss.str().length()), ' ');

                LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                period_count++;
            }

            std::string hash_list_string = hash_list_hash_strings.at(h);
            std::string ores_hash_file_name = "";
            std::string ores_hash_value_string = "";
            uint64_t ores_hash_value = 0;
            std::string ores_ioi_path = "";
            std::string ores_ioi_directory = "";
            std::string ores_base_name = "";

            size_t pos1 = hash_list_string.find("[ores:");
            size_t pos2 = hash_list_string.find("].pc_ores");

            if (pos1 != std::string::npos && pos2 != std::string::npos && (pos2 - pos1) > 11)
            {
                ores_hash_value_string = util::to_upper_case(hash_list_hash_value_strings.at(h));

                ores_hash_file_name = util::to_upper_case(hash_list_hash_file_names.at(h));

                ores_hash_value = std::strtoull(ores_hash_value_string.c_str(), nullptr, 16);

                bool ores_file_already_found = false;

                if (!extract_single_hash || (extract_single_hash && filter == ores_hash_value_string))
                {
                    for (uint64_t i = 0; i < rpkgs.size(); i++)
                    {
                        ores_ioi_path = file::output_path_append("ORES\\" + rpkgs.at(i).rpkg_file_name, output_path);

                        ores_ioi_path.append(hash_list_string.substr((pos1 + 6), hash_list_string.length() - 15));

                        std::replace(ores_ioi_path.begin(), ores_ioi_path.end(), '/', '\\');

                        size_t pos3 = ores_ioi_path.find_last_of("\\");

                        ores_base_name = ores_ioi_path.substr(pos3 + 1);

                        ores_ioi_directory = ores_ioi_path.substr(0, pos3);

                        if (!ores_file_already_found)
                        {
                            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(ores_hash_value);

                            if (it != rpkgs.at(i).hash_map.end())
                            {
                                bool found = false;

                                uint64_t input_filter_index = 0;

                                for (uint64_t z = 0; z < filters.size(); z++)
                                {
                                    std::size_t found_position_hash = ores_hash_file_name.find(filters.at(z));

                                    std::size_t found_position_ores = util::to_upper_case(ores_ioi_path).find(filters.at(z));

                                    if ((found_position_hash != std::string::npos && filters.at(z) != "") || (found_position_ores != std::string::npos && filters.at(z) != ""))
                                    {
                                        found = true;

                                        input_filter_index = z;

                                        break;
                                    }
                                }

                                if (found || filter == "")
                                {
                                    file::create_directories(ores_ioi_directory);

                                    //ores_file_already_found = true;

                                    rpkg_extraction_vars rpkg_vars;

                                    rpkg_vars.input_path = rpkgs.at(i).rpkg_file_path;
                                    rpkg_vars.filter = ores_hash_value_string;
                                    rpkg_vars.ores_path = ores_ioi_path;
                                    rpkg_vars.ores_mode = true;

                                    extract_from_rpkg_with_map(rpkg_vars);
                                }
                            }
                        }
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting ORES linked files: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}