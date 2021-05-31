#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "rpkg.h"
#include "hash.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/json/json.hpp"
#include <iostream>
#include <set>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

using json = nlohmann::ordered_json;

void rpkg_function::rebuild_dlge_from_json_from(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;

    std::string input_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_folder_path))
    {
        std::vector<std::string> json_file_names;
        std::vector<std::string> json_file_paths;
        std::vector<std::string> json_file_base_paths;
        std::vector<uint64_t> dlge_hashes;
        std::vector<std::string> dlge_hash_strings;
        std::vector<std::string> dlge_file_names;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(input_folder_path))
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

                std::string json_file_name = "";
                std::string json_file_base_path = "";
                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos)
                {
                    json_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    json_file_base_path = entry.path().string().substr(0, pos);
                }
                else
                {
                    json_file_name = entry.path().string();
                }

                if (util::to_upper_case(json_file_name.substr((json_file_name.length() - 5), 5)) == ".JSON")
                {
                    hash_file_name = util::to_upper_case(json_file_name.substr(0, (json_file_name.length() - 5)));
                }

                pos = hash_file_name.find_last_of(".");

                if (pos != std::string::npos)
                {
                    hash_string = hash_file_name.substr(0, pos);
                    resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                }

                bool is_dlge_hash_file = true;

                if (hash_string.length() != 16)
                {
                    is_dlge_hash_file = false;
                }

                if (resource_type != "DLGE")
                {
                    is_dlge_hash_file = false;
                }

                if (is_dlge_hash_file)
                {
                    json_file_paths.push_back(entry.path().string());
                    json_file_base_paths.push_back(json_file_base_path);
                    json_file_names.push_back(json_file_name);
                    dlge_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                    dlge_file_names.push_back(util::to_upper_case(hash_file_name));
                    dlge_hash_strings.push_back(util::to_upper_case(hash_string));
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t p = 0; p < json_file_paths.size(); p++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (file::path_exists(json_file_paths.at(p) + ".meta"))
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

                    ss.str(std::string());

                    ss << "Rebuilding JSON as DLGE" << std::string(period_count, '.');

                    timing_string = ss.str();

                    LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                    period_count++;
                }

                std::ifstream input_json_meta_file = std::ifstream(json_file_paths.at(p) + ".meta", std::ifstream::binary);

                if (!input_json_meta_file.good())
                {
                    LOG_AND_EXIT("Error: JSON meta file " + json_file_paths.at(p) + ".meta" + " could not be read.");
                }

                input_json_meta_file.seekg(0, input_json_meta_file.end);

                uint64_t input_json_meta_file_size = input_json_meta_file.tellg();

                input_json_meta_file.seekg(0, input_json_meta_file.beg);

                std::vector<char> input_json_meta(input_json_meta_file_size, 0);

                input_json_meta_file.read(input_json_meta.data(), input_json_meta_file_size);

                input_json_meta_file.close();

                std::ifstream input_json_file(json_file_paths.at(p));

                if (!input_json_file.good())
                {
                    LOG_AND_EXIT("Error: JSON file " + json_file_paths.at(p) + " could not be read.");
                }

                json input_json;

                try
                {
                    input_json_file >> input_json;
                }
                catch (json::parse_error& e)
                {
                    std::stringstream ss;
                    ss <<"Error: " << json_file_paths.at(p) << "\n" << "Error message: " << e.what() << '\n' << "Error exception id: " << e.id << '\n' << "Error byte position of error: " << e.byte;
                    LOG_AND_EXIT(ss.str());
                }
                
                input_json_file.close();

                int category_language_count = 0;

                for (const auto& it : input_json.items())
                {
                    bool language_found = false;

                    if (it.value().contains("Language"))
                    {
                        language_found = true;

                        category_language_count++;
                    }

                    if (!language_found)
                    {
                        LOG_AND_EXIT("Error: JSON file " + json_file_paths.at(p) + " is malformed and can not be rebuilt into a DLGE file/resource.");
                    }
                }

                std::vector<char> dlge_data;

                char char4[4] = "";
                uint32_t bytes4 = 0;

                uint32_t position = 0;

                uint32_t number_of_dlge_categories = 0;

                std::memcpy(&number_of_dlge_categories, &input_json_meta.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                uint32_t number_of_languages = 0;

                std::memcpy(&number_of_languages, &input_json_meta.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                if (category_language_count != (number_of_dlge_categories * number_of_languages))
                {
                    LOG_AND_EXIT("Error: Number of strings in the input DLGE JSON file do not match the number of strings in the meta file.");
                }

                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x1);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);

                for (uint64_t i = 0; i < number_of_dlge_categories; i++)
                {
                    dlge_data.push_back(0x1);

                    std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                    {
                        dlge_data.push_back(char4[j]);
                    }

                    std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                    {
                        dlge_data.push_back(char4[j]);
                    }

                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back((char)0xFF);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);

                    uint32_t key_range_min = (uint32_t)(i * number_of_languages);
                    uint32_t key_range_max = key_range_min + number_of_languages;
                    uint32_t key_current = 0;

                    for (const auto& it : input_json.items())
                    {
                        if (key_current >= key_range_min && key_current < key_range_max)
                        {
                            std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                            position += sizeof(uint32_t);

                            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                            {
                                dlge_data.push_back(char4[j]);
                            }

                            std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                            position += sizeof(uint32_t);

                            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                            {
                                dlge_data.push_back(char4[j]);
                            }

                            std::string temp_string = it.value()["String"];

                            uint32_t string_length = (uint32_t)temp_string.length();

                            while (string_length % 8 != 0)
                            {
                                string_length++;

                                temp_string.push_back(0x0);
                            }

                            std::memcpy(&char4, &string_length, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                dlge_data.push_back(char4[k]);
                            }

                            for (uint32_t k = 0; k < string_length / 8; k++)
                            {
                                uint32_t data[2];
                                std::memcpy(data, &temp_string[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                std::memcpy(data + 1, &temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                crypto::xtea_encrypt_localization(data);

                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                            }

                            for (uint64_t k = 0; k < string_length; k++)
                            {
                                dlge_data.push_back(temp_string[k]);
                            }
                        }

                        key_current++;
                    }
                }

                for (uint64_t k = 0; k < (input_json_meta_file_size - position); k++)
                {
                    dlge_data.push_back(input_json_meta.data()[position + k]);
                }

                std::string current_path = json_file_base_paths.at(p) + "\\DLGE.rebuilt";

                file::create_directories(current_path);

                std::ofstream output_file = std::ofstream(current_path + "\\" + dlge_file_names.at(p), std::ifstream::binary);

                if (!output_file.good())
                {
                    LOG_AND_EXIT("Error: Rebuilt DLGE file " + dlge_file_names.at(p) + " could not be created.");
                }

                output_file.write(dlge_data.data(), dlge_data.size());
            }
            else
            {
                LOG("Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be found.");
                LOG("       Can not rebuild " << dlge_file_names.at(p) << " from JSON file " << json_file_paths.at(p));
            }
        }

        ss.str(std::string());

        ss << "Rebuilding JSON as DLGE: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_folder_path + " to rebuild the DLGE files from does not exist.");
    }
}