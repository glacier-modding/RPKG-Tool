#include "generic_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include <fstream>
#include <iostream>

void generic_function::load_hash_list()
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
            LOG_AND_EXIT("\nError: hash_list.txt not found. Download the lastest hash_list.txt from hitmandb.notex.app/latest-hashes.7z\n");
        }

        hash_list_file.seekg(0, hash_list_file.end);

        uint64_t hash_list_file_size = (uint64_t)hash_list_file.tellg();

        hash_list_file.seekg(0, hash_list_file.beg);

        std::vector<char> hash_list_data(hash_list_file_size, 0);

        hash_list_file.read(hash_list_data.data(), hash_list_file_size);

        bool done = false;
        uint64_t position = 0;
        uint64_t last_position = 0;
        uint64_t line_count = 0;

        while (!done)
        {
            if (hash_list_data.data()[position] == 0x0A)
            {
                line_count++;

                hash_list_data.data()[position] = 0x0;

                if (line_count == 3)
                {
                    std::string temp_hash_string = std::string(reinterpret_cast<char*>(&hash_list_data.data()[last_position]));

                    size_t pos = temp_hash_string.find_first_of('=');

                    std::string hash_list_version_string = temp_hash_string.substr((pos + 1));

                    hash_list_version = std::stoi(hash_list_version_string);
                }
                else if (line_count > 3)
                {
                    std::string temp_hash_string = std::string(reinterpret_cast<char*>(&hash_list_data.data()[last_position]));

                    size_t pos = temp_hash_string.find_first_of(',');

                    temp_hash_string = util::to_upper_case(temp_hash_string);

                    std::string hash = temp_hash_string.substr(0, (pos - 5));

                    std::string hash_file_name = temp_hash_string.substr(0, pos);

                    std::string hash_string = util::to_lower_case(temp_hash_string.substr(pos + 1, temp_hash_string.length() - (pos + 1)));

                    hash_list_hash_file_names.push_back(hash_file_name);

                    hash_list_hash_value_strings.push_back(hash);

                    hash_list_hash_strings.push_back(hash_string);

                    hash_list_hash_map[std::strtoull(hash.c_str(), nullptr, 16)] = hash_list_hash_file_names.size() - 1;
                }

                last_position = position + 1;
            }

            position++;

            if (position > hash_list_file_size)
            {
                done = true;
            }
        }

        hash_list_loaded = true;
    }
}