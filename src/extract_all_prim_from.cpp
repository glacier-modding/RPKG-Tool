#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>

void
rpkg_function::extract_all_prim_from(std::string& input_path, std::string& filter, std::string& output_path, int type) {
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
    } else {
        rpkg_function::import_rpkg(input_path);
    }

    LOG("Scanning folder: Done");

    file::create_directories(file::output_path_append("PRIM", output_path));

    std::vector<std::string> filters = util::parse_input_filter(filter);

    bool extract_single_hash = false;

    if (filters.size() == 1) {
        if (util::is_valid_hash(filters.at(0))) {
            extract_single_hash = true;
        }
    }

    uint64_t prim_count = 0;
    uint64_t prim_hash_size_total = 0;

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "PRIM")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                             rpkg.hash.at(hash_index).hash_resource_type;

                prim_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;

                prim_count++;
            }
        }
    }

    LOG("Scanning RPKGs for PRIM files: Done");

    uint64_t prim_count_current = 0;
    uint64_t prim_hash_size_current = 0;

    if (!filter.empty()) {
        LOG("Extracting PRIM to GLB files with filter \"" << filter << "\"");
    }

    std::unordered_map<std::string, uint32_t> prim_name_map;

    std::vector<std::string> found_in;
    std::vector<std::string> not_found_in;

    for (uint64_t z = 0; z < filters.size(); z++) {
        found_in.emplace_back("");

        not_found_in.emplace_back("");
    }

    for (auto& rpkg : rpkgs) {
        std::vector<bool> extracted;

        for (uint64_t z = 0; z < filters.size(); z++) {
            extracted.push_back(false);
        }

        if (rpkg.rpkg_file_path == input_path || !input_path_is_rpkg_file) {
            for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
                if (rpkg.hash_resource_types.at(r) != "PRIM")
                    continue;

                for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                    uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                    if (gui_control == ABORT_CURRENT_TASK) {
                        return;
                    }

                    prim_hash_size_current += rpkg.hash.at(hash_index).data.resource.size_final;

                    prim_count_current++;

                    if (!extract_single_hash || filter == util::uint64_t_to_hex_string(
                            rpkg.hash.at(hash_index).hash_value)) {
                        std::string hash_file_name =
                                util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                rpkg.hash.at(hash_index).hash_resource_type;

                        bool found = false;

                        uint64_t input_filter_index = 0;

                        for (uint64_t z = 0; z < filters.size(); z++) {
                            std::size_t found_position_hash = hash_file_name.find(filters.at(z));

                            if ((found_position_hash != std::string::npos && filters.at(z) != "")) {
                                found = true;

                                input_filter_index = z;

                                break;
                            }
                        }

                        if (found || filter == "") {
                            std::string prim_output_dir = file::output_path_append("PRIM\\" + rpkg.rpkg_file_name,
                                                                                   output_path);

                            file::create_directories(prim_output_dir);

                            if (!filters.empty()) {
                                extracted.at(input_filter_index) = true;
                            }

                            //std::cout << "rpkg_function::extract_prim_to_gltf_from(" << rpkgs.at(i).rpkg_file_path << ", " << util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) << ", " << prim_output_dir << ");" << std::endl;

                            rpkg_function::extract_prim_from(rpkg.rpkg_file_path, util::uint64_t_to_hex_string(
                                    rpkg.hash.at(hash_index).hash_value), prim_output_dir, type, true);
                        }
                    }
                }
            }
        }

        for (uint64_t z = 0; z < filters.size(); z++) {
            if (extracted.at(z)) {
                if (found_in.at(z).empty()) {
                    found_in.at(z).append(rpkg.rpkg_file_name);
                } else {
                    found_in.at(z).append(", " + rpkg.rpkg_file_name);
                }
            } else {
                if (not_found_in.at(z).empty()) {
                    not_found_in.at(z).append(rpkg.rpkg_file_name);
                } else {
                    not_found_in.at(z).append(", " + rpkg.rpkg_file_name);
                }
            }
        }
    }

    LOG("Done");

    percent_progress = static_cast<uint32_t>(100);

    if (!filter.empty()) {
        for (uint64_t z = 0; z < filters.size(); z++) {
            LOG(std::endl << "\"" << filters.at(z) << "\" was found in and extracted from: " << found_in.at(z));

            LOG(std::endl << "\"" << filters.at(z) << "\" was not found in RPKG file(s): " << not_found_in.at(z));
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
