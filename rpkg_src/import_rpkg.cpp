#include "rpkg_function.h"
#include "file_reader.hpp"
#include "stream_reader.hpp"
#include "global.h"
#include "file.h"
#include "util.h"
#include "hash.h"
#include "console.h"
#include <iostream>
#include <fstream>
#include <sstream>

void rpkg_function::import_rpkg(std::string& rpkg_file_path, bool with_timing)
{
    bool found = false;

    task_single_status = TASK_EXECUTING;

    for (uint64_t j = 0; j < rpkgs.size(); j++)
    {
        if (rpkgs.at(j).rpkg_file_path == rpkg_file_path)
        {
            found = true;
        }
    }

    if (found)
    {
        task_single_status = TASK_SUCCESSFUL;

        return;
    }

    // Emplace back new RPKG in rpkgs
    rpkgs.emplace_back();
    rpkgs.back().rpkg_file_path = rpkg_file_path;
    rpkgs.back().rpkg_file_name = file::get_base_file_name(rpkg_file_path);

    // Load RPKG file with FileReader
    FileReader rpkg_file(rpkg_file_path);

    // Check for empty file
    if (rpkg_file.Size() <= 0x4)
    {
        LOG_AND_RETURN("Error: " + rpkg_file_path + " is a empty RPKG file.");
    }

    // Determine RPKG file version
    char magic[4];
    rpkg_file.Read<char[4]>(&magic);
    if (magic[0] == 'G' && magic[1] == 'K' && magic[2] == 'P' && magic[3] == 'R')
        rpkgs.back().rpkg_file_version = 1;
    else if (magic[0] == '2' && magic[1] == 'K' && magic[2] == 'P' && magic[3] == 'R')
    {
        rpkgs.back().rpkg_file_version = 2;
        rpkgs.back().rpkgv2_header.resize(9);
        rpkg_file.Read<char>(rpkgs.back().rpkgv2_header.data(), 9);
    }
    else
    {
        LOG_AND_RETURN("Error: " + rpkg_file_path + " is not a valid RPKG file.");
    }

    // Read in RPKG main header
    rpkg_file.Read<rpkg::Header>(&rpkgs.back().header);

    // Record patch offset for later, needed if RPKG is a patch file
    uint64_t patch_offset = rpkg_file.Position();

    // Check for empty RPKG file
    if ((rpkgs.back().rpkg_file_version == 1 && rpkg_file.Size() <= 0x14) || (rpkgs.back().rpkg_file_version == 2 && rpkg_file.Size() <= 0x1D))
    {
        LOG_AND_RETURN("Error: " + rpkg_file_path + " is a empty RPKG file.");
    }

    // Perform tests to determine if RPKG is a patch file or not
    if (rpkgs.back().rpkg_file_version == 1 && (rpkgs.back().header.patch_count * 8 + 0x24) >= rpkg_file.Size())
        rpkgs.back().is_patch_file = false;
    else if (rpkgs.back().rpkg_file_version == 2 && (rpkgs.back().header.patch_count * 8 + 0x2D) >= rpkg_file.Size())
        rpkgs.back().is_patch_file = false;
    else
    {
        if (rpkgs.back().rpkg_file_version == 1)
            rpkg_file.SeekTo(rpkgs.back().header.patch_count * 8 + 0x1B);
        else
            rpkg_file.SeekTo(rpkgs.back().header.patch_count * 8 + 0x24);

        uint8_t test_zero_value;
        rpkg_file.Read<uint8_t>(&test_zero_value);
        uint64_t test_header_offset;
        rpkg_file.Read<uint64_t>(&test_header_offset);

        if (rpkgs.back().rpkg_file_version == 1 && test_header_offset == (rpkgs.back().header.hash_header_table_size + rpkgs.back().header.hash_resource_table_size + rpkgs.back().header.patch_count * 8 + 0x14) && test_zero_value == 0)
            rpkgs.back().is_patch_file = true;
        else if (test_header_offset == (rpkgs.back().header.hash_header_table_size + rpkgs.back().header.hash_resource_table_size + rpkgs.back().header.patch_count * 8 + 0x1D) && test_zero_value == 0)
            rpkgs.back().is_patch_file = true;
    }

    // Read in the patch list if RPKG is a patch file with a non zero patch list
    if (rpkgs.back().is_patch_file)
    {
        if (rpkgs.back().header.patch_count > 0)
        {
            rpkg_file.SeekTo(patch_offset);
            rpkgs.back().patch_entry_list.resize(rpkgs.back().header.patch_count);
            rpkg_file.Read<uint64_t>(rpkgs.back().patch_entry_list.data(), rpkgs.back().header.patch_count);
        }
    }

    std::string import_text = "Importing RPKG file: " + rpkgs.back().rpkg_file_name;
    LOG_NO_ENDL(import_text);

    // Seek to the hash data table's offset
    size_t hash_data_offset = 0x10;
    if (rpkgs.back().rpkg_file_version == 2)
        hash_data_offset += 9;
    if (rpkgs.back().is_patch_file)
        hash_data_offset += rpkgs.back().header.patch_count * 8 + 4;

    rpkg_file.SeekTo(hash_data_offset);

    // Read both RPKG's hash tables at once into a temporary char buffer
    size_t hash_tables_combined_size = rpkgs.back().header.hash_header_table_size + rpkgs.back().header.hash_resource_table_size;
    std::vector<char> hash_tables(hash_tables_combined_size, 0);
    rpkg_file.Read<char>(hash_tables.data(), hash_tables_combined_size);

    // Load RPKG's hash tables in StreamReader
    StreamReader hash_tables_stream(hash_tables.data(), hash_tables.size());

    // Init timer
    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    // Read in the hash files/resources data tables
    for (size_t i = 0; i < rpkgs.back().header.hash_count; i++)
    {
        // Emplace back new hash in rpkg
        rpkgs.back().hash.emplace_back();

        hash_tables_stream.Read<hash::HashHeader>(&rpkgs.back().hash.back().data.header);

        rpkgs.back().hash_map[rpkgs.back().hash.back().data.header.hash] = (uint64_t)rpkgs.back().hash_map.size();
    }

    // Emplace back the hash depends map
    hashes_depends_map.emplace_back();

    // Read in the hash files/resources info tables
    for (size_t i = 0; i < rpkgs.back().header.hash_count; i++)
    {
        //HashInfo hash_info;
        hash_tables_stream.Read<hash::HashResource>(&rpkgs.back().hash[i].data.resource);

        // Reverse the hash's resource type string
        char type[4];
        std::memcpy(&type, rpkgs.back().hash[i].data.resource.resource_type, 4);
        rpkgs.back().hash[i].data.resource.resource_type[0] = type[3];
        rpkgs.back().hash[i].data.resource.resource_type[1] = type[2];
        rpkgs.back().hash[i].data.resource.resource_type[2] = type[1];
        rpkgs.back().hash[i].data.resource.resource_type[3] = type[0];

        // Determine hash's size and if it is LZ4ed and/or XORed
        if ((rpkgs.back().hash[i].data.header.data_size & 0x3FFFFFFF) != 0)
        {
            rpkgs.back().hash[i].data.lz4ed = true;
            rpkgs.back().hash[i].data.size = rpkgs.back().hash[i].data.header.data_size;

            if ((rpkgs.back().hash[i].data.header.data_size & 0x80000000) == 0x80000000)
            {
                rpkgs.back().hash[i].data.size &= 0x3FFFFFFF;
                rpkgs.back().hash[i].data.xored = true;
            }
        }
        else
        {
            rpkgs.back().hash[i].data.size = rpkgs.back().hash[i].data.resource.size_final;

            if ((rpkgs.back().hash[i].data.header.data_size & 0x80000000) == 0x80000000)
                rpkgs.back().hash[i].data.xored = true;
        }

        rpkgs.back().hash[i].hash_value = rpkgs.back().hash[i].data.header.hash;
        rpkgs.back().hash[i].hash_resource_type = std::string(rpkgs.back().hash[i].data.resource.resource_type, 4);

        if (rpkgs.back().hash_resource_types.size() > 0)
        {
            bool found = false;

            for (uint32_t j = 0; j < rpkgs.back().hash_resource_types.size(); j++)
            {
                if (rpkgs.back().hash_resource_types.at(j) == rpkgs.back().hash[i].hash_resource_type)
                {
                    found = true;
                    rpkgs.back().hash_resource_types_data_size.at(j) += rpkgs.back().hash[i].data.size;
                    rpkgs.back().hashes_indexes_based_on_resource_types.at(j).push_back(i);
                    rpkgs.back().hashes_based_on_resource_types.at(j).push_back(rpkgs.back().hash[i].data.header.hash);
                }
            }

            if (!found)
            {
                rpkgs.back().hash_resource_types.push_back(rpkgs.back().hash[i].hash_resource_type);
                rpkgs.back().hash_resource_types_data_size.push_back(rpkgs.back().hash[i].data.size);
                std::vector<uint64_t> temp_hashes_indexes_based_on_resource_types;
                temp_hashes_indexes_based_on_resource_types.push_back(i);
                rpkgs.back().hashes_indexes_based_on_resource_types.push_back(temp_hashes_indexes_based_on_resource_types);
                std::vector<uint64_t> temp_hashes_based_on_resource_types;
                temp_hashes_based_on_resource_types.push_back(rpkgs.back().hash[i].data.header.hash);
                rpkgs.back().hashes_based_on_resource_types.push_back(temp_hashes_based_on_resource_types);
            }
        }
        else
        {
            rpkgs.back().hash_resource_types.push_back(rpkgs.back().hash[i].hash_resource_type);
            rpkgs.back().hash_resource_types_data_size.push_back(rpkgs.back().hash[i].data.size);
            std::vector<uint64_t> temp_hashes_indexes_based_on_resource_types;
            temp_hashes_indexes_based_on_resource_types.push_back(i);
            rpkgs.back().hashes_indexes_based_on_resource_types.push_back(temp_hashes_indexes_based_on_resource_types);
            std::vector<uint64_t> temp_hashes_based_on_resource_types;
            temp_hashes_based_on_resource_types.push_back(rpkgs.back().hash[i].data.header.hash);
            rpkgs.back().hashes_based_on_resource_types.push_back(temp_hashes_based_on_resource_types);
        }

        if (rpkgs.back().hash[i].data.resource.reference_table_size > 0)
        {
            uint32_t depends_count;
            hash_tables_stream.Read<uint32_t>(&depends_count);
            rpkgs.back().hash[i].hash_reference_data.hash_reference_count = depends_count;
            depends_count &= 0x3FFFFFFF;

            rpkgs.back().hash[i].hash_reference_data.hash_reference_type.resize(depends_count);
            hash_tables_stream.Read<uint8_t>(rpkgs.back().hash[i].hash_reference_data.hash_reference_type.data(), depends_count);

            rpkgs.back().hash[i].hash_reference_data.hash_reference.resize(depends_count);
            hash_tables_stream.Read<uint64_t>(rpkgs.back().hash[i].hash_reference_data.hash_reference.data(), depends_count);

            rpkgs.back().hash[i].hash_reference_data.hash_value = rpkgs.back().hash[i].data.header.hash;

            for (uint64_t j = 0; j < depends_count; j++)
            {
                std::unordered_map<uint64_t, uint64_t>::iterator it = hashes_depends_map.back().find(rpkgs.back().hash[i].hash_reference_data.hash_reference.back());

                if (it == hashes_depends_map.back().end())
                {
                    hashes_depends_map.back()[rpkgs.back().hash[i].hash_reference_data.hash_reference.back()] = hashes_depends_map.back().size();
                }
            }
        }
        else
        {
            rpkgs.back().hash[i].hash_reference_data.hash_reference_count = 0x0;
        }
    }

    hashes_depends_map_rpkg_file_paths.push_back(rpkgs.back().rpkg_file_path);

    if (with_timing)
    {
        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        std::stringstream ss;

        ss << "completed in " << std::fixed << std::setprecision(6) << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG(std::string((72 - import_text.length() - ss.str().length()), '.') + ss.str());

        percent_progress = (uint32_t)100;
    }

    task_single_status = TASK_SUCCESSFUL;
}