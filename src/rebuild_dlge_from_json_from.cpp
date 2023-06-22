#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <TonyTools/Languages.h>

using json = nlohmann::ordered_json;

void rpkg_function::rebuild_dlge_from_json_from(std::string& input_path, std::string& output_path) {
    task_single_status = TASK_EXECUTING;

    std::string input_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_folder_path + " to rebuild the DLGE files from does not exist.");
    }

    std::vector<std::string> json_file_paths;
    std::vector<std::string> json_file_base_paths;
    std::vector<std::string> dlge_file_names;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_folder_path)) {
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

        if (!std::filesystem::is_regular_file(entry.path().string()))
            continue;

        std::size_t pos = entry.path().string().find_last_of("\\/");

        std::string json_file_name = "";
        std::string json_file_base_path = "";
        std::string hash_file_name = "";
        std::string hash_string = "";
        std::string resource_type = "";

        if (pos != std::string::npos) {
            json_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
            json_file_base_path = entry.path().string().substr(0, pos);
        } else {
            json_file_name = entry.path().string();
        }

        if (util::to_upper_case(json_file_name.substr((json_file_name.length() - 5), 5)) == ".JSON") {
            hash_file_name = util::to_upper_case(json_file_name.substr(0, (json_file_name.length() - 5)));
        }

        pos = hash_file_name.find_last_of('.');

        if (pos != std::string::npos) {
            hash_string = hash_file_name.substr(0, pos);
            resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
        }

        bool is_dlge_hash_file = true;

        if (hash_string.length() != 16) {
            is_dlge_hash_file = false;
        }

        if (resource_type != "DLGE") {
            is_dlge_hash_file = false;
        }

        if (is_dlge_hash_file) {
            json_file_paths.push_back(entry.path().string());
            json_file_base_paths.push_back(json_file_base_path);
            dlge_file_names.push_back(util::to_upper_case(hash_file_name));
        }
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    start_time = std::chrono::high_resolution_clock::now();

    console_update_rate = 1.0 / 2.0;
    period_count = 1;

    for (uint64_t p = 0; p < json_file_paths.size(); p++) {
        if (gui_control == ABORT_CURRENT_TASK) {
            return;
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        double time_in_seconds_from_start_time = (0.000000001 *
                                                    std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                            end_time - start_time).count());

        if (time_in_seconds_from_start_time > console_update_rate) {
            start_time = end_time;

            if (period_count > 3) {
                period_count = 0;
            }

            ss.str(std::string());

            ss << "Rebuilding JSON as DLGE" << std::string(period_count, '.');

            timing_string = ss.str();

            LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

            period_count++;
        }

        std::string inputJson = file::read_file_to_string(json_file_paths.at(p));

        TonyTools::Language::Rebuilt rebuiltDLGE = TonyTools::Language::DLGE::Rebuild(
            TonyTools::Language::Version::H3,
            inputJson
        );

        if (rebuiltDLGE.file.empty() || rebuiltDLGE.meta.empty()) {
            LOG_AND_EXIT("Failed to rebuild JSON to DLGE!");
        }

        std::vector<char> outMeta = json_to_hash_meta(rebuiltDLGE.meta);

        if (outMeta.empty()) {
            LOG_AND_EXIT("Failed to rebuild JSON to DLGE (meta error)!");
        }

        std::string current_path = json_file_base_paths.at(p) + "\\DLGE.rebuilt";

        file::create_directories(current_path);

        std::ofstream output_file = std::ofstream(current_path + "\\" + dlge_file_names.at(p),
                                                    std::ofstream::binary);

        std::ofstream output_meta_file = std::ofstream(current_path + "\\" + dlge_file_names.at(p) + ".meta",
            std::ofstream::binary);

        if (!output_file.good()) {
            LOG_AND_EXIT("Error: Rebuilt DLGE file " + dlge_file_names.at(p) + " could not be created.");
        }

        if (!output_meta_file.good()) {
            LOG_AND_EXIT("Error: Rebuilt DLGE meta file " + dlge_file_names.at(p) + ".meta could not be created.");
        }

        output_file.write(rebuiltDLGE.file.data(), rebuiltDLGE.file.size());
        output_meta_file.write(outMeta.data(), outMeta.size());
    }

    ss.str(std::string());

    ss << "Rebuilding JSON as DLGE: Done";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = static_cast<uint32_t>(100);

    task_single_status = TASK_SUCCESSFUL;
}
