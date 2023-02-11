#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "hash.h"
#include <iostream>
#include <sstream>

void rpkg_function::hash_depends(std::string& input_path, std::string& filter) {
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path +
                     " to search for RPKG files for hash depends mode does not exist.");
    }

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::stringstream ss;

    ss << "Scanning folder: Done";

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    std::vector<std::string> filters = util::parse_input_filter(filter);

    for (auto& filter : filters) {
        std::vector<hash_depends_variables> hash_depends_data;

        uint64_t hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++) {
            auto it2 = rpkgs.at(i).hash_map.find(hash_value);

            hash_depends_variables temp_hash_depends_data;

            temp_hash_depends_data.rpkg_file_name = rpkgs.at(i).rpkg_file_name;

            if (it2 != rpkgs.at(i).hash_map.end()) {
                temp_hash_depends_data.hash_value = hash_value;

                temp_hash_depends_data.hash_string = filter;

                uint32_t temp_hash_reference_count =
                        rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                LOG(filter << " has " << temp_hash_reference_count << " dependencies in "
                           << rpkgs.at(i).rpkg_file_name);

                if (temp_hash_reference_count > 0) {
                    for (uint64_t k = 0; k < temp_hash_reference_count; k++) {
                        std::vector<std::string> dependency_in_rpkg_file;

                        bool found = false;

                        for (uint64_t j = 0; j < rpkgs.size(); j++) {
                            auto it3 = rpkgs.at(j).hash_map.find(
                                    rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference.at(k));

                            if (it3 != rpkgs.at(j).hash_map.end()) {
                                if (!found) {
                                    temp_hash_depends_data.hash_dependency_file_name.push_back(
                                            util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it3->second).hash_value) +
                                            "." + rpkgs.at(j).hash.at(it3->second).hash_resource_type);
                                }

                                found = true;

                                dependency_in_rpkg_file.push_back(rpkgs.at(j).rpkg_file_name);
                            }
                        }

                        if (!found) {
                            temp_hash_depends_data.hash_dependency_file_name.push_back(util::uint64_t_to_hex_string(
                                    rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference.at(k)));
                        }

                        temp_hash_depends_data.hash_dependency_map[hash_value] = temp_hash_depends_data.hash_dependency_map.size();

                        temp_hash_depends_data.hash_dependency.push_back(util::uint64_t_to_hex_string(hash_value));

                        temp_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
                    }
                }

                hash_depends_data.push_back(temp_hash_depends_data);
            }
        }

        int rpkg_dependency_count = 0;

        for (auto& i : hash_depends_data) {
            if (!i.hash_dependency.empty()) {
                rpkg_dependency_count++;
            }
        }

        LOG(filter << " has dependencies in " << rpkg_dependency_count << " RPKG files:" << std::endl);

        for (auto& i : hash_depends_data) {
            if (!i.hash_dependency.empty()) {
                LOG(filter << " depends on " << i.hash_dependency_file_name.size()
                           << " other hash files/resources in RPKG file: " <<
                           i.rpkg_file_name);

                if (!i.hash_dependency_file_name.empty()) {
                    LOG(filter << "'s dependencies:");

                    for (uint64_t j = 0; j < i.hash_dependency_file_name.size(); j++) {
                        LOG("Hash file/resource: " << i.hash_dependency_file_name.at(j));

                        auto it2 = hash_list_hash_map.find(
                                std::strtoull(i.hash_dependency_file_name.at(j).c_str(), nullptr, 16));

                        if (it2 != hash_list_hash_map.end()) {
                            LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
                        } else {
                            LOG("  - IOI String: ");
                        }

                        if (!i.hash_dependency_in_rpkg.at(j).empty()) {
                            LOG_NO_ENDL("  - Found in RPKG files: ");

                            for (uint64_t k = 0; k < i.hash_dependency_in_rpkg.at(j).size(); k++) {
                                LOG_NO_ENDL(i.hash_dependency_in_rpkg.at(j).at(k));

                                if (k < i.hash_dependency_in_rpkg.at(j).size() - 1) {
                                    LOG_NO_ENDL(", ");
                                }
                            }

                            LOG_NO_ENDL(std::endl);
                        } else {
                            LOG("  - Found in RPKG files: None");
                        }
                    }
                }

                LOG_NO_ENDL(std::endl);
            }
        }

        ss.str(std::string());

        uint32_t reverse_dependency_count = 0;

        std::vector<std::string> reverse_dependency;
        std::vector<std::vector<std::string>> reverse_dependency_in_rpkg_file;

        for (auto& rpkg : rpkgs) {
            for (uint64_t j = 0; j < rpkg.hash.size(); j++) {
                uint32_t temp_hash_reference_count =
                        rpkg.hash.at(j).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint64_t k = 0; k < temp_hash_reference_count; k++) {
                    if (rpkg.hash.at(j).hash_reference_data.hash_reference.at(k) != hash_value)
                        continue;

                    reverse_dependency_count++;

                    std::string rd = util::uint64_t_to_hex_string(rpkg.hash.at(j).hash_value) + "." +
                                     rpkg.hash.at(j).hash_resource_type;

                    if (!reverse_dependency.empty()) {
                        bool found = false;

                        for (uint64_t k = 0; k < reverse_dependency.size(); k++) {
                            if (reverse_dependency.at(k) == rd) {
                                found = true;

                                reverse_dependency_in_rpkg_file.at(k).push_back(rpkg.rpkg_file_name);
                            }
                        }

                        if (!found) {
                            reverse_dependency.push_back(rd);

                            std::vector<std::string> rpkgFiles;

                            rpkgFiles.push_back(rpkg.rpkg_file_name);

                            reverse_dependency_in_rpkg_file.push_back(rpkgFiles);
                        }
                    } else {
                        reverse_dependency.push_back(rd);

                        std::vector<std::string> rpkgFiles;

                        rpkgFiles.push_back(rpkg.rpkg_file_name);

                        reverse_dependency_in_rpkg_file.push_back(rpkgFiles);
                    }
                }
            }
        }

        LOG(filter << " has reverse dependencies in " << reverse_dependency.size() << " RPKG files:" << std::endl);

        if (reverse_dependency.empty())
            continue;

        LOG(filter << "'s reverse dependencies:");

        for (uint64_t i = 0; i < reverse_dependency.size(); i++) {
            LOG("Hash file/resource: " << reverse_dependency.at(i));

            auto it2 = hash_list_hash_map.find(std::strtoull(reverse_dependency.at(i).c_str(), nullptr, 16));

            if (it2 != hash_list_hash_map.end()) {
                LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
            } else {
                LOG("  - IOI String: ");
            }

            if (!reverse_dependency_in_rpkg_file.at(i).empty()) {
                LOG_NO_ENDL("  - Found in RPKG files: ");

                for (uint64_t j = 0; j < reverse_dependency_in_rpkg_file.at(i).size(); j++) {
                    LOG_NO_ENDL(reverse_dependency_in_rpkg_file.at(i).at(j));

                    if (j < reverse_dependency_in_rpkg_file.at(i).size() - 1) {
                        LOG_NO_ENDL(", ");
                    }
                }

                LOG_NO_ENDL(std::endl);
            } else {
                LOG("  - Found in RPKG files: None");
            }
        }
    }
}
