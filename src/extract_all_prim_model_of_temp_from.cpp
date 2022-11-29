#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <chrono>
#include <sstream>
#include <filesystem>

void rpkg_function::extract_all_prim_model_of_temp_from(std::string& input_path, const std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    //task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (!file::path_exists(input_path))
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }

    if (!input_path_is_rpkg_file)
    {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }
    else
    {
        //rpkg_function::import_rpkg(input_path);

        const std::filesystem::path base_folder_path = input_path;

        std::string parent_path = base_folder_path.parent_path().string();

        rpkg_function::import_rpkg_files_in_folder(parent_path);
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

    timing_string = "Extracting linked PRIM Models To GLB/TGA File(s)...";

    //std::vector<std::string>().swap(prim_asset_file_names);

    const std::vector<std::string> filters = util::parse_input_filter(filter);

    for (auto& filter : filters)
    {
        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            const uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path != input_path)
                continue;

            auto it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

            if (it != rpkgs.at(rpkg_index).hash_map.end())
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                std::string temp_hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type;

                if (rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type == "TEMP")
                {
                    temp temp_temp(rpkg_index, it->second);

                    for (uint64_t j = 0; j < temp_temp.prim_depends_file_name.size(); j++)
                    {
                        rpkg_function::extract_prim_model_from(rpkgs.at(temp_temp.prim_depends_rpkg_index.at(j).at(temp_temp.prim_depends_rpkg_index_index.at(j))).rpkg_file_path, temp_temp.prim_depends_file_name.at(j), output_path);
                    }
                }
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = PRIM_MODEL_EXTRACT_SUCCESSFUL;
}
