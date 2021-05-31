#include "rpkg_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "hash.h"
#include "console.h"
#include <iostream>
#include <fstream>
#include <sstream>

void rpkg_function::import_rpkg(std::string& rpkg_file_path)
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

    std::ifstream file = std::ifstream(rpkg_file_path, std::ifstream::binary);
    char input[1024];
    char bytes1 = 0;
    uint32_t bytes4 = 0, file_count = 0, table_offset = 0, table_size = 0, patch_entry_count = 0;
    uint64_t bytes8 = 0, offset1 = 0, offset2 = 0, rpkg_file_size = 0;
    std::string message = "Importing RPKG file data: ";

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkg_file_path + " could not be read.");
    }

    file.seekg(0, file.end);
    rpkg_file_size = file.tellg();
    file.seekg(0, file.beg);

    if (rpkg_file_size <= 0x4)
    {
        LOG_AND_RETURN("Error: " + rpkg_file_path + " is not a valid RPKG file.");
    }

    rpkg temp_rpkg;

    temp_rpkg.rpkg_file_path = rpkg_file_path;
    temp_rpkg.rpkg_file_name = file::get_base_file_name(rpkg_file_path);

    file.read(input, 4);
    input[4] = 0;

    if (std::string(input) == "GKPR")
    {
        temp_rpkg.rpkg_file_version = 1;

        LOG("Valid RPKGv1 file magic signature found.");
    }
    else if (std::string(input) == "2KPR")
    {
        temp_rpkg.rpkg_file_version = 2;

        LOG("Valid RPKGv2 file magic signature found.");
    }
    else
    {
        LOG_AND_RETURN("Error: " + rpkg_file_path + " is not a valid RPKG file.");
    }

    if ((temp_rpkg.rpkg_file_version == 1 && rpkg_file_size <= 0x14) || (temp_rpkg.rpkg_file_version == 2 && rpkg_file_size <= 0x1D))
    {
        LOG_AND_RETURN("Error: " + rpkg_file_path + " is a empty RPKG file.");
    }

    if (temp_rpkg.rpkg_file_version == 2)
    {
        file.read(input, sizeof(bytes4));

        file.read(input, sizeof(bytes1));
        std::memcpy(&bytes1, input, sizeof(bytes1));
        temp_rpkg.rpkgv2_chunk_number = bytes1;

        file.read(input, sizeof(bytes4));
    }

    file.read(input, sizeof(file_count));
    std::memcpy(&file_count, input, sizeof(file_count));
    temp_rpkg.rpkg_file_count = file_count;

    file.read(input, sizeof(table_offset));
    std::memcpy(&table_offset, input, sizeof(table_offset));
    temp_rpkg.rpkg_table_offset = table_offset;

    file.read(input, sizeof(table_size));
    std::memcpy(&table_size, input, sizeof(table_size));
    temp_rpkg.rpkg_table_size = table_size;

    uint64_t position = file.tellg();

    file.read(input, sizeof(patch_entry_count));
    std::memcpy(&patch_entry_count, input, sizeof(patch_entry_count));

    temp_rpkg.patch_entry_count = patch_entry_count;

    if (temp_rpkg.rpkg_file_version == 1 && ((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14 + (uint64_t)0x10) >= rpkg_file_size)
    {
        temp_rpkg.is_patch_file = false;

        LOG("RPKGv1 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
    }
    else if (temp_rpkg.rpkg_file_version == 2 && ((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D + (uint64_t)0x10) >= rpkg_file_size)
    {
        temp_rpkg.is_patch_file = false;

        LOG("RPKGv2 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
    }
    else
    {
        char patch_zero_test = 0;
        uint64_t patch_value = 0;

        if (temp_rpkg.rpkg_file_version == 1)
        {
            file.seekg(((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14), file.beg);
        }
        else
        {
            file.seekg(((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D), file.beg);
        }

        file.read(input, 0x7);
        file.read(input, sizeof(bytes1));
        std::memcpy(&patch_zero_test, input, sizeof(bytes1));
        file.read(input, sizeof(bytes8));
        std::memcpy(&patch_value, input, sizeof(bytes8));

        if (temp_rpkg.rpkg_file_version == 1 && patch_value == ((uint64_t)table_offset + (uint64_t)table_size + (uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14) && patch_zero_test == 0x0)
        {
            temp_rpkg.is_patch_file = true;

            LOG("RPKGv1 file " << temp_rpkg.rpkg_file_name << " is a patch file.");
        }
        else if (temp_rpkg.rpkg_file_version == 2 && patch_value == ((uint64_t)table_offset + (uint64_t)table_size + (uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D) && patch_zero_test == 0x0)
        {
            temp_rpkg.is_patch_file = true;

            LOG("RPKGv2 file " << temp_rpkg.rpkg_file_name << " is a patch file.");
        }
        else
        {
            temp_rpkg.is_patch_file = false;

            if (temp_rpkg.rpkg_file_version == 1)
            {
                LOG("RPKGv1 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
            }
            else
            {
                LOG("RPKGv2 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
            }
        }
    }

    file.seekg(position, file.beg);

    uint64_t input_file_size = 0;

    if (temp_rpkg.is_patch_file)
    {
        if (temp_rpkg.rpkg_file_version == 1)
        {
            LOG("Importing index from RPKGv1 file: " << temp_rpkg.rpkg_file_name);
        }
        else
        {
            LOG("Importing index from RPKGv2 file: " << temp_rpkg.rpkg_file_name);
        }

        file.read(input, sizeof(patch_entry_count));
        std::memcpy(&patch_entry_count, input, sizeof(patch_entry_count));
        temp_rpkg.patch_entry_count = patch_entry_count;

        for (uint64_t i = 0; i < patch_entry_count; i++)
        {
            file.read(input, sizeof(bytes8));
            std::memcpy(&bytes8, input, sizeof(bytes8));
            temp_rpkg.patch_entry_list.push_back(bytes8);
        }

        if (temp_rpkg.rpkg_file_version == 1)
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x14 + (uint64_t)patch_entry_count * (uint64_t)0x8;
        }
        else
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x1D + (uint64_t)patch_entry_count * (uint64_t)0x8;
        }
    }
    else
    {
        if (temp_rpkg.rpkg_file_version == 1)
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x10;

            LOG("Importing index from RPKGv1 file: " << temp_rpkg.rpkg_file_name);
        }
        else
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x19;

            LOG("Importing index from RPKGv2 file: " << temp_rpkg.rpkg_file_name);
        }
    }

    position = (uint64_t)file.tellg();

    offset2 = position + table_offset;

    std::vector<char> input_file_data(input_file_size, 0);

    file.seekg(0x0, file.beg);

    file.read(input_file_data.data(), input_file_size);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    std::map<uint64_t, uint64_t> temp_hashes_depends_map;

    for (uint64_t i = 0; i < file_count; i++)
    {
        if (((i * (uint64_t)100000) / (uint64_t)file_count) % (uint64_t)100 == 0 && i > 0)
        {
            stringstream_length = console::update_console(message, file_count, i, start_time, stringstream_length);
        }

        hash temp_hash;

        std::memcpy(&bytes8, &input_file_data[position], sizeof(bytes8));
        position += sizeof(bytes8);

        std::string value = util::uint64_t_to_hex_string(bytes8);
        temp_hash.hash_value = bytes8;
        temp_hash.hash_string = value;

        temp_rpkg.hash_map[bytes8] = (uint64_t)temp_rpkg.hash_map.size();

        std::memcpy(&bytes8, &input_file_data[position], sizeof(bytes8));
        position += sizeof(bytes8);
        temp_hash.hash_offset = bytes8;

        std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        temp_hash.hash_size = bytes4;

        if ((temp_hash.hash_size & 0x3FFFFFFF) != 0)
        {
            temp_hash.is_lz4ed = true;
        }
        else
        {
            temp_hash.is_lz4ed = false;
        }

        if ((temp_hash.hash_size & 0x80000000) == 0x80000000)
        {
            temp_hash.is_xored = true;
        }
        else
        {
            temp_hash.is_xored = false;
        }

        offset1 = position;

        position = offset2;

        std::memcpy(&input, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        input[4] = 0;
        value = std::string(input);
        std::reverse(value.begin(), value.end());
        temp_hash.hash_resource_type = value;
        temp_hash.hash_file_name = temp_hash.hash_string + "." + temp_hash.hash_resource_type;

        std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        temp_hash.hash_reference_table_size = bytes4;

        std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        temp_hash.hash_reference_table_dummy = bytes4;

        std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        temp_hash.hash_size_final = bytes4;

        std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        temp_hash.hash_size_in_memory = bytes4;

        std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
        position += sizeof(bytes4);
        temp_hash.hash_size_in_video_memory = bytes4;

        uint64_t hash_size;

        if (temp_hash.is_lz4ed == 1)
        {
            hash_size = temp_hash.hash_size;

            if (temp_hash.is_xored == 1)
            {
                hash_size &= 0x3FFFFFFF;
            }
        }
        else
        {
            hash_size = temp_hash.hash_size_final;
        }

        if (temp_rpkg.hash_resource_types.size() > 0)
        {
            bool found = false;

            for (uint32_t j = 0; j < temp_rpkg.hash_resource_types.size(); j++)
            {
                if (temp_rpkg.hash_resource_types.at(j) == value)
                {
                    found = true;

                    temp_rpkg.hash_resource_types_data_size.at(j) += hash_size;

                    temp_rpkg.hashes_indexes_based_on_resource_types.at(j).push_back(i);

                    temp_rpkg.hashes_based_on_resource_types.at(j).push_back(temp_hash.hash_file_name);
                }
            }

            if (!found)
            {
                temp_rpkg.hash_resource_types.push_back(value);

                temp_rpkg.hash_resource_types_data_size.push_back(hash_size);

                std::vector<uint64_t> temp_hashes_indexes_based_on_resource_types;

                temp_hashes_indexes_based_on_resource_types.push_back(i);

                temp_rpkg.hashes_indexes_based_on_resource_types.push_back(temp_hashes_indexes_based_on_resource_types);

                std::vector<std::string> temp_hashes_based_on_resource_types;

                temp_hashes_based_on_resource_types.push_back(temp_hash.hash_file_name);

                temp_rpkg.hashes_based_on_resource_types.push_back(temp_hashes_based_on_resource_types);
            }
        }
        else
        {
            temp_rpkg.hash_resource_types.push_back(value);

            temp_rpkg.hash_resource_types_data_size.push_back(hash_size);

            std::vector<uint64_t> temp_hashes_indexes_based_on_resource_types;

            temp_hashes_indexes_based_on_resource_types.push_back(i);

            temp_rpkg.hashes_indexes_based_on_resource_types.push_back(temp_hashes_indexes_based_on_resource_types);

            std::vector<std::string> temp_hashes_based_on_resource_types;

            temp_hashes_based_on_resource_types.push_back(temp_hash.hash_file_name);

            temp_rpkg.hashes_based_on_resource_types.push_back(temp_hashes_based_on_resource_types);
        }

        hash_reference_variables temp_hash_reference_data;

        if (temp_hash.hash_reference_table_size != 0x0)
        {
            bool reference_table_normal = true;

            if (input_file_data[position + (uint64_t)temp_hash.hash_reference_table_size - (uint64_t)0x1] != 0x0)
            {
                reference_table_normal = false;
            }

            temp_hash_reference_data.hash_value = temp_hash.hash_value;

            std::memcpy(&bytes4, &input_file_data[position], sizeof(bytes4));
            position += sizeof(bytes4);
            temp_hash_reference_data.hash_reference_count = bytes4;

            uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            if (reference_table_normal)
            {
                for (uint64_t i = 0; i < temp_hash_reference_count; i++)
                {
                    std::memcpy(&bytes1, &input_file_data[position], sizeof(bytes1));
                    position += sizeof(bytes1);
                    temp_hash_reference_data.hash_reference_type.push_back(bytes1);
                }

                for (uint64_t i = 0; i < temp_hash_reference_count; i++)
                {
                    std::memcpy(&bytes8, &input_file_data[position], sizeof(bytes8));
                    position += sizeof(bytes8);
                    temp_hash_reference_data.hash_reference.push_back(bytes8);

                    value = util::uint64_t_to_hex_string(bytes8);
                    temp_hash_reference_data.hash_reference_string.push_back(value);

                    std::map<uint64_t, uint64_t>::iterator it = temp_hashes_depends_map.find(temp_hash_reference_data.hash_reference.back());

                    if (it == temp_hashes_depends_map.end())
                    {
                        temp_hashes_depends_map[temp_hash_reference_data.hash_reference.back()] = temp_hashes_depends_map.size();
                    }
                }
            }
            else
            {
                for (uint64_t i = 0; i < temp_hash_reference_count; i++)
                {
                    std::memcpy(&bytes8, &input_file_data[position], sizeof(bytes8));
                    position += sizeof(bytes8);
                    temp_hash_reference_data.hash_reference.push_back(bytes8);

                    value = util::uint64_t_to_hex_string(bytes8);
                    temp_hash_reference_data.hash_reference_string.push_back(value);

                    std::map<uint64_t, uint64_t>::iterator it = temp_hashes_depends_map.find(temp_hash_reference_data.hash_reference.back());

                    if (it == temp_hashes_depends_map.end())
                    {
                        temp_hashes_depends_map[temp_hash_reference_data.hash_reference.back()] = temp_hashes_depends_map.size();
                    }
                }

                for (uint64_t i = 0; i < temp_hash_reference_count; i++)
                {
                    std::memcpy(&bytes1, &input_file_data[position], sizeof(bytes1));
                    position += sizeof(bytes1);
                    temp_hash_reference_data.hash_reference_type.push_back(bytes1);
                }
            }

            offset2 = position;
        }
        else
        {
            temp_hash_reference_data.hash_reference_count = 0x0;

            offset2 = position;

            offset2 += temp_hash.hash_reference_table_size;
        }

        temp_hash.hash_reference_data = temp_hash_reference_data;

        temp_rpkg.hash.push_back(temp_hash);

        position = offset1;
    }

    hashes_depends_map_rpkg_file_paths.push_back(temp_rpkg.rpkg_file_path);

    hashes_depends_map.push_back(temp_hashes_depends_map);

    rpkgs.push_back(temp_rpkg);

    file.close();

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    std::stringstream ss;

    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = (uint32_t)100;

    task_single_status = TASK_SUCCESSFUL;
}