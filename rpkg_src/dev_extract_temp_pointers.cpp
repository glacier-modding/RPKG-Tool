#include "dev_function.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "temp.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <string_view>

void dev_function::dev_extract_temp_pointers(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    initialize_enum_map_h2();
    initialize_enum_map_h3();

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path))
    {
        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path, true);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        //LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        //LOG("Loading Hash List: Done");

        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    //temp temp_temp(rpkg_index, it->second);

                    //std::multimap<uint32_t, uint32_t>::iterator itlower = temp_temp.pointers_multimap.upper_bound(0x650000);

                    //for (std::multimap<uint32_t, uint32_t>::iterator itmm = temp_temp.pointers_multimap.begin(); itmm != itlower; itmm++)
                    //{
                        //std::cout << (*itmm).first << ", " << (*itmm).second << std::endl;
                    //}

                    //uint32_t temp_position = temp_temp.temp_footer_offset;

                    //uint32_t crc32_header_value = 0;

                    //uint32_t pointers_size = 0;
                    //uint32_t pointers_count = 0;
                    //std::vector<uint32_t> pointers;
                    //std::map<uint32_t, uint32_t> pointers_map;

                    //uint32_t bytes4 = 0;

                    //bool header_found = false;

                    /*while (crc32_header_value != 0x12EBA5ED && temp_position < temp_temp.temp_data.size())
                    {
                        std::memcpy(&crc32_header_value, &temp_temp.temp_data.data()[temp_position], sizeof(bytes4));

                        if (crc32_header_value != 0x12EBA5ED)
                        {
                            temp_position += 0x4;
                            std::memcpy(&bytes4, &temp_temp.temp_data.data()[temp_position], sizeof(bytes4));
                            temp_position += 0x4;
                            temp_position += bytes4;
                        }
                        else
                        {
                            header_found = true;
                        }
                    }

                    temp_position += 0x4;

                    std::memcpy(&pointers_size, &temp_temp.temp_data.data()[temp_position], sizeof(bytes4));
                    temp_position += 0x4;

                    std::cout << "Pointer table size: " << pointers_size << std::endl;

                    std::memcpy(&pointers_count, &temp_temp.temp_data.data()[temp_position], sizeof(bytes4));
                    temp_position += 0x4;

                    std::cout << "Pointer table count: " << pointers_count << std::endl;

                    for (uint64_t p = 0; p < pointers_count; p++)
                    {
                        std::memcpy(&bytes4, &temp_temp.temp_data.data()[temp_position], sizeof(bytes4));
                        temp_position += 0x4;

                        pointers.push_back(bytes4);

                        pointers_map[pointers.back()] = pointers_map.size();

                        std::cout << "Pointer (" << p << "): 0x" << util::uint32_t_to_hex_string(pointers.back()) << std::endl;
                    }*/
                }
            }
        }

        rpkgs.erase(rpkgs.begin() + 1);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {

        }

        std::vector<temp>().swap(temps);

    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}