#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <sstream>

void rpkg_function::hash_probe(const std::string& input_path, const std::string& filter)
{
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path))
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for hash probe mode does not exist.");
    }

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::stringstream ss;

    ss << "Scanning folder: Done";

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    const std::vector<std::string> filters = util::parse_input_filter(filter);

    for (auto& filter : filters)
    {
        bool found = false;

        int found_count = 0;

        ss.str(std::string());

        uint64_t hash = std::strtoull(filter.c_str(), nullptr, 16);

        if (hash != 0)
        {
            LOG(std::endl << "Searching RPKGs for input filter: " << filter);

            for (auto& rpkg : rpkgs)
            {
                auto it2 = rpkg.hash_map.find(hash);

                if (it2 == rpkg.hash_map.end())
                    continue;

                found = true;

                found_count++;

                ss << std::endl << filter << " is in RPKG file: " << rpkg.rpkg_file_name << std::endl;
                ss << "  - Data offset: " << rpkg.hash.at(it2->second).data.header.data_offset << std::endl;
                ss << "  - Data size: " << (rpkg.hash.at(it2->second).data.header.data_size & 0x3FFFFFFF) << std::endl;

                if (rpkg.hash.at(it2->second).data.lz4ed)
                {
                    ss << "  - LZ4: True" << std::endl;
                }
                else
                {
                    ss << "  - LZ4: False" << std::endl;
                }

                if (rpkg.hash.at(it2->second).data.xored)
                {
                    ss << "  - XOR: True" << std::endl;
                }
                else
                {
                    ss << "  - XOR: False" << std::endl;
                }

                ss << "  - Resource type: " << rpkg.hash.at(it2->second).hash_resource_type << std::endl;
                ss << "  - Hash reference table size: " << rpkg.hash.at(it2->second).data.resource.reference_table_size << std::endl;
                ss << "  - Forward hash depends: " << (rpkg.hash.at(it2->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF) << std::endl;
                ss << "  - Final size: " << rpkg.hash.at(it2->second).data.resource.size_final << std::endl;
                ss << "  - Size in memory: " << rpkg.hash.at(it2->second).data.resource.size_in_memory << std::endl;
                ss << "  - Size in video memory: " << rpkg.hash.at(it2->second).data.resource.size_in_video_memory << std::endl << std::endl;
            }

            if (found)
            {
                LOG("Input filter \"" << filter << "\" was found in " << found_count << " RPKG files.");

                LOG(ss.str());
            }
            else
            {
                LOG("Input filter \"" << filter << "\" was not found in any RPKG files.");
            }
        }
        else
        {
            LOG("Unable to probe RPKG files for \"" << filter << "\".");
            LOG("Input filter \"" << filter << "\" is not a valid IOI hash identifier.");
            LOG("IOI uses 64 bit hash identifiers for all it's hash files/resources/runtimeids.");
        }
    }
}
