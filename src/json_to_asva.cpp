#include "rpkg_function.h"
#include "global.h"
#include "generic_function.h"
#include "file.h"
#include "asva.h"
#include <filesystem>

void rpkg_function::json_to_asva(const std::string& input_path, const std::string& output_path) {
    if (!file::path_exists(input_path)) {
        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
        return;
    }

    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::vector<std::filesystem::path> files;

    if (std::filesystem::is_regular_file(input_path)) {
        files.push_back(std::filesystem::path(input_path));
    } else {
        files = file::get_recursive_file_list(input_path);
    }

    force_load_hash_list();

    for (std::filesystem::path& file : files) {
        if (file::has_extension(file.string(), ".asva.json")) {
            timing_string = "Converting: " + file.filename().string() + " to ASVA (+ ASVA.meta)";

            LOG(timing_string);

            if (output_path.empty()) {
                asva temp_asva(file.string(), file.parent_path().string());
            } else {
                asva temp_asva(file.string(), output_path);
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}