#include "rpkg_function.h"
#include "global.h"
#include "file.h"
#include "mrtr.h"
#include <iostream>
#include <filesystem>

void rpkg_function::mrtr_to_json(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    log_output = false;

    if (file::path_exists(input_path))
    {
        bool output_path_is_file = false;

        std::vector<std::filesystem::path> files;

        if (std::filesystem::is_regular_file(input_path))
        {
            files.push_back(std::filesystem::path(input_path));

            if (output_path != "")
            {
                if (std::filesystem::exists(output_path))
                {
                    if (std::filesystem::is_regular_file(output_path))
                    {
                        output_path_is_file = true;
                    }
                }
                else
                {
                    output_path_is_file = true;
                }
            }
        }
        else
        {
            files = file::get_recursive_file_list(input_path);
        }

        //LOG("Loading Hash List...");
        //generic_function::load_hash_list(true);
        //LOG("Loading Hash List: Done");

        for (std::filesystem::path& file : files)
        {
            uint64_t hash_value = file::get_hash_value_from_path(file, ".mrtr");

            if (hash_value)
            {
                timing_string = "Converting: " + file.filename().string() + " to MRTR JSON";

                LOG(timing_string);

                if (output_path == "")
                {
                    mrtr temp_mrtr(file.string(), "", hash_value, file.parent_path().string(), output_path_is_file);
                }
                else
                {
                    mrtr temp_mrtr(file.string(), "", hash_value, output_path, output_path_is_file);
                }
            }
        }
    }

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}