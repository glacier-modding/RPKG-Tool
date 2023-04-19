#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "asva.h"

void rpkg_function::extract_asva_to_json(std::string& input_path, std::string& filter, std::string& output_path) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;

        LOG_AND_EXIT("Error: The RPKG file " + input_path + " does not exist.");
    }

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::vector<std::string> filters = util::parse_input_filter(filter);

    for (const auto & filter : filters) {
        uint64_t mati_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        uint32_t rpkg_index = rpkg_function::get_latest_hash(mati_hash_value);

        if (rpkg_index != UINT32_MAX) {
            auto it = rpkgs.at(rpkg_index).hash_map.find(
                    mati_hash_value);

            if (it != rpkgs.at(rpkg_index).hash_map.end()) {
                asva temp_material(rpkg_index, it->second);

                std::string temp_output_path;
                bool output_path_is_dir = false;

                if (output_path.length() > 5) {
                    if (util::to_lower_case(output_path).substr(output_path.length() - 5) == ".json") {
                        temp_output_path = output_path;
                    } else {
                        output_path_is_dir = true;
                    }
                } else {
                    output_path_is_dir = true;
                }

                if (output_path_is_dir) {
                    if (!output_path.empty()) {
                        file::create_directories(output_path);
                    }

                    temp_output_path = file::output_path_append(filter + ".asva.json", output_path);
                }

                temp_material.generate_json();

                temp_material.write_json_to_file(temp_output_path);
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}