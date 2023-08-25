#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <TonyTools/Languages.h>

using json = nlohmann::ordered_json;

void rpkg_function::rebuild_rtlv_from_json_from(std::string& input_path, std::string version) {
    TonyTools::Language::Version ttVersion = TonyTools::Language::Version::H3;
    if (version == "HM2") {
        ttVersion = TonyTools::Language::Version::H2;
    }
    else if (version == "HM2016") {
        ttVersion = TonyTools::Language::Version::H2016;
    }

    task_single_status = TASK_EXECUTING;

    std::string input_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_folder_path + " to rebuild the RTLV files from does not exist.");
    }

    std::vector<std::string> json_file_paths;
    std::vector<std::string> json_file_base_paths;
    std::vector<std::string> rtlv_file_names;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_folder_path)) {
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

        bool is_rtlv_hash_file = true;

        if (hash_string.length() != 16) {
            is_rtlv_hash_file = false;
        }

        if (resource_type != "RTLV") {
            is_rtlv_hash_file = false;
        }

        if (is_rtlv_hash_file) {
            json_file_paths.push_back(entry.path().string());
            json_file_base_paths.push_back(json_file_base_path);
            rtlv_file_names.push_back(util::to_upper_case(hash_file_name));
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

            ss << "Rebuilding JSON as RTLV" << std::string(period_count, '.');

            timing_string = ss.str();

            LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

            period_count++;
        }

        std::string inputJson = file::read_file_to_string(json_file_paths.at(p));

        TonyTools::Language::Rebuilt rebuiltRTLV = TonyTools::Language::RTLV::Rebuild(
            ttVersion,
            inputJson
        );

        if (rebuiltRTLV.file.empty() || rebuiltRTLV.meta.empty()) {
            LOG_AND_EXIT("Failed to rebuild JSON to RTLV!");
        }

        std::vector<char> outMeta = json_to_hash_meta(rebuiltRTLV.meta);

        if (outMeta.empty()) {
            LOG_AND_EXIT("Failed to rebuild JSON to RTLV (meta error)!");
        }

        std::string current_path = json_file_base_paths.at(p) + "\\RTLV.rebuilt";

        file::create_directories(current_path);

        std::ofstream output_file = std::ofstream(current_path + "\\" + rtlv_file_names.at(p),
                                                  std::ofstream::binary);

        std::ofstream output_meta_file = std::ofstream(current_path + "\\" + rtlv_file_names.at(p) + ".meta",
            std::ofstream::binary);

        if (!output_file.good()) {
            LOG_AND_EXIT("Error: Rebuilt RTLV file " + rtlv_file_names.at(p) + " could not be created.");
        }

        if (!output_meta_file.good()) {
            LOG_AND_EXIT("Error: Rebuilt RTLV meta file " + rtlv_file_names.at(p) + ".meta could not be created.");
        }

        output_file.write(rebuiltRTLV.file.data(), rebuiltRTLV.file.size());
        output_meta_file.write(outMeta.data(), outMeta.size());
    }

    ss.str(std::string());

    ss << "Rebuilding JSON as RTLV: Done";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = (uint32_t) 100;

    task_single_status = TASK_SUCCESSFUL;
}