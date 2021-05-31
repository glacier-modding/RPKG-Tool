#include "rpkg_function.h"
#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <set>
#include <string_view>

void dev_function::dev_extract_all_entity_ids(std::string& input_path, std::string& filter, std::string& output_path)
{
    input_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_path);

    LOG("Loading Hash List...");

    //generic_function::load_hash_list(false);

    LOG("Loading Hash List: Done");

    initialize_property_map();

    initialize_enum_map();

    ////std::cout << "Extracting all known hashes from loaded RPKGs vs the Hash List..." << std::endl;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
        {
            if (rpkgs.at(i).hash.at(j).hash_resource_type == "TEMP")
            {
                if (rpkgs.at(i).hash.at(j).hash_file_name == filter)
                {
                    std::map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    rpkg_function::recursive_temp_loader(i, j, parents_map, temps_indexes, 0, 0, 0);
                }
            }
        }
    }

    std::set<std::string> parent_string_set;

    for (uint64_t i = 0; i < temps.size(); i++)
    {
        if (temps.at(i).tblu_return_value == TEMP_TBLU_FOUND)
        {
            for (uint64_t j = 0; j < temps.at(i).tblu_entityId.size(); j++)
            {
                dev_extract_all_parent_strings(i, j, "", parent_string_set);
            }
        }
    }
}

void dev_function::dev_extract_all_parent_strings(uint32_t temps_index, uint32_t entry_index, std::string parent_string, std::set<std::string>& parent_string_set)
{
    if (temps.at(temps_index).tblu_return_value == TEMP_TBLU_FOUND)
    {
        parent_string = temps.at(temps_index).tblu_entityName.at(entry_index) + +" (" + util::uint32_t_to_string(temps_index) + "_" + util::uint32_t_to_string(entry_index) + ")/" + parent_string;

        ////std::cout << parent_string << std::endl;

        uint64_t m_eidParent = 0;

        bool m_eidParent_local = false;

        for (uint32_t p = 0; p < temps.at(temps_index).property_crc32_values.at(entry_index).size(); p++)
        {
            std::string property_string = "";

            std::map<uint32_t, std::string>::iterator it = property_map->find(temps.at(temps_index).property_crc32_values.at(entry_index).at(p));

            if (it != property_map->end())
            {
                property_string = it->second;
            }

            uint32_t property_type_index = temps.at(temps_index).property_type_indexes.at(entry_index).at(p);
            uint32_t property_offset = temps.at(temps_index).property_offsets.at(entry_index).at(p);

            property_offset += 0x10;

            char input[1024];
            uint8_t bytes1 = 0;
            uint16_t bytes2 = 0;
            uint32_t bytes4 = 0;
            uint64_t bytes8 = 0;

            if (property_string == "m_eidParent")
            {
                std::memcpy(&bytes8, &temps.at(temps_index).temp_data.data()[property_offset], sizeof(bytes8));

                ////std::cout << "      - m_eidParent offset: 0x" << util::uint32_t_to_hex_string(property_offset) << std::endl;

                property_offset += 0xC;

                if (bytes8 != 0xFFFFFFFFFFFFFFFF)
                {
                    ////std::cout << "      - m_eidParent: " << util::uint64_t_to_hex_string(bytes8) << std::endl;

                    m_eidParent = bytes8;
                }
                else
                {
                    std::memcpy(&bytes4, &temps.at(temps_index).temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;

                    ////std::cout << "      - m_eidParent: " << util::uint32_t_to_hex_string(bytes4) << std::endl;

                    if (bytes4 < temps.at(temps_index).temp_entry_index.size())
                    {
                        ////std::cout << "      - m_eidParent Entity Name: " << temps.at(temps_index).tblu_entityName.at(bytes4) << std::endl;
                    }
                    else
                    {
                        ////std::cout << "      - m_eidParent Entity Name: " << util::uint32_t_to_hex_string(bytes4) << std::endl;
                    }

                    m_eidParent_local = true;

                    m_eidParent = bytes4;
                }
            }
        }

        if (m_eidParent == 0x0)
        {
            if (temps.at(temps_index).tblu_logicalParent_is_entityId.at(entry_index))
            {
                if (temps.at(temps_index).tblu_logicalParent_entityId.at(entry_index) < 0xFFFFFFFFFFFFFFFE)
                {
                    for (uint64_t i = 0; i < temps.size(); i++)
                    {
                        if (temps.at(i).tblu_return_value == TEMP_TBLU_FOUND)
                        {
                            for (uint64_t j = 0; j < temps.at(i).tblu_entityId.size(); j++)
                            {
                                if (temps.at(i).tblu_entityId.at(j) == temps.at(temps_index).tblu_logicalParent_entityId.at(entry_index))
                                {
                                    dev_extract_all_parent_strings(i, j, parent_string, parent_string_set);
                                }
                            }
                        }
                    }
                }
                else
                {
                    parent_string_set.insert(parent_string);
                }
            }
            else
            {
                if (temps.at(temps_index).tblu_logicalParent.at(entry_index) < temps.at(temps_index).tblu_logicalParent.size())
                {
                    dev_extract_all_parent_strings(temps_index, temps.at(temps_index).tblu_logicalParent.at(entry_index), parent_string, parent_string_set);
                }
                else
                {
                    bool temp_found_in_temps = false;

                    for (uint64_t i = 0; i < temps.size(); i++)
                    {
                        if (temps.at(i).tblu_return_value == TEMP_TBLU_FOUND)
                        {
                            for (uint64_t j = 0; j < temps.at(i).temp_depends_file_name.size(); j++)
                            {
                                if (temps.at(i).temp_depends_file_name.at(j) == temps.at(temps_index).temp_file_name)
                                {
                                    for (uint64_t k = 0; k < temps.at(i).temp_entry_index.size(); k++)
                                    {
                                        if (temps.at(i).temp_entityTypeResourceIndex.at(k) == temps.at(i).temp_depends_index.at(j))
                                        {
                                            temp_found_in_temps = true;

                                            dev_extract_all_parent_strings(i, k, parent_string, parent_string_set);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (!temp_found_in_temps)
                    {
                        parent_string_set.insert(parent_string);
                    }
                }
            }
        }
        else
        {
            if (m_eidParent_local)
            {
                if (m_eidParent < temps.at(temps_index).tblu_logicalParent.size())
                {
                    dev_extract_all_parent_strings(temps_index, m_eidParent, parent_string, parent_string_set);
                }
                else
                {
                    parent_string_set.insert(parent_string);
                }
            }
            else
            {
                if (m_eidParent < 0xFFFFFFFFFFFFFFFE)
                {
                    for (uint64_t i = 0; i < temps.size(); i++)
                    {
                        if (temps.at(i).tblu_return_value == TEMP_TBLU_FOUND)
                        {
                            for (uint64_t j = 0; j < temps.at(i).tblu_entityId.size(); j++)
                            {
                                if (temps.at(i).tblu_entityId.at(j) == m_eidParent)
                                {
                                    dev_extract_all_parent_strings(i, j, parent_string, parent_string_set);
                                }
                            }
                        }
                    }
                }
                else
                {
                    parent_string_set.insert(parent_string);
                }
            }
        }
    }
}