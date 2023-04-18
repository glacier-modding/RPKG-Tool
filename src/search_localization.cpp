#include "rpkg_function.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <sstream>

void rpkg_function::search_localization(std::string& input_path, std::string& search, std::string& output_path,
                                        bool search_dlge, bool search_locr, bool search_rtlv, int max_results) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    percent_progress = (uint32_t) 0;

    localization_search_results = "";

    uint32_t results_count = 0;

    std::string search_lower_case = util::to_lower_case(search);

    uint32_t search_crc32_dec = std::strtoul(search.c_str(), nullptr, 10);

    uint32_t search_crc32_hex = std::strtoul(search.c_str(), nullptr, 16);

    uint64_t localization_hash_count = 0;
    uint64_t localization_hash_size_total = 0;

    for (auto& rpkg : rpkgs) {
        LOG("Searching " + rpkg.rpkg_file_name + "...");

        if (input_path.empty() || input_path == rpkg.rpkg_file_path) {
            for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                if (search_dlge && rpkg.hash_resource_types.at(r) == "DLGE") {
                    for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                        uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                        localization_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;
                        localization_hash_count++;
                    }
                }

                if (search_locr && rpkg.hash_resource_types.at(r) == "LOCR") {
                    for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                        uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                        localization_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;
                        localization_hash_count++;
                    }
                }

                if (search_rtlv && rpkg.hash_resource_types.at(r) == "RTLV") {
                    for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                        uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                        localization_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;
                        localization_hash_count++;
                    }
                }
            }
        }
    }

    uint64_t localization_hash_count_current = 0;
    uint64_t localization_hash_size_current = 0;

    for (auto& rpkg : rpkgs) {
        std::string message = "Searching " + rpkg.rpkg_file_name + "...";

        if (input_path.empty() || input_path == rpkg.rpkg_file_path) {
            for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                if (results_count < max_results) {
                    if (search_dlge && rpkg.hash_resource_types.at(r) == "DLGE") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            if (gui_control == ABORT_CURRENT_TASK) {
                                return;
                            }

                            if (results_count < max_results) {
                                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                                std::string temp_hash_string = util::uint64_t_to_hex_string(
                                        rpkg.hash.at(hash_index).hash_value);

                                localization_hash_size_current += rpkg.hash.at(hash_index).data.resource.size_final;
                                localization_hash_count_current++;

                                rpkg_function::extract_dlge_to_json_from(rpkg.rpkg_file_path, temp_hash_string,
                                                                         output_path, true);

                                try {
                                    for (const auto& it : localization_json.items()) {
                                        for (const auto& it2 : it.value().items()) {
                                            //std::cout << " RLTV3: " << it.key() << ", " << it.value() << std::endl;
                                            //std::cout << "SEARCHING lol2: " << it.key() << std::endl;
                                            if (it2.value().is_string()) {
                                                //std::cout << "SEARCHING s: " << it.key() << std::endl;
                                                if (util::to_lower_case(std::string(it2.value())).find(
                                                        search_lower_case) != std::string::npos) {
                                                    std::cout << "FOUND STRING: " << it2.key() << ", " << it2.value()
                                                              << " in " << util::uint64_t_to_hex_string(
                                                            rpkg.hash.at(hash_index).hash_value) + "." +
                                                                           rpkg.hash.at(hash_index).hash_resource_type
                                                              << std::endl;

                                                    localization_search_results += rpkg.rpkg_file_path + "||||" +
                                                                                   util::uint64_t_to_hex_string(
                                                                                           rpkg.hash.at(
                                                                                                   hash_index).hash_value) +
                                                                                   "." + rpkg.hash.at(
                                                            hash_index).hash_resource_type + " " +
                                                                                   util::hash_to_ioi_string(
                                                                                           rpkg.hash.at(
                                                                                                   hash_index).hash_value,
                                                                                           false) + "||||" +
                                                                                   std::string(it2.value()) + "||||||";

                                                    results_count++;
                                                }
                                            }
                                        }
                                    }
                                }
                                catch (json::parse_error& e) {
                                    std::stringstream ss;
                                    ss << "Error: " << "\n"
                                       << "Error message: " << e.what() << '\n'
                                       << "Error exception id: " << e.id << '\n'
                                       << "Error byte position of error: " << e.byte;
                                }
                            }
                        }
                    }

                    if (search_locr && rpkg.hash_resource_types.at(r) == "LOCR") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            if (results_count >= max_results) continue;

                            uint64_t hashIndex = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                            std::string tempHashString = util::uint64_t_to_hex_string(
                                    rpkg.hash.at(hashIndex).hash_value);

                            localization_hash_size_current += rpkg.hash.at(hashIndex).data.resource.size_final;
                            localization_hash_count_current++;

                            rpkg_function::extract_locr_to_json_from(rpkg.rpkg_file_path, tempHashString,
                                                                     output_path, true);

                            try {
                                for (const auto& it1 : localization_json.items()) {
                                    bool language_found = false;

                                    std::string language;

                                    for (const auto& it21 : it1.value().items()) {
                                        if (it21.value().contains("Language")) {
                                            language_found = true;

                                            language = it21.value()["Language"];
                                        }
                                    }

                                    if (language_found) {
                                        for (const auto& it22 : it1.value().items()) {
                                            if (it22.value().contains("StringHash") &&
                                                it22.value().contains("String")) {
                                                if ((uint32_t) it22.value()["StringHash"] == search_crc32_dec ||
                                                    (uint32_t) it22.value()["StringHash"] == search_crc32_hex) {
                                                    localization_search_results += rpkg.rpkg_file_path + "||||" +
                                                                                   util::uint64_t_to_hex_string(
                                                                                           rpkg.hash.at(
                                                                                                   hashIndex).hash_value) +
                                                                                   "." + rpkg.hash.at(
                                                            hashIndex).hash_resource_type + " " +
                                                                                   util::hash_to_ioi_string(
                                                                                           rpkg.hash.at(
                                                                                                   hashIndex).hash_value,
                                                                                           false) + "||||" +
                                                                                   language + ": " +
                                                                                   util::uint32_t_to_string(
                                                                                           it22.value()["StringHash"]) +
                                                                                   ": " + std::string(
                                                            it22.value()["String"]) + "||||||";

                                                    results_count++;
                                                } else if (util::to_lower_case(
                                                        std::string(it22.value()["String"])).find(
                                                        search_lower_case) != std::string::npos) {
                                                    localization_search_results += rpkg.rpkg_file_path + "||||" +
                                                                                   util::uint64_t_to_hex_string(
                                                                                           rpkg.hash.at(
                                                                                                   hashIndex).hash_value) +
                                                                                   "." + rpkg.hash.at(
                                                            hashIndex).hash_resource_type + " " +
                                                                                   util::hash_to_ioi_string(
                                                                                           rpkg.hash.at(
                                                                                                   hashIndex).hash_value,
                                                                                           false) + "||||" +
                                                                                   language + ": " +
                                                                                   util::uint32_t_to_string(
                                                                                           it22.value()["StringHash"]) +
                                                                                   ": " + std::string(
                                                            it22.value()["String"]) + "||||||";

                                                    results_count++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            catch (json::parse_error& e1) {
                                std::stringstream ss1;
                                ss1 << "Error: " << "\n"
                                    << "Error message: " << e1.what() << '\n'
                                    << "Error exception id: " << e1.id << '\n'
                                    << "Error byte position of error: " << e1.byte;
                            }
                        }
                    }

                    if (search_rtlv && rpkg.hash_resource_types.at(r) == "RTLV") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            if (results_count >= max_results) continue;

                            uint64_t hashIndex = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                            std::string tempHashString = util::uint64_t_to_hex_string(
                                    rpkg.hash.at(hashIndex).hash_value);

                            localization_hash_size_current += rpkg.hash.at(hashIndex).data.resource.size_final;
                            localization_hash_count_current++;

                            rpkg_function::extract_rtlv_to_json_from(rpkg.rpkg_file_path, tempHashString,
                                                                     output_path, true);

                            try {
                                for (const auto& it1 : localization_json.items()) {
                                    for (const auto& it21 : it1.value().items()) {
                                        if (it21.value().is_string() &&
                                            util::to_lower_case(std::string(it21.value())).find(search_lower_case) !=
                                            std::string::npos) {
                                            std::cout << "FOUND STRING: " << it21.key() << ", " << it21.value()
                                                      << " in " << util::uint64_t_to_hex_string(
                                                    rpkg.hash.at(hashIndex).hash_value) + "." +
                                                                   rpkg.hash.at(hashIndex).hash_resource_type
                                                      << std::endl;

                                            localization_search_results += rpkg.rpkg_file_path + "||||" +
                                                                           util::uint64_t_to_hex_string(
                                                                                   rpkg.hash.at(
                                                                                           hashIndex).hash_value) +
                                                                           "." + rpkg.hash.at(
                                                    hashIndex).hash_resource_type + " " + util::hash_to_ioi_string(
                                                    rpkg.hash.at(hashIndex).hash_value, false) + "||||" +
                                                                           std::string(it21.value()) + "||||||";

                                            results_count++;
                                        }
                                    }
                                }
                            }
                            catch (json::parse_error& e1) {
                                std::stringstream ss1;
                                ss1 << "Error: " << "\n"
                                    << "Error message: " << e1.what() << '\n'
                                    << "Error exception id: " << e1.id << '\n'
                                    << "Error byte position of error: " << e1.byte;
                            }
                        }
                    }
                }
            }
        }
    }

    LOG("Done");

    percent_progress = (uint32_t) 100;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}