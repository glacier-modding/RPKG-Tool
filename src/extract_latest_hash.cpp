#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <sstream>

void rpkg_function::extract_latest_hash(std::string& input_path, std::string& filter, std::string& output_path) {
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path +
                     " to search for RPKG files for latest hash mode does not exist.");
    }

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::stringstream ss;

    LOG("Scanning folder: Done");

    std::vector<std::string> filters = util::parse_input_filter(filter);

    for (const auto & filter : filters) {
        uint64_t hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        auto it2 = hash_list_hash_map.find(hash_value);

        if (it2 != hash_list_hash_map.end()) {
            LOG("Scanning RPKG files for: " + filter);
            LOG(filter + "'s IOI string is: " + hash_list_hash_strings.at(it2->second));
        } else {
            LOG("Scanning RPKG files for: " + filter);
        }

        uint32_t hash_in_rpkg_index = get_latest_hash(hash_value);

        if (hash_in_rpkg_index != UINT32_MAX) {
            LOG("The latest version of " + filter + " is in: " +
                rpkgs.at(hash_in_rpkg_index).rpkg_file_name);

            rpkg_extraction_vars rpkg_vars;

            rpkg_vars.input_path = rpkgs.at(hash_in_rpkg_index).rpkg_file_path;
            rpkg_vars.filter = util::uint64_t_to_hex_string(hash_value);
            rpkg_vars.output_path = output_path;

            extract_from_rpkg_with_map(rpkg_vars);

            LOG("Extracted " + filter + " from " + rpkgs.at(hash_in_rpkg_index).rpkg_file_name + " to " +
                output_path);
        }
    }
}