#include "rpkg_function.h"
#include "global.h"
#include <iostream>
#include <fstream>
#include <sstream>

void rpkg_function::extract_hash_meta(uint64_t i, uint64_t j, std::string& final_path)
{
    std::vector<char> meta_data;
    char char1[1];
    char char4[4];
    char char8[8];

    std::memcpy(&char8, &rpkgs.at(i).hash.at(j).hash_value, sizeof(uint64_t));
    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
    {
        meta_data.push_back(char8[k]);
    }

    std::memcpy(&char8, &rpkgs.at(i).hash.at(j).hash_offset, sizeof(uint64_t));
    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
    {
        meta_data.push_back(char8[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_size, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_resource_type, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_reference_table_size, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_reference_table_dummy, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_size_final, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_size_in_memory, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_size_in_video_memory, sizeof(uint32_t));
    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
    {
        meta_data.push_back(char4[k]);
    }

    if (rpkgs.at(i).hash.at(j).hash_reference_table_size > 0)
    {
        uint32_t hash_reference_table_size_count = 0;
        uint32_t temp_hash_reference_count = rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        std::memcpy(&char4, &rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_count, sizeof(uint32_t));
        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
        {
            meta_data.push_back(char4[k]);
        }
        hash_reference_table_size_count += 4;

        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
        {
            std::memcpy(&char1, &rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_type.at(k), sizeof(uint8_t));
            for (uint64_t l = 0; l < sizeof(uint8_t); l++)
            {
                meta_data.push_back(char1[l]);
            }
            hash_reference_table_size_count += 1;
        }

        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
        {
            std::memcpy(&char8, &rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference.at(k), sizeof(uint64_t));
            for (uint64_t l = 0; l < sizeof(uint64_t); l++)
            {
                meta_data.push_back(char8[l]);
            }
            hash_reference_table_size_count += 8;
        }

        if (hash_reference_table_size_count != rpkgs.at(i).hash.at(j).hash_reference_table_size)
        {
            LOG_AND_EXIT("Error: Hash meta data for " + rpkgs.at(i).hash.at(j).hash_string + " is corrupt.");
        }
    }

    final_path += ".meta";
    std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

    if (!output_file.good())
    {
        LOG_AND_EXIT("Error: Meta data file " + final_path + " could not be created.");
    }

    output_file.write(meta_data.data(), meta_data.size());

    output_file.close();
}