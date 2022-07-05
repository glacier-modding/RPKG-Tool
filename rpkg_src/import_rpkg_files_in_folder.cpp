#include "rpkg_function.h"
#include "file.h"
#include <filesystem>

void rpkg_function::import_rpkg_files_in_folder(std::string& input_rpkg_folder_path)
{
    // std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    //double console_update_rate = 1.0 / 2.0;
    //int period_count = 1;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path))
    {
        //std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        //double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

        //if (time_in_seconds_from_start_time > console_update_rate)
        //{
            //start_time = end_time;

            //if (period_count > 3)
            //{
                //period_count = 0;
            //}

            //std::stringstream ss;

            //ss << "Scanning folder" << std::string(period_count, '.');

            //timing_string = ss.str();

            //LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

            //period_count++;
        //}

        if (std::filesystem::is_regular_file(entry.path().string()))
        {
            std::size_t pos = entry.path().string().find_last_of("\\/");

            std::string rpkg_file_name;
            std::string hash = "";
            std::string hash_resource_type = "";

            if (pos != std::string::npos)
            {
                rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
            }
            else
            {
                rpkg_file_name = entry.path().string();
            }

            if (file::is_rpkg_file(rpkg_file_name))
            {
                std::string rpkg_to_import = entry.path().string();

                rpkg_function::import_rpkg(rpkg_to_import, true);
            }
        }
    }
}