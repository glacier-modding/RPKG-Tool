#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include <unordered_map>
#include <chrono>

// just because you don't know it msvc doesn't mean it doesn't exist
#pragma warning(disable: 4068)

// we have to be recursive, no other way to do this
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

void recursive_hash_depends_search(uint64_t hash_value, std::vector<uint64_t>& hashes_to_extract,
                                   std::unordered_map<uint64_t, uint64_t>& hashes_to_extract_map)
{
    for (auto& rpkg : rpkgs)
    {
        auto it = rpkg.hash_map.find(hash_value);

        if (it == rpkg.hash_map.end())
            continue;

        const uint32_t hash_reference_count =
                rpkg.hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        for (uint32_t j = 0; j < hash_reference_count; j++)
        {
            auto it2 = hashes_to_extract_map.find(rpkg.hash.at(it->second).hash_reference_data.hash_reference.at(j));

            if (it2 != hashes_to_extract_map.end())
                continue;

            hashes_to_extract.push_back(rpkg.hash.at(it->second).hash_reference_data.hash_reference.at(j));

            hashes_to_extract_map[rpkg.hash.at(it->second).hash_reference_data.hash_reference.at(
                    j)] = hashes_to_extract.size();

            recursive_hash_depends_search(rpkg.hash.at(it->second).hash_reference_data.hash_reference.at(j),
                                          hashes_to_extract, hashes_to_extract_map);
        }
    }
}

#pragma clang diagnostic pop

void
rpkg_function::extract_all_hash_depends_from(std::string& input_path, std::string& unparsed_filters,
                                             std::string& output_path,
                                             HashExtractionStrategy strategy)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::vector<std::string> filters = util::parse_input_filter(unparsed_filters);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    for (auto& filter : filters)
    {
        bool filter_found = false;

        std::string filter_hash_file_name;

        for (auto& rpkg : rpkgs)
        {
            auto it = rpkg.hash_map.find(std::strtoull(filter.c_str(), nullptr, 16));

            if (it == rpkg.hash_map.end())
                continue;

            filter_found = true;

            filter_hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(it->second).hash_value) + "." +
                                    rpkg.hash.at(it->second).hash_resource_type;
        }

        if (!filter_found)
            continue;

        std::vector<uint64_t> hashes_to_extract;
        std::unordered_map<uint64_t, uint64_t> hashes_to_extract_map;

        uint64_t hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        hashes_to_extract.push_back(hash_value);

        hashes_to_extract_map[hash_value] = hashes_depends_map.size();

        recursive_hash_depends_search(hash_value, hashes_to_extract, hashes_to_extract_map);

        std::string message;

        if (strategy == HashExtractionStrategy::PRIMS_ONLY)
        {
            message = "Extracting all hash depends and PRIM models for " + filter + ": ";
        }
        else
        {
            message = "Extracting all hash depends for " + filter + ": ";
        }

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        for (uint64_t x = 0; x < hashes_to_extract.size(); x++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((x * static_cast<uint64_t>(100000)) / hashes_to_extract.size()) % static_cast<uint64_t>(100) == 0 &&
                x > 0)
            {
                stringstream_length = console::update_console(message, hashes_to_extract.size(), x, start_time,
                                                              stringstream_length);
            }

            for (auto& rpkg1 : rpkgs)
            {
                auto it1 = rpkg1.hash_map.find(hashes_to_extract.at(x));

                if (!(it1 != rpkg1.hash_map.end()))
                    continue;

                if (strategy == HashExtractionStrategy::PRIMS_ONLY)
                {
                    if (rpkg1.hash.at(it1->second).hash_resource_type == "PRIM")
                    {
                        std::string prim_output_dir = file::output_path_append(
                                "ALLDEPENDS\\" + filter_hash_file_name + "\\PRIMMODELS\\", output_path);

                        file::create_directories(prim_output_dir);

                        std::string hash_filter = util::uint64_t_to_hex_string(hashes_to_extract.at(x));
                        rpkg_function::extract_prim_model_from(rpkg1.rpkg_file_path, hash_filter, prim_output_dir);
                    }
                    // we only want prims, either way, we're done with this item
                    continue;
                }

                bool condition = true;

                const bool is_boot_chunk = rpkg1.rpkg_file_path.find("chunk0.rpkg") == std::string::npos &&
                                           rpkg1.rpkg_file_path.find("chunk0patch") == std::string::npos;
                const bool is_base_chunk = rpkg1.rpkg_file_path.find("chunk1.rpkg") == std::string::npos &&
                                           rpkg1.rpkg_file_path.find("chunk1patch") == std::string::npos;

                if (strategy == HashExtractionStrategy::ALL_NON_BASE)
                {
                    condition = !(is_boot_chunk || is_base_chunk);
                }
                else if (strategy == HashExtractionStrategy::ALL_NON_BOOT)
                {
                    condition = !is_boot_chunk;
                }

                if (!condition)
                {
                    continue;
                }

                rpkg_extraction_vars rpkg_vars;

                rpkg_vars.input_path = rpkg1.rpkg_file_path;
                rpkg_vars.filter = util::uint64_t_to_hex_string(hashes_to_extract.at(x));
                rpkg_vars.output_path = file::output_path_append("ALLDEPENDS\\" + filter_hash_file_name + "\\",
                                                                 output_path);

                extract_from_rpkg_with_map(rpkg_vars);
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}
