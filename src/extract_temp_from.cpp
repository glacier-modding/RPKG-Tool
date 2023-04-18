#include "rpkg_function.h"
// #include "file.h"
#include "global.h"
// #include "crypto.h"
// #include "console.h"
// #include "util.h"
// #include "generic_function.h"
// #include "thirdparty/lz4/lz4.h"
// #include <iostream>
// #include <unordered_map>
// #include <sstream>
// #include <fstream>
// #include <regex>
#include <filesystem>

void rpkg_function::extract_temp_from(std::string& input_path, std::string& filter, std::string& output_path) {
    /*task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    initialize_property_map();

    initialize_enum_map();

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
            rpkg_function::import_rpkg(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t temp_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            for (uint64_t i = 0; i < rpkgs.size(); i++)
            {
                uint64_t rpkg_index = i;

                if (rpkgs.at(i).rpkg_file_path == input_path)
                {
                    std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                    if (it != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        int return_value = rpkg_function::load_temp_tblu_hash_depends(rpkg_index, it->second);

                        if (return_value == TEMP_TBLU_FOUND)
                        {
                            
                        }
                        else if (return_value == TEMP_TBLU_NOT_FOUND_IN_DEPENDS)
                        {
                            LOG_AND_EXIT_NOP("Error: TEMP_TBLU_NOT_FOUND_IN_DEPENDS");

                            task_multiple_status = return_value;

                            return;
                        }
                        else if (return_value == TEMP_TBLU_NOT_FOUND_IN_RPKG)
                        {
                            LOG_AND_EXIT_NOP("Error: TEMP_TBLU_NOT_FOUND_IN_RPKG");

                            task_multiple_status = return_value;

                            return;
                        }
                        else if (return_value == TEMP_TBLU_TOO_MANY)
                        {
                            LOG_AND_EXIT_NOP("Error: TEMP_TBLU_TOO_MANY");

                            task_multiple_status = return_value;

                            return;
                        }

                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        std::string temp_hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type;

                        uint64_t temp_hash_size;

                        if (rpkgs.at(rpkg_index).hash.at(it->second).data.lz4ed)
                        {
                            temp_hash_size = rpkgs.at(rpkg_index).hash.at(it->second).data.header.data_size;

                            if (rpkgs.at(rpkg_index).hash.at(it->second).data.xored)
                            {
                                temp_hash_size &= 0x3FFFFFFF;
                            }
                        }
                        else
                        {
                            temp_hash_size = rpkgs.at(rpkg_index).hash.at(it->second).data.resource.size_final;
                        }

                        temp_input_data = std::vector<char>(temp_hash_size, 0);

                        std::ifstream file = std::ifstream(rpkgs.at(rpkg_index).rpkg_file_path, std::ifstream::binary);

                        if (!file.good())
                        {
                            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(rpkg_index).rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkgs.at(rpkg_index).hash.at(it->second).data.header.data_offset, file.beg);
                        file.read(temp_input_data.data(), temp_hash_size);
                        file.close();

                        if (rpkgs.at(rpkg_index).hash.at(it->second).data.xored)
                        {
                            crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
                        }

                        uint32_t temp_decompressed_size = rpkgs.at(rpkg_index).hash.at(it->second).data.resource.size_final;

                        temp_output_data = std::vector<char>(temp_decompressed_size, 0);

                        if (rpkgs.at(rpkg_index).hash.at(it->second).data.lz4ed)
                        {
                            LZ4_decompress_safe(temp_input_data.data(), temp_output_data.data(), (int)temp_hash_size, temp_decompressed_size);

                            temp_data = &temp_output_data;
                        }
                        else
                        {
                            temp_data = &temp_input_data;
                        }

                        temp_rpkg_index_1 = rpkg_index;
                        temp_rpkg_index_2 = it->second;

                        if (return_value == TEMP_TBLU_FOUND)
                        {
                            std::string tblu_hash_file_name = rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).hash_file_name;

                            uint64_t tblu_hash_size;

                            if (rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.lz4ed)
                            {
                                tblu_hash_size = rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.header.data_size;

                                if (rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.xored)
                                {
                                    tblu_hash_size &= 0x3FFFFFFF;
                                }
                            }
                            else
                            {
                                tblu_hash_size = rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.resource.size_final;
                            }

                            tblu_input_data = std::vector<char>(tblu_hash_size, 0);

                            std::ifstream file2 = std::ifstream(rpkgs.at(tblu_rpkg_index_1).rpkg_file_path, std::ifstream::binary);

                            if (!file2.good())
                            {
                                LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(tblu_rpkg_index_1).rpkg_file_path + " could not be read.");
                            }

                            file2.seekg(rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.header.data_offset, file2.beg);
                            file2.read(tblu_input_data.data(), tblu_hash_size);
                            file2.close();

                            if (rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.xored)
                            {
                                crypto::xor_data(tblu_input_data.data(), (uint32_t)tblu_hash_size);
                            }

                            uint32_t tblu_decompressed_size = rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.resource.size_final;

                            tblu_output_data = std::vector<char>(tblu_decompressed_size, 0);

                            if (rpkgs.at(tblu_rpkg_index_1).hash.at(tblu_rpkg_index_2).data.lz4ed)
                            {
                                LZ4_decompress_safe(tblu_input_data.data(), tblu_output_data.data(), (int)tblu_hash_size, tblu_decompressed_size);

                                tblu_data = &tblu_output_data;
                            }
                            else
                            {
                                tblu_data = &tblu_input_data;
                            }

                            LOG(temp_decompressed_size);
                            LOG(tblu_decompressed_size);

                            char input[1024];
                            char char4[4];
                            uint8_t bytes1 = 0;
                            uint16_t bytes2 = 0;
                            uint32_t bytes4 = 0;
                            uint64_t bytes8 = 0;

                            uint64_t temp_position = 0;
                            uint64_t tblu_position = 0;

                            uint32_t temp_entry_count = 0;
                            uint32_t tblu_entry_count = 0;

                            uint32_t temp_entry_size = 0xA8;
                            uint32_t tblu_entry_size = 0x70;

                            uint32_t temp_after_header_table_offset = 0;
                            uint32_t temp_footer_offset = 0;

                            uint32_t tblu_header_table_offset = 0;

                            uint32_t temp_file_version = 0;
                            uint32_t tblu_file_version = 0;

                            temp_position = 0x8;
                            tblu_position = 0x18;

                            std::memcpy(&input, &temp_data->data()[temp_position], sizeof(bytes4));
                            char4[0] = input[3];
                            char4[1] = input[2];
                            char4[2] = input[1];
                            char4[3] = input[0];
                            std::memcpy(&temp_footer_offset, &char4, sizeof(bytes4));
                            //temp_position += 0x64;
                            temp_position += 0x20;

                            std::memcpy(&temp_after_header_table_offset, &temp_data->data()[temp_position], sizeof(bytes4));
                            temp_position += 0x44;

                            temp_after_header_table_offset += 0x10;

                            temp_footer_offset += 0x10;

                            LOG("TEMP footer offset: " + util::uint32_t_to_string(temp_footer_offset));

                            std::memcpy(&temp_entry_count, &temp_data->data()[temp_position], sizeof(bytes4));

                            LOG("TEMP entry count: " + util::uint32_t_to_string(temp_entry_count));

                            std::memcpy(&tblu_header_table_offset, &tblu_data->data()[tblu_position], sizeof(bytes4));

                            if (tblu_header_table_offset == 0xA0)
                            {
                                tblu_file_version = 0;

                                tblu_position = 0xAC;

                                LOG("TEMP version: H1");
                            }
                            else if (tblu_header_table_offset == 0xD0)
                            {
                                tblu_file_version = 1;

                                tblu_position = 0xDC;

                                LOG("TEMP version: H2/H3");
                            }
                            else
                            {
                                LOG_AND_EXIT_NOP("Error: TBLU_VERSION_UNKNOWN");

                                task_multiple_status = TBLU_VERSION_UNKNOWN;

                                return;
                            }

                            std::memcpy(&tblu_entry_count, &tblu_data->data()[tblu_position], sizeof(bytes4));

                            LOG("TBLU entry count: " + util::uint32_t_to_string(tblu_entry_count));

                            if (temp_entry_count != tblu_entry_count)
                            {
                                LOG_AND_EXIT_NOP("Error: TEMP_TBLU_ENTRY_COUNT_MISMATCH");

                                task_multiple_status = TEMP_TBLU_ENTRY_COUNT_MISMATCH;

                                return;
                            }

                            uint32_t temp_version_check = (temp_after_header_table_offset - 0x70) / temp_entry_count;

                            if (temp_version_check == 0x58)
                            {
                                temp_file_version = 0;

                                LOG("TEMP version: H1/H2");
                            }
                            else if (temp_version_check == 0x70)
                            {
                                temp_file_version = 1;

                                LOG("TEMP version: H3");
                            }
                            else
                            {
                                LOG_AND_EXIT_NOP("Error: TEMP_VERSION_UNKNOWN");

                                task_multiple_status = TEMP_VERSION_UNKNOWN;

                                return;
                            }

                            temp_position += 0x4;
                            tblu_position += 0x4;*/

    /*
    H1/H2 TEMP:
        SEntityTemplateReference logicalParent; // 0x0
        int32 entityTypeResourceIndex; // 0x20
        TArray<SEntityTemplateProperty> propertyValues; // 0x28
        TArray<SEntityTemplateProperty> postInitPropertyValues; // 0x40
    H3 TEMP:
        SEntityTemplateReference logicalParent; // 0x0
        int32 entityTypeResourceIndex; // 0x20
        TArray<SEntityTemplateProperty> propertyValues; // 0x28
        TArray<SEntityTemplateProperty> postInitPropertyValues; // 0x40
        TArray<SEntityTemplatePlatformSpecificProperty> platformSpecificPropertyValues; // 0x58
    H1/H2/H3 TBLU:
        SEntityTemplateReference logicalParent; // 0x0
        int32 entityTypeResourceIndex; // 0x20
        uint64 entityId; // 0x28
        bool editorOnly; // 0x30
        ZString entityName; // 0x38
        TArray<SEntityTemplatePropertyAlias> propertyAliases; // 0x48
        TArray<SEntityTemplateExposedEntity> exposedEntities; // 0x60
        TArray<TPair<ZString, int32>> exposedInterfaces; // 0x78
        TArray<TPair<ZString, SEntityTemplateEntitySubset>> entitySubsets; // 0x90
    */

    /*(for (uint32_t e = 0; e < tblu_entry_count; e++)
    {
        tblu_entry_index.push_back((uint32_t)e);

        LOG("tblu_entry_index: " + util::uint32_t_to_hex_string(tblu_entry_index.back()));

        tblu_position += 0xC;

        std::memcpy(&bytes4, &tblu_data->data()[tblu_position], sizeof(bytes4));
        tblu_position += 0x14;

        tblu_logicalParent.push_back(bytes4);

        LOG("tblu_logicalParent: " + util::uint32_t_to_hex_string(tblu_logicalParent.back()));

        std::memcpy(&bytes4, &tblu_data->data()[tblu_position], sizeof(bytes4));
        tblu_position += 0x8;

        tblu_entityTypeResourceIndex.push_back(bytes4);

        LOG("tblu_entityTypeResourceIndex: " + util::uint32_t_to_hex_string(tblu_entityTypeResourceIndex.back()));

        std::memcpy(&bytes8, &tblu_data->data()[tblu_position], sizeof(bytes8));
        tblu_position += 0x8;

        tblu_entityId.push_back(bytes8);

        LOG("tblu_entityId: " + util::uint64_t_to_hex_string(tblu_entityId.back()));

        if (tblu_file_version == 1)
        {
            std::memcpy(&bytes4, &tblu_data->data()[tblu_position], sizeof(bytes4));
            tblu_position += 0x8;

            tblu_editorOnly.push_back(bytes4);

            LOG("tblu_editorOnly: " + util::uint32_t_to_hex_string(tblu_editorOnly.back()));
        }

        uint32_t string_size = 0;
        uint32_t string_position = 0;

        std::memcpy(&string_size, &tblu_data->data()[tblu_position], sizeof(bytes4));
        tblu_position += 0x8;

        string_size &= 0x3FFFFFFF;

        LOG("string_size: " + util::uint32_t_to_string(string_size));

        std::memcpy(&string_position, &tblu_data->data()[tblu_position], sizeof(bytes4));

        string_position += 0x10;

        std::memcpy(&input, &tblu_data->data()[string_position], string_size);
        input[string_size] = 0;

        tblu_entityName.push_back(std::string(input));

        LOG("tblu_entityName: " + tblu_entityName.back());

        tblu_position += 0x68;
    }

    for (uint32_t e = 0; e < temp_entry_count; e++)
    {
        temp_entry_index.push_back((uint32_t)e);

        LOG("temp_entry_index: " + util::uint32_t_to_hex_string(temp_entry_index.back()));

        temp_position += 0xC;

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x14;

        temp_logicalParent.push_back(bytes4);

        if (temp_logicalParent.at(e) < temp_entry_count)
        {
            LOG("temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.back()) + " (" + tblu_entityName.at(temp_logicalParent.back()) + ")");
        }
        else
        {
            LOG("temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.back()));
        }

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x8;

        temp_entityTypeResourceIndex.push_back(bytes4);

        LOG("temp_entityTypeResourceIndex: " + util::uint32_t_to_hex_string(temp_entityTypeResourceIndex.back()));

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x8;

        temp_propertyValues_start_offsets.push_back(bytes4);

        LOG("temp_propertyValues_start_offsets: " + util::uint32_t_to_hex_string(temp_propertyValues_start_offsets.back()));

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x10;

        temp_propertyValues_end_offsets.push_back(bytes4);

        LOG("temp_propertyValues_end_offsets: " + util::uint32_t_to_hex_string(temp_propertyValues_end_offsets.back()));

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x8;

        temp_postInitPropertyValues_start_offsets.push_back(bytes4);

        LOG("temp_postInitPropertyValues_start_offsets: " + util::uint32_t_to_hex_string(temp_postInitPropertyValues_start_offsets.back()));

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x10;

        temp_postInitPropertyValues_end_offsets.push_back(bytes4);

        LOG("temp_postInitPropertyValues_end_offsets: " + util::uint32_t_to_hex_string(temp_postInitPropertyValues_end_offsets.back()));

        if (temp_file_version == 1)
        {
            std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
            temp_position += 0x8;

            temp_platformSpecificPropertyValues_start_offsets.push_back(bytes4);

            LOG("temp_platformSpecificPropertyValues_start_offsets: " + util::uint32_t_to_hex_string(temp_platformSpecificPropertyValues_start_offsets.back()));

            std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
            temp_position += 0x10;

            temp_platformSpecificPropertyValues_end_offsets.push_back(bytes4);

            LOG("temp_platformSpecificPropertyValues_end_offsets: " + util::uint32_t_to_hex_string(temp_platformSpecificPropertyValues_end_offsets.back()));
        }
    }

    temp_position = temp_footer_offset;

    uint32_t crc32_header_value = 0;

    bool header_found = false;

    while (crc32_header_value != 0x3989BF9F && temp_position < temp_decompressed_size)
    {
        std::memcpy(&crc32_header_value, &temp_data->data()[temp_position], sizeof(bytes4));

        if (crc32_header_value != 0x3989BF9F)
        {
            temp_position += 0x4;
            std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
            temp_position += 0x4;
            temp_position += bytes4;
        }
        else
        {
            header_found = true;
        }
    }

    if (header_found)
    {
        LOG("Header (0x3989BF9F) found at offset: " + util::uint32_t_to_hex_string((uint32_t)temp_position));

        temp_position += 0x8;

        std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x4;

        temp_position += (uint64_t)bytes4 * (uint64_t)0x4;

        uint32_t property_types_count = 0;

        std::memcpy(&property_types_count, &temp_data->data()[temp_position], sizeof(bytes4));
        temp_position += 0x4;

        uint64_t alignment_offset = 0;

        for (uint32_t e = 0; e < property_types_count; e++)
        {
            temp_position += 0x8;
            alignment_offset += 0x8;

            std::memcpy(&bytes4, &temp_data->data()[temp_position], sizeof(bytes4));
            temp_position += 0x4;
            alignment_offset += 0x4;

            std::memcpy(&input, &temp_data->data()[temp_position], bytes4);
            temp_position += bytes4;
            alignment_offset += bytes4;

            temp_property_types.push_back(std::string(input));

            LOG("temp_property_types: " + temp_property_types.back());

            while (alignment_offset % 0x4 != 0)
            {
                temp_position++;
                alignment_offset++;
            }
        }

        for (uint32_t e = 0; e < property_types_count; e++)
        {
            std::unordered_map<uint32_t, uint32_t> temp_temp_property_types_offsets_map;

            temp_property_types_offsets_map.push_back(temp_temp_property_types_offsets_map);

            std::vector<uint32_t> temp_temp_property_types_offsets;

            temp_property_types_offsets.push_back(temp_temp_property_types_offsets);

            std::vector<std::string> temp_temp_property_types_values;

            temp_property_types_values.push_back(temp_temp_property_types_values);

            std::vector<bool> temp_temp_property_types_shared;

            temp_property_types_shared.push_back(temp_temp_property_types_shared);

            std::vector<uint32_t> temp_temp_property_types_shared_count;

            temp_property_types_shared_count.push_back(temp_temp_property_types_shared_count);
        }

        for (uint32_t e = 0; e < temp_entry_count; e++)
        {
            uint32_t start_offset = temp_propertyValues_start_offsets.at(e);
            uint32_t end_offset = temp_propertyValues_end_offsets.at(e);
            start_offset += 0x10;
            end_offset += 0x10;

            temp_position = start_offset;

            LOG("tblu_entityName: " + tblu_entityName.at(e));
            LOG("  temp_entry_index: " + util::uint32_t_to_hex_string(temp_entry_index.at(e)));

            if (temp_logicalParent.at(e) < temp_entry_count)
            {
                LOG("  temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.at(e)) + " (" + tblu_entityName.at(temp_logicalParent.at(e)) + ")");
            }
            else
            {
                LOG("  temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.at(e)));
            }

            std::vector<uint32_t> temp_property_crc32_values;
            std::vector<uint32_t> temp_property_type_indexes;
            std::vector<uint32_t> temp_property_offsets;
            std::vector<uint32_t> temp_property_pointer_offsets;

            if ((end_offset - start_offset) >= 0x18)
            {
                while (temp_position < end_offset)
                {
                    uint32_t property_crc32_value = 0;
                    uint32_t property_type_index = 0;
                    uint32_t property_offset = 0;
                    uint32_t property_pointer_offset = 0;

                    std::memcpy(&property_crc32_value, &temp_data->data()[temp_position], sizeof(bytes4));
                    temp_position += 0x8;

                    temp_property_crc32_values.push_back(property_crc32_value);

                    std::memcpy(&property_type_index, &temp_data->data()[temp_position], sizeof(bytes4));
                    temp_position += 0x8;

                    temp_property_type_indexes.push_back(property_type_index);

                    std::memcpy(&property_offset, &temp_data->data()[temp_position], sizeof(bytes4));
                    property_pointer_offset = temp_position;
                    temp_position += 0x8;

                    temp_property_offsets.push_back(property_offset);

                    temp_property_pointer_offsets.push_back(property_pointer_offset);

                    property_offset += 0x10;

                    bool shared_value_found = false;

                    std::unordered_map<uint32_t, uint32_t>::iterator it3 = temp_property_types_offsets_map.at(property_type_index).find(property_offset);

                    if (it3 != temp_property_types_offsets_map.at(property_type_index).end())
                    {
                        shared_value_found = true;

                        temp_property_types_shared.at(property_type_index).at(it3->second) = true;
                        temp_property_types_shared_count.at(property_type_index).at(it3->second)++;
                    }
                    else
                    {
                        temp_property_types_shared.at(property_type_index).push_back(false);
                        temp_property_types_shared_count.at(property_type_index).push_back((uint32_t)1);
                        temp_property_types_offsets.at(property_type_index).push_back(property_offset);
                        temp_property_types_values.at(property_type_index).push_back(std::string(""));
                        temp_property_types_offsets_map.at(property_type_index)[property_offset] = temp_property_types_offsets_map.at(property_type_index).size();
                    }

                    std::string property_string = "";

                    std::unordered_map<uint32_t, std::string>::iterator it2 = property_map->find(property_crc32_value);

                    if (it2 != property_map->end())
                    {
                        property_string = it2->second;

                        LOG("  property_crc32_value: " + util::uint32_t_to_hex_string(property_crc32_value) + " (" + property_string + ")");
                        LOG("  property_type_index: " + util::uint32_t_to_hex_string(property_type_index));
                        LOG("  property_offset: " + util::uint32_t_to_hex_string(property_offset));
                        LOG("  property_pointer_offset: " + util::uint32_t_to_hex_string(property_pointer_offset));
                    }
                    else
                    {
                        LOG("  property_crc32_value: " + util::uint32_t_to_hex_string(property_crc32_value));
                        LOG("  property_type_index: " + util::uint32_t_to_hex_string(property_type_index));
                        LOG("  property_offset: " + util::uint32_t_to_hex_string(property_offset));
                        LOG("  property_pointer_offset: " + util::uint32_t_to_hex_string(property_pointer_offset));
                    }

                    LOG("    - " + property_string + "'s type: " + temp_property_types.at(property_type_index));

                    std::unordered_map<std::string, std::unordered_map<uint32_t, std::string>>::iterator it = enum_map->find(temp_property_types.at(property_type_index));

                    if (it != enum_map->end())
                    {
                        std::memcpy(&bytes4, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint32_t_to_string(bytes4);
                        }

                        LOG("    - enum: " + util::uint32_t_to_string(bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "bool")
                    {
                        std::memcpy(&bytes4, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (bytes4 == 0)
                        {
                            if (!shared_value_found)
                            {
                                temp_property_types_values.at(property_type_index).back() = "False";
                            }
                        }
                        else if (bytes4 == 1)
                        {
                            if (!shared_value_found)
                            {
                                temp_property_types_values.at(property_type_index).back() = "True";
                            }
                        }

                        LOG("    - bool: " + util::uint32_t_to_string((bool)bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "uint8")
                    {
                        std::memcpy(&bytes4, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint8_t_to_string((uint8_t)bytes4);
                        }

                        LOG("    - uint8: " + util::uint8_t_to_string((uint8_t)bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "int32")
                    {
                        int32_t temp_int32 = 0;

                        std::memcpy(&temp_int32, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::int32_t_to_string(temp_int32);
                        }

                        LOG("    - int32: " + util::int32_t_to_string(temp_int32));
                    }
                    else if (temp_property_types.at(property_type_index) == "uint32")
                    {
                        std::memcpy(&bytes4, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint32_t_to_string(bytes4);
                        }

                        LOG("    - uint32: " + util::uint32_t_to_string(bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "float32")
                    {
                        float temp_float = 0;

                        std::memcpy(&temp_float, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::float_to_string(temp_float);
                        }

                        LOG("    - float32: " + util::float_to_string(temp_float));
                    }
                    else if (temp_property_types.at(property_type_index) == "SMatrix43")
                    {
                        matrix43 temp_matrix43;

                        std::memcpy(&temp_matrix43.x_axis.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.x_axis.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.x_axis.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        std::memcpy(&temp_matrix43.y_axis.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.y_axis.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.y_axis.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        std::memcpy(&temp_matrix43.z_axis.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.z_axis.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.z_axis.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        std::memcpy(&temp_matrix43.transform.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.transform.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_matrix43.transform.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.x_axis.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.x_axis.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.x_axis.z));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.y_axis.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.y_axis.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.y_axis.z));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.z_axis.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.z_axis.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.z_axis.z));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.transform.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.transform.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.transform.z));
                        }

                        LOG("    - SMatrix43: ");
                        LOG("      - X Axis: " + util::float_to_string(temp_matrix43.x_axis.x) + ", " + util::float_to_string(temp_matrix43.x_axis.y) + ", " + util::float_to_string(temp_matrix43.x_axis.z));
                        LOG("      - Y Axis: " + util::float_to_string(temp_matrix43.y_axis.x) + ", " + util::float_to_string(temp_matrix43.y_axis.y) + ", " + util::float_to_string(temp_matrix43.y_axis.z));
                        LOG("      - Z Axis: " + util::float_to_string(temp_matrix43.z_axis.x) + ", " + util::float_to_string(temp_matrix43.z_axis.y) + ", " + util::float_to_string(temp_matrix43.z_axis.z));
                        LOG("      - Transform: " + util::float_to_string(temp_matrix43.transform.x) + ", " + util::float_to_string(temp_matrix43.transform.y) + ", " + util::float_to_string(temp_matrix43.transform.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "SVector2")
                    {
                        vector2 temp_vector2;

                        std::memcpy(&temp_vector2.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector2.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector2.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector2.y));
                        }

                        LOG("    - SVector2: " + util::float_to_string(temp_vector2.x) + ", " + util::float_to_string(temp_vector2.y));
                    }
                    else if (temp_property_types.at(property_type_index) == "SVector3")
                    {
                        vector3 temp_vector3;

                        std::memcpy(&temp_vector3.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector3.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector3.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.z));
                        }

                        LOG("    - SVector3: " + util::float_to_string(temp_vector3.x) + ", " + util::float_to_string(temp_vector3.y) + ", " + util::float_to_string(temp_vector3.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "SVector4")
                    {
                        vector4 temp_vector4;

                        std::memcpy(&temp_vector4.w, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector4.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector4.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector4.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.w));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.z));
                        }

                        LOG("    - SVector4: " + util::float_to_string(temp_vector4.w) + ", " + util::float_to_string(temp_vector4.x) + ", " + util::float_to_string(temp_vector4.y) + ", " + util::float_to_string(temp_vector4.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "ZGuid")
                    {
                        uint32_t temp_ZGuid1;
                        uint16_t temp_ZGuid2;
                        uint16_t temp_ZGuid3;
                        uint16_t temp_ZGuid4;
                        uint16_t temp_ZGuid5;
                        uint32_t temp_ZGuid6;
                        char char2[2];
                        char char4[4];

                        std::memcpy(&temp_ZGuid1, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_ZGuid2, &temp_data->data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        std::memcpy(&temp_ZGuid3, &temp_data->data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        std::memcpy(&input, &temp_data->data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        char2[0] = input[1];
                        char2[1] = input[0];
                        std::memcpy(&temp_ZGuid4, &char2, sizeof(bytes2));
                        std::memcpy(&input, &temp_data->data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        char2[0] = input[1];
                        char2[1] = input[0];
                        std::memcpy(&temp_ZGuid5, &char2, sizeof(bytes2));
                        std::memcpy(&input, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        char4[0] = input[3];
                        char4[1] = input[2];
                        char4[2] = input[1];
                        char4[3] = input[0];
                        std::memcpy(&temp_ZGuid6, &char4, sizeof(bytes4));

                        std::string guid_string = util::uint32_t_to_hex_string(temp_ZGuid1);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid2);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid3);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid4);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid5);
                        guid_string += util::uint32_t_to_hex_string(temp_ZGuid6);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = guid_string;
                        }

                        LOG("    - ZGuid: " + guid_string);
                    }
                    else if (temp_property_types.at(property_type_index) == "ZRuntimeResourceID")
                    {
                        uint64_t temp_ZRuntimeResourceID;

                        std::memcpy(&temp_ZRuntimeResourceID, &temp_data->data()[property_offset], sizeof(bytes8));
                        property_offset += 0x8;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint64_t_to_hex_string(temp_ZRuntimeResourceID);
                        }

                        LOG("    - ZRuntimeResourceID: " + util::uint64_t_to_hex_string(temp_ZRuntimeResourceID));
                    }
                    else if (temp_property_types.at(property_type_index) == "SColorRGB")
                    {
                        vector3 temp_vector3;

                        std::memcpy(&temp_vector3.x, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector3.y, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        std::memcpy(&temp_vector3.z, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.z));
                        }

                        LOG("    - SColorRGB: " + util::float_to_string(temp_vector3.x) + ", " + util::float_to_string(temp_vector3.y) + ", " + util::float_to_string(temp_vector3.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "ZString")
                    {
                        property_offset += 0x10;

                        std::memcpy(&bytes4, &temp_data->data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;

                        std::memcpy(&input, &temp_data->data()[property_offset], bytes4);
                        property_offset += bytes4;

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = std::string(input);
                        }

                        LOG("    - ZString: " + std::string(input));
                    }
                }

                start_offset = temp_postInitPropertyValues_start_offsets.at(e);
                end_offset = temp_postInitPropertyValues_end_offsets.at(e);
                start_offset += 0x10;
                end_offset += 0x10;

                if (temp_file_version == 1)
                {
                    start_offset = temp_platformSpecificPropertyValues_start_offsets.at(e);
                    end_offset = temp_platformSpecificPropertyValues_end_offsets.at(e);
                    start_offset += 0x10;
                    end_offset += 0x10;
                }
            }

            property_crc32_values.push_back(temp_property_crc32_values);
            property_type_indexes.push_back(temp_property_type_indexes);
            property_offsets.push_back(temp_property_offsets);
            property_pointer_offsets.push_back(temp_property_pointer_offsets);
        }

        for (uint32_t e = 0; e < property_types_count; e++)
        {
            LOG("temp_property_type: " + temp_property_types.at(e));

            for (uint32_t p = 0; p < temp_property_types_offsets.at(e).size(); p++)
            {
                LOG("  - temp_property_types_offsets: " + util::uint32_t_to_hex_string(temp_property_types_offsets.at(e).at(p)));
                LOG("  - temp_property_types_values: " + temp_property_types_values.at(e).at(p));
                LOG("  - temp_property_types_shared: " + util::uint32_t_to_string(temp_property_types_shared.at(e).at(p)));
                LOG("  - temp_property_types_shared_count: " + util::uint32_t_to_string(temp_property_types_shared_count.at(e).at(p)));
            }
        }
    }
    else
    {
        LOG_AND_EXIT_NOP("Error: TEMP_HEADER_NOT_FOUND");

        task_multiple_status = TEMP_HEADER_NOT_FOUND;

        return;
    }
}
}
}
}
}

task_single_status = TASK_SUCCESSFUL;
task_multiple_status = TASK_SUCCESSFUL;
}
else
{
LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
}*/
}
