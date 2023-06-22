#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <TonyTools/Languages.h>

using json = nlohmann::ordered_json;

void rpkg_function::rebuild_locr_from_json_from(std::string& input_path) {
    task_single_status = TASK_EXECUTING;

    std::string input_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_folder_path + " to rebuild the LOCR files from does not exist.");
    }

    std::vector<std::string> json_file_paths;
    std::vector<std::string> json_file_base_paths;
    std::vector<std::string> locr_file_names;

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

        bool is_locr_hash_file = true;

        if (hash_string.length() != 16) {
            is_locr_hash_file = false;
        }

        if (resource_type != "LOCR") {
            is_locr_hash_file = false;
        }

        if (is_locr_hash_file) {
            json_file_paths.push_back(entry.path().string());
            json_file_base_paths.push_back(json_file_base_path);
            locr_file_names.push_back(util::to_upper_case(hash_file_name));
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

        double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(
                end_time - start_time).count());

        if (time_in_seconds_from_start_time > console_update_rate) {
            start_time = end_time;

            if (period_count > 3) {
                period_count = 0;
            }

            ss.str(std::string());

            ss << "Rebuilding JSON as LOCR" << std::string(period_count, '.');

            timing_string = ss.str();

            LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

            period_count++;
        }

        std::ifstream input_json_file(json_file_paths.at(p));

        if (!input_json_file.good()) {
            LOG_AND_EXIT("Error: JSON file " + json_file_paths.at(p) + " could not be read.");
        }
        
        std::string inputJson(std::istreambuf_iterator<char>{input_json_file}, {});

        input_json_file.close();

        TonyTools::Language::Rebuilt rebuiltLocr = TonyTools::Language::LOCR::Rebuild(
            TonyTools::Language::Version::H3,
            inputJson
        );

        if (rebuiltLocr.file.empty() || rebuiltLocr.meta.empty()) {
            LOG_AND_EXIT("Failed to rebuild JSON to LOCR!");
        }

        std::string current_path = json_file_base_paths.at(p) + "\\LOCR.rebuilt";
        std::vector<char> binMeta = json_to_hash_meta(rebuiltLocr.meta);

        if (binMeta.empty()) {
            LOG_AND_EXIT("Failed to rebuild JSON to LOCR (meta issue).");
        }

        file::create_directories(current_path);

        std::ofstream output_file = std::ofstream(current_path + "\\" + locr_file_names.at(p),
                                                  std::ofstream::binary);

        std::ofstream output_meta_file = std::ofstream(current_path + "\\" + locr_file_names.at(p) + ".meta", std::ofstream::binary);

        if (!output_file.good()) {
            LOG_AND_EXIT("Error: Rebuilt LOCR file " + locr_file_names.at(p) + " could not be created.");
        }

        if (!output_meta_file.good()) {
            LOG_AND_EXIT("Error: Rebuilt LOCR meta file " + locr_file_names.at(p) + ".meta could not be created.");
        }

        output_file.write(rebuiltLocr.file.data(), rebuiltLocr.file.size());
        output_meta_file.write(binMeta.data(), binMeta.size());
    }

    ss.str(std::string());

    ss << "Rebuild JSON as LOCR: Done";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = static_cast<uint32_t>(100);

    task_single_status = TASK_SUCCESSFUL;
}
