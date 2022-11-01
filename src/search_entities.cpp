#include "rpkg_function.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>

void rpkg_function::search_entities(std::string& input_path, std::string& search, bool search_entity_ids,
                                    bool search_entity_names, bool search_property_names,
                                    bool search_property_values, int max_results)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    percent_progress = (uint32_t)0;

    log_output = false;

    entities_search_results = "";

    uint32_t results_count = 0;

    uint64_t entities_hash_count = 0;
    uint64_t entities_hash_size_total = 0;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::string message = "Searching " + rpkgs.at(i).rpkg_file_name + "...";

        if (input_path.empty() || input_path == rpkgs.at(i).rpkg_file_path)
        {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                if (rpkgs.at(i).hash_resource_types.at(r) == "TEMP")
                {
                    for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                    {
                        uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                        uint64_t temp_hash_value = rpkgs.at(i).hash.at(hash_index).hash_value;

                        uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_hash_value);

                        if (rpkg_index == UINT32_MAX)
                            continue;

                        if (!input_path.empty() && input_path != rpkgs.at(rpkg_index).rpkg_file_path)
                            continue;

                        auto it6 = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                        if (it6 != rpkgs.at(rpkg_index).hash_map.end())
                        {
                            entities_hash_size_total += rpkgs.at(rpkg_index).hash.at(it6->second).data.resource.size_final;
                            entities_hash_count++;
                        }
                    }
                }
            }
        }
    }

    uint64_t entities_hash_count_current = 0;
    uint64_t entities_hash_size_current = 0;

    std::unordered_map<uint64_t, uint64_t> hash_searched;

    std::string search_lower_case = util::to_lower_case(search);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;
                
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::string message = "Searching " + rpkgs.at(i).rpkg_file_name + "...";

        if (input_path.empty() || input_path == rpkgs.at(i).rpkg_file_path)
        {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                if (results_count >= max_results || rpkgs.at(i).hash_resource_types.at(r) != "TEMP")
                    continue;

                for (uint64_t j2 = 0; j2 < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j2++)
                {
                    if (gui_control == ABORT_CURRENT_TASK)
                    {
                        return;
                    }

                    if (results_count < max_results)
                    {
                        uint64_t hashIndex = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j2);

                        uint64_t tempHashValue = rpkgs.at(i).hash.at(hashIndex).hash_value;

                        uint32_t rpkgIndex = rpkg_function::get_latest_hash(tempHashValue);

                        if (rpkgIndex == UINT32_MAX ||
                            !(input_path.empty() || input_path == rpkgs.at(rpkgIndex).rpkg_file_path))
                            continue;

                        auto it61 = rpkgs.at(rpkgIndex).hash_map.find(tempHashValue);

                        if (!(it61 != rpkgs.at(rpkgIndex).hash_map.end()))
                            continue;

                        if (((entities_hash_count_current * (uint64_t)100000) / (uint64_t)entities_hash_count) % (uint64_t)10 == 0 && entities_hash_count_current > 0)
                        {
                            stringstream_length = console::update_console(message, entities_hash_size_total, entities_hash_size_current, start_time, stringstream_length);
                        }

                        entities_hash_size_current += rpkgs.at(rpkgIndex).hash.at(it61->second).data.resource.size_final;
                        entities_hash_count_current++;

                        auto it4 = hash_searched.find(tempHashValue);

                        if (!(it4 == hash_searched.end()))
                            continue;

                        auto it = deep_search_entities_map.find(tempHashValue);

                        if (it == deep_search_entities_map.end())
                        {
                            deep_search_entities_map.emplace(tempHashValue, entity(rpkgIndex, it61->second, 3));

                            results_count = deep_search_entities_map[tempHashValue].search(search_lower_case, search_entity_ids, search_entity_names, search_property_names, search_property_values, results_count, max_results);
                        }
                        else
                        {
                            results_count = deep_search_entities_map[tempHashValue].search(search_lower_case, search_entity_ids, search_entity_names, search_property_names, search_property_values, results_count, max_results);
                        }

                        hash_searched[tempHashValue] = hash_searched.size();
                    }
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        std::stringstream ss;

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));
    }

    percent_progress = (uint32_t)100;

    log_output = true;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
    /*}
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }*/
}