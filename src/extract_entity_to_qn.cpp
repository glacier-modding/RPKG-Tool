#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "generic_function.h"
#include <iostream>
#include <sstream>
#include <filesystem>

void rpkg_function::extract_entity_to_qn(std::string& input_path, std::string& filter, std::string& output_path) {
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

    std::filesystem::path base_folder_path = input_path;
    std::string parent_path = base_folder_path.parent_path().string();
    rpkg_function::import_rpkg_files_in_folder(parent_path);

    LOG("Scanning folder: Done");

    force_load_hash_list();

    std::vector<std::string> filters = util::parse_input_filter(filter);

    for (auto& filter : filters) {
        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_hash_value);

        if (rpkg_index == UINT32_MAX)
            continue;

        auto it6 = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

        if (it6 == rpkgs.at(rpkg_index).hash_map.end())
            continue;

        std::string temp_output_path = "";
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

            temp_output_path = file::output_path_append(filter + ".entity.json", output_path);
        }

        entity temp_entity(rpkg_index, it6->second, 3, temp_output_path);

        temp_entity.free_yyjson_doc();
        LOG("Done");
    }

    percent_progress = (uint32_t) 100;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
