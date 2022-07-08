#include "dev_function.h"
#include "rpkg_function.h"
#include "generic_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "hash.h"
#include "console.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

void dev_function::dev_diff_rpkg_supermetas(std::string& input_path, std::string& filter, std::string& output_path)
{
    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(true);
        LOG("Loading Hash List: Done");
    }

    input_path = file::parse_input_folder_path(input_path);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    std::map<std::string, uint64_t> supermeta_file_name_map;
    std::vector<std::vector<std::string>> supermeta_file_paths;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_path))
    {
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

            ss << "Scanning folder" << std::string(period_count, '.');

            timing_string = ss.str();

            LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

            period_count++;
        }

        if (std::filesystem::is_regular_file(entry.path().string()))
        {
            std::size_t pos = entry.path().string().find_last_of("\\/");

            std::string supermeta_file_name = "";
            std::string hash = "";
            std::string hash_resource_type = "";

            if (pos != std::string::npos)
            {
                supermeta_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
            }
            else
            {
                supermeta_file_name = entry.path().string();
            }

            if (file::is_supermeta_file(supermeta_file_name))
            {
                std::map<std::string, uint64_t>::iterator it = supermeta_file_name_map.find(supermeta_file_name);

                //std::cout << supermeta_file_name << std::endl;

                if (it == supermeta_file_name_map.end())
                {
                    std::vector<std::string> temp_supermeta_file_paths;

                    temp_supermeta_file_paths.push_back(entry.path().string());

                    supermeta_file_paths.push_back(temp_supermeta_file_paths);

                    supermeta_file_name_map[supermeta_file_name] = supermeta_file_paths.size() - 1;
                }
                else
                {
                    supermeta_file_paths.at(it->second).push_back(entry.path().string());
                }
            }
        }
    }

    for (uint64_t i = 0; i < supermeta_file_paths.size(); i++)
    {
        if (supermeta_file_paths.at(i).size() > 1)
        {
            for (uint64_t j = 0; j < supermeta_file_paths.at(i).size(); j++)
            {
                std::cout << supermeta_file_paths.at(i).at(j) << std::endl;

                std::cout << "Importing RPKG SUPERMETA:\n  - " << supermeta_file_paths.at(i).at(j) << std::endl;

                rpkg_function::import_rpkg(supermeta_file_paths.at(i).at(j), true);
            }

            for (uint64_t x = 0; x < rpkgs.size(); x++)
            {
                std::cout << "RPKG File Details: " << std::endl;
                std::cout << "  - File Path: " << rpkgs.at(x).rpkg_file_path << std::endl;
                std::cout << "  - Hash Count: " << rpkgs.at(x).hash.size() << std::endl;
            }

            for (uint64_t x = 0; x < rpkgs.size(); x++)
            {
                for (uint64_t y = (x + 1); y < rpkgs.size(); y++)
                {
                    std::cout << "Now Comparing: " << std::endl;
                    std::cout << "  - File Path: " << rpkgs.at(x).rpkg_file_path << std::endl;
                    std::cout << "  - Hash Count: " << rpkgs.at(x).hash.size() << std::endl;
                    std::cout << "and" << std::endl;
                    std::cout << "  - File Path: " << rpkgs.at(y).rpkg_file_path << std::endl;
                    std::cout << "  - Hash Count: " << rpkgs.at(y).hash.size() << std::endl;

                    for (uint64_t a = 0; a < rpkgs.at(x).hash.size(); a++)
                    {
                        std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(y).hash_map.find(rpkgs.at(x).hash.at(a).hash_value);

                        if (it == rpkgs.at(y).hash_map.end())
                        {
                            std::string ioi_string = "";

                            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(x).hash.at(a).hash_value);

                            if (it2 != hash_list_hash_map.end())
                            {
                                ioi_string = hash_list_hash_strings.at(it2->second);
                            }

                            std::cout << rpkgs.at(x).hash.at(a).hash_file_name << ":" << std::endl;
                            std::cout << "  - IOI String: " << ioi_string << std::endl;
                            std::cout << "  - Located in: " << rpkgs.at(x).rpkg_file_path << std::endl;
                            std::cout << "  - Not Located in: " << rpkgs.at(y).rpkg_file_path << std::endl;
                        }
                    }

                    for (uint64_t a = 0; a < rpkgs.at(y).hash.size(); a++)
                    {
                        std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(x).hash_map.find(rpkgs.at(y).hash.at(a).hash_value);

                        if (it == rpkgs.at(x).hash_map.end())
                        {
                            std::string ioi_string = "";

                            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(y).hash.at(a).hash_value);

                            if (it2 != hash_list_hash_map.end())
                            {
                                ioi_string = hash_list_hash_strings.at(it2->second);
                            }

                            std::cout << rpkgs.at(y).hash.at(a).hash_file_name << ":" << std::endl;
                            std::cout << "  - IOI String: " << ioi_string << std::endl;
                            std::cout << "  - Located in: " << rpkgs.at(y).rpkg_file_path << std::endl;
                            std::cout << "  - Not Located in: " << rpkgs.at(x).rpkg_file_path << std::endl;
                        }
                    }

                    for (uint64_t a = 0; a < rpkgs.at(x).hash.size(); a++)
                    {
                        std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(y).hash_map.find(rpkgs.at(x).hash.at(a).hash_value);

                        if (it != rpkgs.at(y).hash_map.end())
                        {
                            std::string ioi_string = "";

                            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(x).hash.at(a).hash_value);

                            if (it2 != hash_list_hash_map.end())
                            {
                                ioi_string = hash_list_hash_strings.at(it2->second);
                            }

                            uint64_t hash_size1;

                            if (rpkgs.at(x).hash.at(a).is_lz4ed == 1)
                            {
                                hash_size1 = rpkgs.at(x).hash.at(a).hash_size;

                                if (rpkgs.at(x).hash.at(a).is_xored == 1)
                                {
                                    hash_size1 &= 0x3FFFFFFF;
                                }
                            }
                            else
                            {
                                hash_size1 = rpkgs.at(x).hash.at(a).hash_size_final;
                            }

                            uint64_t hash_size2;

                            if (rpkgs.at(y).hash.at(it->second).is_lz4ed == 1)
                            {
                                hash_size2 = rpkgs.at(y).hash.at(it->second).hash_size;

                                if (rpkgs.at(y).hash.at(it->second).is_xored == 1)
                                {
                                    hash_size2 &= 0x3FFFFFFF;
                                }
                            }
                            else
                            {
                                hash_size2 = rpkgs.at(y).hash.at(it->second).hash_size_final;
                            }

                            if (hash_size1 != hash_size2)
                            {
                                std::cout << rpkgs.at(x).hash.at(a).hash_file_name << ":" << std::endl;
                                std::cout << "  - IOI String: " << ioi_string << std::endl;
                                std::cout << "  - Hash Size Differs: " << ioi_string << std::endl;
                                std::cout << "    - In: " << rpkgs.at(x).rpkg_file_path << std::endl;
                                std::cout << "      - Hash Size: " << hash_size1 << std::endl;
                                std::cout << "    - In: " << rpkgs.at(y).rpkg_file_path << std::endl;
                                std::cout << "      - Hash Size: " << hash_size2 << std::endl;
                            }
                        }
                    }

                    for (uint64_t a = 0; a < rpkgs.at(y).hash.size(); a++)
                    {
                        std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(x).hash_map.find(rpkgs.at(y).hash.at(a).hash_value);

                        if (it != rpkgs.at(x).hash_map.end())
                        {
                            std::string ioi_string = "";

                            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(y).hash.at(a).hash_value);

                            if (it2 != hash_list_hash_map.end())
                            {
                                ioi_string = hash_list_hash_strings.at(it2->second);
                            }

                            uint64_t hash_size1;

                            if (rpkgs.at(y).hash.at(a).is_lz4ed == 1)
                            {
                                hash_size1 = rpkgs.at(y).hash.at(a).hash_size;

                                if (rpkgs.at(y).hash.at(a).is_xored == 1)
                                {
                                    hash_size1 &= 0x3FFFFFFF;
                                }
                            }
                            else
                            {
                                hash_size1 = rpkgs.at(y).hash.at(a).hash_size_final;
                            }

                            uint64_t hash_size2;

                            if (rpkgs.at(x).hash.at(it->second).is_lz4ed == 1)
                            {
                                hash_size2 = rpkgs.at(x).hash.at(it->second).hash_size;

                                if (rpkgs.at(x).hash.at(it->second).is_xored == 1)
                                {
                                    hash_size2 &= 0x3FFFFFFF;
                                }
                            }
                            else
                            {
                                hash_size2 = rpkgs.at(x).hash.at(it->second).hash_size_final;
                            }

                            if (hash_size1 != hash_size2)
                            {
                                std::cout << rpkgs.at(y).hash.at(a).hash_file_name << ":" << std::endl;
                                std::cout << "  - IOI String: " << ioi_string << std::endl;
                                std::cout << "  - Hash Size Differs: " << ioi_string << std::endl;
                                std::cout << "    - In: " << rpkgs.at(y).rpkg_file_path << std::endl;
                                std::cout << "      - Hash Size: " << hash_size1 << std::endl;
                                std::cout << "    - In: " << rpkgs.at(x).rpkg_file_path << std::endl;
                                std::cout << "      - Hash Size: " << hash_size2 << std::endl;
                            }
                        }
                    }
                }
            }

            std::cout << "rpkgs size: " << rpkgs.size() << std::endl;

            std::vector<rpkg>().swap(rpkgs);
        }
        else
        {
            std::cout << "Not enough supermetas found for " << supermeta_file_paths.at(i).at(0) << " to perform a diff operation." << std::endl;
        }
    }
}