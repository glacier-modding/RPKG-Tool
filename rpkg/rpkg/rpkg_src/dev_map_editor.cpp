#include "rpkg_function.h"
#include "dev_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "crypto.h"
#include "temp.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/writer.h"
#include "thirdparty/rapidjson/pointer.h"
#include "thirdparty/rapidjson/stringbuffer.h"

void map_recursive_parent_mapper(uint64_t entityId, uint64_t entityId_index, std::string parent_string, std::map<uint64_t, uint64_t>& entityId_map, std::vector<uint64_t>& entityId_value, std::vector<uint64_t>& entityId_temp_index, std::vector<uint64_t>& entityId_entity_index, std::vector<uint64_t>& entityId_entity_logicalParent_index, std::vector<bool>& entityId_entity_logicalParent_top_level, std::map<std::string, uint64_t>& entityId_temp_entity_index_map, std::multimap<uint64_t, std::string>& entityId_temp_entity_index_map_temp_hash_depend_map, std::vector<std::string>& entityId_entityName, std::vector<bool>& entityId_m_eidParent_found, std::vector<uint64_t>& entityId_m_eidParent_entityID, std::vector<uint64_t>& entityId_m_eidParent_entityIndex)
{
    if (entityId_m_eidParent_found.at(entityId_index))
    {
        if (entityId_entityName.at(entityId_index) == "1100_Section_A")
        {
            std::cout << "LOL!!!" << std::endl;
        }

        if (entityId_m_eidParent_entityID.at(entityId_index) == 0xFFFFFFFFFFFFFFFF)
        {
            std::string temp_entityId_temp_index = std::to_string(entityId_temp_index.at(entityId_index)) + ":" + std::to_string(entityId_m_eidParent_entityIndex.at(entityId_index));

            std::map<std::string, uint64_t>::iterator itmap = entityId_temp_entity_index_map.find(temp_entityId_temp_index);

            if (itmap != entityId_temp_entity_index_map.end())
            {
                map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second, entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map, entityId_value, entityId_temp_index, entityId_entity_index, entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map, entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName, entityId_m_eidParent_found, entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex);
            }
            else
            {
                std::cout << parent_string << std::endl;
            }
        }
        else
        {
            std::map<uint64_t, uint64_t>::iterator itmap = entityId_map.find(entityId_m_eidParent_entityID.at(entityId_index));

            if (itmap != entityId_map.end())
            {
                map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second, entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map, entityId_value, entityId_temp_index, entityId_entity_index, entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map, entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName, entityId_m_eidParent_found, entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex);
            }
            else
            {
                std::cout << parent_string << std::endl;
            }
        }
    }
    else if (!entityId_entity_logicalParent_top_level.at(entityId_index))
    {
        if (entityId_entityName.at(entityId_index) == "1100_Section_A")
        {
            std::cout << "LOL!!!" << std::endl;
        }

        std::string temp_entityId_temp_index = std::to_string(entityId_temp_index.at(entityId_index)) + ":" + std::to_string(entityId_entity_logicalParent_index.at(entityId_index));

        std::map<std::string, uint64_t>::iterator itmap = entityId_temp_entity_index_map.find(temp_entityId_temp_index);

        if (itmap != entityId_temp_entity_index_map.end())
        {
            map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second, entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map, entityId_value, entityId_temp_index, entityId_entity_index, entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map, entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName, entityId_m_eidParent_found, entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex);
        }
        else
        {
            std::cout << parent_string << std::endl;
        }
    }
    else
    {
        if (entityId_entityName.at(entityId_index) == "1100_Section_A")
        {
            std::cout << "LOL!!!" << std::endl;
        }

        bool found = false;

        std::pair <std::multimap<uint64_t, std::string>::iterator, std::multimap<uint64_t, std::string>::iterator> er = entityId_temp_entity_index_map_temp_hash_depend_map.equal_range(rpkgs.at(temps.at(entityId_temp_index.at(entityId_index)).temp_rpkg_index).hash.at(temps.at(entityId_temp_index.at(entityId_index)).temp_hash_index).hash_value);

        for (std::multimap<uint64_t, std::string>::iterator it = er.first; it != er.second; it++)
        {
            std::map<std::string, uint64_t>::iterator itmap = entityId_temp_entity_index_map.find(it->second);

            if (itmap != entityId_temp_entity_index_map.end())
            {
                map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second, entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map, entityId_value, entityId_temp_index, entityId_entity_index, entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map, entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName, entityId_m_eidParent_found, entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex);
            }
            else
            {
                std::cout << parent_string << std::endl;
            }

            found = true;
        }

        if (!found)
        {
            if (parent_string != "")
            {
                std::cout << parent_string << std::endl;
            }
        }
    }
}

void dev_function::dev_map_editor(std::string& input_path, std::string& filter, std::string& output_path)
{
    initialize_enum_map_h2();
    initialize_enum_map_h3();

    initialize_type_map_h2();
    initialize_type_map_h3();

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

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    //temp temp_temp(i, it->second, 3);

                    //temp_temp.load_data();

                    std::map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    rpkg_function::recursive_temp_loader(rpkg_index, it->second, 3, parents_map, temps_indexes, 0, 0, 0);

                    for (uint64_t t = 0; t < temps.size(); t++)
                    {
                        if (temps.at(t).tblu_return_value == TEMP_TBLU_FOUND)
                        {
                            temps.at(t).load_data();
                        }
                    }

                    std::map<uint64_t, uint64_t> entityId_map;
                    std::vector<uint64_t> entityId_value;
                    std::vector<uint64_t> entityId_temp_index;
                    std::vector<uint64_t> entityId_entity_index;
                    std::vector<uint64_t> entityId_entity_logicalParent_index;
                    std::vector<bool> entityId_entity_logicalParent_top_level;
                    std::map<std::string, uint64_t> entityId_temp_entity_index_map;
                    std::multimap<uint64_t, std::string> entityId_temp_entity_index_map_temp_hash_depend_map;
                    std::vector<std::string> entityId_entityName;
                    std::vector<bool> entityId_m_eidParent_found;
                    std::vector<uint64_t> entityId_m_eidParent_entityID;
                    std::vector<uint64_t> entityId_m_eidParent_entityIndex;

                    for (uint64_t t = 0; t < temps.size(); t++)
                    {
                        if (temps.at(t).tblu_return_value == TEMP_TBLU_FOUND)
                        {
                            const rapidjson::Value& temp_json_subEntities = temps.at(t).temp_json_document["subEntities"];

                            const rapidjson::Value& tblu_json_subEntities = temps.at(t).tblu_json_document["subEntities"];

                            for (uint64_t a = 0; a < tblu_json_subEntities.Size(); a++)
                            {
                                rapidjson::Value::ConstMemberIterator it1 = tblu_json_subEntities[a].FindMember("entityId");

                                if (it1 != tblu_json_subEntities[a].MemberEnd())
                                {
                                    //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(a) << " has a entityId of " << it1->value.GetUint64() << std::endl;

                                    std::map<uint64_t, uint64_t>::iterator itmap = entityId_map.find(it1->value.GetUint64());

                                    if (itmap == entityId_map.end())
                                    {
                                        entityId_map[it1->value.GetUint64()] = entityId_map.size();
                                        entityId_value.push_back(it1->value.GetUint64());
                                        entityId_temp_index.push_back(t);
                                        entityId_entity_index.push_back(a);
                                        entityId_entity_logicalParent_index.push_back(temps.at(t).temp_logicalParent.at(a));

                                        if (temps.at(t).temp_logicalParent.at(a) >= temps.at(t).temp_logicalParent.size())
                                        {
                                            entityId_entity_logicalParent_top_level.push_back(true);
                                        }
                                        else
                                        {
                                            entityId_entity_logicalParent_top_level.push_back(false);
                                        }

                                        entityId_temp_entity_index_map[std::to_string(t) + ":" + std::to_string(a)] = entityId_map.size() - 1;
                                        entityId_temp_entity_index_map_temp_hash_depend_map.insert({rpkgs.at(temps.at(t).temp_rpkg_index).hash.at(temps.at(t).temp_hash_index).hash_reference_data.hash_reference.at(temps.at(t).temp_entityTypeResourceIndex.at(a)), std::to_string(t) + ":" + std::to_string(a)});
                                        entityId_entityName.push_back(temps.at(t).tblu_entityName.at(a));

                                        bool m_eidParent_found = false;
                                        uint64_t m_eidParent_entityID = 0;
                                        uint64_t m_eidParent_entityIndex = 0;

                                        uint64_t s = a;

                                        //for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++)
                                        {
                                            rapidjson::Value::ConstMemberIterator it2 = temp_json_subEntities[s].FindMember("propertyValues");

                                            if (it2 != temp_json_subEntities[s].MemberEnd())
                                            {
                                                for (uint64_t p = 0; p < it2->value.Size(); p++)
                                                {
                                                    rapidjson::Value::ConstMemberIterator it3 = it2->value[p].FindMember("nPropertyID");

                                                    if (it3 != it2->value[p].MemberEnd())
                                                    {
                                                        if (it3->value.IsString())
                                                        {
                                                            if (std::strcmp(it3->value.GetString(), "m_mTransform") == 0)
                                                            {
                                                                //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_mTransform in propertyValues" << std::endl;
                                                            }
                                                            else if (std::strcmp(it3->value.GetString(), "m_eidParent") == 0)
                                                            {
                                                                //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_eidParent in propertyValues" << std::endl;

                                                                rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                                                                if (it4 != it2->value[p].MemberEnd())
                                                                {
                                                                    rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$val");

                                                                    if (it5 != it4->value.MemberEnd())
                                                                    {
                                                                        rapidjson::Value::ConstMemberIterator it6 = it5->value.FindMember("entityID");

                                                                        m_eidParent_found = true;

                                                                        if (it6 != it5->value.MemberEnd())
                                                                        {
                                                                            //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityID is " << it6->value.GetUint64() << std::endl;

                                                                            m_eidParent_entityID = it6->value.GetUint64();
                                                                        }

                                                                        it6 = it5->value.FindMember("entityIndex");

                                                                        if (it6 != it5->value.MemberEnd())
                                                                        {
                                                                            //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityIndex is " << it6->value.GetUint64() << std::endl;

                                                                            m_eidParent_entityIndex = it6->value.GetUint64();
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }

                                            it2 = temp_json_subEntities[s].FindMember("postInitPropertyValues");

                                            if (it2 != temp_json_subEntities[s].MemberEnd())
                                            {
                                                for (uint64_t p = 0; p < it2->value.Size(); p++)
                                                {
                                                    rapidjson::Value::ConstMemberIterator it3 = it2->value[p].FindMember("nPropertyID");

                                                    if (it3 != it2->value[p].MemberEnd())
                                                    {
                                                        if (it3->value.IsString())
                                                        {
                                                            if (std::strcmp(it3->value.GetString(), "m_mTransform") == 0)
                                                            {
                                                                //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_mTransform in postInitPropertyValues" << std::endl;
                                                            }
                                                            else if (std::strcmp(it3->value.GetString(), "m_eidParent") == 0)
                                                            {
                                                                //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_eidParent in postInitPropertyValues" << std::endl;

                                                                rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                                                                if (it4 != it2->value[p].MemberEnd())
                                                                {
                                                                    rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$val");

                                                                    if (it5 != it4->value.MemberEnd())
                                                                    {
                                                                        rapidjson::Value::ConstMemberIterator it6 = it5->value.FindMember("entityID");

                                                                        m_eidParent_found = true;

                                                                        if (it6 != it5->value.MemberEnd())
                                                                        {
                                                                            //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityID is " << it6->value.GetUint64() << std::endl;

                                                                            m_eidParent_entityID = it6->value.GetUint64();
                                                                        }

                                                                        it6 = it5->value.FindMember("entityIndex");

                                                                        if (it6 != it5->value.MemberEnd())
                                                                        {
                                                                            //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityIndex is " << it6->value.GetUint64() << std::endl;

                                                                            m_eidParent_entityIndex = it6->value.GetUint64();
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        entityId_m_eidParent_found.push_back(m_eidParent_found);
                                        entityId_m_eidParent_entityID.push_back(m_eidParent_entityID);
                                        entityId_m_eidParent_entityIndex.push_back(m_eidParent_entityIndex);
                                    }
                                    else
                                    {
                                        std::cout <<"DUPLICATE FOUND: " << temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(a) << " has a entityId of " << it1->value.GetUint64() << std::endl;
                                    }
                                }
                            }
                        }

                        std::cout <<"LOL!!!" << std::endl;
                    }

                    for (std::map<uint64_t, uint64_t>::iterator itmap = entityId_map.begin(); itmap != entityId_map.end(); itmap++)
                    {
                        map_recursive_parent_mapper(itmap->first, itmap->second, entityId_entityName.at(itmap->second), entityId_map, entityId_value, entityId_temp_index, entityId_entity_index, entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map, entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName, entityId_m_eidParent_found, entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex);
                    }
                }
            }
        }
    }
}