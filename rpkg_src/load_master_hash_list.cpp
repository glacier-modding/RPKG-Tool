#include "generic_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include <fstream>
#include <iostream>

void generic_function::load_hash_list(bool exit_if_no_hash_list)
{
    if (!hash_list_loaded)
    {
        std::ifstream hash_list_file = std::ifstream("hash_list.txt", std::ifstream::binary);

        if (file::path_exists("hash_list.txt"))
        {
            if (!hash_list_file.good())
            {
                LOG_AND_EXIT("Error: hash_list.txt could not be read.");
            }
        }
        else
        {
            if (exit_if_no_hash_list)
            {
                LOG_AND_EXIT("\nError: hash_list.txt not found. Download the lastest hash_list.txt from hitmandb.notex.app/latest-hashes.7z\n");
            }
            else
            {
                LOG_AND_RETURN("\nError: hash_list.txt not found. Download the lastest hash_list.txt from hitmandb.notex.app/latest-hashes.7z\n");
            }
        }

        hash_list_file.seekg(0, hash_list_file.end);

        uint64_t hash_list_file_size = hash_list_file.tellg();

        hash_list_file.seekg(0, hash_list_file.beg);

        std::vector<char> hash_list_data(hash_list_file_size, 0);

        hash_list_file.read(hash_list_data.data(), hash_list_file_size);

        const auto* file_data = hash_list_data.data();
    	
        // Count approximate number of hashes.
        uint64_t approximate_hash_count = 0;

    	for (uint64_t i = 0; i < hash_list_file_size; ++i)
    	{
            if (file_data[i] == '\n')
            {
                ++approximate_hash_count;
            }
    	}

    	// Reverse enough slots in the hash lists so we avoid unnecessary re-allocations.
        hash_list_hash_file_names.reserve(approximate_hash_count);
        hash_list_hash_value_strings.reserve(approximate_hash_count);
        hash_list_hash_strings.reserve(approximate_hash_count);
    	
        bool done = false;
        uint64_t position = 0;
        uint64_t last_position = 0;
        uint64_t line_count = 0;

        while (!done)
        {
            if (file_data[position] == '\n')
            {
                line_count++;

                if (line_count == 3)
                {
                    std::string_view temp_hash_string(&file_data[last_position], position - last_position);

                    size_t pos = temp_hash_string.find_first_of('=');

                    std::string_view hash_list_version_string = temp_hash_string.substr((pos + 1));

                    hash_list_version = std::stoi(std::string(hash_list_version_string));
                }
                else if (line_count > 3)
                {
                    std::string_view temp_hash_string(&file_data[last_position], position - last_position);

                    size_t pos = temp_hash_string.find_first_of(',');

                    std::string_view hash = temp_hash_string.substr(0, (pos - 5));

                    std::string_view hash_file_name = temp_hash_string.substr(0, pos);

                    std::string_view hash_string = temp_hash_string.substr(pos + 1, temp_hash_string.length() - (pos + 1));

                    std::string hash_str = std::string(hash);
                	
                    hash_list_hash_file_names.push_back(std::string(hash_file_name));

                    hash_list_hash_value_strings.push_back(hash_str);

                    hash_list_hash_strings.push_back(std::string(hash_string));

                    hash_list_hash_map[std::strtoull(hash_str.c_str(), nullptr, 16)] = hash_list_hash_file_names.size() - 1;
                }

                last_position = position + 1;
            }

            position++;

            if (position >= hash_list_file_size)
            {
                done = true;
            }
        }

        hash_list_loaded = true;
    }
}