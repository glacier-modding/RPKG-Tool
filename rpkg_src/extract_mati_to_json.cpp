#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "mati.h"
#include "thirdparty/lz4/lz4.h"
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <iostream>

void rpkg_function::extract_mati_to_json(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (!std::filesystem::is_regular_file(input_path))
    {
        LOG_AND_RETURN("Error: The input file path " + input_path + " is not a valid file.");
    }

    if (file::path_exists(input_path))
    {
        log_output = false;

        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        rpkg_function::import_rpkg(input_path, true);

        std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t text_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            for (uint64_t i = 0; i < rpkgs.size(); i++)
            {
                uint64_t rpkg_index = i;

                if (rpkgs.at(i).rpkg_file_path == input_path)
                {
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(text_hash_value);

                    if (it != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        timing_string = "Converting: " + rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + " to MATI JSON";

                        mati temp_mati(rpkg_index, it->second);

                        temp_mati.generate_json(output_path);
                    }
                }
            }
        }

        log_output = true;

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The RPKG file " + input_path + " does not exist.");
    }
}