#include "rpkg_function.h"
#include "file.h"
#include "util.h"
#include "global.h"
#include "crypto.h"
#include "thirdparty/lz4/lz4.h"
#include <filesystem>
#include <sstream>
#include <fstream>

constexpr char bin1_header[] = {0x42, 0x49, 0x4E, 0x31, 0x00, 0x08, 0x01, 0x00};

void rpkg_function::extract_gfxf_from(std::string& input_path, std::string& filter, std::string& output_path) {
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

    file::create_directories(file::output_path_append("GFXF", output_path));

    std::vector<std::string> filters = util::parse_input_filter(filter);

    bool extract_single_hash = false;

    if (filters.size() == 1) {
        if (util::is_valid_hash(filters.at(0))) {
            extract_single_hash = true;
        }
    }

    uint64_t gfxf_count = 0;
    uint64_t gfxf_hash_size_total = 0;

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "GFXF")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                             rpkg.hash.at(hash_index).hash_resource_type;

                gfxf_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;

                gfxf_count++;
            }
        }
    }

    LOG("Scanning RPKGs for GFXF files: Done");

    if (!filter.empty()) {
        LOG("Extracting GFXF files with filter \"" << filter << "\"");
    }

    uint64_t gfxf_count_current = 0;
    uint64_t gfxf_hash_size_current = 0;

    std::vector<std::string> found_in;
    std::vector<std::string> not_found_in;

    for (uint64_t z = 0; z < filters.size(); z++) {
        found_in.emplace_back("");

        not_found_in.emplace_back("");
    }

    for (uint64_t i = 0; i < rpkgs.size(); i++) {
        std::vector<bool> extracted;

        for (uint64_t z = 0; z < filters.size(); z++) {
            extracted.push_back(false);
        }

        if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file) {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++) {
                if (rpkgs.at(i).hash_resource_types.at(r) != "GFXF")
                    continue;

                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    if (gui_control == ABORT_CURRENT_TASK) {
                        return;
                    }

                    std::string hash_file_name =
                            util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." +
                            rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    gfxf_hash_size_current += rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                    gfxf_count_current++;

                    if (!extract_single_hash || (extract_single_hash && filter == util::uint64_t_to_hex_string(
                            rpkgs.at(i).hash.at(hash_index).hash_value))) {
                        std::string current_path = file::output_path_append("GFXF\\" + rpkgs.at(i).rpkg_file_name,
                                                                            output_path);

                        uint64_t hash_size;

                        if (rpkgs.at(i).hash.at(hash_index).data.lz4ed) {
                            hash_size = rpkgs.at(i).hash.at(hash_index).data.header.data_size;

                            if (rpkgs.at(i).hash.at(hash_index).data.xored) {
                                hash_size &= 0x3FFFFFFF;
                            }
                        } else {
                            hash_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;
                        }

                        std::vector<char> input_data(hash_size, 0);

                        std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                        if (!file.good()) {
                            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkgs.at(i).hash.at(hash_index).data.header.data_offset, std::ifstream::beg);
                        file.read(input_data.data(), hash_size);
                        file.close();

                        if (rpkgs.at(i).hash.at(hash_index).data.xored) {
                            crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                        }

                        uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                        std::vector<char> output_data(decompressed_size, 0);

                        std::vector<char>* gfxf_data;

                        if (rpkgs.at(i).hash.at(hash_index).data.lz4ed) {
                            LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                                decompressed_size);

                            gfxf_data = &output_data;
                        } else {
                            gfxf_data = &input_data;
                        }

                        uint64_t position = 0;

                        std::vector<char> gfxf_meta_data;

                        bool is_valid_bin1_header = true;

                        for (uint64_t k = 0; k < sizeof(bin1_header); k++) {
                            if ((*gfxf_data)[k] != bin1_header[k]) {
                                is_valid_bin1_header = false;
                            }

                            position++;
                        }

                        if (!is_valid_bin1_header) {
                            LOG(hash_file_name + " does not have a valid BIN1 IOI header.");
                        } else {
                            bool gfxf_contains_dds = false;

                            char input[1024];
                            char char4[4];
                            uint32_t bytes4 = 0;
                            uint64_t bytes8 = 0;

                            uint32_t gfxf_file_length = 0;
                            uint32_t gfx_file_offset = 0;
                            uint32_t gfx_file_length = 0;
                            uint32_t dds_names_offset_start = 0;
                            uint32_t dds_names_offset_end = 0;
                            uint32_t dds_data_offset_start = 0;
                            uint32_t dds_data_offset_end = 0;

                            std::memcpy(&input, &(*gfxf_data)[position], BYTES4);
                            position += 0x4;

                            char4[0] = input[3];
                            char4[1] = input[2];
                            char4[2] = input[1];
                            char4[3] = input[0];

                            //std::cout << char4 << std::endl;

                            std::memcpy(&gfxf_file_length, &char4[0], BYTES4);

                            //std::cout << std::hex << gfxf_file_length << std::endl;

                            std::memcpy(&bytes4, &(*gfxf_data)[position], BYTES4);
                            position += 0x4;

                            std::memcpy(&gfx_file_offset, &(*gfxf_data)[position], BYTES4);
                            gfx_file_offset += 0x10;
                            position += 0x4;

                            std::memcpy(&bytes4, &(*gfxf_data)[position], BYTES4);
                            position += 0x4;

                            std::memcpy(&gfx_file_length, &(*gfxf_data)[position], BYTES4);
                            position += 0x4;

                            std::memcpy(&bytes4, &(*gfxf_data)[position], BYTES4);
                            position += 0x4;

                            //std::cout << rpkgs.at(i).rpkg_file_name << "," << hash_file_name << std::endl;

                            std::memcpy(&bytes8, &(*gfxf_data)[position], BYTES8);

                            if (bytes8 != 0xFFFFFFFFFFFFFFFF) {
                                gfxf_contains_dds = true;
                            }

                            if (!gfxf_contains_dds) {
                                position += 0x30;
                            }

                            if (gfxf_contains_dds) {
                                //std::cout << "GFXF contains DDS files." << std::endl;

                                std::memcpy(&dds_names_offset_start, &(*gfxf_data)[position], BYTES4);
                                dds_names_offset_start += 0x10;
                                position += 0x8;

                                std::memcpy(&dds_names_offset_end, &(*gfxf_data)[position], BYTES4);
                                dds_names_offset_end += 0x10;
                                position += 0x8;

                                std::memcpy(&dds_names_offset_end, &(*gfxf_data)[position], BYTES4);
                                dds_names_offset_end += 0x10;
                                position += 0x8;

                                std::memcpy(&dds_data_offset_start, &(*gfxf_data)[position], BYTES4);
                                dds_data_offset_start += 0x10;
                                position += 0x8;

                                std::memcpy(&dds_data_offset_end, &(*gfxf_data)[position], BYTES4);
                                dds_data_offset_end += 0x10;
                                position += 0x8;

                                std::memcpy(&dds_data_offset_end, &(*gfxf_data)[position], BYTES4);
                                dds_data_offset_end += 0x10;
                                position += 0x8;
                            }

                            std::memcpy(&bytes4, &(*gfxf_data)[position], BYTES4);
                            position += 0x4;

                            //std::cout << bytes4 << std::endl;
                            //std::cout << gfx_file_length << std::endl;

                            if (bytes4 != gfx_file_length) {
                                LOG(hash_file_name + " does not have a valid BIN1 IOI header.");
                            }

                            std::vector<char> gfx_data;

                            for (uint64_t k = 0; k < gfx_file_length; k++) {
                                gfx_data.push_back((*gfxf_data)[position]);

                                position++;
                            }

                            std::string gfxf_output_dir = file::output_path_append(
                                    "GFXF\\" + rpkgs.at(i).rpkg_file_name + "\\" + hash_file_name, output_path);

                            file::create_directories(gfxf_output_dir);

                            std::ofstream gfx_output_file = std::ofstream(
                                    gfxf_output_dir + "\\" + hash_file_name + ".GFX", std::ofstream::binary);

                            if (!gfx_output_file.good()) {
                                LOG_AND_EXIT("Error: DDS file " + gfxf_output_dir + "\\" + hash_file_name + ".GFX" +
                                             " could not be created.");
                            }

                            gfx_output_file.write(gfx_data.data(), gfx_file_length);

                            gfx_output_file.close();

                            if (gfxf_contains_dds) {
                                uint32_t dds_names_count = 0;
                                std::vector<std::string> dds_names;
                                std::vector<uint32_t> dds_names_lengths;

                                while (((position & 0xF) != 0x4) && ((position & 0xF) != 0xC)) {
                                    position++;
                                }

                                std::memcpy(&dds_names_count, &(*gfxf_data)[position], BYTES4);

                                for (uint64_t k = 0; k < BYTES4; k++) {
                                    gfxf_meta_data.push_back((*gfxf_data)[position + k]);
                                }

                                position += 0x4;

                                //std::cout << std::hex << dds_names_count << std::endl;

                                for (uint64_t d = 0; d < dds_names_count; d++) {
                                    uint32_t dds_name_length = 0;
                                    uint32_t dds_name_offset = 0;

                                    std::memcpy(&dds_name_length, &(*gfxf_data)[position], BYTES4);
                                    dds_name_length &= 0xBFFFFFFF;

                                    std::memcpy(&input, &dds_name_length, BYTES4);

                                    for (uint64_t k = 0; k < BYTES4; k++) {
                                        gfxf_meta_data.push_back(input[k]);
                                    }

                                    position += 0x8;

                                    //std::cout << std::hex << dds_name_length << std::endl;

                                    std::memcpy(&dds_name_offset, &(*gfxf_data)[position], BYTES4);
                                    dds_name_offset += 0x10;
                                    position += 0x8;

                                    //std::cout << std::hex << dds_name_offset << std::endl;

                                    std::memcpy(&input, &(*gfxf_data)[dds_name_offset],
                                                (static_cast<uint64_t>(dds_name_length) + static_cast<uint64_t>(1)));

                                    for (uint64_t k = 0; k < static_cast<uint64_t>(dds_name_length); k++) {
                                        gfxf_meta_data.push_back((*gfxf_data)[dds_name_offset + k]);
                                    }

                                    dds_names.emplace_back(input);

                                    //std::cout << dds_names.back() << std::endl;
                                }

                                position = dds_data_offset_start;

                                for (uint64_t d = 0; d < dds_names_count; d++) {
                                    uint32_t temp_dds_data_length = 0;
                                    uint32_t temp_dds_data_offset_start = 0;
                                    uint32_t temp_dds_data_offset_end = 0;

                                    std::memcpy(&temp_dds_data_offset_start, &(*gfxf_data)[position],
                                                BYTES4);
                                    temp_dds_data_offset_start += 0x10;
                                    position += 0x8;

                                    std::memcpy(&temp_dds_data_offset_end, &(*gfxf_data)[position],
                                                BYTES4);
                                    temp_dds_data_offset_end += 0x10;
                                    position += 0x8;

                                    std::memcpy(&temp_dds_data_offset_end, &(*gfxf_data)[position],
                                                BYTES4);
                                    temp_dds_data_offset_end += 0x10;
                                    position += 0x8;

                                    temp_dds_data_length = temp_dds_data_offset_end - temp_dds_data_offset_start;

                                    //std::cout << std::hex << temp_dds_data_length << std::endl;

                                    //std::cout << dds_names.at(d) << "," << temp_dds_data_offset_start << "," << temp_dds_data_offset_end << "," << temp_dds_data_length << std::endl;

                                    std::ofstream output_file = std::ofstream(gfxf_output_dir + "\\" + dds_names.at(d),
                                                                              std::ofstream::binary);

                                    if (!output_file.good()) {
                                        LOG_AND_EXIT("Error: DDS file " + gfxf_output_dir + "\\" + dds_names.at(d) +
                                                     " could not be created.");
                                    }

                                    output_file.write(&(*gfxf_data)[temp_dds_data_offset_start],
                                                      temp_dds_data_length);

                                    output_file.close();

                                    if (d == (dds_names_count - 1)) {
                                        position = temp_dds_data_offset_end;
                                    }
                                }
                            } else {
                                gfxf_meta_data.push_back(0x0);
                                gfxf_meta_data.push_back(0x0);
                                gfxf_meta_data.push_back(0x0);
                                gfxf_meta_data.push_back(0x0);
                            }

                            //std::cout << std::hex << position << std::endl;

                            std::ofstream gfx_meta_output_file = std::ofstream(gfxf_output_dir + "\\" + "meta",
                                                                               std::ofstream::binary);

                            if (!gfx_meta_output_file.good()) {
                                LOG_AND_EXIT("Error: DDS file " + gfxf_output_dir + "\\" + "meta" +
                                             " could not be created.");
                            }

                            if ((decompressed_size - position) > 0) {
                                for (uint64_t k = 0; k < (decompressed_size - position); k++) {
                                    gfxf_meta_data.push_back((*gfxf_data)[position + k]);
                                }
                            }

                            gfx_meta_output_file.write(gfxf_meta_data.data(), gfxf_meta_data.size());

                            gfx_meta_output_file.close();

                            std::string final_path = gfxf_output_dir + "\\" + util::uint64_t_to_hex_string(
                                    rpkgs.at(i).hash.at(hash_index).hash_value) + "." +
                                                     rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                            rpkg_function::extract_hash_meta(i, hash_index, final_path);
                        }
                    }
                }
            }
        }

        if (filter.empty())
            continue;

        for (uint64_t z = 0; z < filters.size(); z++) {
            if (extracted.at(z)) {
                if (found_in.at(z).empty()) {
                    found_in.at(z).append(rpkgs.at(i).rpkg_file_name);
                } else {
                    found_in.at(z).append(", " + rpkgs.at(i).rpkg_file_name);
                }
            } else {
                if (not_found_in.at(z).empty()) {
                    not_found_in.at(z).append(rpkgs.at(i).rpkg_file_name);
                } else {
                    not_found_in.at(z).append(", " + rpkgs.at(i).rpkg_file_name);
                }
            }
        }
    }

    LOG("Done");

    percent_progress = static_cast<uint32_t>(100);

    if (!filter.empty()) {
        for (uint64_t z = 0; z < filters.size(); z++) {
            LOG(std::endl << "\"" << filters.at(z) << "\" was found in and extracted from: " << found_in.at(z));

            LOG(std::endl << "\"" << filters.at(z) << "\" was not found in RPKG file(s): " << not_found_in.at(z));
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
