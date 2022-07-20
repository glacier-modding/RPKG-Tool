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

void rpkg_function::search_localization(std::string& input_path, std::string& filter, std::string& search, std::string& search_type, std::string& output_path, bool search_dlge, bool search_locr, bool search_rtlv, int max_results)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    percent_progress = (uint32_t)0;

    log_output = false;

    localization_search_results = "";

    uint32_t results_count = 0;

    /*bool input_path_is_rpkg_file = false;

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

        //rpkg_function::import_rpkg_files_in_folder(parent_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        //timing_string = "Searching entities (TEMP/TBLU)...";

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            //generic_function::load_hash_list(true);
            LOG("Loading Hash List: Done");
        }*/

    std::string search_lower_case = util::to_lower_case(search);

    uint32_t search_crc32_dec = std::strtoul(search.c_str(), nullptr, 10);

    uint32_t search_crc32_hex = std::strtoul(search.c_str(), nullptr, 16);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    uint64_t localization_hash_count = 0;
    uint64_t localization_hash_size_total = 0;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::string message = "Searching " + rpkgs.at(i).rpkg_file_name + "...";

        if (input_path == "" || input_path == rpkgs.at(i).rpkg_file_path)
        {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                if (search_dlge)
                {
                    if (rpkgs.at(i).hash_resource_types.at(r) == "DLGE")
                    {
                        for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                        {
                            uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                            localization_hash_size_total += rpkgs.at(i).hash.at(hash_index).hash_size_final;
                            localization_hash_count++;
                        }
                    }
                }

                if (search_locr)
                {
                    if (rpkgs.at(i).hash_resource_types.at(r) == "LOCR")
                    {
                        for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                        {
                            uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                            localization_hash_size_total += rpkgs.at(i).hash.at(hash_index).hash_size_final;
                            localization_hash_count++;
                        }
                    }
                }

                if (search_rtlv)
                {
                    if (rpkgs.at(i).hash_resource_types.at(r) == "RTLV")
                    {
                        for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                        {
                            uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                            localization_hash_size_total += rpkgs.at(i).hash.at(hash_index).hash_size_final;
                            localization_hash_count++;
                        }
                    }
                }
            }
        }
    }

    uint64_t localization_hash_count_current = 0;
    uint64_t localization_hash_size_current = 0;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::string message = "Searching " + rpkgs.at(i).rpkg_file_name + "...";

        if (input_path == "" || input_path == rpkgs.at(i).rpkg_file_path)
        {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                if (results_count < max_results)
                {
                    if (search_dlge)
                    {
                        if (rpkgs.at(i).hash_resource_types.at(r) == "DLGE")
                        {
                            for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                            {
                                if (gui_control == ABORT_CURRENT_TASK)
                                {
                                    return;
                                }

                                if (results_count < max_results)
                                {
                                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                                    std::string temp_hash_string = rpkgs.at(i).hash.at(hash_index).hash_string;

                                    if (((localization_hash_count_current * (uint64_t)100000) / (uint64_t)localization_hash_count) % (uint64_t)10 == 0 && localization_hash_count_current > 0)
                                    {
                                        stringstream_length = console::update_console(message, localization_hash_size_total, localization_hash_size_current, start_time, stringstream_length);
                                    }

                                    localization_hash_size_current += rpkgs.at(i).hash.at(hash_index).hash_size_final;
                                    localization_hash_count_current++;

                                    rpkg_function::extract_dlge_to_json_from(rpkgs.at(i).rpkg_file_path, temp_hash_string, output_path, true);

                                    try
                                    {
                                        for (const auto& it : localization_json.items())
                                        {
                                            for (const auto& it2 : it.value().items())
                                            {
                                                //std::cout << " RLTV3: " << it.key() << ", " << it.value() << std::endl;
                                                //std::cout << "SEARCHING lol2: " << it.key() << std::endl;
                                                if (it2.value().is_string())
                                                {
                                                    //std::cout << "SEARCHING s: " << it.key() << std::endl;
                                                    if (util::to_lower_case(std::string(it2.value())).find(search_lower_case) != std::string::npos)
                                                    {
                                                        std::cout << "FOUND STRING: " << it2.key() << ", " << it2.value() << " in " << rpkgs.at(i).hash.at(hash_index).hash_file_name << std::endl;

                                                        localization_search_results += rpkgs.at(i).rpkg_file_path + "||||" + rpkgs.at(i).hash.at(hash_index).hash_file_name + " " + util::hash_to_ioi_string(rpkgs.at(i).hash.at(hash_index).hash_value, false) + "||||" + std::string(it2.value()) + "||||||";

                                                        results_count++;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    catch (json::parse_error& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error: " << "\n"
                                            << "Error message: " << e.what() << '\n'
                                            << "Error exception id: " << e.id << '\n'
                                            << "Error byte position of error: " << e.byte;
                                    }
                                }
                            }
                        }
                    }

                    if (search_locr)
                    {
                        if (rpkgs.at(i).hash_resource_types.at(r) == "LOCR")
                        {
                            for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                            {
                                if (results_count < max_results)
                                {
                                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                                    std::string temp_hash_string = rpkgs.at(i).hash.at(hash_index).hash_string;

                                    if (((localization_hash_count_current * (uint64_t)100000) / (uint64_t)localization_hash_count) % (uint64_t)10 == 0 && localization_hash_count_current > 0)
                                    {
                                        stringstream_length = console::update_console(message, localization_hash_size_total, localization_hash_size_current, start_time, stringstream_length);
                                    }

                                    localization_hash_size_current += rpkgs.at(i).hash.at(hash_index).hash_size_final;
                                    localization_hash_count_current++;

                                    rpkg_function::extract_locr_to_json_from(rpkgs.at(i).rpkg_file_path, temp_hash_string, output_path, true);

                                    try
                                    {
                                        for (const auto& it : localization_json.items())
                                        {
                                            bool language_found = false;

                                            std::string language = "";

                                            for (const auto& it2 : it.value().items())
                                            {
                                                if (it2.value().contains("Language"))
                                                {
                                                    language_found = true;

                                                    language = it2.value()["Language"];
                                                }
                                            }

                                            if (language_found)
                                            {
                                                for (const auto& it2 : it.value().items())
                                                {
                                                    if (it2.value().contains("StringHash") && it2.value().contains("String"))
                                                    {
                                                        if ((uint32_t)it2.value()["StringHash"] == search_crc32_dec || (uint32_t)it2.value()["StringHash"] == search_crc32_hex)
                                                        {
                                                            localization_search_results += rpkgs.at(i).rpkg_file_path + "||||" + rpkgs.at(i).hash.at(hash_index).hash_file_name + " " + util::hash_to_ioi_string(rpkgs.at(i).hash.at(hash_index).hash_value, false) + "||||" + language + ": " + util::uint32_t_to_string(it2.value()["StringHash"]) + ": " + std::string(it2.value()["String"]) + "||||||";

                                                            results_count++;
                                                        }
                                                        else if (util::to_lower_case(std::string(it2.value()["String"])).find(search_lower_case) != std::string::npos)
                                                        {
                                                            localization_search_results += rpkgs.at(i).rpkg_file_path + "||||" + rpkgs.at(i).hash.at(hash_index).hash_file_name + " " + util::hash_to_ioi_string(rpkgs.at(i).hash.at(hash_index).hash_value, false) + "||||" + language + ": " + util::uint32_t_to_string(it2.value()["StringHash"]) + ": " + std::string(it2.value()["String"]) + "||||||";

                                                            results_count++;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    catch (json::parse_error& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error: " << "\n"
                                            << "Error message: " << e.what() << '\n'
                                            << "Error exception id: " << e.id << '\n'
                                            << "Error byte position of error: " << e.byte;
                                    }
                                }
                            }
                        }
                    }

                    if (search_rtlv)
                    {
                        if (rpkgs.at(i).hash_resource_types.at(r) == "RTLV")
                        {
                            for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                            {
                                if (results_count < max_results)
                                {
                                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                                    std::string temp_hash_string = rpkgs.at(i).hash.at(hash_index).hash_string;

                                    if (((localization_hash_count_current * (uint64_t)100000) / (uint64_t)localization_hash_count) % (uint64_t)10 == 0 && localization_hash_count_current > 0)
                                    {
                                        stringstream_length = console::update_console(message, localization_hash_size_total, localization_hash_size_current, start_time, stringstream_length);
                                    }

                                    localization_hash_size_current += rpkgs.at(i).hash.at(hash_index).hash_size_final;
                                    localization_hash_count_current++;

                                    rpkg_function::extract_rtlv_to_json_from(rpkgs.at(i).rpkg_file_path, temp_hash_string, output_path, true);

                                    try
                                    {
                                        for (const auto& it : localization_json.items())
                                        {
                                            for (const auto& it2 : it.value().items())
                                            {
                                                //std::cout << " RLTV3: " << it.key() << ", " << it.value() << std::endl;
                                                //std::cout << "SEARCHING lol2: " << it.key() << std::endl;
                                                if (it2.value().is_string())
                                                {
                                                    //std::cout << "SEARCHING s: " << it.key() << std::endl;
                                                    if (util::to_lower_case(std::string(it2.value())).find(search_lower_case) != std::string::npos)
                                                    {
                                                        std::cout << "FOUND STRING: " << it2.key() << ", " << it2.value() << " in " << rpkgs.at(i).hash.at(hash_index).hash_file_name << std::endl;

                                                        localization_search_results += rpkgs.at(i).rpkg_file_path + "||||" + rpkgs.at(i).hash.at(hash_index).hash_file_name + " " + util::hash_to_ioi_string(rpkgs.at(i).hash.at(hash_index).hash_value, false) + "||||" + std::string(it2.value()) + "||||||";

                                                        results_count++;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    catch (json::parse_error& e)
                                    {
                                        std::stringstream ss;
                                        ss << "Error: " << "\n"
                                            << "Error message: " << e.what() << '\n'
                                            << "Error exception id: " << e.id << '\n'
                                            << "Error byte position of error: " << e.byte;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        std::stringstream ss;

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));
    }

    percent_progress = (uint32_t)100;

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
    /*}
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }*/
}