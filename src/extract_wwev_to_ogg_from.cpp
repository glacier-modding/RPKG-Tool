#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include <lz4.h>
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void rpkg_function::extract_wwev_to_ogg_from(std::string& input_path, std::string& filter, std::string& output_path) {
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

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    file::create_directories(file::output_path_append("WWEV", output_path));

    std::vector<std::string> filters = util::parse_input_filter(filter);

    bool extract_single_hash = false;

    if (filters.size() == 1) {
        if (util::is_valid_hash(filters.at(0))) {
            extract_single_hash = true;
        }
    }

    uint64_t wwev_count = 0;
    uint64_t wwev_hash_size_total = 0;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "WWEV")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                             rpkg.hash.at(hash_index).hash_resource_type;

                std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                double time_in_seconds_from_start_time = (0.000000001 *
                                                          std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                                  end_time - start_time).count());

                if (time_in_seconds_from_start_time > console_update_rate) {
                    start_time = end_time;

                    if (period_count > 3) {
                        period_count = 0;
                    }

                    std::stringstream ss;

                    ss << "Scanning RPKGs for WWEV files" << std::string(period_count, '.');

                    timing_string = ss.str();

                    LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                    period_count++;
                }

                wwev_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;

                wwev_count++;
            }
        }
    }

    ss.str(std::string());

    ss << "Scanning RPKGs for WWEV files: Done";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    std::string message = "Extracting WWEV files: ";

    if (filter != "") {
        LOG("Extracting WWEV to *.wem/*.ogg files with filter \"" << filter << "\"");
    }

    uint64_t wwev_count_current = 0;
    uint64_t wwev_hash_size_current = 0;

    std::vector<std::string> found_in;
    std::vector<std::string> not_found_in;

    for (uint64_t z = 0; z < filters.size(); z++) {
        found_in.push_back("");

        not_found_in.push_back("");
    }

    for (uint64_t i = 0; i < rpkgs.size(); i++) {
        bool archive_folder_created = false;

        std::vector<bool> extracted;

        for (uint64_t z = 0; z < filters.size(); z++) {
            extracted.push_back(false);
        }

        if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file) {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++) {
                if (rpkgs.at(i).hash_resource_types.at(r) != "WWEV")
                    continue;

                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    if (gui_control == ABORT_CURRENT_TASK) {
                        return;
                    }

                    std::string hash_file_name =
                            util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." +
                            rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    if (((wwev_count_current * static_cast<uint64_t>(100000)) / (uint64_t) wwev_count) %
                        static_cast<uint64_t>(10) == 0 && wwev_count_current > 0) {
                        stringstream_length = console::update_console(message, wwev_hash_size_total,
                                                                      wwev_hash_size_current, start_time,
                                                                      stringstream_length);
                    }

                    wwev_hash_size_current += rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                    wwev_count_current++;

                    if (extract_single_hash && (!extract_single_hash || filter != util::uint64_t_to_hex_string(
                            rpkgs.at(i).hash.at(hash_index).hash_value)))
                        continue;

                    std::string current_path = file::output_path_append("WWEV\\" + rpkgs.at(i).rpkg_file_name,
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

                    file.seekg(rpkgs.at(i).hash.at(hash_index).data.header.data_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkgs.at(i).hash.at(hash_index).data.xored) {
                        crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                    }

                    uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* wwev_data;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed) {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                            decompressed_size);

                        wwev_data = &output_data;
                    } else {
                        wwev_data = &input_data;
                    }

                    uint32_t wwev_file_name_length = 0;
                    uint32_t wwev_file_count = 0;
                    uint32_t wwev_file_count_test = 0;

                    uint32_t position = 0;

                    char input[1024];
                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    std::memcpy(&wwev_file_name_length, &wwev_data->data()[position], sizeof(bytes4));

                    std::vector<char> wwev_meta_data;

                    char hash[8];

                    std::memcpy(&hash, &rpkgs.at(i).hash.at(hash_index).hash_value, 0x8);

                    for (char& k : hash) {
                        wwev_meta_data.push_back(k);
                    }

                    std::memcpy(&input, &wwev_data->data()[position],
                                (wwev_file_name_length + static_cast<uint64_t>(0xC)));
                    for (uint64_t k = 0; k < (wwev_file_name_length + static_cast<uint64_t>(0xC)); k++) {
                        wwev_meta_data.push_back(input[k]);
                    }

                    position += 0x4;

                    std::vector<char> wwev_file_name(
                            static_cast<uint64_t>(wwev_file_name_length) + static_cast<uint64_t>(1), 0);
                    wwev_file_name[wwev_file_name_length] = 0;

                    std::memcpy(wwev_file_name.data(), &wwev_data->data()[position], wwev_file_name_length);
                    position += wwev_file_name_length;
                    position += 0x4;

                    std::memcpy(&wwev_file_count, &wwev_data->data()[position], sizeof(bytes4));
                    position += 0x4;

                    std::memcpy(&wwev_file_count_test, &wwev_data->data()[position], sizeof(bytes4));

                    if (wwev_file_count == -1 || wwev_file_count == 1 && wwev_file_count_test < 5000) {
                        // Hitman 2016 file format has an extra int32 here
                        // The 2000 in the check is a bit arbitrary, but I don't know anything better to test.
                        // Will give false negatives (2016 wwev detected as newer version) if there are more than 5000 embedded sounds
                        // Will give false positives (newer wwev detected as 2016 version) if there is exactly one embedded sound with a wem id lower than 5000
                        // -grappigegovert
                        wwev_file_count = wwev_file_count_test;
                        position += 4;
                    }

                    bool found = false;

                    uint64_t input_filter_index = 0;

                    for (uint64_t z = 0; z < filters.size(); z++) {
                        std::size_t found_position_hash = hash_file_name.find(filters.at(z));

                        std::size_t found_position_wwev = util::to_upper_case(std::string(wwev_file_name.data())).find(
                                filters.at(z));

                        if ((found_position_hash != std::string::npos && !filters.at(z).empty()) ||
                            (found_position_wwev != std::string::npos && !filters.at(z).empty())) {
                            found = true;

                            input_filter_index = z;

                            break;
                        }
                    }

                    if (!found && !filter.empty())
                        continue;

                    if (!filters.empty()) {
                        extracted.at(input_filter_index) = true;
                    }

                    if (!archive_folder_created) {
                        file::create_directories(current_path);

                        archive_folder_created = true;
                    }

                    current_path.append("\\");
                    current_path.append(std::string(wwev_file_name.data()));

                    file::create_directories(current_path);

                    std::string wem_path = current_path + "\\wem";

                    file::create_directories(wem_path);

                    std::string ogg_path = current_path + "\\ogg";

                    file::create_directories(ogg_path);

                    std::ofstream wwev_meta_data_file = std::ofstream(current_path + "\\" + "meta",
                                                                      std::ofstream::binary);

                    if (!wwev_meta_data_file.good()) {
                        LOG_AND_EXIT("Error: meta file " + current_path + "\\" + "meta" + " could not be created.");
                    }

                    if (wwev_file_count > 0) {
                        for (uint64_t k = 0; k < wwev_file_count; k++) {
                            std::memcpy(&input, &wwev_data->data()[position], 0x8);
                            for (uint64_t l = 0; l < 0x8; l++) {
                                wwev_meta_data.push_back(input[l]);
                            }

                            position += 0x4;

                            uint32_t wem_size;

                            std::memcpy(&wem_size, &wwev_data->data()[position], sizeof(bytes4));
                            position += 0x4;

                            std::vector<char> wwev_file_data(wem_size, 0);

                            std::memcpy(wwev_file_data.data(), &wwev_data->data()[position], wem_size);
                            position += wem_size;

                            std::string wem_file = wem_path + "\\" + std::to_string(k) + ".wem";

                            std::ofstream output_file = std::ofstream(wem_file, std::ofstream::binary);

                            if (!output_file.good()) {
                                LOG_AND_EXIT("Error: wem file " + wem_file + " could not be created.");
                            }

                            output_file.write(wwev_file_data.data(), wem_size);

                            output_file.close();

                            if (!file::path_exists("packed_codebooks_aoTuV_603.bin")) {
                                LOG("Error: Missing packed_codebooks_aoTuV_603.bin file.");
                                LOG("       Attempting to create the packed_codebooks_aoTuV_603.bin file.");

                                output_file = std::ofstream("packed_codebooks_aoTuV_603.bin", std::ofstream::binary);

                                if (!output_file.good()) {
                                    LOG_AND_EXIT(
                                            "Error: packed_codebooks_aoTuV_603.bin file packed_codebooks_aoTuV_603.bin could not be created.");
                                }

                                output_file.write((const char*) codebook, sizeof(codebook));
                            }

                            std::string ogg_file = ogg_path + "\\" + std::to_string(k) + ".ogg";

                            output_file = std::ofstream(ogg_file, std::ofstream::binary);

                            if (!output_file.good()) {
                                LOG_AND_EXIT("Error: ogg file " + ogg_file + " could not be created.");
                            }

                            try {
                                Wwise_RIFF_Vorbis ww(wem_file, "packed_codebooks_aoTuV_603.bin", false, false,
                                                     kNoForcePacketFormat);

                                ww.generate_ogg(output_file);
                            }
                            catch (const Parse_error& pe) {
                                LOG("WWEV resource found: " << hash_file_name << " in RPKG file: "
                                                            << rpkgs.at(i).rpkg_file_name);
                                LOG("Error parsing ogg file " << wem_file << " could not be created.");
                                LOG("Error: " << pe);
                            }

                            output_file.close();

                            revorb(ogg_file);
                        }
                    } else {
                        std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                        if (bytes4 == 0) {
                            std::memcpy(&input, &wwev_data->data()[position], 0x4);
                            for (uint64_t k = 0; k < 0x4; k++) {
                                wwev_meta_data.push_back(input[k]);
                            }
                        } else {
                            std::memcpy(&input, &wwev_data->data()[position], 0x4);
                            for (uint64_t k = 0; k < 0x4; k++) {
                                wwev_meta_data.push_back(input[k]);
                            }

                            std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                            position += 0x4;

                            for (uint64_t k = 0; k < bytes4; k++) {
                                std::memcpy(&input, &wwev_data->data()[position], 0x4);
                                for (uint64_t l = 0; l < 0x4; l++) {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                std::memcpy(&input, &wwev_data->data()[position], 0x4);
                                for (uint64_t l = 0; l < 0x4; l++) {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                uint32_t wem_size;

                                std::memcpy(&wem_size, &wwev_data->data()[position], sizeof(bytes4));

                                std::memcpy(&input, &wwev_data->data()[position], 0x4);
                                for (uint64_t l = 0; l < 0x4; l++) {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                position += wem_size;
                            }
                        }
                    }

                    if ((position + 0x4) <= decompressed_size) {
                        std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                        if (bytes4 == 0) {
                            std::memcpy(&input, &wwev_data->data()[position], 0x4);
                            for (uint64_t k = 0; k < 0x4; k++) {
                                wwev_meta_data.push_back(input[k]);
                            }
                        } else {
                            std::memcpy(&input, &wwev_data->data()[position], 0x4);
                            for (uint64_t k = 0; k < 0x4; k++) {
                                wwev_meta_data.push_back(input[k]);
                            }

                            std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                            position += 0x4;

                            for (uint64_t k = 0; k < bytes4; k++) {
                                std::memcpy(&input, &wwev_data->data()[position], 0x4);
                                for (uint64_t l = 0; l < 0x4; l++) {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                std::memcpy(&input, &wwev_data->data()[position], 0x4);
                                for (uint64_t l = 0; l < 0x4; l++) {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                uint32_t wem_size;

                                std::memcpy(&wem_size, &wwev_data->data()[position], sizeof(bytes4));

                                std::memcpy(&input, &wwev_data->data()[position], 0x4);
                                for (uint64_t l = 0; l < 0x4; l++) {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                position += wem_size;
                            }
                        }
                    }

                    wwev_meta_data_file.write(wwev_meta_data.data(), wwev_meta_data.size());

                    wwev_meta_data_file.close();

                    std::string final_path = current_path + "\\" +
                                             util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) +
                                             "." + rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    rpkg_function::extract_hash_meta(i, hash_index, final_path);
                }
            }
        }

        if (filter.empty())
            continue;

        for (uint64_t z = 0; z < filters.size(); z++) {
            if (extracted.at(z)) {
                if (found_in.at(z) == "") {
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

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    ss.str(std::string());

    ss << message << "100% Done in "
       << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

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
