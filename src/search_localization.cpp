#include "rpkg_function.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include <iostream>
#include <chrono>
#include <sstream>

void add_search_result(rpkg rpkg, uint32_t hash_index, std::string data) {
    localization_search_results += rpkg.rpkg_file_path + "||||" +
        util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) +
            "." + rpkg.hash.at(hash_index).hash_resource_type + " " +
        util::hash_to_ioi_string(rpkg.hash.at(hash_index).hash_value, false) + "||||" +
        data + "||||||";
}

void rpkg_function::search_localization(std::string& input_path, std::string& search, std::string& output_path,
                                        bool search_dlge, bool search_locr, bool search_rtlv, int max_results) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    percent_progress = (uint32_t) 0;

    log_output = false;

    localization_search_results = "";

    uint32_t results_count = 0;

    std::string search_upper_case = util::to_upper_case(search);

    std::string search_lower_case = util::to_lower_case(search);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    uint64_t localization_hash_count = 0;
    uint64_t localization_hash_size_total = 0;

    for (auto& rpkg : rpkgs) {
        std::string message = "Searching " + rpkg.rpkg_file_name + "...";

        if (input_path.empty() || input_path == rpkg.rpkg_file_path) {
            for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                if (search_dlge) {
                    if (rpkg.hash_resource_types.at(r) == "DLGE") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                            localization_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;
                            localization_hash_count++;
                        }
                    }
                }

                if (search_locr) {
                    if (rpkg.hash_resource_types.at(r) == "LOCR") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                            localization_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;
                            localization_hash_count++;
                        }
                    }
                }

                if (search_rtlv) {
                    if (rpkg.hash_resource_types.at(r) == "RTLV") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                            localization_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;
                            localization_hash_count++;
                        }
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

                                if (((localization_hash_count_current * (uint64_t) 100000) /
                                     (uint64_t) localization_hash_count) % (uint64_t) 10 == 0 &&
                                    localization_hash_count_current > 0) {
                                    stringstream_length = console::update_console(message, localization_hash_size_total,
                                                                                  localization_hash_size_current,
                                                                                  start_time, stringstream_length);
                                }

                                localization_hash_size_current += rpkg.hash.at(hash_index).data.resource.size_final;
                                localization_hash_count_current++;

                                rpkg_function::extract_dlge_to_json_from(rpkg.rpkg_file_path, temp_hash_string,
                                                                         output_path, true, "HM3");

                                try {
                                    /*for (const auto& it : localization_json.items()) {
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
                                    }*/
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
                            if (results_count < max_results) {
                                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                                std::string temp_hash_string = util::uint64_t_to_hex_string(
                                        rpkg.hash.at(hash_index).hash_value);

                                if (((localization_hash_count_current * (uint64_t) 100000) /
                                     (uint64_t) localization_hash_count) % (uint64_t) 10 == 0 &&
                                    localization_hash_count_current > 0) {
                                    stringstream_length = console::update_console(message, localization_hash_size_total,
                                                                                  localization_hash_size_current,
                                                                                  start_time, stringstream_length);
                                }

                                localization_hash_size_current += rpkg.hash.at(hash_index).data.resource.size_final;
                                localization_hash_count_current++;

                                rpkg_function::extract_locr_to_json_from(rpkg.rpkg_file_path, temp_hash_string,
                                                                         output_path, true, "HM3");

                                try {
                                    for (const auto &[lang, strings] : localization_json.at("languages").items()) {
                                        for (const auto &[hash, string] : strings.items()) {
                                            if (
                                                hash.find(search_upper_case) != std::string::npos ||
                                                util::to_lower_case(string.get<std::string>()).find(search_lower_case) != std::string::npos
                                            ) {
                                                add_search_result(rpkg, hash_index, lang + ": " + hash + ": " + string.get<std::string>());

                                                results_count++;
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

                    if (search_rtlv && rpkg.hash_resource_types.at(r) == "RTLV") {
                        for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                            if (results_count < max_results) {
                                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                                std::string temp_hash_string = util::uint64_t_to_hex_string(
                                        rpkg.hash.at(hash_index).hash_value);

                                if (((localization_hash_count_current * (uint64_t) 100000) /
                                     (uint64_t) localization_hash_count) % (uint64_t) 10 == 0 &&
                                    localization_hash_count_current > 0) {
                                    stringstream_length = console::update_console(message, localization_hash_size_total,
                                                                                  localization_hash_size_current,
                                                                                  start_time, stringstream_length);
                                }

                                localization_hash_size_current += rpkg.hash.at(hash_index).data.resource.size_final;
                                localization_hash_count_current++;

                                rpkg_function::extract_rtlv_to_json_from(rpkg.rpkg_file_path, temp_hash_string,
                                                                         output_path, true, "HM3");

                                try {
                                    for (const auto &[lang, string] : localization_json.at("subtitles").items()) {
                                        if (util::to_lower_case(string.get<std::string>()).find(search_lower_case) != std::string::npos) {
                                            add_search_result(rpkg, hash_index, lang + ": " + string.get<std::string>());

                                            results_count++;
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
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        std::stringstream ss;

        ss << message << "100% Done in "
           << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count())
           << "s";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));
    }

    percent_progress = (uint32_t) 100;

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}