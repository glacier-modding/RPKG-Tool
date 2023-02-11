#include "rpkg_function.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <regex>
#include <filesystem>

void rpkg_function::search_repo(std::string& input_path, std::string& search, int max_results) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    percent_progress = (uint32_t) 0;

    log_output = false;

    if (std::filesystem::is_regular_file(input_path)) {
        rpkg_function::import_rpkg(input_path, false);
    } else {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }

    uint32_t results_count = 0;

    std::string search_lower_case = util::to_lower_case(search);

    // uint32_t search_crc32_dec = std::strtoul(search.c_str(), nullptr, 10);

    // uint32_t search_crc32_hex = std::strtoul(search.c_str(), nullptr, 16);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    // int stringstream_length = 80;

    std::string message = "Searching REPO (00204D1AFD76AB13)...";

    uint64_t repo_hash_value = 0x00204D1AFD76AB13;

    uint32_t repo_rpkg_index = rpkg_function::get_latest_hash(repo_hash_value);

    if (repo_rpkg_index != UINT32_MAX) {
        const auto it = rpkgs.at(repo_rpkg_index).hash_map.find(repo_hash_value);

        if (it != rpkgs.at(repo_rpkg_index).hash_map.end()) {
            uint64_t ores_hash_value = 0x00858D45F5F9E3CA;

            uint32_t ores_rpkg_index = rpkg_function::get_latest_hash(ores_hash_value);

            if (ores_rpkg_index != UINT32_MAX) {
                auto it2 = rpkgs.at(ores_rpkg_index).hash_map.find(ores_hash_value);

                if (it2 != rpkgs.at(ores_rpkg_index).hash_map.end()) {
                    repos.emplace_back(repo_rpkg_index, it->second);

                    repos[0].load_ores(ores_rpkg_index, it2->second);

                    repos[0].load_repo();

                    repos[0].get_npcs();

                    std::cout << repo_response_data << std::endl;

                    std::string id = "08366aec-7f5b-4c1e-9f47-00ec2df7d185";

                    repos[0].get_child_entries(id);

                    std::cout << repo_response_data << std::endl;

                    id = "4e45ed21-8d88-4fc9-99ff-31d303f69601";

                    repos[0].get_child_entries(id);

                    std::cout << repo_response_data << std::endl;

                    id = "270de950-858a-46cb-9c84-4007a5f914fc";

                    repos[0].get_child_entries(id);

                    std::cout << repo_response_data << std::endl;
                }
            } else {
                LOG_AND_RETURN("ORES hash 00858D45F5F9E3CA does not exist in any loaded RPKG files.");
            }
        }
    } else {
        LOG_AND_RETURN("REPO hash 00204D1AFD76AB13 does not exist in any loaded RPKG files.");
    }

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    std::stringstream ss;

    ss << message << "100% Done in "
       << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = (uint32_t) 100;

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}