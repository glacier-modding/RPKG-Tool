#include "dev_function.h"
#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include "thirdparty/json/json.hpp"

void dev_function::dev_dlge_names(std::string& input_path, std::string& filter, std::string& output_path)
{
    input_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_path);

    nlohmann::ordered_json json;

    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(true);
        LOG("Loading Hash List: Done");
    }

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "DLGE")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    uint32_t rpkg_index = rpkg_function::get_latest_hash(rpkgs.at(i).hash.at(hash_index).hash_value);

                    if (rpkg_index != UINT32_MAX)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(rpkgs.at(i).hash.at(hash_index).hash_value);

                        if (it != rpkgs.at(rpkg_index).hash_map.end())
                        {
                            uint64_t hash_index = rpkgs.at(rpkg_index).hash.at(it->second).hash_value;

                            std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type;

                            nlohmann::ordered_json temp_json = nlohmann::ordered_json::array();

                            for (uint32_t h = 0; h < rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.size(); h++)
                            {
                                temp_json.push_back(util::hash_to_ioi_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(h), true));
                            }

                            json[util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value)] = temp_json;
                        }
                    }
                }
            }
        }
    }

    std::ofstream json_file = std::ofstream(file::output_path_append("DLGEs.JSON", output_path), std::ofstream::binary);

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}