#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/json/json.hpp"
#include <set>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

using json = nlohmann::ordered_json;

void rpkg_function::extract_rtlv_to_json_from(std::string &input_path, std::string &filter, std::string &output_path, bool output_to_string)
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

    if (!file::path_exists(input_path))
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }

    if (!input_path_is_rpkg_file)
    {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    if (output_to_string)
    {
        localization_string = "";
    }
    else
    {
        file::create_directories(file::output_path_append("RTLV", output_path));
    }

    std::vector<std::string> filters = util::parse_input_filter(filter);

    uint64_t rtlv_count = 0;
    uint64_t rtlv_current_count = 0;

    for (auto& rpkg : rpkgs)
    {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++)
        {
            if (rpkg.hash_resource_types.at(r) == "RTLV")
            {
                rtlv_count += rpkg.hashes_indexes_based_on_resource_types.at(r).size();
            }
        }
    }

    timing_string = "Extracting RTLV as JSON...";

    if (log_output)
        LOG("Extracting RTLV as JSON...");

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    for (auto& rpkg : rpkgs)
    {
        if (rpkg.rpkg_file_path != input_path && input_path_is_rpkg_file)
            continue;

        bool archive_folder_created = false;

        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++)
        {
            if (rpkg.hash_resource_types.at(r) != "RTLV")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++)
            {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                    rpkg.hash.at(hash_index).hash_resource_type;

                bool found = false;

                for (uint64_t z = 0; z < filters.size(); z++)
                {
                    std::size_t found_position = hash_file_name.find(filters.at(z));

                    if (found_position != std::string::npos && filters.at(z) != "")
                    {
                        found = true;

                        break;
                    }
                }

                if (found || filter == "")
                {
                    std::string message = "Extracting RTLV as JSON: ";

                    if (((rtlv_current_count * static_cast<uint64_t>(100000)) / rtlv_count) % static_cast<uint64_t>(100) == 0 && rtlv_current_count > 0 && !output_to_string)
                    {
                        stringstream_length = console::update_console(message, rtlv_count, rtlv_current_count, start_time, stringstream_length);
                    }

                    rtlv_current_count++;

                    std::string current_path = file::output_path_append("RTLV\\" + rpkg.rpkg_file_name, output_path);

                    if (!archive_folder_created && !output_to_string)
                    {
                        file::create_directories(current_path);

                        archive_folder_created = true;
                    }

                    uint64_t hash_size;

                    if (rpkg.hash.at(hash_index).data.lz4ed)
                    {
                        hash_size = rpkg.hash.at(hash_index).data.header.data_size;

                        if (rpkg.hash.at(hash_index).data.xored)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkg.hash.at(hash_index).data.resource.size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkg.rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkg.rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkg.hash.at(hash_index).data.header.data_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkg.hash.at(hash_index).data.xored)
                    {
                        crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                    }

                    uint32_t decompressed_size = rpkg.hash.at(hash_index).data.resource.size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char> *rtlv_data;

                    if (rpkg.hash.at(hash_index).data.lz4ed)
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size), decompressed_size);

                        rtlv_data = &output_data;
                    }
                    else
                    {
                        rtlv_data = &input_data;
                    }

                    uint32_t rtlv_data_size = 0;
                    uint64_t rtlv_header_value = 0;
                    std::vector<char> rtlv_footer;
                    uint32_t rtlv_header_data_size = 0;
                    std::vector<char> rtlv_header;
                    uint32_t languages_starting_offset = 0;
                    uint32_t number_of_languages = 0;
                    uint32_t check_for_languages = 0;
                    std::vector<uint32_t> language_offsets;
                    std::vector<std::vector<char>> language_data;
                    std::vector<uint16_t> language_data_sizes;
                    std::vector<uint16_t> language_unknowns;
                    std::vector<std::vector<std::string>> language_string;
                    std::vector<std::set<std::pair<uint32_t, std::string>>> language_string_set;
                    std::vector<std::string> languages;

                    std::vector<char> json_meta_data;

                    json json_object;

                    uint32_t position = 0;

                    char char4[4] = "";
                    uint16_t bytes2 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    position = 0x58;
                    std::memcpy(&languages_starting_offset, &rtlv_data->data()[position], sizeof(bytes4));

                    // Quick fix for New Hitman 3 LOCR
                    std::memcpy(&check_for_languages, &rtlv_data->data()[0xC4], sizeof(bytes4));
                    if (check_for_languages == 9)
                    {
                        languages.push_back("xx");
                        languages.push_back("en");
                        languages.push_back("fr");
                        languages.push_back("it");
                        languages.push_back("de");
                        languages.push_back("es");
                        languages.push_back("ru");
                        languages.push_back("cn");
                        languages.push_back("tc");
                    }
                    else
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

                    position = 0;
                    std::memcpy(&rtlv_header_value, &rtlv_data->data()[position], sizeof(bytes8));

                    for (uint64_t k = 0; k < sizeof(bytes8); k++)
                    {
                        json_meta_data.push_back(rtlv_data->data()[position + k]);
                    }

                    position += sizeof(bytes8);

                    std::memcpy(&bytes4, &rtlv_data->data()[position], sizeof(bytes4));
                    position += sizeof(bytes4);

                    rtlv_data_size = ((bytes4 & 0x000000FF) << 0x18) + ((bytes4 & 0x0000FF00) << 0x8) + ((bytes4 & 0x00FF0000) >> 0x8) + ((bytes4 & 0xFF000000) >> 0x18);

                    if (languages_starting_offset < rtlv_data_size)
                    {
                        languages_starting_offset += 0xC;

                        rtlv_header_data_size = languages_starting_offset - position;

                        std::memcpy(&char4, &rtlv_header_data_size, sizeof(bytes4));

                        for (char& k : char4)
                        {
                            json_meta_data.push_back(k);
                        }

                        for (uint64_t k = 0; k < rtlv_header_data_size; k++)
                        {
                            rtlv_header.push_back(rtlv_data->data()[position]);

                            json_meta_data.push_back(rtlv_data->data()[position]);

                            position += 1;
                        }

                        std::memcpy(&number_of_languages, &rtlv_data->data()[position], sizeof(bytes4));

                        for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        {
                            json_meta_data.push_back(rtlv_data->data()[position + k]);
                        }

                        position += sizeof(bytes4);

                        for (uint64_t k = 0; k < number_of_languages; k++)
                        {
                            uint16_t section_length = 0;

                            std::memcpy(&section_length, &rtlv_data->data()[position], sizeof(bytes2));
                            position += sizeof(bytes2);

                            std::memcpy(&bytes2, &rtlv_data->data()[position], sizeof(bytes2));
                            position += sizeof(bytes2);

                            language_unknowns.push_back(bytes2);

                            std::memcpy(&bytes4, &rtlv_data->data()[position], sizeof(bytes4));
                            position += sizeof(bytes4);

                            std::memcpy(&bytes4, &rtlv_data->data()[position], sizeof(bytes4));
                            position += sizeof(bytes4);

                            language_offsets.push_back(bytes4);

                            std::memcpy(&bytes4, &rtlv_data->data()[position], sizeof(bytes4));
                            position += sizeof(bytes4);
                        }

                        for (uint64_t k = 0; k < number_of_languages; k++)
                        {
                            json temp_language_json_object;

                            temp_language_json_object["Language"] = languages.at(k);

                            std::memcpy(&bytes4, &rtlv_data->data()[position], sizeof(bytes4));
                            position += sizeof(bytes4);

                            language_data_sizes.push_back(bytes4);

                            std::vector<char> temp_string;

                            if (language_data_sizes.back() == 0)
                            {
                                std::memcpy(&bytes4, &rtlv_data->data()[position], sizeof(bytes4));
                                position += sizeof(bytes4);

                                temp_language_json_object["String"] = "";
                            }
                            else
                            {
                                for (uint64_t l = 0; l < language_data_sizes.back(); l++)
                                {
                                    temp_string.push_back(rtlv_data->data()[position]);
                                    position += 1;
                                }

                                if (language_data_sizes.back() % 8 != 0)
                                {
                                    LOG_AND_EXIT("Error: RTLV file " + hash_file_name + " in " + rpkg.rpkg_file_name + " is malformed.");
                                }

                                for (uint32_t m = 0; m < static_cast<uint32_t>(language_data_sizes.back()) / 8; m++)
                                {
                                    uint32_t data[2];
                                    std::memcpy(data, &temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8)], sizeof(uint32_t));
                                    std::memcpy(data + 1, &temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8) + static_cast<uint64_t>(4)], sizeof(uint32_t));

                                    crypto::xtea_decrypt_localization(data);

                                    std::memcpy(&temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8)], data, sizeof(uint32_t));
                                    std::memcpy(&temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8) + static_cast<uint64_t>(4)], data + 1, sizeof(uint32_t));
                                }

                                uint32_t last_zero_position = static_cast<uint32_t>(temp_string.size());

                                if (temp_string.size() > 0)
                                {
                                    uint32_t m = static_cast<uint32_t>(temp_string.size() - 1);

                                    while (m >= 0)
                                    {
                                        if (temp_string.at(m) != 0)
                                        {
                                            break;
                                        }

                                        last_zero_position--;
                                        m--;
                                    }
                                }

                                std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(), temp_string.end()).substr(0, last_zero_position);

                                temp_language_json_object["String"] = temp_string_with_zero_pad_removed;
                            }

                            json_object.push_back(temp_language_json_object);
                        }

                        if ((decompressed_size - position) > 0)
                        {
                            for (uint64_t k = 0; k < (decompressed_size - position); k++)
                            {
                                json_meta_data.push_back(rtlv_data->data()[position + k]);
                            }
                        }

                        if (output_to_string)
                        {
                            std::stringstream ss;

                            ss << std::setw(4) << json_object << std::endl;

                            localization_string = ss.str();

                            localization_json = json_object;
                        }
                        else
                        {
                            std::string json_path = current_path + "\\" + hash_file_name + ".JSON";

                            std::ofstream json_file = std::ofstream(json_path, std::ofstream::binary);

                            if (!json_file.good())
                            {
                                LOG_AND_EXIT("Error: JSON file " + json_path + " could not be created.");
                            }

                            json_file << std::setw(4) << json_object << std::endl;

                            json_file.close();

                            std::string json_meta_path = current_path + "\\" + hash_file_name + ".JSON.meta";

                            std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ofstream::binary);

                            if (!json_meta_file.good())
                            {
                                LOG_AND_EXIT("Error: JSON meta file " + json_meta_path + " could not be created.");
                            }

                            json_meta_file.write(json_meta_data.data(), json_meta_data.size());

                            json_meta_file.close();
                        }
                    }
                }
            }
        }
    }

    ss.str(std::string());

    ss << "Extracting RTLV as JSON: Done";

    if (log_output)
        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    if (!output_to_string)
    {
        percent_progress = static_cast<uint32_t>(100);

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
}
