#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include <lz4.h>
#include <nlohmann/json.hpp>
#include <set>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <TonyTools/Languages.h>

using json = nlohmann::ordered_json;

void rpkg_function::extract_rtlv_to_json_from(std::string& input_path, std::string& filter, std::string& output_path,
                                              bool output_to_string, std::string version) {
    TonyTools::Language::Version ttVersion = TonyTools::Language::Version::H3;
    if (version == "HM2") {
        ttVersion = TonyTools::Language::Version::H2;
    }
    else if (version == "HM2016") {
        ttVersion = TonyTools::Language::Version::H2016;
    }

    util::load_hmla_hash_list();

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

    if (output_to_string) {
        localization_string = "";
    } else {
        file::create_directories(file::output_path_append("RTLV", output_path));
    }

    std::vector<std::string> filters = util::parse_input_filter(filter);

    uint64_t rtlv_count = 0;
    uint64_t rtlv_current_count = 0;

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) == "RTLV") {
                rtlv_count += rpkg.hashes_indexes_based_on_resource_types.at(r).size();
            }
        }
    }

    timing_string = "Extracting RTLV as JSON...";

    if (log_output)
            LOG("Extracting RTLV as JSON...");

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    for (uint32_t i = 0; i < rpkgs.size(); i++) {
        auto& rpkg = rpkgs.at(i);

        if (rpkg.rpkg_file_path != input_path && input_path_is_rpkg_file)
            continue;

        bool archive_folder_created = false;

        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "RTLV")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                             rpkg.hash.at(hash_index).hash_resource_type;

                bool found = false;

                for (const auto & filter : filters) {
                    std::size_t found_position = hash_file_name.find(filter);

                    if (found_position != std::string::npos && !filter.empty()) {
                        found = true;

                        break;
                    }
                }

                if (found || filter.empty()) {
                    std::string message = "Extracting RTLV as JSON: ";

                    if (((rtlv_current_count * static_cast<uint64_t>(100000)) / rtlv_count) %
                        static_cast<uint64_t>(100) == 0 && rtlv_current_count > 0 && !output_to_string) {
                        stringstream_length = console::update_console(message, rtlv_count, rtlv_current_count,
                                                                      start_time, stringstream_length);
                    }

                    rtlv_current_count++;

                    std::string current_path = file::output_path_append("RTLV\\" + rpkg.rpkg_file_name, output_path);

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
                        crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                    }

                    uint32_t decompressed_size = rpkg.hash.at(hash_index).data.resource.size_final;

                    std::vector<char> rtlv_data(decompressed_size, 0);

                    if (rpkg.hash.at(hash_index).data.lz4ed) {
                        LZ4_decompress_safe(input_data.data(), rtlv_data.data(), static_cast<int>(hash_size),
                                            decompressed_size);
                    } else {
                        rtlv_data = input_data;
                    }

                    std::string rtlvJson = TonyTools::Language::RTLV::Convert(
                        ttVersion,
                        rtlv_data,
                        generate_hash_meta_json(i, hash_index)
                    );

                    if (rtlvJson.empty()) {
                        LOG_AND_EXIT("Failed to convert RTLV to JSON!");
                    }

                    json parsedJson = json::parse(rtlvJson);

                    if (output_to_string) {
                        std::stringstream ss;

                        ss << std::setw(4) << parsedJson << std::endl;

                        localization_string = ss.str();

                        localization_json = parsedJson;
                    } else {
                        std::string json_path = current_path + "\\" + hash_file_name + ".JSON";

                        std::ofstream json_file = std::ofstream(json_path, std::ofstream::binary);

                        if (!json_file.good()) {
                            LOG_AND_EXIT("Error: JSON file " + json_path + " could not be created.");
                        }

                        json_file << std::setw(4) << parsedJson << std::endl;

                        json_file.close();
                    }
                }
            }
        }
    }

    ss.str(std::string());

    ss << "Extracting RTLV as JSON: Done";

    if (log_output)
            LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    if (!output_to_string) {
        percent_progress = static_cast<uint32_t>(100);

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
}
