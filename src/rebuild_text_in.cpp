#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <set>

void rpkg_function::rebuild_text_in(std::string& input_path, std::string& output_path, bool generate_rpkgs) {
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to rebuild the TEXTs does not exist.");
    }

    std::vector<std::string> text_folders;

    std::string input_folder = input_rpkg_folder_path;

    std::string rpkgs_path = input_rpkg_folder_path + "RPKGS";

    if (generate_rpkgs) {
        file::create_directories(rpkgs_path);
    }

    std::filesystem::path input_folder_path = input_rpkg_folder_path;

    text_folders.push_back(input_folder_path.string());

    if (input_folder.substr(input_folder.length() - 1, 1) == "\\") {
        input_folder = input_folder.substr(0, input_folder.length() - 1);
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path)) {
        if (std::filesystem::is_directory(entry.path().string())) {
            input_folder = entry.path().string();

            if (input_folder.substr(input_folder.length() - 1, 1) == "\\") {
                input_folder = input_folder.substr(0, input_folder.length() - 1);
            }

            bool text_folder_found = false;

            for (const auto& text_folder : text_folders) {
                if (input_folder == text_folder) {
                    text_folder_found = true;
                }
            }

            if (!text_folder_found) {
                text_folders.push_back(input_folder);
            }
        }
    }

    LOG("Scanning folder: Done");

    std::string message = "Rebuilding TEXT files...";

    timing_string = message;

    std::set<std::string> directory_set;

    for (auto & text_folder : text_folders) {
        if (gui_control == ABORT_CURRENT_TASK) {
            return;
        }

        std::vector<std::string> tga_file_names;
        std::vector<std::string> tga_file_paths;
        std::vector<std::string> meta_file_paths;
        std::vector<uint64_t> text_hashes;
        std::vector<std::string> text_hash_strings;
        std::vector<std::string> text_file_names;

        for (const auto& entry : std::filesystem::directory_iterator(text_folder)) {
            if (std::filesystem::is_regular_file(entry.path().string())) {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string file_name = "";
                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos) {
                    file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                } else {
                    file_name = entry.path().string();
                }

                if (file_name.length() > 4) {
                    if (util::to_upper_case(file_name.substr((file_name.length() - 4), 4)) == ".TGA") {
                        hash_file_name = util::to_upper_case(file_name.substr(0, (file_name.length() - 4)));
                        std::string meta_file_path = file::output_path_append(
                                "metas\\" + hash_file_name + ".tga.meta", text_folder);

                        pos = hash_file_name.find_last_of('.');

                        if (pos != std::string::npos) {
                            hash_string = hash_file_name.substr(0, pos);
                            resource_type = util::to_upper_case(
                                    hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1)));
                        }

                        bool is_tga_hash_file = true;
                        bool tga_hash_meta = true;

                        if (hash_string.length() != 16) {
                            is_tga_hash_file = false;
                        }

                        if (resource_type != "TEXT") {
                            is_tga_hash_file = false;
                        }

                        if (!file::path_exists(meta_file_path)) {
                            tga_hash_meta = false;

                            LOG("TEXT TGA file " + entry.path().string() +
                                " found but TEXT TGA Meta file is missing, can't rebuild.");
                        }

                        if (is_tga_hash_file && tga_hash_meta) {
                            tga_file_paths.push_back(entry.path().string());
                            tga_file_names.push_back(file_name);
                            meta_file_paths.push_back(meta_file_path);
                            text_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                            text_file_names.push_back(util::to_upper_case(hash_file_name));
                            text_hash_strings.push_back(util::to_upper_case(hash_string));
                        }
                    }
                }
            }
        }

        for (uint64_t j = 0; j < tga_file_paths.size(); j++) {
            std::string rpkg_output_path = "";

            bool texture_rebuilt = rpkg_function::rebuild_text(text_folder, tga_file_paths.at(j),
                                                               text_file_names.at(j), meta_file_paths.at(j),
                                                               rpkg_output_path, rpkgs_path, generate_rpkgs);

            if (texture_rebuilt) {
                directory_set.insert(rpkg_output_path);
            }
        }
    }

    if (generate_rpkgs) {
        for (auto it = directory_set.begin(); it != directory_set.end(); it++) {
            std::string input_path = *it;
            std::string filter = "";
            std::string output_path_string = rpkgs_path;

            //std::cout << *it << std::endl;
            //std::cout << output_path_string << std::endl;

            rpkg_function::generate_rpkg_from(input_path, output_path_string, true);
        }
    }

    LOG("Done");

    percent_progress = (uint32_t) 100;

    task_single_status = TASK_SUCCESSFUL;
}
