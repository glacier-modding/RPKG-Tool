#include "generic_function.h"
#include "global.h"
#include "file.h"
#include <fstream>
#include <iostream>

void generic_function::load_hash_list(bool exit_if_no_hash_list, std::string path)
{
    if (!hash_list_loaded)
    {
        std::string hash_path = (path == "" ? (exe_path + std::string("\\hash_list.txt")) : path);
        std::ifstream hash_list_file = std::ifstream(hash_path, std::ifstream::binary);

        if (file::path_exists(hash_path))
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
                LOG_AND_EXIT("\nError: hash_list.txt not found. Download the lastest hash_list.txt from https://hitmandb.glaciermodding.org/latest-hashes.7z\n");
            }
            else
            {
                LOG_AND_RETURN("\nError: hash_list.txt not found. Download the lastest hash_list.txt from https://hitmandb.glaciermodding.org/latest-hashes.7z\n");
            }
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

        while (position < hash_list_data.size())
        {
            if (hash_list_data[position] == 0x0A)
            {
                line_count++;

                hash_list_data[position] = 0x0;

                if (line_count == 3)
                {
                    std::string_view temp_string_view = std::string_view(reinterpret_cast<char*>(&hash_list_data[last_position]));

                    size_t pos = temp_string_view.find_first_of('=');
                    hash_list_version = std::stoi(std::string(temp_string_view.substr((pos + 1))));
                }
                else if (line_count > 3)
                {
                    std::string_view temp_string_view = std::string_view(reinterpret_cast<char*>(&hash_list_data[last_position]));

                    size_t pos = temp_string_view.find_first_of(',');
                    hash_list_hash_file_names.push_back(std::string(temp_string_view.substr(0, pos)));
                    hash_list_hash_value_strings.push_back(std::string(temp_string_view.substr(0, (pos - 5))));
                    hash_list_hash_strings.push_back(std::string(temp_string_view.substr(pos + 1, temp_string_view.length() - (pos + 1))));
                    hash_list_hash_map[std::strtoull(std::string(temp_string_view.substr(0, (pos - 5))).c_str(), nullptr, 16)] = hash_list_hash_file_names.size() - 1;
                }

                last_position = position + 1;
            }

            position++;
        }

        hash_list_loaded = true;
    }
}
