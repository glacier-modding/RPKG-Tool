#include "dev_function.h"
#include "../global.h"
#include "../crypto.h"
#include "../thirdparty/lz4/lz4.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void dev_function::dev_extract_ores_strings(std::string& input_path, std::string& filter, std::string& output_path)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "ORES")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    std::string hash_file_name = rpkgs.at(i).hash.at(hash_index).hash_file_name;

                    if (hash_file_name == "00858D45F5F9E3CA.ORES")
                    {
                        uint64_t hash_size;

                        if (rpkgs.at(i).hash.at(hash_index).is_lz4ed == 1)
                        {
                            hash_size = rpkgs.at(i).hash.at(hash_index).hash_size;

                            if (rpkgs.at(i).hash.at(hash_index).is_xored == 1)
                            {
                                hash_size &= 0x3FFFFFFF;
                            }
                        }
                        else
                        {
                            hash_size = rpkgs.at(i).hash.at(hash_index).hash_size_final;
                        }

                        std::vector<char> input_data(hash_size, 0);

                        std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                        if (!file.good())
                        {
                            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkgs.at(i).hash.at(hash_index).hash_offset, file.beg);
                        file.read(input_data.data(), hash_size);
                        file.close();

                        if (rpkgs.at(i).hash.at(hash_index).is_xored == 1)
                        {
                            crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                        }

                        uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).hash_size_final;

                        std::vector<char> output_data(decompressed_size, 0);

                        std::vector<char>* ores_data;

                        if (rpkgs.at(i).hash.at(hash_index).is_lz4ed)
                        {
                            LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                            ores_data = &output_data;
                        }
                        else
                        {
                            ores_data = &input_data;
                        }

                        std::vector<uint64_t> ores_hash_resource;
                        std::vector<std::string> ores_hash_resource_file_path;
                        std::vector<std::vector<std::string>> ores_hash_resource_file_path_elements;

                        uint32_t ores_hash_resource_file_count = 0;

                        uint32_t position = 0x10;

                        uint8_t bytes1 = 0;
                        uint32_t bytes4 = 0;
                        uint64_t bytes8 = 0;

                        std::memcpy(&bytes4, (&ores_data->data()[0] + position), sizeof(bytes4));
                        position = bytes4 + 0xC;

                        std::memcpy(&ores_hash_resource_file_count, (&ores_data->data()[0] + position), sizeof(bytes4));
                        position += 0x4;

                        // std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
                        // int stringstream_length = 80;

                        for (uint64_t k = 0; k < ores_hash_resource_file_count; k++)
                        {
                            uint32_t string_offset = 0;
                            uint8_t string_length = 0;
                            uint64_t hash_lsb = 0;
                            uint64_t hash_msb = 0;
                            uint64_t hash = 0;

                            std::memcpy(&string_length, (&ores_data->data()[0] + position), sizeof(bytes1));
                            position += 0x8;

                            std::memcpy(&string_offset, (&ores_data->data()[0] + position), sizeof(bytes4));
                            string_offset += 0x10;
                            position += 0x8;

                            std::memcpy(&hash_lsb, (&ores_data->data()[0] + position), sizeof(bytes4));
                            position += 0x4;

                            std::memcpy(&hash_msb, (&ores_data->data()[0] + position), sizeof(bytes4));
                            position += 0x4;

                            hash = (hash_lsb << 32) | hash_msb;

                            //std::cout << main_data->console_prefix << "String length: " << string_length << std::endl;
                            //std::cout << main_data->console_prefix << "String offset: " << string_offset << std::endl;

                            bool found = false;

                            for (uint64_t i = 0; i < rpkgs.size(); i++)
                            {
                                if (!found)
                                {
                                    std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(i).hash_map.find(hash);

                                    if (it2 != rpkgs.at(i).hash_map.end())
                                    {
                                        found = true;

                                        std::cout << rpkgs.at(i).hash.at(it2->second).hash_file_name << "," << std::string(&ores_data->data()[0] + string_offset) << std::endl;
                                    }
                                }
                            }

                            if (!found)
                            {
                                std::cout << hash_file_name << "," << std::string(&ores_data->data()[0] + string_offset) << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}