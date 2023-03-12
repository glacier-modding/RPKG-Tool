#include "dev_function.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "generic_function.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <unordered_set>

void dev_function::dev_extract_all_entities_to_qn(std::string& input_path, const std::string& output_path) {
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path)) {
        if (!hash_list_loaded) {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        force_load_hash_list();

        std::unordered_set<uint64_t> entities;

        for (auto& rpkg : rpkgs) {
            for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
                if (rpkg.hash_resource_types.at(r) != "TEMP")
                    continue;

                for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                    uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                    if (entities.find(rpkg.hash.at(hash_index).hash_value) != entities.end())
                        continue;

                    uint32_t rpkg_index = rpkg_function::get_latest_hash(rpkg.hash.at(hash_index).hash_value);

                    if (rpkg_index == UINT32_MAX)
                        continue;

                    auto it6 = rpkgs.at(rpkg_index).hash_map.find(rpkg.hash.at(hash_index).hash_value);

                    if (it6 == rpkgs.at(rpkg_index).hash_map.end())
                        continue;

                    std::string temp_output_path = file::output_path_append(rpkgs.at(rpkg_index).rpkg_file_name, output_path);
                    std::string hash_string = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value);
                    file::create_directories(temp_output_path);
                    temp_output_path = file::output_path_append(hash_string + ".entity.json", temp_output_path);

                    std::cout << "Converting " << hash_string << " entity (TEMP/TBLU) to QN (QuickEntity) JSON...";

                    entity temp_entity(rpkg_index, it6->second, 3, temp_output_path);
                    temp_entity.free_yyjson_doc();

                    entities.insert(rpkg.hash.at(hash_index).hash_value);
                }
            }
        }
    }
    else {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path +
            " to search for RPKG files for latest hash mode does not exist.");
    }
}