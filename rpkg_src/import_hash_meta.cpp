#include "rpkg_function.h"
#include "file.h"
#include "hash.h"
#include "global.h"
#include <iostream>
#include <fstream>
#include <string>

void rpkg_function::import_hash_meta(hash& meta_data, std::string& hash_meta_file_path)
{
    std::ifstream meta_file = std::ifstream(hash_meta_file_path, std::ifstream::binary);

    if (!meta_file.good())
    {
        LOG_AND_EXIT("Error: Hash meta file " + hash_meta_file_path + " could not be read.");
    }

    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    char input[1024];

    meta_file.seekg(0, meta_file.beg);

    meta_file.read(input, sizeof(bytes8));
    std::memcpy(&bytes8, input, sizeof(bytes8));
    meta_data.hash_value = bytes8;

    meta_file.read(input, sizeof(bytes8));
    std::memcpy(&bytes8, input, sizeof(bytes8));
    meta_data.data.header.data_offset = bytes8;

    meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.header.data_size = bytes4;

    if ((meta_data.data.header.data_size & 0x3FFFFFFF) != 0)
    {
        meta_data.data.lz4ed = true;
    }
    else
    {
        meta_data.data.lz4ed = false;
    }

    if ((meta_data.data.header.data_size & 0x80000000) == 0x80000000)
    {
        meta_data.data.xored = true;
    }
    else
    {
        meta_data.data.xored = false;
    }

    meta_file.read(input, sizeof(bytes4));
    input[4] = 0x0;
    meta_data.hash_resource_type = std::string(input);

    meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.reference_table_size = bytes4;

    meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.reference_table_dummy = bytes4;

    meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_final = bytes4;

    meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_in_memory = bytes4;

    meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_in_video_memory = bytes4;

    hash_reference_variables temp_hash_reference_data;

    if (meta_data.data.resource.reference_table_size != 0x0)
    {
        temp_hash_reference_data.hash_value = meta_data.hash_value;

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        temp_hash_reference_data.hash_reference_count = bytes4;

        uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        for (uint64_t j = 0; j < temp_hash_reference_count; j++)
        {
            meta_file.read(input, sizeof(bytes1));
            std::memcpy(&bytes1, input, sizeof(bytes1));
            temp_hash_reference_data.hash_reference_type.push_back(bytes1);
        }

        for (uint64_t j = 0; j < temp_hash_reference_count; j++)
        {
            meta_file.read(input, sizeof(bytes8));
            std::memcpy(&bytes8, input, sizeof(bytes8));
            temp_hash_reference_data.hash_reference.push_back(bytes8);
        }
    }
    else
    {
        temp_hash_reference_data.hash_reference_count = 0x0;
    }

    meta_data.hash_reference_data = temp_hash_reference_data;

    meta_file.close();
}