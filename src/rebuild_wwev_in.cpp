#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void rpkg_function::rebuild_wwev_in(std::string& input_path) {
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to rebuild the WWEVs does not exist.");
    }

    std::vector<std::string> wwev_folders;

    std::string input_folder = input_rpkg_folder_path;

    if (input_folder.substr(input_folder.length() - 1, 1) == "\\") {
        input_folder = input_folder.substr(0, input_folder.length() - 1);
    }

    if (util::to_upper_case(input_folder.substr((input_folder.length() - 4), 4)) != "\\OGG" &&
        util::to_upper_case(input_folder.substr((input_folder.length() - 4), 4)) != "\\WEM" &&
        file::path_exists(input_folder + "\\meta") && file::path_exists(input_folder + "\\wem")) {
        bool wwev_folder_found = false;

        for (const auto& wwev_folder : wwev_folders) {
            if (input_folder == wwev_folder) {
                wwev_folder_found = true;
            }
        }

        if (!wwev_folder_found) {
            wwev_folders.push_back(input_folder);
        }
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path)) {
        if (std::filesystem::is_directory(entry.path().string())) {
            input_folder = entry.path().string();

            if (input_folder.substr(input_folder.length() - 1, 1) == "\\") {
                input_folder = input_folder.substr(0, input_folder.length() - 1);
            }

            if (util::to_upper_case(input_folder.substr((input_folder.length() - 4), 4)) != "\\OGG" &&
                util::to_upper_case(input_folder.substr((input_folder.length() - 4), 4)) != "\\WEM" &&
                file::path_exists(input_folder + "\\meta") && file::path_exists(input_folder + "\\wem")) {
                bool wwev_folder_found = false;

                for (const auto& wwev_folder : wwev_folders) {
                    if (input_folder == wwev_folder) {
                        wwev_folder_found = true;
                    }
                }

                if (!wwev_folder_found) {
                    wwev_folders.push_back(input_folder);
                }
            }
        }
    }

    LOG("Scanning folder: Done");

    for (uint64_t i = 0; i < wwev_folders.size(); i++) {
        if (gui_control == ABORT_CURRENT_TASK) {
            return;
        }

        std::vector<std::string> wem_file_names;
        std::vector<std::string> wem_file_paths;
        std::vector<std::string> wem_file_root;

        for (const auto& entry : std::filesystem::directory_iterator(wwev_folders.at(i) + "\\wem")) {
            if (std::filesystem::is_regular_file(entry.path().string())) {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string wem_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos) {
                    wem_file_name = entry.path().string().substr(pos + 1,
                                                                 entry.path().string().length() - (pos + 1));
                } else {
                    wem_file_name = entry.path().string();
                }

                std::smatch m;
                std::regex re("[0-9]+.wem$");

                std::regex_search(wem_file_name, m, re);

                if (m.size() > 0) {
                    wem_file_paths.push_back(entry.path().string());
                    wem_file_names.push_back(wem_file_name);
                }
            }
        }

        bool all_wems_found = true;

        for (uint64_t j = 0; j < wem_file_paths.size(); j++) {
            if (!file::path_exists(wwev_folders.at(i) + "\\wem\\" + std::to_string(j) + ".wem")) {
                all_wems_found = false;

                LOG(std::to_string(j) + ".wem" << " is missing.");
            }
        }

        if (all_wems_found) {
            std::ifstream meta_file = std::ifstream(wwev_folders.at(i) + "\\meta", std::ifstream::binary);

            if (!meta_file.good()) {
                LOG_AND_EXIT("Error: meta file " + wwev_folders.at(i) + "\\meta" + " could not be opened.");
            }

            meta_file.seekg(0, meta_file.end);

            uint64_t meta_file_size = meta_file.tellg();

            meta_file.seekg(0, meta_file.beg);

            std::vector<char> meta_data(meta_file_size, 0);

            uint64_t wwev_hash = 0;
            uint32_t wwev_file_name_length = 0;
            uint32_t wwev_file_count = 0;
            uint32_t wwev_file_header_id = 0;
            std::string wwev_file_name;

            char input[1024];
            uint64_t position = 0;

            meta_file.read(meta_data.data(), meta_file_size);

            meta_file.close();

            std::memcpy(&wwev_hash, &meta_data.data()[position], sizeof(uint64_t));
            position += sizeof(uint64_t);

            std::memcpy(&wwev_file_name_length, &meta_data.data()[position], sizeof(uint32_t));
            position += sizeof(uint32_t);

            std::memcpy(&input, &meta_data.data()[position], wwev_file_name_length);
            position += wwev_file_name_length;

            for (uint64_t j = 0; j < wwev_file_name_length; j++) {
                wwev_file_name.push_back(input[j]);
            }

            std::memcpy(&wwev_file_header_id, &meta_data.data()[position], sizeof(uint32_t));
            position += sizeof(uint32_t);

            std::memcpy(&wwev_file_count, &meta_data.data()[position], sizeof(uint32_t));
            position += sizeof(uint32_t);

            if (wwev_file_count != wem_file_paths.size()) {
                wwev_file_count = (uint32_t) wem_file_paths.size();

                LOG("Error: Mismatch between meta WWEV file count and number of wem files in directory: "
                            << wwev_folders.at(i));
            }

            std::vector<uint32_t> wem_hashes;
            std::vector<uint32_t> wem_sizes;

            for (uint64_t j = 0; j < wwev_file_count; j++) {
                uint32_t wem_hash = 0;
                uint32_t wem_size = 0;

                std::memcpy(&wem_hash, &meta_data.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                std::memcpy(&wem_size, &meta_data.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                wem_hashes.push_back(wem_hash);
                wem_sizes.push_back(wem_size);
            }

            std::string wwev_hash_file_name =
                    wwev_folders.at(i) + "\\" + util::uint64_t_to_hex_string(wwev_hash) + ".WWEV";

            std::ofstream wwev_file = std::ofstream(wwev_hash_file_name, std::ofstream::binary);

            if (!wwev_file.good()) {
                LOG_AND_EXIT("Error: WWEV file " + wwev_hash_file_name + " could not be created.");
            }

            std::memcpy(&input, &wwev_file_name_length, sizeof(uint32_t));
            wwev_file.write(input, sizeof(uint32_t));

            wwev_file.write(wwev_file_name.c_str(), wwev_file_name_length);

            std::memcpy(&input, &wwev_file_header_id, sizeof(uint32_t));
            wwev_file.write(input, sizeof(uint32_t));

            std::memcpy(&input, &wwev_file_count, sizeof(uint32_t));
            wwev_file.write(input, sizeof(uint32_t));

            for (uint64_t j = 0; j < wem_file_paths.size(); j++) {
                std::string wem_file_name = wwev_folders.at(i) + "\\wem\\" + std::to_string(j) + ".wem";

                std::ifstream wem_file = std::ifstream(wem_file_name, std::ifstream::binary);

                if (!wem_file.good()) {
                    LOG_AND_EXIT("Error: wem file " + wem_file_name + " could not be opened.");
                }

                wem_file.seekg(0, wem_file.end);

                uint64_t wem_file_size = wem_file.tellg();

                wem_file.seekg(0, wem_file.beg);

                std::vector<char> wem_data(wem_file_size, 0);

                wem_file.read(wem_data.data(), wem_file_size);

                wem_file.close();

                std::memcpy(&input, &wem_hashes.at(j), sizeof(uint32_t));
                wwev_file.write(input, sizeof(uint32_t));

                std::memcpy(&input, &wem_file_size, sizeof(uint32_t));
                wwev_file.write(input, sizeof(uint32_t));

                wwev_file.write(wem_data.data(), wem_file_size);
            }

            if (meta_file_size > position) {
                wwev_file.write(&meta_data.data()[position], (meta_file_size - position));
            }

            wwev_file.close();
        } else {
            LOG("Can't create WWEV from path: " << wwev_folders.at(i));
        }
    }

    LOG("Done");

    percent_progress = (uint32_t) 100;

    task_single_status = TASK_SUCCESSFUL;
}