#include "rpkg_function.h"
#include "file.h"
#include "rpkg.h"
#include "hash.h"
#include "util.h"
#include "crypto.h"
#include "global.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>

void rpkg_function::generate_rpkg_from(std::string& input_path, std::string& output_path, bool increased_compression) {
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to generate the RPKG file does not exist.");
    }

    rpkg temp_rpkg_data;

    std::string rpkg_file_name;
    std::string rpkg_meta_file_name;

    std::string base_folder_name;

    std::size_t pos = input_rpkg_folder_path.find_last_of("\\/");

    if (pos == (input_rpkg_folder_path.length() - 1)) {
        input_rpkg_folder_path = input_rpkg_folder_path.substr(0, input_rpkg_folder_path.length() - 1);

        pos = input_rpkg_folder_path.find_last_of("\\/");

        if (pos != std::string::npos) {
            base_folder_name = input_rpkg_folder_path.substr(pos + 1, input_rpkg_folder_path.length() - (pos + 1));
            rpkg_file_name = base_folder_name + ".rpkg";
            rpkg_meta_file_name = base_folder_name + ".meta";
        } else {
            rpkg_file_name = input_rpkg_folder_path + ".rpkg";
            rpkg_meta_file_name = input_rpkg_folder_path + ".meta";
        }
    } else {
        if (pos != std::string::npos) {
            base_folder_name = input_rpkg_folder_path.substr(pos + 1, input_rpkg_folder_path.length() - (pos + 1));
            rpkg_file_name = base_folder_name + ".rpkg";
            rpkg_meta_file_name = base_folder_name + ".meta";
        } else {
            rpkg_file_name = input_rpkg_folder_path + ".rpkg";
            rpkg_meta_file_name = input_rpkg_folder_path + ".meta";
        }
    }

    rpkg rpkg_meta_data;

    std::string rpkg_meta_file_path = input_rpkg_folder_path + "\\" + rpkg_meta_file_name;

    bool use_rpkg_file_meta_data = rpkg_function::import_rpkg_meta(rpkg_meta_data, rpkg_meta_file_path,
                                                                   input_rpkg_folder_path);

    std::vector<std::string> files;
    std::vector<uint64_t> hashes;
    std::vector<bool> hash_in_rpkg_meta;
    std::unordered_map<uint64_t, uint64_t> hash_map;
    std::vector<std::string> hash_file_names;
    std::vector<std::string> hash_strings;
    std::vector<std::string> hash_resource_types;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path)) {
        if (!std::filesystem::is_regular_file(entry.path().string())) continue;

        std::size_t pos1 = entry.path().string().find_last_of("\\/");

        std::string hashFileName;
        std::string hash_string = "";
        std::string resource_type = "";

        if (pos1 != std::string::npos) {
            hashFileName = entry.path().string().substr(pos1 + 1, entry.path().string().length() - (pos1 + 1));
        } else {
            hashFileName = entry.path().string();
        }

        pos1 = hashFileName.find_last_of('.');

        if (pos1 != std::string::npos) {
            hash_string = hashFileName.substr(0, pos1);
            resource_type = hashFileName.substr(pos1 + 1, hashFileName.length() - (pos1 + 1));
        }

        bool is_hash_file = true;

        if (hash_string.length() != 16) {
            is_hash_file = false;
        }

        if (resource_type.length() != 4) {
            is_hash_file = false;
        }

        if (is_hash_file) {
            files.push_back(entry.path().string());
            hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
            hash_map[hashes.back()] = static_cast<int>(hashes.size() - 1);
            hash_in_rpkg_meta.push_back(false);
            hash_file_names.push_back(util::to_upper_case(hashFileName));
            hash_strings.push_back(util::to_upper_case(hash_string));
            hash_resource_types.push_back(util::to_upper_case(resource_type));
        }
    }

    LOG("Scanning folder: Done");

    std::vector<uint64_t> input_data_length;
    std::vector<uint64_t> output_data_length;

    const std::string temp_file_name = file::output_path_append(rpkg_file_name + ".tmp", output_path);

    std::ofstream temp_file_output = std::ofstream(temp_file_name, std::ofstream::binary);

    if (!temp_file_output.good()) {
        LOG_AND_EXIT("Error: Temporary file " + temp_file_name + " could not be read.");
    }

    LOG("Writing to temporary RPKG file: " + temp_file_name);

    bool found_all_meta_files = true;

    std::vector<uint64_t> files_index;

    if (use_rpkg_file_meta_data) {
        for (auto& i : rpkg_meta_data.meta_hash_value) {
            auto it = hash_map.find(i);

            if (it != hash_map.end()) {
                files_index.push_back(it->second);

                hash_in_rpkg_meta.at(it->second) = true;
            }
        }
    }

    for (uint64_t i = 0; i < hashes.size(); i++) {
        if (!hash_in_rpkg_meta.at(i)) {
            files_index.push_back(i);
        }
    }

    for (const auto& i : files_index) {
        if (gui_control == ABORT_CURRENT_TASK) {
            return;
        }

        std::ifstream file = std::ifstream(files.at(i), std::ifstream::binary);

        if (!file.good()) {
            LOG_AND_EXIT("Error: Hash file " + files.at(i) + " could not be read.");
        }

        file.seekg(0, std::ifstream::end);
        uint64_t input_file_size = file.tellg();
        file.seekg(0, std::ifstream::beg);

        std::vector<char> input_file_data(input_file_size, 0);
        std::vector<char> output_file_data;

        input_data_length.push_back(input_file_size);
        file.read(input_file_data.data(), input_file_size);
        file.close();

        hash temp_hash_data;
        hash meta_data;

        bool use_hash_file_meta_data = false;

        if (file::path_exists(files.at(i) + ".meta")) {
            use_hash_file_meta_data = true;

            std::string hash_file_name = files.at(i) + ".meta";

            rpkg_function::import_hash_meta(meta_data, hash_file_name);

            uint64_t test_hash_value = std::strtoll(hash_strings.at(i).c_str(), nullptr, 16);

            if (meta_data.hash_value != test_hash_value && test_hash_value > 0x0) {
                meta_data.hash_value = test_hash_value;
            }

            temp_hash_data.hash_reference_data = meta_data.hash_reference_data;
        } else if (file::path_exists(files.at(i) + ".meta.json")) {
            std::string hash_file_name = files.at(i) + ".meta.json";

            if (rpkg_function::import_hash_meta_json(meta_data, hash_file_name)) {
                use_hash_file_meta_data = true;

                uint64_t test_hash_value = std::strtoll(hash_strings.at(i).c_str(), nullptr, 16);

                if (meta_data.hash_value != test_hash_value && test_hash_value > 0x0) {
                    meta_data.hash_value = test_hash_value;
                }

                temp_hash_data.hash_reference_data = meta_data.hash_reference_data;
            }
        }

        if (!use_hash_file_meta_data) {
            found_all_meta_files = false;

            meta_data.data.lz4ed = true;
            meta_data.data.xored = true;

            hash_reference_variables temp_hash_reference_data;

            temp_hash_data.hash_reference_data = temp_hash_reference_data;
        }

        if (meta_data.data.lz4ed) {
            temp_hash_data.data.lz4ed = true;

            uint32_t compressed_size = LZ4_compressBound(static_cast<int>(input_file_size));

            output_file_data = std::vector<char>(compressed_size, 0);

            uint64_t compressed_size_final;

            if (increased_compression) {
                compressed_size_final = LZ4_compress_HC(input_file_data.data(), output_file_data.data(),
                                                        static_cast<int>(input_file_size), compressed_size,
                                                        LZ4HC_CLEVEL_MAX);
            } else {
                compressed_size_final = LZ4_compress_fast(input_file_data.data(), output_file_data.data(),
                                                          static_cast<int>(input_file_size), compressed_size, 15);
            }

            output_data_length.push_back(compressed_size_final);

            if (meta_data.data.xored) {
                crypto::xor_data(output_file_data.data(), static_cast<uint32_t>(compressed_size_final));

                temp_hash_data.data.xored = true;
                temp_hash_data.data.header.data_size =
                        static_cast<uint32_t>(compressed_size_final) | (uint32_t) 0x80000000;
            } else {
                temp_hash_data.data.xored = false;
                temp_hash_data.data.header.data_size = static_cast<uint32_t>(compressed_size_final);
            }

            if (use_hash_file_meta_data) {
                temp_hash_data.hash_value = meta_data.hash_value;
                //temp_hash_data.hash_string = util::uint64_t_to_hex_string(meta_data.hash_value);
                temp_hash_data.hash_resource_type = meta_data.hash_resource_type;
                temp_hash_data.data.resource.reference_table_size = meta_data.data.resource.reference_table_size;
                temp_hash_data.data.resource.reference_table_dummy = meta_data.data.resource.reference_table_dummy;
                temp_hash_data.data.resource.size_final = static_cast<uint32_t>(input_file_size);

                if (meta_data.data.resource.size_in_memory == 0xFFFFFFFF) {
                    temp_hash_data.data.resource.size_in_memory = meta_data.data.resource.size_in_memory;
                    temp_hash_data.data.resource.size_in_video_memory = meta_data.data.resource.size_in_video_memory;
                } else {
                    auto resized_size_in_memory = static_cast<uint32_t>((double) input_file_size *
                                                                        ((double) meta_data.data.resource.size_in_memory /
                                                                         (double) meta_data.data
                                                                                 .resource.size_final));

                    const uint32_t resized_difference_value =
                            meta_data.data.resource.size_in_memory - static_cast<uint32_t>(
                                    (double) meta_data.data.resource.size_final *
                                    ((double) meta_data.data.resource.size_in_memory / (
                                            double) meta_data.data.resource.size_final));

                    resized_size_in_memory += resized_difference_value;

                    temp_hash_data.data.resource.size_in_memory = resized_size_in_memory;
                    temp_hash_data.data.resource.size_in_video_memory = meta_data.data.resource.size_in_video_memory;
                }
            } else {
                uint64_t hash_value = std::strtoll(hash_strings.at(i).c_str(), nullptr, 16);
                temp_hash_data.hash_value = hash_value;
                //temp_hash_data.hash_string = hash_strings.at(files_index.at(i));
                temp_hash_data.hash_resource_type = hash_resource_types.at(i);
                temp_hash_data.data.resource.reference_table_size = 0x0;
                temp_hash_data.data.resource.reference_table_dummy = 0x0;
                temp_hash_data.data.resource.size_final = static_cast<uint32_t>(input_file_size);
                temp_hash_data.data.resource.size_in_memory = static_cast<uint32_t>(compressed_size_final);
                temp_hash_data.data.resource.size_in_video_memory = 0x0;
            }
        } else {
            temp_hash_data.data.lz4ed = false;

            output_file_data = std::vector<char>(input_file_size, 0);

            output_data_length.push_back(input_file_size);

            std::memcpy(output_file_data.data(), input_file_data.data(), input_file_size);

            if (meta_data.data.xored) {
                crypto::xor_data(output_file_data.data(), static_cast<uint32_t>(input_file_size));

                temp_hash_data.data.xored = true;
                temp_hash_data.data.header.data_size = 0x80000000;
            } else {
                temp_hash_data.data.xored = false;
                temp_hash_data.data.header.data_size = 0x0;
            }

            if (use_hash_file_meta_data) {
                temp_hash_data.hash_value = meta_data.hash_value;
                //temp_hash_data.hash_string = util::uint64_t_to_hex_string(meta_data.hash_value);
                temp_hash_data.hash_resource_type = meta_data.hash_resource_type;
                temp_hash_data.data.resource.reference_table_size = meta_data.data.resource.reference_table_size;
                temp_hash_data.data.resource.reference_table_dummy = meta_data.data.resource.reference_table_dummy;
                temp_hash_data.data.resource.size_final = static_cast<uint32_t>(input_file_size);

                if (meta_data.data.resource.size_in_memory == 0xFFFFFFFF) {
                    temp_hash_data.data.resource.size_in_memory = meta_data.data.resource.size_in_memory;
                    temp_hash_data.data.resource.size_in_video_memory = meta_data.data.resource.size_in_video_memory;
                } else {
                    auto resized_size_in_memory = static_cast<uint32_t>((double) input_file_size *
                                                                        ((double) meta_data.data.resource.size_in_memory /
                                                                         (double) meta_data.data
                                                                                 .resource.size_final));

                    const uint32_t resized_difference_value =
                            meta_data.data.resource.size_in_memory - static_cast<uint32_t>(
                                    (double) meta_data.data.resource.size_final *
                                    ((double) meta_data.data.resource.size_in_memory / (
                                            double) meta_data.data.resource.size_final));

                    resized_size_in_memory += resized_difference_value;

                    temp_hash_data.data.resource.size_in_memory = resized_size_in_memory;
                    temp_hash_data.data.resource.size_in_video_memory = meta_data.data.resource.size_in_video_memory;
                }
            } else {
                uint64_t hash_value = std::strtoll(hash_strings.at(i).c_str(), nullptr, 16);
                temp_hash_data.hash_value = hash_value;
                //temp_hash_data.hash_string = hash_strings.at(files_index.at(i));
                temp_hash_data.hash_resource_type = hash_resource_types.at(i);
                temp_hash_data.data.resource.reference_table_size = 0x0;
                temp_hash_data.data.resource.reference_table_dummy = 0x0;
                temp_hash_data.data.resource.size_final = static_cast<uint32_t>(input_file_size);
                temp_hash_data.data.resource.size_in_memory = static_cast<uint32_t>(input_file_size);
                temp_hash_data.data.resource.size_in_video_memory = 0x0;
            }
        }

        temp_file_output.write(output_file_data.data(), output_data_length.back());

        temp_rpkg_data.hash.push_back(temp_hash_data);
    }

    LOG("Done (P1)");

    percent_progress = static_cast<uint32_t>(100);

    if (!found_all_meta_files) {
        LOG("Some hash files did not have meta files.");
        LOG("When a hash file does not have a meta file the defaults used are:");
        LOG("  - The hash file is XORed and LZ4ed");
        LOG("  - The hash file's ref table is set to 0, meaning the hash file's");
        LOG("    reference data linking to other hashes will not be included.");
    }

    temp_file_output.close();

    const auto hash_count = static_cast<uint32_t>(temp_rpkg_data.hash.size());
    const uint32_t table_offset = hash_count * 0x14;
    uint32_t table_size = 0x0;
    uint64_t hash_offset = 0x0;

    for (auto& j : temp_rpkg_data.hash) {
        table_size += 0x18;
        table_size += j.data.resource.reference_table_size;
    }

    if (rpkg_meta_data.is_patch_file) {
        if (rpkg_meta_data.rpkg_file_version == 1) {
            hash_offset = static_cast<uint64_t>(table_offset) + static_cast<uint64_t>(table_size) +
                          static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) +
                          static_cast<uint64_t>(0x14);
        } else {
            hash_offset = static_cast<uint64_t>(table_offset) + static_cast<uint64_t>(table_size) +
                          static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) +
                          static_cast<uint64_t>(0x1D);
        }
    } else {
        if (rpkg_meta_data.rpkg_file_version == 1) {
            hash_offset = static_cast<uint64_t>(table_offset) + static_cast<uint64_t>(table_size) +
                          static_cast<uint64_t>(0x10);
        } else {
            hash_offset = static_cast<uint64_t>(table_offset) + static_cast<uint64_t>(table_size) +
                          static_cast<uint64_t>(0x19);
        }
    }

    std::ofstream file = std::ofstream(file::output_path_append(rpkg_file_name, output_path), std::ofstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkg_file_name + " could not be created.");
    }

    char char1[1];
    char char4[4];
    char char8[8];

    if (rpkg_meta_data.rpkg_file_version == 1) {
        file.write("G", 0x1);
        file.write("K", 0x1);
        file.write("P", 0x1);
        file.write("R", 0x1);
    } else {
        file.write("2", 0x1);
        file.write("K", 0x1);
        file.write("P", 0x1);
        file.write("R", 0x1);
    }

    if (rpkg_meta_data.rpkg_file_version == 2) {
        for (const char& j : rpkg_meta_data.rpkgv2_header) {
            std::memcpy(&char1, &j, sizeof(uint8_t));

            file.write(char1, 0x1);
        }
    }

    std::memcpy(&char4, &hash_count, sizeof(uint32_t));
    file.write(char4, sizeof(uint32_t));
    std::memcpy(&char4, &table_offset, sizeof(uint32_t));
    file.write(char4, sizeof(uint32_t));
    std::memcpy(&char4, &table_size, sizeof(uint32_t));
    file.write(char4, sizeof(uint32_t));

    if (rpkg_meta_data.is_patch_file) {
        std::memcpy(&char4, &rpkg_meta_data.header.patch_count, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));

        for (uint64_t i = 0; i < rpkg_meta_data.header.patch_count; i++) {
            std::memcpy(&char8, &rpkg_meta_data.patch_entry_list.at(i), sizeof(uint64_t));
            file.write(char8, sizeof(uint64_t));
        }
    }

    for (const auto& i : temp_rpkg_data.hash) {
        std::memcpy(&char8, &i.hash_value, sizeof(uint64_t));
        file.write(char8, sizeof(uint64_t));
        std::memcpy(&char8, &hash_offset, sizeof(uint64_t));
        file.write(char8, sizeof(uint64_t));

        std::memcpy(&char4, &i.data.header.data_size, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));

        if (i.data.lz4ed) {
            hash_offset += i.data.header.data_size & 0x3FFFFFFF;
        } else {
            hash_offset += i.data.resource.size_final;
        }
    }

    LOG("Done (P2)");

    percent_progress = static_cast<uint32_t>(100);

    for (const auto& i : temp_rpkg_data.hash) {
        file.write(&i.hash_resource_type[3], 0x1);
        file.write(&i.hash_resource_type[2], 0x1);
        file.write(&i.hash_resource_type[1], 0x1);
        file.write(&i.hash_resource_type[0], 0x1);

        std::memcpy(&char4, &i.data.resource.reference_table_size, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &i.data.resource.reference_table_dummy, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &i.data.resource.size_final, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &i.data.resource.size_in_memory, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &i.data.resource.size_in_video_memory, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));

        uint32_t temp_hash_reference_count =
                i.hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        if (i.data.resource.reference_table_size > 0) {
            std::memcpy(&char4, &i.hash_reference_data.hash_reference_count, sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));
        }

        if (temp_hash_reference_count > 0) {
            for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
                std::memcpy(&char1, &i.hash_reference_data.hash_reference_type.at(j),
                            sizeof(uint8_t));
                file.write(char1, sizeof(uint8_t));
            }

            for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
                std::memcpy(&char8, &i.hash_reference_data.hash_reference.at(j),
                            sizeof(uint64_t));
                file.write(char8, sizeof(uint64_t));
            }
        }
    }

    LOG("Done (P3)");

    std::ifstream temp_file_input = std::ifstream(temp_file_name, std::ifstream::binary);

    if (!temp_file_input.good()) {
        LOG_AND_EXIT("Error: Temporary file " + temp_file_name + " could not be read.");
    }

    if (rpkg_meta_data.rpkg_file_version == 1) {
        LOG("Merging RPKGv1 file and temporary RPKG files...");
    } else {
        LOG("Merging RPKGv2 file and temporary RPKG files...");
    }

    file << temp_file_input.rdbuf();

    if (rpkg_meta_data.rpkg_file_version == 1) {
        LOG("Merged RPKGv1 file and temporary RPKGv1 file");
    } else {
        LOG("Merged RPKGv2 file and temporary RPKGv2 file");
    }

    temp_file_input.close();

    std::remove(temp_file_name.c_str());

    file.close();

    if (rpkg_meta_data.rpkg_file_version == 1) {
        LOG("Successfully generated RPKGv1 file: " << rpkg_file_name);
    } else {
        LOG("Successfully generated RPKGv2 file: " << rpkg_file_name);
    }

    percent_progress = static_cast<uint32_t>(100);

    task_single_status = TASK_SUCCESSFUL;
}
