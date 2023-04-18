#include "rpkg_function.h"
#include "dev_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "temp.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/stringbuffer.h"

void extract_glb_and_return_coords(uint32_t temps_index, uint32_t entry_index, std::string parent_string,
                                   std::vector<uint64_t>& entityId_temp_index,
                                   std::vector<uint64_t>& entityId_m_eidParent_entityID,
                                   std::vector<uint64_t>& entityId_m_eidParent_entityIndex,
                                   std::multimap<std::string, uint64_t>& entityId_m_eidParent_entityIndex_map,
                                   std::vector<bool>& entityId_entity_logicalParent_top_level,
                                   std::vector<bool>& entityId_has_m_mTransform_and_m_eidParent,
                                   std::unordered_map<std::string, uint64_t>& entityId_temp_index_entityIndex_map,
                                   std::vector<std::string>& entityId_m_mTransform) {
    if (temps.at(temps_index).tblu_return_value != TEMP_TBLU_FOUND) return;

    std::string entry_name = "";

    if (entry_index < 0 || entry_index >= temps.at(temps_index).tblu_entityName.size()) {

    } else {
        entry_name.append(temps.at(temps_index).tblu_entityName.at(entry_index));
    }

    //util::replace_all_string_in_string(entry_name, " ", "_");

    std::string property_name = "";

    auto it4 = entityId_temp_index_entityIndex_map.find(
            std::to_string(temps_index) + ":" + util::int32_t_to_string(entry_index));

    if (it4 != entityId_temp_index_entityIndex_map.end()) {
        if (entityId_has_m_mTransform_and_m_eidParent.at(it4->second)) {
            property_name = entry_name + " (M) (" + util::uint32_t_to_string(temps_index) + "_" +
                            util::uint32_t_to_string(entry_index) + ")";

            if (!entry_name.empty()) {
                map_editor_parents.push_back(parent_string);

                if (parent_string.empty()) {
                    parent_string += property_name;
                } else {
                    parent_string += "/" + property_name;
                }

                std::cout << parent_string << std::endl;

                std::cout << entityId_m_mTransform.at(it4->second) << std::endl;

                map_editor_property_names.push_back(property_name);

                map_editor_matrixes.push_back(map_editor_temp_matrix);

                map_editor_glb_file_names.push_back(prim_asset_file_names);

                auto it = temps_map.find(rpkgs.at(temps.at(temps_index).temp_rpkg_index).hash.at(
                        temps.at(temps_index).temp_hash_index).hash_reference_data.hash_reference.at(
                        temps.at(temps_index).temp_entityTypeResourceIndex.at(entry_index)));

                if (it != temps_map.end()) {
                    if (temps.at(it->second).tblu_return_value == TEMP_TBLU_FOUND) {
                        if (!temps.at(it->second).temp_depends_file_name.empty()) {
                            std::set<uint32_t> logical_parents_set;

                            for (uint32_t e = 0; e < temps.at(it->second).temp_logicalParent.size(); e++) {
                                if (temps.at(it->second).temp_logicalParent.at(e) >=
                                    temps.at(it->second).temp_logicalParent.size()) {
                                    //logical_parents_set.insert(temps.at(it->second).temp_logicalParent.at(e));
                                    logical_parents_set.insert(e);
                                }
                            }

                            for (auto it2 = logical_parents_set.begin(); it2 != logical_parents_set.end(); it2++) {
                                std::cout << *it2 << std::endl;

                                auto it3 = temps_map.find(rpkgs.at(temps.at(it->second).temp_rpkg_index).hash.at(
                                        temps.at(it->second).temp_hash_index).hash_reference_data.hash_reference.at(
                                        temps.at(it->second).temp_entityTypeResourceIndex.at(*it2)));

                                if (it3 != temps_map.end()) {
                                    //if (temps.at(it3->second).tblu_return_value == TEMP_TBLU_FOUND)
                                    //{
                                    if (!temps.at(it3->second).prim_depends_file_name.empty()) {
                                        for (const auto& p : temps.at(it3->second).prim_depends_file_name) {
                                            std::cout << "PRIM: " << p << std::endl;
                                        }
                                    }
                                    //}
                                }
                            }
                        } else {
                            if (!temps.at(it->second).prim_depends_file_name.empty()) {
                                for (const auto& p : temps.at(it->second).prim_depends_file_name) {
                                    std::cout << "PRIM: " << p << std::endl;
                                }
                            }
                        }
                    }

                    if (!temps.at(it->second).prim_depends_file_name.empty()) {
                        for (const auto& p : temps.at(it->second).prim_depends_file_name) {
                            std::cout << "PRIM: " << p << std::endl;
                        }
                    }
                }
            }
        } else {
            property_name = entry_name + " (NM) (" + util::uint32_t_to_string(temps_index) + "_" +
                            util::uint32_t_to_string(entry_index) + ")";

            if (!entry_name.empty()) {
                map_editor_parents.push_back(parent_string);

                if (parent_string.empty()) {
                    parent_string += property_name;
                } else {
                    parent_string += "/" + property_name;
                }

                std::cout << parent_string << std::endl;

                map_editor_property_names.push_back(property_name);

                map_editor_matrixes.push_back(map_editor_temp_matrix);

                map_editor_glb_file_names.push_back(prim_asset_file_names);
            }
        }
    }

    bool m_eidParent_found = false;
    uint64_t entityId_temp = 0;
    uint64_t m_eidParent_entityID = 0;
    int32_t m_eidParent_entityIndex = 0;

    if (entry_index >= 0 && entry_index < temps.at(temps_index).tblu_entityName.size()) {
        std::pair<std::multimap<std::string, uint64_t>::iterator, std::multimap<std::string, uint64_t>::iterator> er = entityId_m_eidParent_entityIndex_map.equal_range(
                std::to_string(temps_index) + ":" + util::int32_t_to_string(entry_index));

        for (auto it3 = er.first; it3 != er.second; it3++) {
            if (!entityId_entity_logicalParent_top_level.at(it3->second)) {
                m_eidParent_found = true;

                entityId_temp = entityId_temp_index.at(it3->second);

                m_eidParent_entityID = entityId_m_eidParent_entityID.at(it3->second);

                m_eidParent_entityIndex = entityId_m_eidParent_entityIndex.at(it3->second);

                extract_glb_and_return_coords(entityId_temp, m_eidParent_entityIndex, parent_string,
                                              entityId_temp_index, entityId_m_eidParent_entityID,
                                              entityId_m_eidParent_entityIndex,
                                              entityId_m_eidParent_entityIndex_map,
                                              entityId_entity_logicalParent_top_level,
                                              entityId_has_m_mTransform_and_m_eidParent,
                                              entityId_temp_index_entityIndex_map, entityId_m_mTransform);
            }
        }
    }

    if (!m_eidParent_found) {
        bool logical_parent_found = false;

        for (uint32_t e = 0; e < temps.at(temps_index).temp_logicalParent.size(); e++) {
            if (temps.at(temps_index).temp_logicalParent.at(e) == entry_index) {
                if (entry_index == 1) {
                    std::cout << parent_string << std::endl;
                }

                extract_glb_and_return_coords(temps_index, e, parent_string, entityId_temp_index,
                                              entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex,
                                              entityId_m_eidParent_entityIndex_map,
                                              entityId_entity_logicalParent_top_level,
                                              entityId_has_m_mTransform_and_m_eidParent,
                                              entityId_temp_index_entityIndex_map, entityId_m_mTransform);

                logical_parent_found = true;
            }
        }
    }

    if (entry_index < 0 || entry_index >= temps.at(temps_index).tblu_entityName.size()) {
        return;
    }

    auto it1 = temps_map.find(
            rpkgs.at(temps.at(temps_index).temp_rpkg_index).hash.at(
                    temps.at(temps_index).temp_hash_index).hash_reference_data.hash_reference.at(
                    temps.at(temps_index).temp_entityTypeResourceIndex.at(entry_index)));

    if (it1 != temps_map.end()) {
        if (temps.at(it1->second).tblu_return_value == TEMP_TBLU_FOUND) {
            std::set<uint32_t> logicalParentsSet;

            for (uint32_t e1 = 0; e1 < temps.at(it1->second).temp_logicalParent.size(); e1++) {
                if (temps.at(it1->second).temp_logicalParent.at(e1) >=
                    temps.at(it1->second).temp_logicalParent.size()) {
                    logicalParentsSet.insert(temps.at(it1->second).temp_logicalParent.at(e1));
                }
            }

            for (auto it21 = logicalParentsSet.begin();
                 it21 != logicalParentsSet.end(); it21++) {
                std::cout << *it21 << std::endl;

                extract_glb_and_return_coords(it1->second, *it21, parent_string, entityId_temp_index,
                                              entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex,
                                              entityId_m_eidParent_entityIndex_map,
                                              entityId_entity_logicalParent_top_level,
                                              entityId_has_m_mTransform_and_m_eidParent,
                                              entityId_temp_index_entityIndex_map, entityId_m_mTransform);
            }
        }
    }
}

void map_recursive_parent_mapper(uint64_t entityId, uint64_t entityId_index, const std::string& parent_string,
                                 std::unordered_map<uint64_t, uint64_t>& entityId_map,
                                 std::vector<uint64_t>& entityId_value, std::vector<uint64_t>& entityId_temp_index,
                                 std::vector<uint64_t>& entityId_entity_index,
                                 std::vector<uint64_t>& entityId_entity_logicalParent_index,
                                 std::vector<bool>& entityId_entity_logicalParent_top_level,
                                 std::unordered_map<std::string, uint64_t>& entityId_temp_entity_index_map,
                                 std::multimap<uint64_t, std::string>& entityId_temp_entity_index_map_temp_hash_depend_map,
                                 std::vector<std::string>& entityId_entityName,
                                 std::vector<bool>& entityId_m_eidParent_found,
                                 std::vector<uint64_t>& entityId_m_eidParent_entityID,
                                 std::vector<uint64_t>& entityId_m_eidParent_entityIndex) {
    if (entityId_m_eidParent_found.at(entityId_index)) {
        if (entityId_m_eidParent_entityID.at(entityId_index) == 0xFFFFFFFFFFFFFFFF) {
            std::string temp_entityId_temp_index = std::to_string(entityId_temp_index.at(entityId_index)) + ":" +
                                                   std::to_string(entityId_m_eidParent_entityIndex.at(entityId_index));

            auto itmap = entityId_temp_entity_index_map.find(temp_entityId_temp_index);

            if (itmap != entityId_temp_entity_index_map.end()) {
                map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second,
                                            entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map,
                                            entityId_value, entityId_temp_index, entityId_entity_index,
                                            entityId_entity_logicalParent_index,
                                            entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map,
                                            entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName,
                                            entityId_m_eidParent_found, entityId_m_eidParent_entityID,
                                            entityId_m_eidParent_entityIndex);
            } else {
                std::cout << parent_string << std::endl;
            }
        } else {
            auto itmap = entityId_map.find(entityId_m_eidParent_entityID.at(entityId_index));

            if (itmap != entityId_map.end()) {
                map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second,
                                            entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map,
                                            entityId_value, entityId_temp_index, entityId_entity_index,
                                            entityId_entity_logicalParent_index,
                                            entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map,
                                            entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName,
                                            entityId_m_eidParent_found, entityId_m_eidParent_entityID,
                                            entityId_m_eidParent_entityIndex);
            } else {
                std::cout << parent_string << std::endl;
            }
        }
    } else if (!entityId_entity_logicalParent_top_level.at(entityId_index)) {
        std::string temp_entityId_temp_index = std::to_string(entityId_temp_index.at(entityId_index)) + ":" +
                                               std::to_string(entityId_entity_logicalParent_index.at(entityId_index));

        auto itmap = entityId_temp_entity_index_map.find(temp_entityId_temp_index);

        if (itmap != entityId_temp_entity_index_map.end()) {
            map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second,
                                        entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map,
                                        entityId_value, entityId_temp_index, entityId_entity_index,
                                        entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level,
                                        entityId_temp_entity_index_map,
                                        entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName,
                                        entityId_m_eidParent_found, entityId_m_eidParent_entityID,
                                        entityId_m_eidParent_entityIndex);
        } else {
            std::cout << parent_string << std::endl;
        }
    } else {
        bool found = false;

        std::pair<std::multimap<uint64_t, std::string>::iterator, std::multimap<uint64_t, std::string>::iterator> er = entityId_temp_entity_index_map_temp_hash_depend_map.equal_range(
                rpkgs.at(temps.at(entityId_temp_index.at(entityId_index)).temp_rpkg_index).hash.at(
                        temps.at(entityId_temp_index.at(entityId_index)).temp_hash_index).hash_value);

        for (auto it = er.first; it != er.second; it++) {
            auto itmap = entityId_temp_entity_index_map.find(it->second);

            if (itmap != entityId_temp_entity_index_map.end()) {
                map_recursive_parent_mapper(entityId_value.at(itmap->second), itmap->second,
                                            entityId_entityName.at(itmap->second) + "/" + parent_string, entityId_map,
                                            entityId_value, entityId_temp_index, entityId_entity_index,
                                            entityId_entity_logicalParent_index,
                                            entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map,
                                            entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName,
                                            entityId_m_eidParent_found, entityId_m_eidParent_entityID,
                                            entityId_m_eidParent_entityIndex);
            } else {
                std::cout << parent_string << std::endl;
            }

            found = true;
        }

        if (!found && !parent_string.empty()) {
            std::cout << parent_string << std::endl;
        }
    }
}

/*void map_recursive_parent_mapper(uint64_t entityId, uint64_t entityId_index, std::string parent_string, std::unordered_map<uint64_t, uint64_t>& entityId_map, std::vector<uint64_t>& entityId_value, std::vector<uint64_t>& entityId_temp_index, std::vector<uint64_t>& entityId_entity_index, std::vector<uint64_t>& entityId_entity_logicalParent_index, std::vector<bool>& entityId_entity_logicalParent_top_level, std::unordered_map<std::string, uint64_t>& entityId_temp_entity_index_map, std::multimap<uint64_t, std::string>& entityId_temp_entity_index_map_temp_hash_depend_map, std::vector<std::string>& entityId_entityName, std::vector<bool>& entityId_m_eidParent_found, std::vector<uint64_t>& entityId_m_eidParent_entityID, std::vector<uint64_t>& entityId_m_eidParent_entityIndex)
{
    if (entityId_m_eidParent_found.at(entityId_index))
    {
        if (entityId_m_eidParent_entityID.at(entityId_index) == 0xFFFFFFFFFFFFFFFF)
        {
            std::string temp_entityId_temp_index = std::to_string(entityId_temp_index.at(entityId_index)) + ":" + std::to_string(entityId_m_eidParent_entityIndex.at(entityId_index));

            std::unordered_map<std::string, uint64_t>::iterator itmap = entityId_temp_entity_index_map.find(temp_entityId_temp_index);

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
            std::unordered_map<uint64_t, uint64_t>::iterator itmap = entityId_map.find(entityId_m_eidParent_entityID.at(entityId_index));

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
        std::string temp_entityId_temp_index = std::to_string(entityId_temp_index.at(entityId_index)) + ":" + std::to_string(entityId_entity_logicalParent_index.at(entityId_index));

        std::unordered_map<std::string, uint64_t>::iterator itmap = entityId_temp_entity_index_map.find(temp_entityId_temp_index);

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
        bool found = false;

        std::pair <std::multimap<uint64_t, std::string>::iterator, std::multimap<uint64_t, std::string>::iterator> er = entityId_temp_entity_index_map_temp_hash_depend_map.equal_range(rpkgs.at(temps.at(entityId_temp_index.at(entityId_index)).temp_rpkg_index).hash.at(temps.at(entityId_temp_index.at(entityId_index)).temp_hash_index).hash_value);

        for (std::multimap<uint64_t, std::string>::iterator it = er.first; it != er.second; it++)
        {
            std::unordered_map<std::string, uint64_t>::iterator itmap = entityId_temp_entity_index_map.find(it->second);

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
}*/

void dev_function::dev_map_editor(std::string& input_path, std::string& filter, std::string& output_path) {
    initialize_enum_map_h2();
    initialize_enum_map_h3();

    initialize_type_map_h2();
    initialize_type_map_h3();

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path)) {
        input_path_is_rpkg_file = true;
    } else {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path)) {
        if (!input_path_is_rpkg_file) {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        } else {
            rpkg_function::import_rpkg(input_path);

            std::string path = R"(C:\Program Files\Epic Games\HITMAN3\Runtime\)";

            rpkg_function::import_rpkg_files_in_folder(path);

            path = R"(C:\Program Files\Epic Games\HITMAN3\Runtime\chunk0patch1.rpkg)";

            //rpkg_function::import_rpkg(path, true);

            path = R"(C:\Program Files\Epic Games\HITMAN3\Runtime\chunk1.rpkg)";

            //rpkg_function::import_rpkg(path, true);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        //force_load_hash_list();

        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++) {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file) {
                auto it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end()) {
                    //temp temp_temp(i, it->second, 3);

                    //temp_temp.load_data();

                    std::unordered_map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    rpkg_function::recursive_temp_loader(rpkg_index, it->second, 3, parents_map, temps_indexes, 0, 0,
                                                         0);

                    for (auto& temp : temps) {
                        if (temp.tblu_return_value == TEMP_TBLU_FOUND) {
                            temp.load_data();
                        }
                    }

                    std::vector<uint64_t> entityId_temp_index;
                    std::vector<uint64_t> entityId_m_eidParent_entityID;
                    std::vector<uint64_t> entityId_m_eidParent_entityIndex;
                    std::multimap<std::string, uint64_t> entityId_m_eidParent_entityIndex_map;
                    std::vector<bool> entityId_entity_logicalParent_top_level;
                    std::unordered_map<std::string, uint64_t> entityId_temp_index_entityIndex_map;
                    std::vector<bool> entityId_has_m_mTransform_and_m_eidParent;
                    std::vector<std::string> entityId_m_mTransform;

                    for (uint64_t t = 0; t < temps.size(); t++) {
                        if (temps.at(t).tblu_return_value == TEMP_TBLU_FOUND) {
                            const rapidjson::Value& temp_json_subEntities = temps.at(
                                    t).temp_json_document["subEntities"];

                            for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++) {
                                bool m_eidParent_found = true;
                                uint64_t m_eidParent_entityID = 0xFFFFFFFFFFFFFFFF;
                                int32_t m_eidParent_entityIndex = temps.at(t).temp_logicalParent.at(s);
                                bool logicalParent_top_level = false;
                                bool has_m_mTransform = false;
                                bool has_m_eidParent = false;
                                bool has_m_mTransform_and_m_eidParent = false;
                                std::string m_mTransform = "";

                                if (temps.at(t).temp_logicalParent.at(s) >= temps.at(t).temp_logicalParent.size()) {
                                    logicalParent_top_level = true;
                                } else {
                                    logicalParent_top_level = false;
                                }

                                //for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++)
                                {
                                    rapidjson::Value::ConstMemberIterator it2 = temp_json_subEntities[s].FindMember(
                                            "propertyValues");

                                    if (it2 != temp_json_subEntities[s].MemberEnd()) {
                                        for (uint64_t p = 0; p < it2->value.Size(); p++) {
                                            rapidjson::Value::ConstMemberIterator it3 = it2->value[p].FindMember(
                                                    "nPropertyID");

                                            if (it3 != it2->value[p].MemberEnd()) {
                                                if (it3->value.IsString()) {
                                                    if (std::strcmp(it3->value.GetString(), "m_mTransform") == 0) {
                                                        has_m_mTransform = true;

                                                        const rapidjson::Value& value = it2->value[p]["value"];

                                                        if (!value.IsNull()) {
                                                            const rapidjson::Value& val = value["$val"];

                                                            if (!val.IsNull()) {
                                                                std::cout << val["XAxis"]["x"].GetFloat() << std::endl;
                                                                std::cout << val["XAxis"]["y"].GetFloat() << std::endl;
                                                                std::cout << val["XAxis"]["z"].GetFloat() << std::endl;
                                                                std::cout << val["YAxis"]["x"].GetFloat() << std::endl;
                                                                std::cout << val["YAxis"]["y"].GetFloat() << std::endl;
                                                                std::cout << val["YAxis"]["z"].GetFloat() << std::endl;
                                                                std::cout << val["ZAxis"]["x"].GetFloat() << std::endl;
                                                                std::cout << val["ZAxis"]["y"].GetFloat() << std::endl;
                                                                std::cout << val["ZAxis"]["z"].GetFloat() << std::endl;
                                                                std::cout << val["Trans"]["x"].GetFloat() << std::endl;
                                                                std::cout << val["Trans"]["y"].GetFloat() << std::endl;
                                                                std::cout << val["Trans"]["z"].GetFloat() << std::endl;

                                                                m_mTransform.append(util::float_to_string(
                                                                        val["XAxis"]["x"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["YAxis"]["x"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["ZAxis"]["x"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["XAxis"]["y"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["YAxis"]["y"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["ZAxis"]["y"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["XAxis"]["z"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["YAxis"]["z"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["ZAxis"]["z"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["Trans"]["x"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["Trans"]["y"].GetFloat()));
                                                                m_mTransform.push_back(',');
                                                                m_mTransform.append(util::float_to_string(
                                                                        val["Trans"]["z"].GetFloat()));
                                                            }
                                                        }

                                                        //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_mTransform in propertyValues" << std::endl;
                                                    } else if (std::strcmp(it3->value.GetString(), "m_eidParent") ==
                                                               0) {
                                                        //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_eidParent in propertyValues" << std::endl;

                                                        rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember(
                                                                "value");

                                                        if (it4 != it2->value[p].MemberEnd()) {
                                                            rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember(
                                                                    "$val");

                                                            if (it5 != it4->value.MemberEnd()) {
                                                                rapidjson::Value::ConstMemberIterator it6 = it5->value.FindMember(
                                                                        "entityID");

                                                                has_m_eidParent = true;

                                                                if (it6 != it5->value.MemberEnd()) {
                                                                    //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityID is " << it6->value.GetUint64() << std::endl;

                                                                    m_eidParent_entityID = it6->value.GetUint64();
                                                                }

                                                                it6 = it5->value.FindMember("entityIndex");

                                                                if (it6 != it5->value.MemberEnd()) {
                                                                    //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityIndex is " << it6->value.GetUint64() << std::endl;

                                                                    m_eidParent_entityIndex = it6->value.GetInt();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    it2 = temp_json_subEntities[s].FindMember("postInitPropertyValues");

                                    if (it2 != temp_json_subEntities[s].MemberEnd()) {
                                        for (uint64_t p = 0; p < it2->value.Size(); p++) {
                                            rapidjson::Value::ConstMemberIterator it3 = it2->value[p].FindMember(
                                                    "nPropertyID");

                                            if (it3 != it2->value[p].MemberEnd() && it3->value.IsString()) {
                                                if (std::strcmp(it3->value.GetString(), "m_mTransform") == 0) {
                                                    has_m_mTransform = true;

                                                    const rapidjson::Value& value = it2->value[p]["value"];

                                                    if (!value.IsNull()) {
                                                        const rapidjson::Value& val = value["$val"];

                                                        if (!val.IsNull()) {
                                                            m_mTransform = "";

                                                            std::cout << val["XAxis"]["x"].GetFloat() << std::endl;
                                                            std::cout << val["XAxis"]["y"].GetFloat() << std::endl;
                                                            std::cout << val["XAxis"]["z"].GetFloat() << std::endl;
                                                            std::cout << val["YAxis"]["x"].GetFloat() << std::endl;
                                                            std::cout << val["YAxis"]["y"].GetFloat() << std::endl;
                                                            std::cout << val["YAxis"]["z"].GetFloat() << std::endl;
                                                            std::cout << val["ZAxis"]["x"].GetFloat() << std::endl;
                                                            std::cout << val["ZAxis"]["y"].GetFloat() << std::endl;
                                                            std::cout << val["ZAxis"]["z"].GetFloat() << std::endl;
                                                            std::cout << val["Trans"]["x"].GetFloat() << std::endl;
                                                            std::cout << val["Trans"]["y"].GetFloat() << std::endl;
                                                            std::cout << val["Trans"]["z"].GetFloat() << std::endl;

                                                            m_mTransform.append(util::float_to_string(
                                                                    val["XAxis"]["x"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["YAxis"]["x"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["ZAxis"]["x"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["XAxis"]["y"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["YAxis"]["y"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["ZAxis"]["y"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["XAxis"]["z"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["YAxis"]["z"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["ZAxis"]["z"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["Trans"]["x"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["Trans"]["y"].GetFloat()));
                                                            m_mTransform.push_back(',');
                                                            m_mTransform.append(util::float_to_string(
                                                                    val["Trans"]["z"].GetFloat()));
                                                        }
                                                    }

                                                    //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_mTransform in postInitPropertyValues" << std::endl;
                                                } else if (std::strcmp(it3->value.GetString(), "m_eidParent") == 0) {
                                                    //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " has a m_eidParent in postInitPropertyValues" << std::endl;

                                                    rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember(
                                                            "value");

                                                    if (it4 != it2->value[p].MemberEnd()) {
                                                        rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember(
                                                                "$val");

                                                        if (it5 != it4->value.MemberEnd()) {
                                                            rapidjson::Value::ConstMemberIterator it6 = it5->value.FindMember(
                                                                    "entityID");

                                                            has_m_eidParent = true;

                                                            if (it6 != it5->value.MemberEnd()) {
                                                                //std::cout <<temps.at(t).temp_file_name << ": " << temps.at(t).tblu_entityName.at(s) << " m_eidParent entityID is " << it6->value.GetUint64() << std::endl;

                                                                m_eidParent_entityID = it6->value.GetUint64();
                                                            }

                                                            it6 = it5->value.FindMember("entityIndex");

                                                            if (it6 != it5->value.MemberEnd()) {
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

                                entityId_temp_index.push_back(t);
                                entityId_m_eidParent_entityID.push_back(m_eidParent_entityID);
                                entityId_m_eidParent_entityIndex.push_back(s);
                                entityId_m_eidParent_entityIndex_map.insert(
                                        {std::to_string(t) + ":" + util::uint32_t_to_string(m_eidParent_entityIndex),
                                         entityId_m_eidParent_entityIndex_map.size()});
                                entityId_temp_index_entityIndex_map[std::to_string(t) + ":" + util::uint32_t_to_string(
                                        s)] = entityId_temp_index_entityIndex_map.size();
                                entityId_entity_logicalParent_top_level.push_back(logicalParent_top_level);

                                if (has_m_mTransform && has_m_eidParent) {
                                    has_m_mTransform_and_m_eidParent = true;
                                }

                                entityId_has_m_mTransform_and_m_eidParent.push_back(has_m_mTransform_and_m_eidParent);
                                entityId_m_mTransform.push_back(m_mTransform);
                            }
                        }
                    }

                    std::set<uint32_t> logical_parents_set;

                    for (uint32_t e = 0; e < temps.at(0).temp_logicalParent.size(); e++) {
                        if (temps.at(0).temp_logicalParent.at(e) >= temps.at(0).temp_logicalParent.size()) {
                            logical_parents_set.insert(temps.at(0).temp_logicalParent.at(e));
                        }
                    }

                    for (auto it = logical_parents_set.begin(); it != logical_parents_set.end(); it++) {
                        std::cout << *it << std::endl;

                        extract_glb_and_return_coords(0, *it, "", entityId_temp_index, entityId_m_eidParent_entityID,
                                                      entityId_m_eidParent_entityIndex,
                                                      entityId_m_eidParent_entityIndex_map,
                                                      entityId_entity_logicalParent_top_level,
                                                      entityId_has_m_mTransform_and_m_eidParent,
                                                      entityId_temp_index_entityIndex_map, entityId_m_mTransform);
                    }

                    /*std::unordered_map<uint64_t, uint64_t> entityId_map;
                    std::vector<uint64_t> entityId_value;
                    std::vector<uint64_t> entityId_temp_index;
                    std::vector<uint64_t> entityId_entity_index;
                    std::vector<uint64_t> entityId_entity_logicalParent_index;
                    std::vector<bool> entityId_entity_logicalParent_top_level;
                    std::unordered_map<std::string, uint64_t> entityId_temp_entity_index_map;
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

                                    std::unordered_map<uint64_t, uint64_t>::iterator itmap = entityId_map.find(it1->value.GetUint64());

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

                                                                            m_eidParent_entityIndex = it6->value.GetInt();
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
                    }

                    for (std::unordered_map<uint64_t, uint64_t>::iterator itmap = entityId_map.begin(); itmap != entityId_map.end(); itmap++)
                    {
                        map_recursive_parent_mapper(itmap->first, itmap->second, entityId_entityName.at(itmap->second), entityId_map, entityId_value, entityId_temp_index, entityId_entity_index, entityId_entity_logicalParent_index, entityId_entity_logicalParent_top_level, entityId_temp_entity_index_map, entityId_temp_entity_index_map_temp_hash_depend_map, entityId_entityName, entityId_m_eidParent_found, entityId_m_eidParent_entityID, entityId_m_eidParent_entityIndex);
                    }*/
                }
            }
        }
    }
}