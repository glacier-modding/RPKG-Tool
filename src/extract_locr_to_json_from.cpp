#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/json/json.hpp"
#include <iostream>
#include <set>
#include <sstream>
#include <fstream>
#include <filesystem>

using json = nlohmann::ordered_json;

void rpkg_function::extract_locr_to_json_from(std::string& input_path, std::string& output_path, bool output_to_string)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    if (output_to_string)
    {
        localization_string = "";
    }
    else
    {
        file::create_directories(file::output_path_append("LOCR", output_path));
    }

    if (log_output)
        LOG("Extracting LOCR as JSON...");

    const uintmax_t input_size = std::filesystem::file_size(input_path);

    std::vector<char> input_data(input_size, 0);

    std::ifstream file = std::ifstream(input_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: LOCR file " + input_path + " could not be read.");
    }

    file.read(input_data.data(), input_size);
    file.close();

    std::vector<char> output_data(input_size, 0);

    uint32_t number_of_languages = 0;
    std::vector<uint32_t> language_offsets;
    std::vector<uint32_t> language_string_count;
    std::vector<std::vector<uint32_t>> language_string_hash;
    std::vector<std::vector<uint32_t>> language_string_length;
    std::vector<std::vector<std::string>> language_string;
    std::vector<std::set<std::pair<uint32_t, std::string>>> language_string_set;
    std::vector<std::string> languages;

    std::vector<char> json_meta_data;

    json json_object;

    uint32_t position = 0;

    uint32_t bytes4 = 0;
    bool isLOCRv2 = false;
    bool symKey = false;

    if ((unsigned int) input_data[0] == 0 || (unsigned int) input_data[0] == 1)
    {
        position += 1;
        std::memcpy(&number_of_languages, &input_data[position], sizeof(bytes4));
        number_of_languages = (number_of_languages - 1) / 4;
        isLOCRv2 = true;
    }
    else
    {
        std::memcpy(&number_of_languages, &input_data[position], sizeof(bytes4));
        number_of_languages = (number_of_languages) / 4;
    }

    if (log_output)
        LOG((isLOCRv2 ? "LOCRv2 identified" : "LOCRv1 identified"));

    if (number_of_languages == 10 && !isLOCRv2)
    {
        LOG("Symmetric key cipher identified");
        symKey = true;
    }

    // Quick fix for New Hitman 3 LOCR
    if (number_of_languages == 9 && isLOCRv2)
    {
        languages.emplace_back("xx");
        languages.emplace_back("en");
        languages.emplace_back("fr");
        languages.emplace_back("it");
        languages.emplace_back("de");
        languages.emplace_back("es");
        languages.emplace_back("ru");
        languages.emplace_back("cn");
        languages.emplace_back("tc");
    }
    else if (number_of_languages == 10 && isLOCRv2)
    {
        languages.emplace_back("xx");
        languages.emplace_back("en");
        languages.emplace_back("fr");
        languages.emplace_back("it");
        languages.emplace_back("de");
        languages.emplace_back("es");
        languages.emplace_back("ru");
        languages.emplace_back("cn");
        languages.emplace_back("tc");
        languages.emplace_back("jp");
    }
    else
    {
        languages.emplace_back("xx");
        languages.emplace_back("en");
        languages.emplace_back("fr");
        languages.emplace_back("it");
        languages.emplace_back("de");
        languages.emplace_back("es");
        languages.emplace_back("ru");
        languages.emplace_back("mx");
        languages.emplace_back("br");
        languages.emplace_back("pl");
        languages.emplace_back("cn");
        languages.emplace_back("jp");
        languages.emplace_back("tc");
    }

    for (uint64_t k = 0; k < number_of_languages; k++)
    {
        std::memcpy(&bytes4, &input_data[position], sizeof(bytes4));
        position += 4;

        language_offsets.push_back(bytes4);
    }

    const uint64_t fmt_offset = isLOCRv2 ? 0x1 : 0x0;

    for (uint64_t k = 0; k < number_of_languages * (uint64_t) 0x4 + fmt_offset; k++)
    {
        json_meta_data.push_back(input_data[k]);
    }

    for (uint64_t k = 0; k < number_of_languages; k++)
    {
        if (language_offsets.at(k) == 0xFFFFFFFF)
            continue;

        std::memcpy(&bytes4, &input_data[position], sizeof(bytes4));
        position += 4;

        language_string_count.push_back(bytes4);

        std::vector<uint32_t> temp_language_string_hash;
        std::vector<uint32_t> temp_language_string_length;
        std::vector<std::string> temp_language_string;
        std::set<std::pair<uint32_t, std::string>> temp_language_string_set;
        json temp_language_json_object;

        json temp_json_object1;

        temp_json_object1["Language"] = languages.at(k);

        temp_language_json_object.push_back(temp_json_object1);

        for (uint64_t l = 0; l < language_string_count.back(); l++)
        {
            std::vector<char> temp_string;

            std::memcpy(&bytes4, &input_data[position], sizeof(bytes4));
            position += 4;

            temp_language_string_hash.push_back(bytes4);

            std::memcpy(&bytes4, &input_data[position], sizeof(bytes4));
            position += 4;

            temp_language_string_length.push_back(bytes4);

            for (uint64_t m = 0; m < temp_language_string_length.back(); m++)
            {
                temp_string.push_back(input_data[position]);
                position += 1;
            }

            position += 1;

            if (symKey)
            {
                for (uint32_t m = 0; m < temp_language_string_length.back(); m++)
                {
                    temp_string.at(m) = crypto::symmetric_key_decrypt_localization(
                            temp_string.at(m));
                }

                json temp_json_object2;

                temp_json_object2["StringHash"] = temp_language_string_hash.back();
                temp_json_object2["String"] = std::string(temp_string.begin(),
                                                          temp_string.end());

                temp_language_json_object.push_back(temp_json_object2);
            }
            else
            {
                if (temp_language_string_length.back() % 8 != 0)
                {
                    LOG_AND_EXIT("Error: LOCR file is malformed.");
                }

                for (uint32_t m = 0; m < temp_language_string_length.back() / 8; m++)
                {
                    uint32_t data[2];
                    std::memcpy(data, &temp_string[(uint64_t) m * (uint64_t) 8],
                                sizeof(uint32_t));
                    std::memcpy(data + 1,
                                &temp_string[(uint64_t) m * (uint64_t) 8 + (uint64_t) 4],
                                sizeof(uint32_t));

                    crypto::xtea_decrypt_localization(data);

                    std::memcpy(&temp_string[(uint64_t) m * (uint64_t) 8], data,
                                sizeof(uint32_t));
                    std::memcpy(&temp_string[(uint64_t) m * (uint64_t) 8 + (uint64_t) 4],
                                data + 1, sizeof(uint32_t));
                }

                auto last_zero_position = (uint32_t) temp_string.size();

                if (!temp_string.empty())
                {
                    auto m = (uint32_t) (temp_string.size() - 1);

                    while (m >= 0)
                    {
                        if (temp_string.at(m) != 0)
                        {
                            break;
                        }
                        else
                        {
                            last_zero_position--;
                        }

                        m--;
                    }
                }

                std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(),
                                                                            temp_string.end()).substr(0, last_zero_position);

                temp_language_string.push_back(temp_string_with_zero_pad_removed);

                std::pair<uint32_t, std::string> temp_pair;

                temp_pair.first = temp_language_string_hash.back();
                temp_pair.second = temp_language_string.back();

                temp_language_string_set.insert(temp_pair);

                json temp_json_object2;

                temp_json_object2["StringHash"] = temp_language_string_hash.back();
                temp_json_object2["String"] = temp_language_string.back();

                temp_language_json_object.push_back(temp_json_object2);
            }
        }

        json_object.push_back(temp_language_json_object);
    }

    const std::string json_path = input_path + ".JSON";

    std::ofstream json_file = std::ofstream(json_path, std::ofstream::binary);

    if (!json_file.good())
    {
        LOG_AND_EXIT("Error: JSON file " + json_path + " could not be created.");
    }

    json_file << std::setw(4) << json_object << std::endl;

    json_file.close();

    const std::string json_meta_path = input_path + ".JSON.meta";

    std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ofstream::binary);

    if (!json_meta_file.good())
    {
        LOG_AND_EXIT("Error: JSON meta file " + json_meta_path + " could not be created.");
    }

    json_meta_file.write(json_meta_data.data(), json_meta_data.size());
    json_meta_file.close();

    percent_progress = (uint32_t) 100;
    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
