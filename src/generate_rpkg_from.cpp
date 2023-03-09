#include "rpkg_function.h"
#include "file.h"
#include "rpkg.h"
#include "hash.h"
#include "util.h"
#include "console.h"
#include "crypto.h"
#include "global.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <chrono>
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

    std::string base_folder_name = "";

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

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path)) {
        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(
                end_time - start_time).count());

        if (time_in_seconds_from_start_time > console_update_rate) {
            start_time = end_time;

            if (period_count > 3) {
                period_count = 0;
            }

            std::stringstream ss;

            ss << "Scanning folder" << std::string(period_count, '.');

            timing_string = ss.str();

            LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

            period_count++;
        }

        if (std::filesystem::is_regular_file(entry.path().string())) {
            std::size_t pos = entry.path().string().find_last_of("\\/");

            std::string hash_file_name = "";
            std::string hash_string = "";
            std::string resource_type = "";

            if (pos != std::string::npos) {
                hash_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
            } else {
                hash_file_name = entry.path().string();
            }

            pos = hash_file_name.find_last_of('.');

            if (pos != std::string::npos) {
                hash_string = hash_file_name.substr(0, pos);
                resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
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
                hash_file_names.push_back(util::to_upper_case(hash_file_name));
                hash_strings.push_back(util::to_upper_case(hash_string));
                hash_resource_types.push_back(util::to_upper_case(resource_type));
            }
        }
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    std::vector<uint64_t> input_data_length;
    std::vector<uint64_t> output_data_length;

    std::string temp_file_name = file::output_path_append(rpkg_file_name + ".tmp", output_path);

    std::ofstream temp_file_output = std::ofstream(temp_file_name, std::ofstream::binary);

    if (!temp_file_output.good()) {
        LOG_AND_EXIT("Error: Temporary file " + temp_file_name + " could not be read.");
    }

    LOG("Writing to temporary RPKG file: " + temp_file_name);

    timing_string = "Generating RPKG file: " + temp_file_name;

    start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    std::string message = "Generating RPKG file: ";

    bool found_all_meta_files = true;

    std::vector<uint64_t> files_index;

    if (use_rpkg_file_meta_data) {
        for (unsigned long long i : rpkg_meta_data.meta_hash_value) {
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

    for (uint64_t i = 0; i < files_index.size(); i++) {
        if (gui_control == ABORT_CURRENT_TASK) {
            return;
        }

        if (((i * static_cast<uint64_t>(100000)) / files_index.size()) % static_cast<uint64_t>(100) == 0 && i > 0) {
            stringstream_length = console::update_console(message, files_index.size(), i, start_time,
                                                          stringstream_length);
        }

        std::ifstream file = std::ifstream(files.at(files_index.at(i)), std::ifstream::binary);

        if (!file.good()) {
            LOG_AND_EXIT("Error: Hash file " + files.at(files_index.at(i)) + " could not be read.");
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

        if (file::path_exists(files.at(files_index.at(i)) + ".meta")) {
            use_hash_file_meta_data = true;

            std::string hash_file_name = files.at(files_index.at(i)) + ".meta";

            rpkg_function::import_hash_meta(meta_data, hash_file_name);

            uint64_t test_hash_value = std::strtoll(hash_strings.at(files_index.at(i)).c_str(), nullptr, 16);

            if (meta_data.hash_value != test_hash_value && test_hash_value > 0x0) {
                meta_data.hash_value = test_hash_value;
            }

            temp_hash_data.hash_reference_data = meta_data.hash_reference_data;
        } else if (file::path_exists(files.at(files_index.at(i)) + ".meta.json")) {
            std::string hash_file_name = files.at(files_index.at(i)) + ".meta.json";

            if (rpkg_function::import_hash_meta_json(meta_data, hash_file_name)) {
                use_hash_file_meta_data = true;

                uint64_t test_hash_value = std::strtoll(hash_strings.at(files_index.at(i)).c_str(), nullptr, 16);

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
                    uint32_t resized_size_in_memory = static_cast<uint32_t>((double) input_file_size *
                                                                            ((double) meta_data.data.resource.size_in_memory /
                                                                             (double) meta_data.data
                                                                                     .resource.size_final));
                    uint32_t resized_difference_value = meta_data.data.resource.size_in_memory - static_cast<uint32_t>(
                            (double) meta_data.data.resource.size_final *
                            ((double) meta_data.data.resource.size_in_memory / (
                                    double) meta_data.data.resource.size_final));

                    resized_size_in_memory += resized_difference_value;

                    temp_hash_data.data.resource.size_in_memory = resized_size_in_memory;
                    temp_hash_data.data.resource.size_in_video_memory = meta_data.data.resource.size_in_video_memory;
                }
            } else {
                uint64_t hash_value = std::strtoll(hash_strings.at(files_index.at(i)).c_str(), nullptr, 16);
                temp_hash_data.hash_value = hash_value;
                //temp_hash_data.hash_string = hash_strings.at(files_index.at(i));
                temp_hash_data.hash_resource_type = hash_resource_types.at(files_index.at(i));
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
                    uint32_t resized_size_in_memory = static_cast<uint32_t>((double) input_file_size *
                                                                            ((double) meta_data.data.resource.size_in_memory /
                                                                             (double) meta_data.data
                                                                                     .resource.size_final));
                    uint32_t resized_difference_value = meta_data.data.resource.size_in_memory - static_cast<uint32_t>(
                            (double) meta_data.data.resource.size_final *
                            ((double) meta_data.data.resource.size_in_memory / (
                                    double) meta_data.data.resource.size_final));

                    resized_size_in_memory += resized_difference_value;

                    temp_hash_data.data.resource.size_in_memory = resized_size_in_memory;
                    temp_hash_data.data.resource.size_in_video_memory = meta_data.data.resource.size_in_video_memory;
                }
            } else {
                uint64_t hash_value = std::strtoll(hash_strings.at(files_index.at(i)).c_str(), nullptr, 16);
                temp_hash_data.hash_value = hash_value;
                //temp_hash_data.hash_string = hash_strings.at(files_index.at(i));
                temp_hash_data.hash_resource_type = hash_resource_types.at(files_index.at(i));
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

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    ss.str(std::string());

    ss << message << "100% Done in "
       << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = static_cast<uint32_t>(100);

    if (!found_all_meta_files) {
        LOG("Some hash files did not have meta files.");
        LOG("When a hash file does not have a meta file the defaults used are:");
        LOG("  - The hash file is XORed and LZ4ed");
        LOG("  - The hash file's ref table is set to 0, meaning the hash file's");
        LOG("    reference data linking to other hashes will not be included.");
    }

    temp_file_output.close();

    uint32_t hash_count = static_cast<uint32_t>(temp_rpkg_data.hash.size());
    uint32_t table_offset = hash_count * 0x14;
    uint32_t table_size = 0x0;
    uint64_t hash_offset = 0x0;

    table_size = 0x0;

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
        for (char& j : rpkg_meta_data.rpkgv2_header) {
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

    if (rpkg_meta_data.rpkg_file_version == 1) {
        message = "Writing RPKGv1 offset index: ";
    } else {
        message = "Writing RPKGv2 offset index: ";
    }

    start_time = std::chrono::high_resolution_clock::now();
    stringstream_length = 80;

    for (uint64_t i = 0; i < temp_rpkg_data.hash.size(); i++) {
        if (((i * static_cast<uint64_t>(100000)) / (uint64_t) temp_rpkg_data.hash.size()) %
            static_cast<uint64_t>(100) == 0 && i > 0) {
            stringstream_length = console::update_console(message, temp_rpkg_data.hash.size(), i, start_time,
                                                          stringstream_length);
        }

        std::memcpy(&char8, &temp_rpkg_data.hash.at(i).hash_value, sizeof(uint64_t));
        file.write(char8, sizeof(uint64_t));
        std::memcpy(&char8, &hash_offset, sizeof(uint64_t));
        file.write(char8, sizeof(uint64_t));

        std::memcpy(&char4, &temp_rpkg_data.hash.at(i).data.header.data_size, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));

        if (temp_rpkg_data.hash.at(i).data.lz4ed) {
            hash_offset += temp_rpkg_data.hash.at(i).data.header.data_size & 0x3FFFFFFF;
        } else {
            hash_offset += temp_rpkg_data.hash.at(i).data.resource.size_final;
        }
    }

    end_time = std::chrono::high_resolution_clock::now();

    ss.str(std::string());

    ss << message << "100% Done in "
       << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    timing_string = ss.str();

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = static_cast<uint32_t>(100);

    if (rpkg_meta_data.rpkg_file_version == 1) {
        message = "Writing RPKGv1 info index: ";
    } else {
        message = "Writing RPKGv2 info index: ";
    }

    start_time = std::chrono::high_resolution_clock::now();
    stringstream_length = 80;

    for (uint64_t i = 0; i < temp_rpkg_data.hash.size(); i++) {
        if (((i * static_cast<uint64_t>(100000)) / temp_rpkg_data.hash.size()) % static_cast<uint64_t>(100) == 0 &&
            i > 0) {
            stringstream_length = console::update_console(message, temp_rpkg_data.hash.size(), i, start_time,
                                                          stringstream_length);
        }

        file.write(&temp_rpkg_data.hash.at(i).hash_resource_type[3], 0x1);
        file.write(&temp_rpkg_data.hash.at(i).hash_resource_type[2], 0x1);
        file.write(&temp_rpkg_data.hash.at(i).hash_resource_type[1], 0x1);
        file.write(&temp_rpkg_data.hash.at(i).hash_resource_type[0], 0x1);

        std::memcpy(&char4, &temp_rpkg_data.hash.at(i).data.resource.reference_table_size, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &temp_rpkg_data.hash.at(i).data.resource.reference_table_dummy, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &temp_rpkg_data.hash.at(i).data.resource.size_final, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &temp_rpkg_data.hash.at(i).data.resource.size_in_memory, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &temp_rpkg_data.hash.at(i).data.resource.size_in_video_memory, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));

        uint32_t temp_hash_reference_count =
                temp_rpkg_data.hash.at(i).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        if (temp_rpkg_data.hash.at(i).data.resource.reference_table_size > 0) {
            std::memcpy(&char4, &temp_rpkg_data.hash.at(i).hash_reference_data.hash_reference_count, sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));
        }

        if (temp_hash_reference_count > 0) {
            for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
                std::memcpy(&char1, &temp_rpkg_data.hash.at(i).hash_reference_data.hash_reference_type.at(j),
                            sizeof(uint8_t));
                file.write(char1, sizeof(uint8_t));
            }

            for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
                std::memcpy(&char8, &temp_rpkg_data.hash.at(i).hash_reference_data.hash_reference.at(j),
                            sizeof(uint64_t));
                file.write(char8, sizeof(uint64_t));
            }
        }
    }

    end_time = std::chrono::high_resolution_clock::now();

    ss.str(std::string());

    ss << message << "100% Done in "
       << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    timing_string = ss.str();

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    std::ifstream temp_file_input = std::ifstream(temp_file_name, std::ifstream::binary);

    if (!temp_file_input.good()) {
        LOG_AND_EXIT("Error: Temporary file " + temp_file_name + " could not be read.");
    }

    if (rpkg_meta_data.rpkg_file_version == 1) {
        LOG("Merging RPKGv1 file and temporary RPKG files...");
    } else {
        LOG("Merging RPKGv2 file and temporary RPKG files...");
    }

    start_time = std::chrono::high_resolution_clock::now();

    file << temp_file_input.rdbuf();

    end_time = std::chrono::high_resolution_clock::now();

    if (rpkg_meta_data.rpkg_file_version == 1) {
        LOG("Merged RPKGv1 file and temporary RPKGv1 file in " << (0.000000001 *
                                                                   std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                                           end_time - start_time).count()) << "s");
    } else {
        LOG("Merged RPKGv2 file and temporary RPKGv2 file in " << (0.000000001 *
                                                                   std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                                           end_time - start_time).count()) << "s");
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
