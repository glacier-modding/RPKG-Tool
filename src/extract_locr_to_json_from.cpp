#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/json/json.hpp"
#include <iostream>
#include <set>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>

using json = nlohmann::ordered_json;

void rpkg_function::extract_locr_to_json_from(std::string& input_path, std::string& filter, std::string& output_path,
                                              bool output_to_string) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path)) {
        input_path_is_rpkg_file = true;
    } else {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (!file::path_exists(input_path)) {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }

    if (!input_path_is_rpkg_file) {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }

    LOG("Scanning folder: Done");

    if (output_to_string) {
        localization_string = "";
    } else {
        file::create_directories(file::output_path_append("LOCR", output_path));
    }

    std::vector<std::string> filters = util::parse_input_filter(filter);

    uint64_t locr_count = 0;
    uint64_t locr_current_count = 0;

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) == "LOCR") {
                locr_count += rpkg.hashes_indexes_based_on_resource_types.at(r).size();
            }
        }
    }

    if (log_output) {
        LOG("Extracting LOCR as JSON...");
    }

    for (auto& rpkg : rpkgs) {
        if (rpkg.rpkg_file_path != input_path && input_path_is_rpkg_file)
            continue;

        bool archive_folder_created = false;

        for (uint64_t r1 = 0; r1 < rpkg.hash_resource_types.size(); r1++) {
            if (rpkg.hash_resource_types.at(r1) != "LOCR")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r1).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r1).at(j);

                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                std::string hash_file_name =
                        util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                        rpkg.hash.at(hash_index).hash_resource_type;

                bool found = false;

                uint64_t input_filter_index = 0;

                for (uint64_t z = 0; z < filters.size(); z++) {
                    std::size_t found_position = hash_file_name.find(filters.at(z));

                    if (found_position != std::string::npos && !filters.at(z).empty()) {
                        found = true;

                        input_filter_index = z;

                        break;
                    }
                }

                if (found || filter.empty()) {
                    locr_current_count++;

                    std::string current_path = file::output_path_append("LOCR\\" + rpkg.rpkg_file_name,
                                                                        output_path);

                    if (!archive_folder_created && !output_to_string) {
                        file::create_directories(current_path);

                        archive_folder_created = true;
                    }

                    uint64_t hash_size;

                    if (rpkg.hash.at(hash_index).data.lz4ed) {
                        hash_size = rpkg.hash.at(hash_index).data.header.data_size;

                        if (rpkg.hash.at(hash_index).data.xored) {
                            hash_size &= 0x3FFFFFFF;
                        }
                    } else {
                        hash_size = rpkg.hash.at(hash_index).data.resource.size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkg.rpkg_file_path, std::ifstream::binary);

                    if (!file.good()) {
                        LOG_AND_EXIT("Error: RPKG file " + rpkg.rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkg.hash.at(hash_index).data.header.data_offset, std::ifstream::beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkg.hash.at(hash_index).data.xored) {
                        crypto::xor_data(input_data.data(), (uint32_t) hash_size);
                    }

                    uint32_t decompressed_size = rpkg.hash.at(hash_index).data.resource.size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* locr_data;

                    if (rpkg.hash.at(hash_index).data.lz4ed) {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int) hash_size,
                                            decompressed_size);

                        locr_data = &output_data;
                    } else {
                        locr_data = &input_data;
                    }

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

                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;
                    bool isLOCRv2 = false;
                    bool symKey = false;

                    if ((unsigned int) (*locr_data)[0] == 0 || (unsigned int) (*locr_data)[0] == 1) {
                        position += 1;
                        std::memcpy(&number_of_languages, &(*locr_data)[position], BYTES4);
                        number_of_languages = (number_of_languages - 1) / 4;
                        isLOCRv2 = true;
                    } else {
                        std::memcpy(&number_of_languages, &(*locr_data)[position], BYTES4);
                        number_of_languages = (number_of_languages) / 4;
                    }

                    LOG((isLOCRv2 ? "LOCRv2 identified" : "LOCRv1 identified"));

                    if (number_of_languages == 10 && !isLOCRv2) {
                        LOG("Symmetric key cipher identified");
                        symKey = true;
                    }

                    // Quick fix for New Hitman 3 LOCR
                    if (number_of_languages == 9 && isLOCRv2) {
                        languages.emplace_back("xx");
                        languages.emplace_back("en");
                        languages.emplace_back("fr");
                        languages.emplace_back("it");
                        languages.emplace_back("de");
                        languages.emplace_back("es");
                        languages.emplace_back("ru");
                        languages.emplace_back("cn");
                        languages.emplace_back("tc");
                    } else if (number_of_languages == 10 && isLOCRv2) {
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
                    } else {
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

                    for (uint64_t k = 0; k < number_of_languages; k++) {
                        std::memcpy(&bytes4, &(*locr_data)[position], BYTES4);
                        position += 4;

                        language_offsets.push_back(bytes4);
                    }

                    for (uint64_t k = 0; k < (number_of_languages * (uint64_t) 0x4 +
                                              (isLOCRv2 ? (uint64_t) 0x1 : (uint64_t) 0x0)); k++) {
                        json_meta_data.push_back((*locr_data)[k]);
                    }

                    for (uint64_t k = 0; k < number_of_languages; k++) {
                        if (language_offsets.at(k) == 0xFFFFFFFF)
                            continue;

                        std::memcpy(&bytes4, &(*locr_data)[position], BYTES4);
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

                        for (uint64_t l = 0; l < language_string_count.back(); l++) {
                            std::vector<char> temp_string;

                            std::memcpy(&bytes4, &(*locr_data)[position], BYTES4);
                            position += 4;

                            temp_language_string_hash.push_back(bytes4);

                            std::memcpy(&bytes4, &(*locr_data)[position], BYTES4);
                            position += 4;

                            temp_language_string_length.push_back(bytes4);

                            for (uint64_t m = 0; m < temp_language_string_length.back(); m++) {
                                temp_string.push_back((*locr_data)[position]);
                                position += 1;
                            }

                            position += 1;

                            if (symKey) {
                                for (uint32_t m = 0; m < temp_language_string_length.back(); m++) {
                                    temp_string.at(m) = crypto::symmetric_key_decrypt_localization(
                                            temp_string.at(m));
                                }

                                json temp_json_object2;

                                temp_json_object2["StringHash"] = temp_language_string_hash.back();
                                temp_json_object2["String"] = std::string(temp_string.begin(),
                                                                          temp_string.end());

                                temp_language_json_object.push_back(temp_json_object2);
                            } else {
                                if (temp_language_string_length.back() % 8 != 0) {
                                    LOG_AND_EXIT("Error: LOCR file " + hash_file_name + " in " +
                                                 rpkg.rpkg_file_name + " is malformed.");
                                }

                                for (uint32_t m = 0; m < temp_language_string_length.back() / 8; m++) {
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

                                if (!temp_string.empty()) {
                                    auto m = (uint32_t) (temp_string.size() - 1);

                                    while (m >= 0) {
                                        if (temp_string.at(m) != 0) {
                                            break;
                                        } else {
                                            last_zero_position--;
                                        }

                                        m--;
                                    }
                                }

                                std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(),
                                                                                            temp_string.end()).substr(
                                        0, last_zero_position);

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

                    if (output_to_string) {
                        std::stringstream ss1;

                        ss1 << std::setw(4) << json_object << std::endl;

                        localization_string = ss1.str();

                        localization_json = json_object;
                    } else {
                        std::string json_path = current_path + "\\" + hash_file_name + ".JSON";

                        std::ofstream json_file = std::ofstream(json_path, std::ofstream::binary);

                        if (!json_file.good()) {
                            LOG_AND_EXIT("Error: JSON file " + json_path + " could not be created.");
                        }

                        json_file << std::setw(4) << json_object << std::endl;

                        json_file.close();

                        std::string json_meta_path = current_path + "\\" + hash_file_name + ".JSON.meta";

                        std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ofstream::binary);

                        if (!json_meta_file.good()) {
                            LOG_AND_EXIT("Error: JSON meta file " + json_meta_path + " could not be created.");
                        }

                        json_meta_file.write(json_meta_data.data(), json_meta_data.size());

                        json_meta_file.close();
                    }
                }
            }
        }
    }

    if (log_output) {
        LOG("Extracting LOCR as JSON: Done");
    }

    if (!output_to_string) {
        percent_progress = (uint32_t) 100;

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
}
