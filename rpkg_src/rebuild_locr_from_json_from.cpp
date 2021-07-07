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

void rpkg_function::rebuild_locr_from_json_from(std::string &input_path, std::string &filter, std::string &output_path)
{
    task_single_status = TASK_EXECUTING;

    std::string input_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_folder_path))
    {
        std::vector<std::string> json_file_names;
        std::vector<std::string> json_file_paths;
        std::vector<std::string> json_file_base_paths;
        std::vector<uint64_t> locr_hashes;
        std::vector<std::string> locr_hash_strings;
        std::vector<std::string> locr_file_names;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(input_folder_path))
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

                bool is_locr_hash_file = true;

                if (hash_string.length() != 16)
                {
                    is_locr_hash_file = false;
                }

                if (resource_type != "LOCR")
                {
                    is_locr_hash_file = false;
                }

                if (is_locr_hash_file)
                {
                    json_file_paths.push_back(entry.path().string());
                    json_file_base_paths.push_back(json_file_base_path);
                    json_file_names.push_back(json_file_name);
                    locr_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                    locr_file_names.push_back(util::to_upper_case(hash_file_name));
                    locr_hash_strings.push_back(util::to_upper_case(hash_string));
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

                ss << "Rebuilding JSON as LOCR" << std::string(period_count, '.');

                timing_string = ss.str();

                LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                period_count++;
            }

            if (file::path_exists(json_file_paths.at(p) + ".meta"))
            {
                std::ifstream input_json_meta_file = std::ifstream(json_file_paths.at(p) + ".meta", std::ifstream::binary);

                if (!input_json_meta_file.good())
                {
                    LOG_AND_EXIT("Error: JSON meta file " + json_file_paths.at(p) + ".meta" + " could not be read.");
                }

                bool isLOCRv2 = false;
                bool symKey = false;
				int identifyByte = input_json_meta_file.get();

                if (identifyByte == 0 || identifyByte == 1)
                {
                    isLOCRv2 = true;
                }

				#ifdef _DEBUG
					LOG((isLOCRv2 ? "LOCRv2 identified" : "LOCRv1 identified"));
				#endif

                input_json_meta_file.seekg(0, std::ios_base::end);

                uint64_t input_json_meta_file_size = input_json_meta_file.tellg();

                input_json_meta_file.seekg(0, input_json_meta_file.beg);

                if ((isLOCRv2 && (input_json_meta_file_size - 1) % 4 != 0) || (!isLOCRv2 && input_json_meta_file_size % 4 != 0))
                {
                    LOG_AND_EXIT("Error: JSON meta file " + json_file_paths.at(p) + ".meta" + " is corrupt.");
                }

                std::vector<char> input_json_meta_header(input_json_meta_file_size, 0);

                input_json_meta_file.read(input_json_meta_header.data(), input_json_meta_file_size);

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
                catch (json::parse_error &e)
                {
                    std::stringstream ss;
                    ss << "Error: " << json_file_paths.at(p) << "\n"
                       << "Error message: " << e.what() << '\n'
                       << "Error exception id: " << e.id << '\n'
                       << "Error byte position of error: " << e.byte;
                    LOG_AND_EXIT(ss.str());
                }

                input_json_file.close();

                int language_count = 0;

                for (const auto &it : input_json.items())
                {
                    bool language_found = false;

                    for (const auto &it2 : it.value().items())
                    {
                        if (it2.value().contains("Language"))
                        {
                            language_found = true;

                            language_count++;
                        }
                    }

                    if (!language_found)
                    {
                        LOG_AND_EXIT("Error: JSON file " + json_file_paths.at(p) + " is malformed and can not be rebuilt into a LOCR file/resource.");
                    }
                }

                std::vector<char> locr_data;

                std::vector<std::string> languages;

                if (input_json_meta_file_size == 0x19)
                {
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                }
                else if (input_json_meta_file_size == 0x35)
                {
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");
                    languages.push_back("cn");
                    languages.push_back("jp");
                    languages.push_back("tc");
                }
                else if (input_json_meta_file_size == 0x30)
                {
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");
                    languages.push_back("cn");
                    languages.push_back("jp");
                }
                else if (input_json_meta_file_size == 0x28)
                {
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");

                    LOG("Symmetric key cipher identified");
                    symKey = true;
                }

                std::vector<bool> language_in_locr;

                for (uint64_t i = 0; i < languages.size(); i++)
                {
                    uint32_t offset = 0;

                    std::memcpy(&offset, &input_json_meta_header.data()[isLOCRv2 ? i * 0x4 + 0x1 : i * 0x4], sizeof(uint32_t));

                    if (offset != 0xFFFFFFFF)
                    {
                        language_in_locr.push_back(true);
                    }
                    else
                    {
                        language_in_locr.push_back(false);
                    }
                }

                if (isLOCRv2)
                    locr_data.push_back(input_json_meta_header.data()[0]);

                uint32_t offset = input_json_meta_file_size;
                uint32_t prevoffset;
                char char4[4];

                std::vector<std::vector<uint32_t>> locr_language_section_string_hashes;
                std::vector<std::vector<std::string>> locr_language_section_strings;
                std::vector<std::vector<uint32_t>> locr_language_section_string_lengths;

                for (uint64_t i = 0; i < languages.size(); i++)
                {
                    if (language_in_locr.at(i))
                    {
                        for (const auto &it : input_json.items())
                        {
                            for (const auto &it2 : it.value().items())
                            {
                                if (it2.value().contains("Language"))
                                {
                                    if (it2.value()["Language"] == languages.at(i) && language_in_locr.at(i))
                                    {
                                        std::vector<uint32_t> temp_locr_language_section_string_hashes;
                                        std::vector<std::string> temp_locr_language_section_strings;
                                        std::vector<uint32_t> temp_locr_language_section_string_lengths;

                                        uint32_t locr_section_size = 0x4;

                                        for (const auto &it3 : it.value().items())
                                        {
                                            if (!it3.value().contains("Language"))
                                            {
                                                temp_locr_language_section_string_hashes.push_back((uint32_t)it3.value()["StringHash"]);

                                                std::string temp_string = it3.value()["String"];

                                                uint32_t string_length = (uint32_t)temp_string.length();

                                                if (symKey)
                                                {
                                                    locr_section_size += (uint32_t)temp_string.size() + (uint32_t)0x9;
                                                }
                                                else
                                                {
                                                    while (string_length % 8 != 0)
                                                    {
                                                        string_length++;

                                                        temp_string.push_back(0x0);
                                                    }

                                                    locr_section_size += (uint32_t)temp_string.size() + (uint32_t)0x9;
                                                }

                                                temp_locr_language_section_strings.push_back(temp_string);
                                                temp_locr_language_section_string_lengths.push_back((uint32_t)temp_string.size());
                                            }
                                        }

                                        prevoffset = offset;
                                        offset += locr_section_size;

                                        std::memcpy(&char4, &prevoffset, sizeof(uint32_t));

                                        for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                                        {
                                            locr_data.push_back(char4[j]);
                                        }

                                        locr_language_section_string_hashes.push_back(temp_locr_language_section_string_hashes);
                                        locr_language_section_strings.push_back(temp_locr_language_section_strings);
                                        locr_language_section_string_lengths.push_back(temp_locr_language_section_string_lengths);
                                    }
                                    else if (it2.value()["Language"] == languages.at(i) && !language_in_locr.at(i))
                                    {
                                        LOG_AND_EXIT("Error: LOCR has language in JSON but not in the meta file.");
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        std::vector<uint32_t> temp_locr_language_section_string_hashes;
                        std::vector<std::string> temp_locr_language_section_strings;
                        std::vector<uint32_t> temp_locr_language_section_string_lengths;

                        locr_data.push_back(0xFF);
                        locr_data.push_back(0xFF);
                        locr_data.push_back(0xFF);
                        locr_data.push_back(0xFF);

                        locr_language_section_string_hashes.push_back(temp_locr_language_section_string_hashes);
                        locr_language_section_strings.push_back(temp_locr_language_section_strings);
                        locr_language_section_string_lengths.push_back(temp_locr_language_section_string_lengths);
                    }
                }

                for (uint64_t i = 0; i < languages.size(); i++)
                {
                    for (const auto &it : input_json.items())
                    {
                        for (const auto &it2 : it.value().items())
                        {
                            if (it2.value().contains("Language"))
                            {
                                if (it2.value()["Language"] == languages.at(i))
                                {
                                    uint32_t section_string_count = (uint32_t)locr_language_section_string_hashes.at(i).size();

                                    std::memcpy(&char4, &section_string_count, sizeof(uint32_t));

                                    for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                                    {
                                        locr_data.push_back(char4[j]);
                                    }

                                    for (uint64_t j = 0; j < locr_language_section_strings.at(i).size(); j++)
                                    {
                                        std::memcpy(&char4, &locr_language_section_string_hashes.at(i).at(j), sizeof(uint32_t));

                                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                        {
                                            locr_data.push_back(char4[k]);
                                        }

                                        std::memcpy(&char4, &locr_language_section_string_lengths.at(i).at(j), sizeof(uint32_t));

                                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                        {
                                            locr_data.push_back(char4[k]);
                                        }

                                        if (symKey)
                                        {
                                            for (uint32_t k = 0; k < locr_language_section_string_lengths.at(i).at(j); k++)
                                            {
                                                locr_data.push_back(crypto::symmetric_key_encrypt_localization(locr_language_section_strings.at(i).at(j)[(uint64_t)k]));
                                            }
                                        }
                                        else
                                        {
                                            for (uint32_t k = 0; k < locr_language_section_string_lengths.at(i).at(j) / 8; k++)
                                            {
                                                uint32_t data[2];
                                                std::memcpy(data, &locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                                std::memcpy(data + 1, &locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                                crypto::xtea_encrypt_localization(data);

                                                std::memcpy(&locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                                std::memcpy(&locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                                            }

                                            for (uint64_t k = 0; k < locr_language_section_string_lengths.at(i).at(j); k++)
                                            {
                                                locr_data.push_back(locr_language_section_strings.at(i).at(j)[k]);
                                            }
                                        }

                                        locr_data.push_back(0x0);
                                    }
                                }
                            }
                        }
                    }
                }

                std::string current_path = json_file_base_paths.at(p) + "\\LOCR.rebuilt";

                file::create_directories(current_path);

                std::ofstream output_file = std::ofstream(current_path + "\\" + locr_file_names.at(p), std::ofstream::binary);

                if (!output_file.good())
                {
                    LOG_AND_EXIT("Error: Rebuilt LOCR file " + locr_file_names.at(p) + " could not be created.");
                }

                output_file.write(locr_data.data(), locr_data.size());
            }
            else
            {
                LOG("Error: JSON meta file " << json_file_paths.at(p) + ".meta"
                                             << " could not be found.");
                LOG("       Can not rebuild " << locr_file_names.at(p) << " from JSON file " << json_file_paths.at(p));
            }
        }

        ss.str(std::string());

        ss << "Rebuild JSON as LOCR: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_folder_path + " to rebuild the LOCR files from does not exist.");
    }
}