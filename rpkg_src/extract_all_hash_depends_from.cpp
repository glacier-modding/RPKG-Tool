#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include "rpkg.h"
#include "hash.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <set>

void recursive_hash_depends_search(uint64_t hash_value, std::vector<uint64_t>& hashes_to_extract, std::map<uint64_t, uint64_t>& hashes_to_extract_map)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

        if (it != rpkgs.at(i).hash_map.end())
        {
            uint32_t hash_reference_count = rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint32_t j = 0; j < hash_reference_count; j++)
            {
                std::map<uint64_t, uint64_t>::iterator it2 = hashes_to_extract_map.find(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j));

                if (it2 == hashes_to_extract_map.end())
                {
                    hashes_to_extract.push_back(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j));

                    hashes_to_extract_map[rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j)] = hashes_to_extract.size();

                    recursive_hash_depends_search(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j), hashes_to_extract, hashes_to_extract_map);
                }
            }
        }
    }
}

void rpkg_function::extract_all_hash_depends_from(std::string& input_path, std::string& filter, std::string& output_path, bool extract_prim_models)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::vector<std::string> filters = util::parse_input_filter(filter);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (uint64_t z = 0; z < filters.size(); z++)
    {
        bool filter_found = false;

        std::string filter_hash_file_name = "";

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(std::strtoull(filters.at(z).c_str(), nullptr, 16));

            if (it != rpkgs.at(i).hash_map.end())
            {
                filter_found = true;

                filter_hash_file_name = rpkgs.at(i).hash.at(it->second).hash_file_name;
            }
        }

        if (filter_found)
        {
            std::vector<uint64_t> hashes_to_extract;
            std::map<uint64_t, uint64_t> hashes_to_extract_map;

            uint64_t hash_value = std::strtoull(filters.at(z).c_str(), nullptr, 16);

            hashes_to_extract.push_back(hash_value);

            hashes_to_extract_map[hash_value] = hashes_depends_map.size();

            recursive_hash_depends_search(hash_value, hashes_to_extract, hashes_to_extract_map);

            std::string message = "";

            if (extract_prim_models)
            {
                message = "Extracting all hash depends and PRIM models for " + filters.at(z) + ": ";
            }
            else
            {
                message = "Extracting all hash depends for " + filters.at(z) + ": ";
            }

            start_time = std::chrono::high_resolution_clock::now();
            int stringstream_length = 80;

            for (uint64_t x = 0; x < hashes_to_extract.size(); x++)
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                if (((x * (uint64_t)100000) / hashes_to_extract.size()) % (uint64_t)100 == 0 && x > 0)
                {
                    stringstream_length = console::update_console(message, hashes_to_extract.size(), x, start_time, stringstream_length);
                }

                for (uint64_t i = 0; i < rpkgs.size(); i++)
                {
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hashes_to_extract.at(x));

                    if (it != rpkgs.at(i).hash_map.end())
                    {
                        if (extract_prim_models)
                        {
                            if (rpkgs.at(i).hash.at(it->second).hash_resource_type == "PRIM")
                            {
                                std::string prim_output_dir = file::output_path_append("ALLDEPENDS\\" + filter_hash_file_name + "\\PRIMMODELS\\", output_path);

                                file::create_directories(prim_output_dir);

                                //std::cout << "rpkg_function::extract_prim_model_from(" << rpkgs.at(i).rpkg_file_path << ", " << rpkgs.at(i).hash.at(hash_index).hash_string << ", " << prim_output_dir << ");" << std::endl;

                                std::string hash_filter = util::uint64_t_to_hex_string(hashes_to_extract.at(x));

                                rpkg_function::extract_prim_model_from(rpkgs.at(i).rpkg_file_path, hash_filter, prim_output_dir);
                            }
                        }
                        else
                        {
                            rpkg_extraction_vars rpkg_vars;

                            rpkg_vars.input_path = rpkgs.at(i).rpkg_file_path;
                            rpkg_vars.filter = util::uint64_t_to_hex_string(hashes_to_extract.at(x));
                            rpkg_vars.output_path = file::output_path_append("ALLDEPENDS\\" + filter_hash_file_name + "\\", output_path);

                            extract_from_rpkg_with_map(rpkg_vars);
                        }
                    }
                }
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}

void rpkg_function::extract_non_base_hash_depends_from(std::string& input_path, std::string& filter, std::string& output_path, bool extract_prim_models)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::vector<std::string> filters = util::parse_input_filter(filter);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (uint64_t z = 0; z < filters.size(); z++)
    {
        bool filter_found = false;

        std::string filter_hash_file_name = "";

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(std::strtoull(filters.at(z).c_str(), nullptr, 16));

            if (it != rpkgs.at(i).hash_map.end())
            {
                filter_found = true;

                filter_hash_file_name = rpkgs.at(i).hash.at(it->second).hash_file_name;
            }
        }

        if (filter_found)
        {
            std::vector<uint64_t> hashes_to_extract;
            std::map<uint64_t, uint64_t> hashes_to_extract_map;

            uint64_t hash_value = std::strtoull(filters.at(z).c_str(), nullptr, 16);

            hashes_to_extract.push_back(hash_value);

            hashes_to_extract_map[hash_value] = hashes_depends_map.size();

            recursive_hash_depends_search(hash_value, hashes_to_extract, hashes_to_extract_map);

            std::string message = "";

            if (extract_prim_models)
            {
                message = "Extracting all hash depends and PRIM models for " + filters.at(z) + ": ";
            }
            else
            {
                message = "Extracting all hash depends for " + filters.at(z) + ": ";
            }

            start_time = std::chrono::high_resolution_clock::now();
            int stringstream_length = 80;

            for (uint64_t x = 0; x < hashes_to_extract.size(); x++)
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                if (((x * (uint64_t)100000) / hashes_to_extract.size()) % (uint64_t)100 == 0 && x > 0)
                {
                    stringstream_length = console::update_console(message, hashes_to_extract.size(), x, start_time, stringstream_length);
                }

                for (uint64_t i = 0; i < rpkgs.size(); i++)
                {
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hashes_to_extract.at(x));

                    if (it != rpkgs.at(i).hash_map.end())
                    {
                        if (extract_prim_models)
                        {
                            if (rpkgs.at(i).hash.at(it->second).hash_resource_type == "PRIM")
                            {
                                std::string prim_output_dir = file::output_path_append("ALLDEPENDS\\" + filter_hash_file_name + "\\PRIMMODELS\\", output_path);

                                file::create_directories(prim_output_dir);

                                //std::cout << "rpkg_function::extract_prim_model_from(" << rpkgs.at(i).rpkg_file_path << ", " << rpkgs.at(i).hash.at(hash_index).hash_string << ", " << prim_output_dir << ");" << std::endl;

                                std::string hash_filter = util::uint64_t_to_hex_string(hashes_to_extract.at(x));

                                rpkg_function::extract_prim_model_from(rpkgs.at(i).rpkg_file_path, hash_filter, prim_output_dir);
                            }
                        }
                        else
                        {
                            if ((rpkgs.at(i).rpkg_file_path.find("chunk0.rpkg") == std::string::npos) && (rpkgs.at(i).rpkg_file_path.find("chunk0patch") == std::string::npos) && (rpkgs.at(i).rpkg_file_path.find("chunk1.rpkg") == std::string::npos) && (rpkgs.at(i).rpkg_file_path.find("chunk1patch") == std::string::npos))
                            {
                                rpkg_extraction_vars rpkg_vars;

                                rpkg_vars.input_path = rpkgs.at(i).rpkg_file_path;
                                rpkg_vars.filter = util::uint64_t_to_hex_string(hashes_to_extract.at(x));
                                rpkg_vars.output_path = file::output_path_append("ALLDEPENDS\\" + filter_hash_file_name + "\\", output_path);

                                extract_from_rpkg_with_map(rpkg_vars);
                            }
                        }
                    }
                }
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}