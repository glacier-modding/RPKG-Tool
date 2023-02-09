#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

void rpkg_function::extract_prel_refs(std::string& input_path) {
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

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(true);
        LOG("Loading Hash List: Done");
    }

    if (!input_path_is_rpkg_file) {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

    for (auto& rpkg : rpkgs) {
        if (rpkg.rpkg_file_path != input_path && input_path_is_rpkg_file)
            continue;

        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "PREL")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                             rpkg.hash.at(hash_index).hash_resource_type;

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
                    crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                }

                uint32_t decompressed_size = rpkg.hash.at(hash_index).data.resource.size_final;

                std::vector<char> output_data(decompressed_size, 0);

                std::vector<char>* prel_data;

                if (rpkg.hash.at(hash_index).data.lz4ed) {
                    LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                        decompressed_size);

                    prel_data = &output_data;
                } else {
                    prel_data = &input_data;
                }

                uint64_t position = 0;
                uint64_t master_hash_value = 0;
                uint64_t master_hash_depends_count = 0;
                std::vector<uint64_t> master_hash_depends;

                std::memcpy(&master_hash_value, &prel_data->data()[position], sizeof(uint64_t));
                position += 0x8;

                std::memcpy(&master_hash_depends_count, &prel_data->data()[position], sizeof(uint64_t));
                position += 0x8;

                std::cout << std::endl << std::endl << "PREL file name: " << hash_file_name << std::endl;

                std::cout << "Master hash: " << util::uint64_t_to_hex_string(master_hash_value) << std::endl;

                std::string ioi_string = "";

                auto it = hash_list_hash_map.find(master_hash_value);

                if (it != hash_list_hash_map.end()) {
                    ioi_string = hash_list_hash_strings.at(it->second);
                }

                std::cout << "  - IOI String: " << ioi_string << std::endl << std::endl;

                for (uint64_t k = 0; k < master_hash_depends_count; k++) {
                    uint64_t temp_master_hash_depend_hash_value = 0;

                    std::memcpy(&temp_master_hash_depend_hash_value, &prel_data->data()[position], sizeof(uint64_t));
                    position += 0x8;

                    master_hash_depends.push_back(temp_master_hash_depend_hash_value);

                    std::cout << "Master hash depend (" << k << "): "
                              << util::uint64_t_to_hex_string(master_hash_depends.back()) << std::endl;

                    std::string ioi_string = "";

                    auto it2 = hash_list_hash_map.find(master_hash_depends.back());

                    if (it2 != hash_list_hash_map.end()) {
                        ioi_string = hash_list_hash_strings.at(it2->second);
                    }

                    std::cout << "  - IOI String: " << ioi_string << std::endl;
                }
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
