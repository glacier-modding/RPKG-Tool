#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "rpkg.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <filesystem>

void rpkg_function::latest_hash(std::string& input_path, std::string& filter, std::string& output_path)
{
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        std::vector<std::string> filters = util::parse_input_filter(filter);

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            uint64_t hash_value = std::strtoull(filters.at(z).c_str(), nullptr, 16);

            std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(hash_value);

            if (it2 != hash_list_hash_map.end())
            {
                LOG("Scanning RPKG files for: " + filters.at(z));
                LOG(filters.at(z) + "'s IOI string is: " + hash_list_hash_strings.at(it2->second));
            }
            else
            {
                LOG("Scanning RPKG files for: " + filters.at(z));
            }

            uint32_t hash_in_rpkg_index = get_latest_hash(hash_value);

            if (hash_in_rpkg_index != UINT32_MAX)
            {
                LOG("The latest version of " + filters.at(z) + " is in: " + rpkgs.at(hash_in_rpkg_index).rpkg_file_name);

                if (z != filters.size() - 1)
                {
                    LOG("");
                }
            }
        }
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for latest hash mode does not exist.");
    }
}