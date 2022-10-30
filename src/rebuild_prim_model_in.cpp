#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <set>

void rpkg_function::rebuild_prim_model_in(std::string& input_path, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        std::string filter_string = "";

        LOG("Rebuilding PRIM files in" + input_rpkg_folder_path);

        rpkg_function::rebuild_prim_in(input_path, false);

        if (task_multiple_status != PRIM_REBUILD_SUCCESSFUL)
        {
            return;
        }

        LOG("Rebuilding TEXT/TEXD files in" + input_rpkg_folder_path);

        rpkg_function::rebuild_text_in(input_path, filter_string, output_path, false);

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        std::vector<std::string> rebuilt_folders;
        std::vector<std::string> rpkg_file_names;

        std::string input_folder = input_rpkg_folder_path;

        std::string rpkgs_path = input_rpkg_folder_path + "RPKGS";

        file::create_directories(rpkgs_path);

        std::filesystem::path input_folder_path = input_rpkg_folder_path;

        if (input_folder.substr(input_folder.length() - 1, 1) == "\\")
        {
            input_folder = input_folder.substr(0, input_folder.length() - 1);
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                timing_string = ss.str();

                LOG_NO_ENDL("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

                period_count++;
            }

            if (!std::filesystem::is_directory(entry.path().string()))
                continue;

            input_folder = entry.path().string();

            if (input_folder.substr(input_folder.length() - 1, 1) == "\\")
            {
                input_folder = input_folder.substr(0, input_folder.length() - 1);
            }

            if (input_folder.length() <= 7 || util::to_upper_case(input_folder.substr(input_folder.length() - 7, 7)) != "REBUILT" || !file::path_exists(input_folder + "\\rpkgfilename.txt"))
                continue;

            std::ifstream rpkg_file_name_file = std::ifstream(input_folder + "\\rpkgfilename.txt", std::ifstream::binary);

            if (!rpkg_file_name_file.good())
            {
                LOG_AND_EXIT("Error: RPKG file name file " + input_folder + "\\rpkgfilename.txt" + " could not be opened.");
            }

            rpkg_file_name_file.seekg(0, rpkg_file_name_file.end);

            uint32_t rpkg_file_name_file_size = rpkg_file_name_file.tellg();

            rpkg_file_name_file.seekg(0, rpkg_file_name_file.beg);

            char input[1024];

            rpkg_file_name_file.read(input, rpkg_file_name_file_size);

            input[rpkg_file_name_file_size] = 0;

            rpkg_file_name_file.close();

            std::string rpkg_output_file_name_string = std::string(input);

            if (rpkg_output_file_name_string != "")
            {
                bool prim_folder_found = false;

                for (auto& rebuilt_folder : rebuilt_folders)
                {
                    if (input_folder == rebuilt_folder)
                    {
                        prim_folder_found = true;
                    }
                }

                if (!prim_folder_found)
                {
                    rebuilt_folders.push_back(input_folder);

                    rpkg_file_names.push_back(rpkg_output_file_name_string);
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        std::string message = "Rebuilding PRIM MODEL files...";

        timing_string = message;

        std::set<std::string> directory_set;

        for (uint64_t i = 0; i < rebuilt_folders.size(); i++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((i * (uint64_t)100000) / rebuilt_folders.size()) % (uint64_t)10 == 0 && i > 0)
            {
                stringstream_length = console::update_console(message, rebuilt_folders.size(), i, start_time, stringstream_length);
            }

            std::vector<std::string> rebuilt_file_names;
            std::vector<std::string> rebuilt_file_paths;

            for (const auto& entry : std::filesystem::directory_iterator(rebuilt_folders.at(i)))
            {
                if (std::filesystem::is_regular_file(entry.path().string()))
                {
                    std::size_t pos = entry.path().string().find_last_of("\\/");

                    std::string file_name = "";
                    std::string hash_file_name = "";
                    std::string hash_string = "";
                    std::string resource_type = "";
                    std::string rpkg_name = "";

                    if (pos != std::string::npos)
                    {
                        file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    }
                    else
                    {
                        file_name = entry.path().string();
                    }

                    if (file_name.length() > 5)
                    {
                        std::string file_extension = util::to_upper_case(file_name.substr((file_name.length() - 5), 5));

                        if (file_extension == ".PRIM")
                        {
                            rebuilt_file_names.push_back(file_name);
                            rebuilt_file_paths.push_back(entry.path().string());
                        }
                        else if (file_extension == ".TEXT")
                        {
                            rebuilt_file_names.push_back(file_name);
                            rebuilt_file_paths.push_back(entry.path().string());
                        }
                        else if (file_extension == ".TEXD")
                        {
                            rebuilt_file_names.push_back(file_name);
                            rebuilt_file_paths.push_back(entry.path().string());
                        }
                        else if (file_extension == ".META")
                        {
                            rebuilt_file_names.push_back(file_name);
                            rebuilt_file_paths.push_back(entry.path().string());
                        }
                    }
                }
            }

            for (uint64_t j = 0; j < rebuilt_file_names.size(); j++)
            {
                std::cout << rebuilt_file_names.at(j) << std::endl;

                directory_set.insert(rpkgs_path + "\\" + rpkg_file_names.at(i));

                file::create_directories(rpkgs_path + "\\" + rpkg_file_names.at(i));

                std::filesystem::copy(rebuilt_file_paths.at(j), rpkgs_path + "\\" + rpkg_file_names.at(i), std::filesystem::copy_options::overwrite_existing);
            }
        }

        for (std::set<std::string>::iterator it = directory_set.begin(); it != directory_set.end(); it++)
        {
            std::string input_path = *it;
            std::string filter = "";
            std::string output_path_string = rpkgs_path;

            //std::cout << *it << std::endl;
            //std::cout << output_path_string << std::endl;

            rpkg_function::generate_rpkg_from(input_path, output_path_string);
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;;

        task_multiple_status = PRIM_MODEL_REBUILD_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to rebuild the PRIMs does not exist.");
    }
}