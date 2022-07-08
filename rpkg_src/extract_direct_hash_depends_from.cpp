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

void rpkg_function::extract_direct_hash_depends_from(std::string& input_path, std::string& filter, std::string& output_path, bool extract_prim_models)
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
        std::vector<hash_depends_variables> hash_depends_data;

        uint64_t hash_value = std::strtoull(filters.at(z).c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(i).hash_map.find(hash_value);

            hash_depends_variables temp_hash_depends_data;

            temp_hash_depends_data.rpkg_file_name = rpkgs.at(i).rpkg_file_name;

            if (it2 != rpkgs.at(i).hash_map.end())
            {
                temp_hash_depends_data.hash_value = hash_value;

                temp_hash_depends_data.hash_string = filters.at(z);

                uint32_t temp_hash_reference_count = rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                LOG(filters.at(z) << " has " << temp_hash_reference_count << " dependencies in " << rpkgs.at(i).rpkg_file_name);

                if (temp_hash_reference_count > 0)
                {
                    for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                    {
                        std::vector<std::string> dependency_in_rpkg_file;

                        bool found = false;

                        for (uint64_t j = 0; j < rpkgs.size(); j++)
                        {
                            std::map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(j).hash_map.find(rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference.at(k));

                            if (it3 != rpkgs.at(j).hash_map.end())
                            {
                                if (!found)
                                {
                                    temp_hash_depends_data.hash_dependency_file_name.push_back(rpkgs.at(j).hash.at(it3->second).hash_file_name);
                                }

                                found = true;

                                dependency_in_rpkg_file.push_back(rpkgs.at(j).rpkg_file_name);
                            }
                        }

                        if (!found)
                        {
                            temp_hash_depends_data.hash_dependency_file_name.push_back(rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference_string.at(k));
                        }

                        temp_hash_depends_data.hash_dependency_map[hash_value] = temp_hash_depends_data.hash_dependency_map.size();

                        temp_hash_depends_data.hash_dependency.push_back(util::uint64_t_to_hex_string(hash_value));

                        temp_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
                    }
                }

                hash_depends_data.push_back(temp_hash_depends_data);
            }
        }

        int rpkg_dependency_count = 0;

        for (uint64_t i = 0; i < hash_depends_data.size(); i++)
        {
            if (hash_depends_data.at(i).hash_dependency.size() > 0)
            {
                rpkg_dependency_count++;
            }
        }

        LOG(filters.at(z) << " has dependencies in " << rpkg_dependency_count << " RPKG files:" << std::endl);

        for (uint64_t i = 0; i < hash_depends_data.size(); i++)
        {
            if (hash_depends_data.at(i).hash_dependency.size() > 0)
            {
                LOG(filters.at(z) << " depends on " << hash_depends_data.at(i).hash_dependency_file_name.size() << " other hash files/resources in RPKG file: " << hash_depends_data.at(i).rpkg_file_name);

                if (hash_depends_data.at(i).hash_dependency_file_name.size() > 0)
                {
                    LOG(filters.at(z) << "'s dependencies:");

                    for (uint64_t j = 0; j < hash_depends_data.at(i).hash_dependency_file_name.size(); j++)
                    {
                        LOG_NO_ENDL("Hash file/resource: " << hash_depends_data.at(i).hash_dependency_file_name.at(j));

                        if (hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).size() > 0)
                        {
                            LOG_NO_ENDL(", Found in RPKG files: ");

                            for (uint64_t k = 0; k < hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).size(); k++)
                            {
                                LOG_NO_ENDL(hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).at(k));

                                if (k < hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).size() - 1)
                                {
                                    LOG_NO_ENDL(", ");
                                }
                            }

                            LOG_NO_ENDL(std::endl);
                        }
                        else
                        {
                            LOG(", Found in RPKG files: None");
                        }
                    }
                }

                LOG_NO_ENDL(std::endl);
            }
        }

        uint32_t reverse_dependency_count = 0;

        std::vector<std::string> reverse_dependency;
        std::vector<std::vector<std::string>> reverse_dependency_in_rpkg_file;

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
            {
                uint32_t temp_hash_reference_count = rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                {
                    if (rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference.at(k) == hash_value)
                    {
                        reverse_dependency_count++;

                        std::string rd = rpkgs.at(i).hash.at(j).hash_file_name;

                        if (reverse_dependency.size() > 0)
                        {
                            bool found = false;

                            for (uint64_t k = 0; k < reverse_dependency.size(); k++)
                            {
                                if (reverse_dependency.at(k) == rd)
                                {
                                    found = true;

                                    reverse_dependency_in_rpkg_file.at(k).push_back(rpkgs.at(i).rpkg_file_name);
                                }
                            }

                            if (!found)
                            {
                                reverse_dependency.push_back(rd);

                                std::vector<std::string> rpkgFiles;

                                rpkgFiles.push_back(rpkgs.at(i).rpkg_file_name);

                                reverse_dependency_in_rpkg_file.push_back(rpkgFiles);
                            }
                        }
                        else
                        {
                            reverse_dependency.push_back(rd);

                            std::vector<std::string> rpkgFiles;

                            rpkgFiles.push_back(rpkgs.at(i).rpkg_file_name);

                            reverse_dependency_in_rpkg_file.push_back(rpkgFiles);
                        }
                    }
                }
            }
        }

        LOG(filters.at(z) << " has reverse dependencies in " << reverse_dependency.size() << " RPKG files:" << std::endl);

        if (reverse_dependency.size() > 0)
        {
            LOG(filters.at(z) << "'s reverse dependencies:");

            for (uint64_t i = 0; i < reverse_dependency.size(); i++)
            {
                LOG_NO_ENDL("Hash file/resource: " << reverse_dependency.at(i));

                if (reverse_dependency_in_rpkg_file.at(i).size() > 0)
                {
                    LOG_NO_ENDL(", Found in RPKG files: ");

                    for (uint64_t j = 0; j < reverse_dependency_in_rpkg_file.at(i).size(); j++)
                    {
                        LOG_NO_ENDL(reverse_dependency_in_rpkg_file.at(i).at(j));

                        if (j < reverse_dependency_in_rpkg_file.at(i).size() - 1)
                        {
                            LOG_NO_ENDL(", ");
                        }
                    }

                    LOG_NO_ENDL(std::endl);
                }
                else
                {
                    LOG(", Found in RPKG files: None");
                }
            }
        }
    }

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
            std::vector<std::string> hashes_to_extract_strings;
            std::string filter_hash_string = "";

            for (uint64_t i = 0; i < rpkgs.size(); i++)
            {
                uint64_t hash_value = std::strtoull(filters.at(z).c_str(), nullptr, 16);

                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

                if (it != rpkgs.at(i).hash_map.end())
                {
                    filter_hash_string = rpkgs.at(i).hash.at(it->second).hash_string;

                    uint32_t hash_reference_count = rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                    for (uint32_t j = 0; j < hash_reference_count; j++)
                    {
                        hashes_to_extract.push_back(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j));
                        hashes_to_extract_strings.push_back(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_string.at(j));
                    }
                }
            }

            for (uint64_t x = 0; x < hashes_to_extract.size(); x++)
            {
                for (uint64_t i = 0; i < rpkgs.size(); i++)
                {
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hashes_to_extract.at(x));

                    if (it != rpkgs.at(i).hash_map.end())
                    {
                        if (extract_prim_models)
                        {
                            if (rpkgs.at(i).hash.at(it->second).hash_resource_type == "PRIM")
                            {
                                std::string prim_output_dir = file::output_path_append("DIRECTDEPENDS\\" + filter_hash_file_name + "\\PRIMMODELS\\", output_path);

                                file::create_directories(prim_output_dir);

                                //std::cout << "rpkg_function::extract_prim_model_from(" << rpkgs.at(i).rpkg_file_path << ", " << rpkgs.at(i).hash.at(hash_index).hash_string << ", " << prim_output_dir << ");" << std::endl;

                                rpkg_function::extract_prim_model_from(rpkgs.at(i).rpkg_file_path, hashes_to_extract_strings.at(x), prim_output_dir);
                            }
                        }
                        else
                        {
                            rpkg_extraction_vars rpkg_vars;

                            rpkg_vars.input_path = rpkgs.at(i).rpkg_file_path;
                            rpkg_vars.filter = hashes_to_extract_strings.at(x);
                            rpkg_vars.output_path = file::output_path_append("DIRECTDEPENDS\\" + filter_hash_file_name + "\\", output_path);

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