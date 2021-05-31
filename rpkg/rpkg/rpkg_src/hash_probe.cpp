#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include "rpkg.h"
#include "hash.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>

void rpkg_function::hash_probe(std::string& input_path, std::string& filter, std::string& output_path)
{
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        std::vector<std::string> filters = util::parse_input_filter(filter);

        bool found = false;

        int found_count = 0;

        ss.str(std::string());

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            uint64_t hash = std::strtoull(filters.at(z).c_str(), nullptr, 16);

            if (hash != 0)
            {
                LOG(std::endl << "Searching RPKGs for input filter: " << filters.at(z));

                for (uint64_t i = 0; i < rpkgs.size(); i++)
                {
                    std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(i).hash_map.find(hash);

                    if (it2 != rpkgs.at(i).hash_map.end())
                    {
                        found = true;

                        found_count++;

                        ss << std::endl << filters.at(z) << " is in RPKG file: " << rpkgs.at(i).rpkg_file_name << std::endl;
                        ss << "  - Data offset: " << rpkgs.at(i).hash.at(it2->second).hash_offset << std::endl;
                        ss << "  - Data size: " << (rpkgs.at(i).hash.at(it2->second).hash_size & 0x3FFFFFFF) << std::endl;

                        if (rpkgs.at(i).hash.at(it2->second).is_lz4ed)
                        {
                            ss << "  - LZ4: True" << std::endl;
                        }
                        else
                        {
                            ss << "  - LZ4: False" << std::endl;
                        }

                        if (rpkgs.at(i).hash.at(it2->second).is_xored)
                        {
                            ss << "  - XOR: True" << std::endl;
                        }
                        else
                        {
                            ss << "  - XOR: False" << std::endl;
                        }

                        ss << "  - Resource type: " << rpkgs.at(i).hash.at(it2->second).hash_resource_type << std::endl;
                        ss << "  - Hash reference table size: " << rpkgs.at(i).hash.at(it2->second).hash_reference_table_size << std::endl;
                        ss << "  - Forward hash depends: " << (rpkgs.at(i).hash.at(it2->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF) << std::endl;
                        ss << "  - Final size: " << rpkgs.at(i).hash.at(it2->second).hash_size_final << std::endl;
                        ss << "  - Size in memory: " << rpkgs.at(i).hash.at(it2->second).hash_size_in_memory << std::endl;
                        ss << "  - Size in video memory: " << rpkgs.at(i).hash.at(it2->second).hash_size_in_video_memory << std::endl << std::endl;

                    }
                }

                if (found)
                {
                    LOG("Input filter \"" << filters.at(z) << "\" was found in " << found_count << " RPKG files.");

                    LOG(ss.str());
                }
                else
                {
                    LOG("Input filter \"" << filters.at(z) << "\" was not found in any RPKG files.");
                }
            }
            else
            {
                LOG("Unable to probe RPKG files for \"" << filters.at(z) << "\".");
                LOG("Input filter \"" << filters.at(z) << "\" is not a valid IOI hash identifier.");
                LOG("IOI uses 64 bit hash identifiers for all it's hash files/resources/runtimeids.");
            }
        }
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for hash probe mode does not exist.");
    }
}