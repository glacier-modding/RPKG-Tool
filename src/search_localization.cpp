#include "rpkg_function.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include <iostream>
#include <chrono>
#include <sstream>

void add_search_result(rpkg& rpkg, uint32_t hash_index, std::string& data) {
    localization_search_results += rpkg.rpkg_file_path + "||||" +
        util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) +
            "." + rpkg.hash.at(hash_index).hash_resource_type + " " +
        util::hash_to_ioi_string(rpkg.hash.at(hash_index).hash_value, false) + "||||" +
        data + "||||||";
}

uint32_t search_dlge_container(
    json container,
    rpkg& rpkg,
    uint32_t hash_index,
    std::string search_term
) {
    uint32_t results_count = 0;
    json type = container.at("type");
    if (util::to_lower_case(type.get<std::string>()).find(search_term) != std::string::npos) {
        add_search_result(rpkg, hash_index, type.get<std::string>());
        results_count++;
    }

    switch (type.get<DLGE_Type>()) {
        case DLGE_Type::eDEIT_WavFile: {
            // God I hate this.
            if (util::to_lower_case(container.at("wavName").get<std::string>()).find(search_term) != std::string::npos) {
                add_search_result(rpkg, hash_index, container.at("wavName").get<std::string>());
                return ++results_count;
            }

            if (util::to_lower_case(container.at("soundtag").get<std::string>()).find(search_term) != std::string::npos) {
                add_search_result(rpkg, hash_index, container.at("soundtag").get<std::string>());
                return ++results_count;
            }

            if (util::to_lower_case(container.at("defaultWav").get<std::string>()).find(search_term) != std::string::npos) {
                add_search_result(rpkg, hash_index, container.at("defaultWav").get<std::string>());
                return ++results_count;
            }

            if (util::to_lower_case(container.at("defaultFfx").get<std::string>()).find(search_term) != std::string::npos) {
                add_search_result(rpkg, hash_index, container.at("defaultFfx").get<std::string>());
                return ++results_count;
            }

            for (const auto &[lang, entry] : container.at("languages").items()) {
                if (entry.is_object()) {
                    if (util::to_lower_case(entry.at("wav").get<std::string>()).find(search_term) != std::string::npos) {
                        add_search_result(rpkg, hash_index, entry.at("wav").get<std::string>());
                        results_count++;
                    }

                    if (util::to_lower_case(entry.at("ffx").get<std::string>()).find(search_term) != std::string::npos) {
                        add_search_result(rpkg, hash_index, entry.at("ffx").get<std::string>());
                        results_count++;
                    }

                    if (entry.contains("subtitle")) {
                        if (util::to_lower_case(entry.at("subtitle").get<std::string>()).find(search_term) != std::string::npos) {
                            add_search_result(rpkg, hash_index, lang + ": " + entry.at("subtitle").get<std::string>());
                            results_count++;
                        }
                    }
                } else {
                    if (util::to_lower_case(entry.get<std::string>()).find(search_term) != std::string::npos) {
                        add_search_result(rpkg, hash_index, lang + ": " + entry.get<std::string>());
                        results_count++;
                    }
                }
            }

            return results_count;
        }
        case DLGE_Type::eDEIT_RandomContainer: {
            // Types are already searched at the top so all we need to do is process wavfiles.
            for (const json &childContainer : container.at("containers")) {
                results_count += search_dlge_container(childContainer, rpkg, hash_index, search_term);
            }

            return results_count;
        }
        case DLGE_Type::eDEIT_SwitchContainer: {
            if (util::to_lower_case(container.at("switchKey")).find(search_term) != std::string::npos) {
                add_search_result(rpkg, hash_index, container.at("switchKey").get<std::string>());
                results_count++;
            }

            if (util::to_lower_case(container.at("default")).find(search_term) != std::string::npos) {
                add_search_result(rpkg, hash_index, container.at("default").get<std::string>());
                results_count++;
            }

            for (const json &childContainer : container.at("containers")) {
                for (const json &sCase : childContainer.at("cases")) {
                    if (util::to_lower_case(sCase.get<std::string>()).find(search_term) != std::string::npos) {
                        add_search_result(rpkg, hash_index, sCase.get<std::string>());
                        results_count++;
                    }
                }

                results_count += search_dlge_container(childContainer, rpkg, hash_index, search_term);
            }

            return results_count;
        }
        case DLGE_Type::eDEIT_SequenceContainer: {
            // Already checked the type, now to process the containers.
            for (const json& childContainer : container.at("containers")) {
                results_count += search_dlge_container(childContainer, rpkg, hash_index, search_term);
            }

            return results_count;
        }
    }
}

void rpkg_function::search_localization(std::string& input_path, std::string& search, std::string& output_path,
                                        bool search_dlge, bool search_locr, bool search_rtlv, int max_results, std::string version) {
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
                                                                         output_path, true, version);

                                try {
                                    results_count += search_dlge_container(
                                        localization_json.at("rootContainer"),
                                        rpkg,
                                        hash_index,
                                        search_lower_case
                                    );
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
                                                                         output_path, true, version);

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
                                                                         output_path, true, version);

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