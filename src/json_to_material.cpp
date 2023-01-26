#include "rpkg_function.h"
#include "global.h"
#include "generic_function.h"
#include "file.h"
#include "material.h"
#include <iostream>
#include <filesystem>

void rpkg_function::json_to_material(const std::string& input_path, const std::string& output_path)
{
    if (!file::path_exists(input_path))
    {
        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
        return;
    }

    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    bool output_path_is_file = false;

    std::vector<std::filesystem::path> files;

    if (std::filesystem::is_regular_file(input_path))
    {
        files.push_back(std::filesystem::path(input_path));
    }
    else
    {
        files = file::get_recursive_file_list(input_path);
    }

    LOG("Loading Hash List...");
    generic_function::load_hash_list(true);
    LOG("Loading Hash List: Done");

    for (std::filesystem::path& file : files)
    {
        if (file::has_extension(file.string(), ".material.json"))
        {
            timing_string = "Converting: " + file.filename().string() + " to Material files (MATI, MATT, MATB)";

            LOG(timing_string);

            if (output_path.empty())
            {
                material temp_material(file.string(), file.parent_path().string());
            }
            else
            {
                material temp_material(file.string(), output_path);
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}