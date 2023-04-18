#include "rpkg_function.h"
#include "generic_function.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <sstream>

void rpkg_function::search_entities(std::string& input_path,
                                    char** search_strings,
                                    int* search_types,
                                    int* search_categories,
                                    int search_strings_count,
                                    int max_results,
                                    bool store_jsons,
                                    bool use_latest_hashes) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    percent_progress = (uint32_t) 0;
    log_output = false;
    //rpkg_function::import_rpkg_files_in_folder("H:\\HITMAN3\\Runtime");
    //input_path = "";
    std::string().swap(entities_search_results);

    force_load_hash_list();

    uint32_t results_count = 0;

    uint64_t entities_hash_count = 0;

    for (auto & rpkg : rpkgs) {
        std::string message = "Searching " + rpkg.rpkg_file_name + "...";

        if (!input_path.empty() && input_path != rpkg.rpkg_file_path) continue;

        for (uint64_t r1 = 0; r1 < rpkg.hash_resource_types.size(); r1++) {
            if (gui_control == ABORT_CURRENT_TASK) {
                return;
            }

            if (rpkg.hash_resource_types.at(r1) == "TEMP") {
                entities_hash_count += rpkg.hashes_indexes_based_on_resource_types.at(r1).size();
            }
        }
    }

    uint64_t entities_hash_count_current = 0;

    std::unordered_map<uint64_t, uint64_t> hash_searched;

    if (!store_jsons) {
        for (auto entity : deep_search_entities_map)
            entity.second.free_yyjson_doc();
        std::unordered_map<uint64_t, entity>().swap(deep_search_entities_map);
    }

    std::vector<entity::search_item> search_items;

    for (int i = 0; i < search_strings_count; i++)
    {
        search_items.emplace_back();

        search_items.back().category = (entity::search_category)search_categories[i];

        if (search_types[i] == (int)entity::search_type::DEFAULT) {
            search_items.back().type = entity::search_type::DEFAULT;
            search_items.back().search = util::to_lower_case(search_strings[i]);
        }
        else if (search_types[i] == (int)entity::search_type::REGEX) {
            search_items.back().type = entity::search_type::REGEX;
            search_items.back().search = search_strings[i];
        }
    }

    for (uint64_t i = 0; i < rpkgs.size(); i++) {
        LOG("Searching " + rpkgs.at(i).rpkg_file_name + "...");

        if (input_path.empty() || input_path == rpkgs.at(i).rpkg_file_path) {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++) {
                if (gui_control == ABORT_CURRENT_TASK) {
                    return;
                }

                if (results_count >= max_results || rpkgs.at(i).hash_resource_types.at(r) != "TEMP")
                    continue;

                for (uint64_t j2 = 0; j2 < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j2++) {
                    if (gui_control == ABORT_CURRENT_TASK) {
                        return;
                    }

                    if (results_count >= max_results)
                        break;

                    if (results_count < max_results) {
                        uint64_t hashIndex = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j2);

                        uint64_t tempHashValue = rpkgs.at(i).hash.at(hashIndex).hash_value;

                        uint32_t rpkgIndex = rpkg_function::get_latest_hash(tempHashValue);

                        if (rpkgIndex == UINT32_MAX)
                            continue;

                        if (use_latest_hashes && (uint64_t) i != rpkgIndex)
                            continue;

                        auto it61 = rpkgs.at(rpkgIndex).hash_map.find(tempHashValue);

                        if (!(it61 != rpkgs.at(rpkgIndex).hash_map.end()))
                            continue;

                        entities_hash_count_current++;

                        auto it4 = hash_searched.find(tempHashValue);

                        if (!(it4 == hash_searched.end()))
                            continue;

                        if (store_jsons) {
                            auto it = deep_search_entities_map.find(tempHashValue);

                            if (it == deep_search_entities_map.end()) {
                                timing_string = "Loading entity " + util::hash_to_ioi_string(
                                        rpkgs.at(rpkgIndex).hash.at(it61->second).hash_value, true) + "...";
                                std::string empty = "";
                                deep_search_entities_map.emplace(tempHashValue,
                                                                 entity(rpkgIndex, it61->second, 3, empty));
                            }

                            results_count = deep_search_entities_map[tempHashValue].search(search_items,
                                                                                           results_count,
                                                                                           max_results);
                        } else {
                            std::string empty = "";
                            entity temp_entity(rpkgIndex, it61->second, 3, empty);

                            results_count = temp_entity.search(search_items,
                                                               results_count,
                                                               max_results);
                            temp_entity.free_yyjson_doc();
                        }

                        hash_searched[tempHashValue] = hash_searched.size();
                    }
                }
            }
        }
    }

    LOG("Done");

    percent_progress = (uint32_t) 100;

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
