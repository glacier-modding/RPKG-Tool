#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "material.h"
#include <chrono>
#include <iostream>

void rpkg_function::extract_all_materials_to_json(std::string& input_path, std::string& output_path) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The RPKG folder " + input_path + " does not exist.");
    }

    if (!file::path_exists(output_path)) {
        file::create_directories(output_path);
	}

    log_output = false;

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    for (auto& rpkg : rpkgs) {
        for (auto& hash_pair : rpkg.hash_map) {
            uint64_t current_hash_value = hash_pair.first;

            const uint32_t rpkg_index = rpkg_function::get_latest_hash(current_hash_value);

            if (rpkg_index != UINT32_MAX) {
                auto it = rpkgs.at(rpkg_index).hash_map.find(current_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end()) {
                    if (rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type == "MATI") {
                        timing_string = "Converting: " +
                            util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) +
                            "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type +
                            " to Material Entity (MATI/MATT/MATB) JSON";

                        material temp_material(rpkg_index, it->second);

                        std::string temp_output_path = file::output_path_append(
                            util::uint64_t_to_hex_string(current_hash_value) + ".material.json", output_path
                        );

                        temp_material.generate_json();
                        temp_material.write_json_to_file(temp_output_path);
                    }
                }
            }
        }
    }

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}