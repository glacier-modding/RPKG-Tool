#include "rpkg_function.h"
#include "global.h"
#include "generic_function.h"
#include "file.h"
#include "util.h"
#include "mati.h"
#include <iostream>
#include <filesystem>

void rpkg_function::mati_to_json(std::string& input_path, std::string& filter, std::string& output_path) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    if (!file::path_exists(input_path)) {
        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
        return;
    }

    bool output_path_is_file = false;

    std::vector<std::filesystem::path> files;

    if (std::filesystem::is_regular_file(input_path)) {
        files.emplace_back(input_path);

        if (!output_path.empty()) {
            if (std::filesystem::exists(output_path)) {
                if (std::filesystem::is_regular_file(output_path)) {
                    output_path_is_file = true;
                }
            } else {
                output_path_is_file = true;
            }
        }
    } else {
        files = file::get_recursive_file_list(input_path);
    }

    force_load_hash_list();

    for (std::filesystem::path& file : files) {
        uint64_t hash_value = file::get_hash_value_from_path(file, ".MATI");

        if (!hash_value) continue;

        if (!file::path_exists(file.string() + ".meta")) continue;

        LOG("Converting: " + file.filename().string() + " (+ MATI.meta) to MATI JSON");

        if (output_path.empty()) {
            mati temp_mati(file.string(), file.string() + ".meta", hash_value, file.parent_path().string(),
                           output_path_is_file);
        } else {
            mati temp_mati(file.string(), file.string() + ".meta", hash_value, output_path,
                           output_path_is_file);
        }
    }
}