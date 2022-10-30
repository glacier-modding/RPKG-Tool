#include "rpkg_function.h"
#include "global.h"
#include <fstream>

void rpkg_function::extract_hash_meta(const uint64_t i, const uint64_t j, std::string& final_path)
{
    std::vector<char> meta_data;

    // pre-allocate the memory to speed up the loops
    meta_data.reserve(sizeof(hash::hash_value));

    for (int k = 0; k < sizeof(hash::hash_value); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).hash_value) + k));
    }

    for (int k = 0; k < sizeof(hash::data.header.data_offset); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.header.data_offset) + k));
    }

    for (int k = 0; k < sizeof(hash::data.header.data_size); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.header.data_size) + k));
    }

    for (int k = 0; k < rpkgs.at(i).hash.at(j).hash_resource_type.length(); k++)
    {
        meta_data.push_back(*(rpkgs.at(i).hash.at(j).hash_resource_type.data() + k));
    }

    for (int k = 0; k < sizeof(hash::data.resource.reference_table_size); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.resource.reference_table_size) + k));
    }

    for (int k = 0; k < sizeof(hash::data.resource.reference_table_dummy); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.resource.reference_table_dummy) + k));
    }

    for (int k = 0; k < sizeof(hash::data.resource.size_final); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.resource.size_final) + k));
    }

    for (int k = 0; k < sizeof(hash::data.resource.size_in_memory); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.resource.size_in_memory) + k));
    }

    for (int k = 0; k < sizeof(hash::data.resource.size_in_video_memory); k++)
    {
        meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).data.resource.size_in_video_memory) + k));
    }

    if (rpkgs.at(i).hash.at(j).data.resource.reference_table_size > 0)
    {
        uint32_t hash_reference_table_size_count = 0;
        const uint32_t temp_hash_reference_count = rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        for (int k = 0; k < sizeof(hash_reference_variables::hash_reference_count); k++)
        {
            meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_count) + k));
        }
        hash_reference_table_size_count += sizeof(hash_reference_variables::hash_reference_count);

        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
        {
            for (int l = 0; l < sizeof(decltype(hash_reference_variables::hash_reference_type)::value_type); l++)
            {
                meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_type.at(k)) + l));
            }
            hash_reference_table_size_count += sizeof(decltype(hash_reference_variables::hash_reference_type)::value_type);
        }

        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
        {
            for (int l = 0; l < sizeof(decltype(hash_reference_variables::hash_reference)::value_type); l++)
            {
                meta_data.push_back(*(reinterpret_cast<char*>(&rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference.at(k)) + l));
            }
            hash_reference_table_size_count += sizeof(decltype(hash_reference_variables::hash_reference)::value_type);
        }

        if (hash_reference_table_size_count != rpkgs.at(i).hash.at(j).data.resource.reference_table_size)
        {
            LOG_AND_EXIT("Error: Hash meta data for " + util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(j).hash_value) + " is corrupt.");
        }
    }

    final_path += ".meta";

    std::ofstream output_file = std::ofstream(final_path, std::ofstream::binary);

    if (!output_file.good())
    {
        LOG_AND_EXIT("Error: Meta data file " + final_path + " could not be created.");
    }

    output_file.write(meta_data.data(), meta_data.size());

    output_file.close();
}