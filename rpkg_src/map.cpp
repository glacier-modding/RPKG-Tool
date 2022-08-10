#include "rpkg_function.h"
#include "dev_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "crypto.h"
#include "temp.h"
#include "prim.h"
#include "gltf.h"
#include "map.h"
#include "generic_function.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <algorithm>
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/prettywriter.h"
#include "thirdparty/rapidjson/writer.h"
#include "thirdparty/rapidjson/pointer.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/directxmath/DirectXMath.h"
#include "thirdparty/directxmath/DirectXCollision.h"
#include "thirdparty/directxmath/DirectXColors.h"
#include "thirdparty/directxmath/DirectXPackedVector.h"

map::map()
{

}

map::map(uint64_t hash_value)
{

}

void map::export_map(std::string& input_path, std::string& filter, std::string& map_filters, std::string& output_path, bool textured)
{
    log_output = false;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        std::vector<std::string> filters = util::parse_input_filter(filter);

        if (filters.size() != 1)
        {
            task_status_string = "Error: This function only takes one hash filter value, and it should be the root map entity TEMP hash value.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        uint64_t hash_value = std::strtoull(filters.at(0).c_str(), nullptr, 16);

        uint32_t rpkg_index = rpkg_function::get_latest_hash(hash_value);

        if (rpkg_index == UINT32_MAX)
        {
            task_status_string = "Error: The input entity (TEMP) hash " + filters.at(0) + " could not be found in any RPKGs.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        std::unordered_map<uint64_t, uint64_t>::iterator ith = rpkgs.at(rpkg_index).hash_map.find(hash_value);

        std::string map_hash_file_name = "";

        if (ith != rpkgs.at(rpkg_index).hash_map.end())
        {
            map_hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(ith->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(ith->second).hash_resource_type;
        }
        else
        {
            task_status_string = "Error: The input entity (TEMP) hash " + filters.at(0) + " could not be found in any RPKGs.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        output_path = file::output_path_append("MAP_" + map_hash_file_name, output_path);

        file::create_directories(output_path);

        map temp_map(hash_value);

        std::vector<std::string> map_filters_values;

        std::smatch m;
        std::regex re("^([^, ]+)[, ]+");
        std::regex_search(map_filters, m, re);

        if (m.size() > 0)
        {
            map_filters_values.push_back(util::to_upper_case(m[1].str()));

            std::smatch m2;
            re.assign("[, ]+([^, ]+)");

            while (std::regex_search(map_filters, m2, re))
            {
                map_filters_values.push_back(util::to_upper_case(m2[1].str()));

                map_filters = m2.suffix().str();
            }
        }
        else
        {
            if (map_filters != "")
            {
                map_filters_values.push_back(util::to_upper_case(map_filters));
            }
        }

        if (map_filters_values.size() == 0)
        {
            LOG("No value for -map_filter specified, using the defaults:");
            LOG("Nodes With Volume Boxes: " + util::bool_to_string(temp_map.map_filter_volume_boxes));
            LOG("Nodes With Volume Spheres: " + util::bool_to_string(temp_map.map_filter_volume_spheres));
            LOG("Hide Nodes With m__bVisible==False In Godot: " + util::bool_to_string(temp_map.map_filter_visible));
        }
        else
        {
            bool map_filter_volume_boxes_found = false;
            bool map_filter_volume_spheres_found = false;
            bool map_filter_visible_found = false;

            for (uint32_t f = 0; f < map_filters_values.size(); f++)
            {
                bool filters_malformed = false;

                size_t pos = map_filters_values.at(f).find("=");

                if (pos != std::string::npos)
                {
                    std::string temp_map_filter = util::to_upper_case(map_filters_values.at(f).substr(0, pos));
                    std::string temp_map_filter_value = util::to_upper_case(map_filters_values.at(f).substr(pos + 1));

                    if (temp_map_filter == "" || temp_map_filter_value == "")
                    {
                        filters_malformed = true;
                    }
                    else
                    {
                        if (temp_map_filter_value == "TRUE" || temp_map_filter_value == "FALSE")
                        {
                            if (temp_map_filter == "BOXES")
                            {
                                map_filter_volume_boxes_found = true;

                                if (temp_map_filter_value == "TRUE")
                                {
                                    temp_map.map_filter_volume_boxes = true;
                                }
                                else if (temp_map_filter_value == "FALSE")
                                {
                                    temp_map.map_filter_volume_boxes = false;
                                }
                            }
                            else if (temp_map_filter == "SPHERES")
                            {
                                map_filter_volume_spheres_found = true;

                                if (temp_map_filter_value == "TRUE")
                                {
                                    temp_map.map_filter_volume_spheres = true;
                                }
                                else if (temp_map_filter_value == "FALSE")
                                {
                                    temp_map.map_filter_volume_spheres = false;
                                }
                            }
                            else if (temp_map_filter == "VISIBLE")
                            {
                                map_filter_visible_found = true;

                                if (temp_map_filter_value == "TRUE")
                                {
                                    temp_map.map_filter_visible = true;
                                }
                                else if (temp_map_filter_value == "FALSE")
                                {
                                    temp_map.map_filter_visible = false;
                                }
                            }
                        }
                        else
                        {
                            filters_malformed = true;
                        }
                    }
                }
                else
                {
                    filters_malformed = true;
                }

                if (filters_malformed)
                {
                    task_status_string = "Error: The input value for -map_filters is malformed.";

                    task_map_status = MAP_ERROR;

                    LOG_AND_RETURN(task_status_string);
                }
            }

            if (map_filter_volume_boxes_found || map_filter_volume_spheres_found || map_filter_visible_found)
            {
                LOG("Filters set via -map_filter:");

                if (map_filter_volume_boxes_found)
                {
                    LOG("Nodes With Volume Boxes: " + util::bool_to_string(temp_map.map_filter_volume_boxes));
                }

                if (map_filter_volume_spheres_found)
                {
                    LOG("Nodes With Volume Spheres: " + util::bool_to_string(temp_map.map_filter_volume_spheres));
                }

                if (map_filter_visible_found)
                {
                    LOG("Hide Nodes With m__bVisible==False In Godot: " + util::bool_to_string(temp_map.map_filter_visible));
                }
            }

            if (!map_filter_volume_boxes_found || !map_filter_volume_spheres_found || !map_filter_visible_found)
            {
                LOG("Using default map filter values for:");

                if (!map_filter_volume_boxes_found)
                {
                    LOG("Nodes With Volume Boxes: " + util::bool_to_string(temp_map.map_filter_volume_boxes));
                }

                if (!map_filter_volume_spheres_found)
                {
                    LOG("Nodes With Volume Spheres: " + util::bool_to_string(temp_map.map_filter_volume_spheres));
                }

                if (!map_filter_visible_found)
                {
                    LOG("Hide Nodes With m_bVisible==False In Godot: " + util::bool_to_string(temp_map.map_filter_visible));
                }
            }
        }

        task_map_status = MAP_RECURSIVE_TEMP_LOADING_EXECUTING;

        LOG("Calculating number of recursive entities (TEMP/TBLU)...");
        timing_string = "Calculating number of recursive entities (TEMP/TBLU)...";

        temp_map.map_recursive_temp_loader(rpkg_index, hash_value);

        LOG("Number of recursive entities (TEMP/TBLU) found: " + util::uint32_t_to_string(temp_map.map_temps.size()));
        timing_string = "Number of recursive entities (TEMP/TBLU) found: " + util::uint32_t_to_string(temp_map.map_temps.size());
        temp_map.start_time = std::chrono::high_resolution_clock::now();
        temp_map.stringstream_length = 80;
        temp_map.message = "Loading recursive TEMP/TBLU entities: ";
        timing_string = temp_map.message + "0% done";
    
        for (uint64_t t = 0; t < temp_map.map_temps.size(); t++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((t * (uint64_t)10000) / (uint64_t)temp_map.map_temps.size()) % (uint64_t)10 == 0 && t > 0)
            {
                temp_map.stringstream_length = console::update_console(temp_map.message, temp_map.map_temps.size(), t, temp_map.start_time, temp_map.stringstream_length);
            }

            map_percent_progress_recursive_temp = percent_progress;

            if (temp_map.map_temps.at(t).tblu_return_value == TEMP_TBLU_FOUND)
            {
                temp_map.map_temps.at(t).load_data();
            }
        }

        temp_map.end_time = std::chrono::high_resolution_clock::now();
        std::stringstream ss;
        ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
        LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
        timing_string = ss.str();

        temp_map.root_scenes[hash_value] = temp_map.root_scenes.size();

        temp_map.get_root_scenes();

        //std::unordered_map<uint64_t, uint64_t>::iterator it;

        //for (it = temp_map.root_scenes.begin(); it != temp_map.root_scenes.end(); ++it)
        //{
            //std::cout << "Root scenes (TEMPs): " << util::uint64_t_to_hex_string(it->first) << std::endl;
        //}

        task_map_status = MAP_GET_MAP_NODES_EXECUTING;

        LOG("Calculating number of map nodes...");
        timing_string = "Calculating number of map node...";
        temp_map.map_node_count = 0;

        for (uint64_t t = 0; t < temp_map.map_temps.size(); t++)
        {
            const rapidjson::Value& temp_json_subEntities = temp_map.map_temps.at(t).temp_json_document["subEntities"];

            temp_map.map_node_count += temp_json_subEntities.Size();
        }

        LOG("Number of map nodes found: " + util::uint32_t_to_string(temp_map.map_node_count));
        timing_string = "Number of map nodes found: " + util::uint32_t_to_string(temp_map.map_node_count);
        temp_map.start_time = std::chrono::high_resolution_clock::now();
        temp_map.stringstream_length = 80;
        temp_map.message = "Loading map nodes: ";
        timing_string = temp_map.message + "0% done";
        temp_map.map_node_count_current = 0;

        for (uint64_t t = 0; t < temp_map.map_temps.size(); t++)
        {
            temp_map.get_map_node(temp_map.map_temps.at(t));
        }

        temp_map.end_time = std::chrono::high_resolution_clock::now();
        ss.str(std::string(""));
        ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
        LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
        timing_string = ss.str();

        task_map_status = MAP_EXTRACT_MAP_NODES_PRIMS_EXECUTING;

        LOG("Calculating number of map nodes with PRIMs...");
        timing_string = "Calculating number of map node with PRIMs...";
        temp_map.map_node_prim_count = 0;

        for (uint64_t m = 0; m < temp_map.map_nodes.size(); m++)
        {
            if (temp_map.map_nodes.at(m).has_prim_resource)
            {
                temp_map.map_node_prim_count++;
            }
        }

        LOG("Number of map nodes with PRIMs found: " + util::uint32_t_to_string(temp_map.map_node_prim_count));
        timing_string = "Number of map nodes with PRIMs found: " + util::uint32_t_to_string(temp_map.map_node_prim_count);
        temp_map.start_time = std::chrono::high_resolution_clock::now();
        temp_map.stringstream_length = 80;
        temp_map.message = "Extracting all map's PRIMs as GLBs: ";
        timing_string = temp_map.message + "0% done";
        temp_map.map_node_count_current = 0;

        temp_map.extract_map_prims(output_path, textured);

        temp_map.end_time = std::chrono::high_resolution_clock::now();
        ss.str(std::string(""));
        ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
        LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
        timing_string = ss.str();

        /*for (uint64_t m = 0; m < temp_map.map_nodes.size(); m++)
        {
            std::cout << std::endl;
            std::cout << "temp_hash: " << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).temp_hash) << std::endl;
            std::cout << "entityId: " << temp_map.map_nodes.at(m).entityId << std::endl;
            std::cout << "subEntity: " << temp_map.map_nodes.at(m).subEntity << std::endl;
            std::cout << "entryName: " << temp_map.map_nodes.at(m).entryName << std::endl;
            std::cout << "entityTypeResourceIndex: " << temp_map.map_nodes.at(m).entityTypeResourceIndex << std::endl;
            std::cout << "entityTypeResourceIndex_hash: " << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).entityTypeResourceIndex_hash) << std::endl;
            std::cout << "entityTypeResourceIndex_hash_type: " << temp_map.map_nodes.at(m).entityTypeResourceIndex_hash_type << std::endl;

            std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(temp_map.map_nodes.at(m).entityTypeResourceIndex_hash);

            if (it2 != hash_list_hash_map.end())
            {
                std::cout << "entityTypeResourceIndex_hash IOI string: " << hash_list_hash_strings.at(it2->second) << std::endl;
            }

            if (temp_map.map_nodes.at(m).has_m_eidParent)
            {
                std::cout << "has_m_eidParent: " << temp_map.map_nodes.at(m).has_m_eidParent << std::endl;
                std::cout << "has_m_eidParent_local: " << temp_map.map_nodes.at(m).has_m_eidParent_local << std::endl;
                std::cout << "has_m_eidParent_external: " << temp_map.map_nodes.at(m).has_m_eidParent_external << std::endl;
                std::cout << "entityID: " << temp_map.map_nodes.at(m).entityID << std::endl;
                std::cout << "externalSceneIndex: " << temp_map.map_nodes.at(m).externalSceneIndex << std::endl;

                if (temp_map.map_nodes.at(m).has_m_eidParent_external)
                {
                    std::cout << "externalSceneIndex_hash: " << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).externalSceneIndex_hash) << std::endl;

                    std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(temp_map.map_nodes.at(m).externalSceneIndex_hash);

                    if (it2 != hash_list_hash_map.end())
                    {
                        std::cout << "externalSceneIndex_hash IOI string: " << hash_list_hash_strings.at(it2->second) << std::endl;
                    }
                }

                std::cout << "entityIndex: " << temp_map.map_nodes.at(m).entityIndex << std::endl;
            }

            if (temp_map.map_nodes.at(m).has_m_mTransform)
            {
                std::cout << "has_m_mTransform: " << temp_map.map_nodes.at(m).has_m_mTransform << std::endl;
            }

            //std::cout << "m_mTransform:" << map_nodes.at(m).m_mTransform << std::endl;

            if (temp_map.map_nodes.at(m).has_prim_resource)
            {
                std::cout << "has_prim_resource: " << temp_map.map_nodes.at(m).has_prim_resource << std::endl;
                std::cout << "prim_hash: " << temp_map.map_nodes.at(m).prim_hash << std::endl;
                std::cout << "m_IDHigh: " << temp_map.map_nodes.at(m).m_IDHigh << std::endl;
                std::cout << "m_IDLow: " << temp_map.map_nodes.at(m).m_IDLow << std::endl;
                std::cout << "prim_hash: " << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).prim_hash) << std::endl;

                it2 = hash_list_hash_map.find(temp_map.map_nodes.at(m).prim_hash);

                if (it2 != hash_list_hash_map.end())
                {
                    std::cout << "PRIM file IOI string: " << hash_list_hash_strings.at(it2->second) << std::endl;
                }
            }
        }*/

        for (uint64_t m = 0; m < temp_map.map_nodes.size(); m++)
        {
            bool is_parent_node_local = false;
            bool temp_has_m_eidParents = false;
            uint32_t local_temp_entity_count = 0;

            std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it = temp_map.temp_hash_map_node_map.find(temp_map.map_nodes.at(m).temp_hash);

            if (it != temp_map.temp_hash_map_node_map.end())
            {
                for (uint32_t i = 0; i < it->second.size(); i++)
                {
                    uint32_t n = it->second.at(i);

                    if (temp_map.map_nodes.at(n).has_m_eidParent_local)
                    {
                        local_temp_entity_count++;

                        temp_has_m_eidParents = true;

                        if (temp_map.map_nodes.at(m).subEntity == temp_map.map_nodes.at(n).entityIndex)
                        {
                            //std::cout << temp_map.map_nodes.at(m).entryName << "(" << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).temp_hash) << ")" << " is an m_eidParent to " << temp_map.map_nodes.at(n).entryName << std::endl;

                            is_parent_node_local = true;
                        }
                    }
                }
            }

            if (local_temp_entity_count == 0 || (!temp_has_m_eidParents && local_temp_entity_count > 0))
            {
                is_parent_node_local = true;
            }

            bool in_root_scene = false;

            for (std::unordered_map<uint64_t, uint64_t>::iterator it2 = temp_map.root_scenes.begin(); it2 != temp_map.root_scenes.end(); ++it2)
            {
                if (temp_map.map_nodes.at(m).temp_hash == it2->first)
                {
                    //std::cout << temp_map.map_nodes.at(m).entryName << "(" << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).temp_hash) << ")" << " is in root scene " << util::uint64_t_to_hex_string(it->first) << std::endl;

                    in_root_scene = true;
                }
            }

            if (!temp_map.map_nodes.at(m).has_m_eidParent && is_parent_node_local && in_root_scene)
            {
                //std::cout << temp_map.map_nodes.at(m).entryName << "(" << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).temp_hash) << ")" << " is a root scene top level map node!" << std::endl;

                temp_map.map_nodes.at(m).is_top_level_root_node = true;

                bool map_root_node_found = false;

                for (uint64_t n = 0; n < temp_map.map_root_nodes.size(); n++)
                {
                    if (m == temp_map.map_root_nodes.at(n))
                    {
                        map_root_node_found = true;
                    }
                }

                if (!map_root_node_found)
                {
                    temp_map.map_root_nodes.push_back(m);
                }
            }
            else if (!temp_map.map_nodes.at(m).has_m_eidParent && is_parent_node_local)
            {
                //std::cout << temp_map.map_nodes.at(m).entryName << "(" << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).temp_hash) << ")" << " is a local top level map node!" << std::endl;

                temp_map.map_nodes.at(m).is_top_level_Local_node = true;

                temp_map.add_to_temp_hash_is_top_level_Local_node_map(m, temp_map.map_nodes.at(m).temp_hash);
            }
        }

        temp_map.godot_tscn_file = "[gd_scene load_steps=" + util::uint32_t_to_string(temp_map.map_prims.size() + 2) + " format=3]\n\n";

        for (uint64_t p = 0; p < temp_map.map_prims.size(); p++)
        {
            std::string prim_hash_string = util::uint64_t_to_hex_string(temp_map.map_prims.at(p));

            temp_map.godot_tscn_file += "[ext_resource type=\"PackedScene\" path=\"res://" + prim_hash_string + ".PRIM.glb\" id=" + util::uint32_t_to_string(p) + "]\n";
        }

        temp_map.godot_tscn_file += "[node name=\"Node3D\" type=\"Node3D\"]\n";
        temp_map.godot_tscn_file += "transform = Transform3D(1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0)\n\n";

        task_map_status = MAP_GENERATE_GODOT_PROJECT_EXECUTING;

        temp_map.start_time = std::chrono::high_resolution_clock::now();
        temp_map.stringstream_length = 80;
        temp_map.message = "Generating Godot project files: ";
        timing_string = temp_map.message + "0% done";
        temp_map.map_node_count_current = 0;

        for (uint32_t m = 0; m < temp_map.map_root_nodes.size(); m++)
        {
            temp_map.generate_map_node_strings(temp_map.map_root_nodes.at(m), 0, "", 0);
        }

        /*std::string UE5_file = "map_root_nodes=";

        for (uint32_t m = 0; m < temp_map.map_root_nodes.size(); m++)
        {
            UE5_file += util::uint32_t_to_string(temp_map.map_root_nodes.at(m));

            if (m < (temp_map.map_root_nodes.size() - 1))
            {
                UE5_file += ",";
            }
        }

        UE5_file += "\n";

        for (uint32_t m = 0; m < temp_map.map_nodes.size(); m++)
        {
            if (temp_map.map_nodes.at(m).has_parent_map_node)
            {
                UE5_file += util::uint32_t_to_string(m) + ",";

                for (uint32_t p = 0; p < temp_map.map_nodes.at(m).parent_map_nodes.size(); p++)
                {
                    UE5_file += util::uint32_t_to_string(temp_map.map_nodes.at(m).parent_map_nodes.at(p));

                    if (p < (temp_map.map_nodes.at(m).parent_map_nodes.size() - 1))
                    {
                        UE5_file += "|";
                    }
                    else
                    {
                        UE5_file += ",";
                    }
                }

                UE5_file += temp_map.map_nodes.at(m).type + ",";
                UE5_file += temp_map.map_nodes.at(m).m_mTransform_string + ",";
                UE5_file += temp_map.map_nodes.at(m).has_prim_resource ? "1," : "0,";
                UE5_file += util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).prim_hash) + ",";
                UE5_file += util::string_to_hex_string(temp_map.map_nodes.at(m).entryName) + "\n";
            }
            else
            {
                for (uint32_t n = 0; n < temp_map.map_root_nodes.size(); n++)
                {
                    if (m == temp_map.map_root_nodes.at(n))
                    {
                        UE5_file += util::uint32_t_to_string(m) + ",";
                        UE5_file += "0,";
                        UE5_file += temp_map.map_nodes.at(m).type + ",";
                        UE5_file += "1,0,0,0,1,0,0,0,1,0,0,0,";
                        UE5_file += temp_map.map_nodes.at(m).has_prim_resource ? "1," : "0,";
                        UE5_file += util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).prim_hash) + ",";
                        UE5_file += util::string_to_hex_string(temp_map.map_nodes.at(m).entryName) + "\n";
                    }
                }
            }
        }

        if (!file::write_to_file(file::output_path_append("UE5.txt", output_path), UE5_file))
        {
            LOG("Error: File " + file::output_path_append("UE5.txt", output_path) + " could not be created.");
        }*/

        //std::cout << temp_map.godot_tscn_file << std::endl;

        std::string godot_godot_file = "config_version = 5\n\n";
        godot_godot_file += "[application]\n\n";
        godot_godot_file += "config/name=\"map\"\n";
        godot_godot_file += "run/main_scene=\"res://" + map_hash_file_name + ".tscn\"\n";
        godot_godot_file += "config/icon=\"res://icon.png\"\n";
        godot_godot_file += "config/features=PackedStringArray(\"4.0\", \"Vulkan Clustered\")\n";
        godot_godot_file += "[memory]\n";
        godot_godot_file += "limits/message_queue/max_size_kb = 2048000\n";

        if (!file::write_to_file(file::output_path_append(map_hash_file_name + ".tscn", output_path), temp_map.godot_tscn_file))
        {
            task_status_string = "Error: File " + file::output_path_append(map_hash_file_name + ".tscn", output_path) + " could not be created.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        if (!file::write_to_file(file::output_path_append("project.godot", output_path), godot_godot_file))
        {
            task_status_string = "Error: File " + file::output_path_append("project.godot", output_path) + " could not be created.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        temp_map.end_time = std::chrono::high_resolution_clock::now();
        ss.str(std::string(""));
        ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
        LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
        timing_string = ss.str();

        task_map_status = MAP_EXPORT_SUCCESSFUL;
    }
    else
    {
        task_status_string = "Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for map editor mode does not exist.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    log_output = true;
}

void map::import_map(std::string& input_path, std::string& map_path, std::string& qn_format, std::string& output_path)
{
    log_output = false;

    if (input_path == "")
    {
        task_status_string = "Error: The input_path folder does not exist.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    if (map_path == "")
    {
        task_status_string = "Error: The map_path folder " + map_path + " does not exist, missing -map_path in the command line?";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    bool input_path_is_rpkg_file = false;
    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    bool map_path_is_file = false;
    if (std::filesystem::is_regular_file(map_path))
    {
        map_path_is_file = true;
    }
    else
    {
        map_path = file::parse_input_folder_path(map_path);
    }

    if (!file::path_exists(input_path))
    {
        task_status_string = "Error: The input_path folder " + input_path + " does not exist.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    if (!file::path_exists(map_path))
    {
        task_status_string = "Error: The map_path folder " + map_path + " does not exist.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    if (map_path_is_file)
    {
        task_status_string = "Error: The map_path needs to be the path to the root map's folder and not a file.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    if (!input_path_is_rpkg_file)
    {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }
    else
    {
        rpkg_function::import_rpkg(input_path, false);
    }

    std::string map_file_path = "";
    std::string map_root_tscn_file_name = "";
    std::string map_root_hash_file_name = "";
    std::string map_root_hash_string = "";
    uint64_t map_root_hash_value = 0;

    for (const auto& entry : std::filesystem::directory_iterator(map_path))
    {
        if (map_root_hash_string == "")
        {
            std::cout << entry.path().string() << std::endl;

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::cout << entry.path().string() << std::endl;

                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string file_name = "";
                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos)
                {
                    file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    file_name = entry.path().string();
                }

                if (file_name.length() > 5)
                {
                    if (util::to_upper_case(file_name.substr((file_name.length() - 5), 5)) == ".TSCN")
                    {
                        hash_file_name = util::to_upper_case(file_name.substr(0, (file_name.length() - 5)));

                        pos = hash_file_name.find_last_of(".");

                        if (pos != std::string::npos)
                        {
                            hash_string = hash_file_name.substr(0, pos);
                            resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                        }

                        bool is_temp_hash_file = true;
                        bool temp_hash_meta = true;

                        if (hash_string.length() != 16)
                        {
                            is_temp_hash_file = false;
                        }

                        if (resource_type != "TEMP")
                        {
                            is_temp_hash_file = false;
                        }

                        if (is_temp_hash_file && temp_hash_meta)
                        {
                            map_file_path = entry.path().string();
                            map_root_tscn_file_name = file_name;
                            map_root_hash_file_name = hash_file_name;
                            map_root_hash_string = hash_string;
                            map_root_hash_value = std::strtoull(hash_string.c_str(), nullptr, 16);
                        }
                    }
                }
            }
        }
    }

    if (map_root_hash_string == "")
    {
        task_status_string = "Error: The map_path folder does not contain a Godot scene file of the format: 0000000000000000.TEMP.TSCN\nwhere 0000000000000000 is the hash value for the map's main entity (TEMP) file/resource.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    std::filesystem::path base_folder_path = input_path;

    std::string parent_path = base_folder_path.parent_path().string();

    rpkg_function::import_rpkg_files_in_folder(parent_path);

    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(true);
        LOG("Loading Hash List: Done");
    }

    uint32_t rpkg_index = rpkg_function::get_latest_hash(map_root_hash_value);

    if (rpkg_index == UINT32_MAX)
    {
        task_status_string = "Error: The input entity hash " + util::uint64_t_to_hex_string(map_root_hash_value) + " could not be found in any RPKGs.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    std::unordered_map<uint64_t, uint64_t>::iterator it6 = rpkgs.at(rpkg_index).hash_map.find(map_root_hash_value);

    if (it6 != rpkgs.at(rpkg_index).hash_map.end())
    {
        temp temp_temp(rpkg_index, it6->second, 3);

        temp_temp.load_data();

        //temp_temp.rt_json_to_qn_json();
    }

    std::ifstream input_file = std::ifstream(map_file_path);

    if (!input_file.good())
    {
        task_status_string = "Error: Godot tscn file " + map_file_path + " could not be read.";

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    input_file.seekg(0, input_file.end);
    uint64_t input_file_size = (uint64_t)input_file.tellg();
    input_file.seekg(0, input_file.beg);
    std::string godot_scene_file(input_file_size, '\0');
    input_file.read(&godot_scene_file[0], input_file_size);
    input_file.close();

    std::string_view godot_scene_file_string_view(godot_scene_file.c_str(), input_file_size);

    response_string = "";

    std::string godot_version_test = util::find_string_between_strings(godot_scene_file_string_view, "format=", "]");

    uint32_t godot_version = std::strtoul(godot_version_test.c_str(), nullptr, 10);
    godot_version++;

    if (godot_version < 3 || godot_version > 4)
    {
        task_status_string = "Error: Godot version could not be determined from the scene file " + map_root_tscn_file_name;

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }

    map temp_map;

    if (qn_format == "")
    {
        task_status_string = "No -qn_format parameter specified, using the default: " + temp_map.qn_format;

        task_map_status = MAP_ERROR;

        LOG_AND_RETURN(task_status_string);
    }
    else
    {
        qn_format = util::to_lower_case(qn_format);

        if (qn_format == "entity" || qn_format == "patch")
        {
            temp_map.qn_format = qn_format;
        }
        else
        {
            task_status_string = "Error: The -qn_format parameter must either be 'entity' or 'patch' (without the quotes).";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }
    }

    std::set<uint64_t> map_temp_hashes;
    std::vector<uint64_t> map_node_node_positions;

    task_map_status = MAP_GODOT_MAP_NODE_IMPORTING;

    LOG("Calculating number of map nodes in Godot scene file...");
    timing_string = "Calculating number of map nodes in Godot scene file...";

    util::split_string_view(godot_scene_file_string_view, "[node", map_node_node_positions);

    LOG("Number of map nodes in Godot scene file found: " + util::uint32_t_to_string(map_node_node_positions.size()));
    timing_string = "Number of map nodes in Godot scene file found: " + util::uint32_t_to_string(map_node_node_positions.size());
    temp_map.start_time = std::chrono::high_resolution_clock::now();
    temp_map.stringstream_length = 80;
    temp_map.message = "Importing map node data from Godot scene file: ";
    timing_string = temp_map.message + "0% done";

    for (uint64_t p = 1; p < map_node_node_positions.size(); p++)
    {
        if (gui_control == ABORT_CURRENT_TASK)
        {
            return;
        }

        if (((p * (uint64_t)10000) / (uint64_t)map_node_node_positions.size()) % (uint64_t)10 == 0 && p > 0)
        {
            temp_map.stringstream_length = console::update_console(temp_map.message, map_node_node_positions.size(), p, temp_map.start_time, temp_map.stringstream_length);
        }

        map_percent_progress_godot_map_nodes = percent_progress;

        map_node temp_map_node;

        //std::cout << map_node_node_positions.at(p) << std::endl;

        std::vector<uint64_t> map_node_name_positions;

        std::string_view temp_string_view;

        if (p == (map_node_node_positions.size() - 1))
        {
            temp_string_view = godot_scene_file_string_view.substr(map_node_node_positions.at(p));
        }
        else
        {
            temp_string_view = godot_scene_file_string_view.substr(map_node_node_positions.at(p), (map_node_node_positions.at(p + 1) - map_node_node_positions.at(p)));
        }

        temp_map_node.entryName = util::find_string_between_strings(temp_string_view, "name=\"", "\"");

        if (temp_string_view.find("CSGBox") != std::string::npos)
        {
            temp_map_node.is_volume_box = true;
        }

        if (temp_string_view.find("CSGSphere") != std::string::npos)
        {
            temp_map_node.is_volume_sphere = true;
        }

        std::string meta_data = util::find_string_between_strings(temp_string_view, "editor_description = \"", "\"");

        size_t pos1 = meta_data.find(",");

        if (pos1 != std::string::npos)
        {
            size_t pos2 = meta_data.substr(pos1 + 1).find(",");

            if (pos2 != std::string::npos)
            {
                std::string temp_hash = meta_data.substr(0, pos1);
                std::string entityId = meta_data.substr((pos1 + 1), pos2);
                std::string use_transform = meta_data.substr(pos1 + pos2 + 2);

                if (temp_hash != "" && entityId != "")
                {
                    temp_map_node.temp_hash_string = temp_hash;

                    temp_map_node.temp_hash = strtoull(temp_map_node.temp_hash_string.c_str(), nullptr, 16);

                    map_temp_hashes.insert(temp_map_node.temp_hash);

                    temp_map_node.entityId = strtoull(entityId.c_str(), nullptr, 16);

                    if (use_transform == "0")
                    {
                        temp_map_node.use_m_mTransform = false;
                    }
                    else if (use_transform == "1")
                    {
                        temp_map_node.use_m_mTransform = true;
                    }
                }
                else
                {
                    task_status_string = "Error: The Godot scene file " + map_root_tscn_file_name + " is corrupt.";

                    task_map_status = MAP_ERROR;

                    LOG_AND_RETURN(task_status_string);
                }
            }
            else
            {
                task_status_string = "Error: The Godot scene file " + map_root_tscn_file_name + " is corrupt.";

                task_map_status = MAP_ERROR;

                LOG_AND_RETURN(task_status_string);
            }
        }
        else
        {
            task_status_string = "Error: The Godot scene file " + map_root_tscn_file_name + " is corrupt.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        //std::cout << temp_map_node_name << std::endl;

        std::string temp_map_node_transform = "";

        if (godot_version == 3)
        {
            temp_map_node_transform = util::find_string_between_strings(temp_string_view, "Transform(", ")");
        }
        else
        {
            temp_map_node_transform = util::find_string_between_strings(temp_string_view, "Transform3D(", ")");
        }

        //std::cout << temp_map_node_transform << std::endl;

        std::vector<uint64_t> transform_comma_positions;

        temp_string_view = temp_map_node_transform;

        util::split_string_view(temp_string_view, ",", transform_comma_positions);

        if (transform_comma_positions.size() != 0 && transform_comma_positions.size() != 11)
        {
            task_status_string = "Error: The Godot scene file " + map_root_tscn_file_name + " is corrupt.";

            task_map_status = MAP_ERROR;

            LOG_AND_RETURN(task_status_string);
        }

        float matrix_values[12] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };

        if (transform_comma_positions.size() != 0)
        {
            for (uint64_t c = 0; c < (transform_comma_positions.size() + 1); c++)
            {
                if (c == 0)
                {
                    //std::cout << std::strtof(std::string(temp_string_view.substr(0, transform_comma_positions.at(c))).c_str(), nullptr) << std::endl;

                    matrix_values[c] = std::strtof(std::string(temp_string_view.substr(0, transform_comma_positions.at(c))).c_str(), nullptr);
                }
                else if (c == transform_comma_positions.size())
                {
                    //std::cout << std::strtof(std::string(temp_string_view.substr(transform_comma_positions.at(c - 1) + 1)).c_str(), nullptr) << std::endl;

                    matrix_values[c] = std::strtof(std::string(temp_string_view.substr(transform_comma_positions.at(c - 1) + 1)).c_str(), nullptr);
                }
                else
                {
                    //std::cout << std::strtof(std::string(temp_string_view.substr(transform_comma_positions.at(c - 1) + 1, transform_comma_positions.at(c))).c_str(), nullptr) << std::endl;

                    matrix_values[c] = std::strtof(std::string(temp_string_view.substr(transform_comma_positions.at(c - 1) + 1, transform_comma_positions.at(c))).c_str(), nullptr);
                }
            }
        }

        temp_map_node.m_mTransform.x_axis.x = matrix_values[0];
        temp_map_node.m_mTransform.y_axis.x = matrix_values[1];
        temp_map_node.m_mTransform.z_axis.x = matrix_values[2];
        temp_map_node.m_mTransform.x_axis.y = matrix_values[3];
        temp_map_node.m_mTransform.y_axis.y = matrix_values[4];
        temp_map_node.m_mTransform.z_axis.y = matrix_values[5];
        temp_map_node.m_mTransform.x_axis.z = matrix_values[6];
        temp_map_node.m_mTransform.y_axis.z = matrix_values[7];
        temp_map_node.m_mTransform.z_axis.z = matrix_values[8];
        temp_map_node.m_mTransform.transform.x = matrix_values[9];
        temp_map_node.m_mTransform.transform.y = matrix_values[10];
        temp_map_node.m_mTransform.transform.z = matrix_values[11];

        matrix43 matrix = temp_map_node.m_mTransform;
        vector3 scale;

        if (temp_map.extract_scale_from_transform(temp_map_node.m_mTransform, scale))
        {
            temp_map_node.scaled_on_import = true;
        }

        temp_map_node.m_PrimitiveScale = scale;
        temp_map_node.m_vGlobalSize = scale;

        temp_map.map_nodes.push_back(temp_map_node);
    }

    temp_map.end_time = std::chrono::high_resolution_clock::now();
    std::stringstream ss;
    ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
    LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
    timing_string = ss.str();

    task_map_status = MAP_NODE_TEMP_LOADING;

    LOG("Number of map node unique entities (TEMP/TBLU) found: " + util::uint32_t_to_string(map_temp_hashes.size()));
    timing_string = "Number of map node unique entities (TEMP/TBLU) found: " + util::uint32_t_to_string(map_temp_hashes.size());
    temp_map.start_time = std::chrono::high_resolution_clock::now();
    temp_map.stringstream_length = 80;
    temp_map.message = "Loading map node unique entities (TEMP/TBLU): ";
    timing_string = temp_map.message + "0% done";

    uint32_t map_node_temp_load_count = 0;

    for (const uint64_t& temp_hash : map_temp_hashes)
    {
        if (gui_control == ABORT_CURRENT_TASK)
        {
            return;
        }

        if (((map_node_temp_load_count * (uint64_t)10000) / (uint64_t)map_temp_hashes.size()) % (uint64_t)10 == 0 && map_node_temp_load_count > 0)
        {
            temp_map.stringstream_length = console::update_console(temp_map.message, map_temp_hashes.size(), map_node_temp_load_count, temp_map.start_time, temp_map.stringstream_length);
        }

        map_node_temp_load_count++;

        map_percent_progress_map_node_temp_loading = percent_progress;

        //std::cout << util::uint64_t_to_hex_string(temp_hash) << std::endl;

        uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_hash);

        if (rpkg_index != UINT32_MAX)
        {
            std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash);

            if (it != rpkgs.at(rpkg_index).hash_map.end())
            {
                temp_map.map_temps.emplace_back(temp(rpkg_index, it->second, 3));

                uint32_t temp_index = temp_map.map_temps.size() - 1;

                temp_map.map_temps.at(temp_index).load_data();

                temp_map.map_temps_map[temp_hash] = temp_map.map_temps_map.size();
            }
        }
    }

    temp_map.end_time = std::chrono::high_resolution_clock::now();
    ss.str(std::string(""));
    ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
    LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
    timing_string = ss.str();

    std::unordered_map<uint64_t, bool> temps_modified_map;

    task_map_status = MAP_NODE_CHANGES_CHECK;

    temp_map.start_time = std::chrono::high_resolution_clock::now();
    temp_map.stringstream_length = 80;
    temp_map.message = "Checking map nodes for changes: ";
    timing_string = temp_map.message + "0% done";

    for (uint64_t m = 0; m < temp_map.map_nodes.size(); m++)
    {
        if (gui_control == ABORT_CURRENT_TASK)
        {
            return;
        }

        if (((m * (uint64_t)10000) / (uint64_t)temp_map.map_nodes.size()) % (uint64_t)10 == 0 && m > 0)
        {
            temp_map.stringstream_length = console::update_console(temp_map.message, temp_map.map_nodes.size(), m, temp_map.start_time, temp_map.stringstream_length);
        }

        map_percent_progress_map_node_changes_check = percent_progress;

        if (temp_map.map_nodes.at(m).use_m_mTransform)
        {
            std::unordered_map<uint64_t, uint64_t>::iterator it = temp_map.map_temps_map.find(temp_map.map_nodes.at(m).temp_hash);

            if (it != temp_map.map_temps_map.end())
            {
                std::unordered_map<uint64_t, uint64_t>::iterator it2 = temp_map.map_temps.at(it->second).tblu_entityId_map.find(temp_map.map_nodes.at(m).entityId);

                if (it2 != temp_map.map_temps.at(it->second).tblu_entityId_map.end())
                {
                    bool has_transform = false;
                    bool has_primitive_scale = false;
                    bool has_global_size = false;

                    std::string transform_property_pointer = "";
                    std::string primitive_scale_property_pointer = "";
                    std::string global_size_property_pointer = "";
                    std::string transform_post_init_property_pointer = "";
                    std::string primitive_scale_post_init_property_pointer = "";
                    std::string global_size_post_init_property_pointer = "";
                    vector3 primitive_scale;
                    vector3 global_size;
                    matrix43 m_mTransform;

                    uint32_t propertyValues_count = 0;
                    uint32_t postInitPropertyValues_count = 0;
                    
                    std::string json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues";
                    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                    {
                        propertyValues_count = temp_json_value->GetArray().Size();
                    }

                    for (uint32_t p = 0; p < propertyValues_count; p++)
                    {
                        json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(p) + "/nPropertyID";
                        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                        {
                            if (temp_json_value->IsString())
                            {
                                if (std::strcmp(temp_json_value->GetString(), "m_mTransform") == 0)
                                {
                                    has_transform = true;

                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(p) + "/value/$val";

                                    transform_property_pointer = json_pointer;

                                    if (rapidjson::Value* temp_json_value2 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        m_mTransform.x_axis.x = temp_json_value2->GetObject()["XAxis"]["x"].GetFloat();
                                        m_mTransform.y_axis.x = temp_json_value2->GetObject()["YAxis"]["x"].GetFloat();
                                        m_mTransform.z_axis.x = temp_json_value2->GetObject()["ZAxis"]["x"].GetFloat();
                                        m_mTransform.x_axis.y = temp_json_value2->GetObject()["XAxis"]["y"].GetFloat();
                                        m_mTransform.y_axis.y = temp_json_value2->GetObject()["YAxis"]["y"].GetFloat();
                                        m_mTransform.z_axis.y = temp_json_value2->GetObject()["ZAxis"]["y"].GetFloat();
                                        m_mTransform.x_axis.z = temp_json_value2->GetObject()["XAxis"]["z"].GetFloat();
                                        m_mTransform.y_axis.z = temp_json_value2->GetObject()["YAxis"]["z"].GetFloat();
                                        m_mTransform.z_axis.z = temp_json_value2->GetObject()["ZAxis"]["z"].GetFloat();
                                        m_mTransform.transform.x = temp_json_value2->GetObject()["Trans"]["x"].GetFloat();
                                        m_mTransform.transform.y = temp_json_value2->GetObject()["Trans"]["y"].GetFloat();
                                        m_mTransform.transform.z = temp_json_value2->GetObject()["Trans"]["z"].GetFloat();
                                    }
                                }
                                else if (std::strcmp(temp_json_value->GetString(), "m_PrimitiveScale") == 0)
                                {
                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(p) + "/value/$type";
                                    if (rapidjson::Value* temp_json_value2 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        if (std::strcmp(temp_json_value2->GetString(), "SVector3") == 0)
                                        {
                                            json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(p) + "/value/$val";

                                            primitive_scale_property_pointer = json_pointer;

                                            if (rapidjson::Value* temp_json_value3 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                            {
                                                primitive_scale.x = temp_json_value3->GetObject()["x"].GetFloat();
                                                primitive_scale.y = temp_json_value3->GetObject()["y"].GetFloat();
                                                primitive_scale.z = temp_json_value3->GetObject()["z"].GetFloat();

                                                if (!util::floats_equal(primitive_scale.x, 0.0f) && !util::floats_equal(primitive_scale.y, 0.0f) && !util::floats_equal(primitive_scale.z, 0.0f))
                                                {
                                                    has_primitive_scale = true;
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (std::strcmp(temp_json_value->GetString(), "m_vGlobalSize") == 0)
                                {
                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(p) + "/value/$type";
                                    if (rapidjson::Value* temp_json_value2 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        if (std::strcmp(temp_json_value2->GetString(), "SVector3") == 0)
                                        {
                                            json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(p) + "/value/$val";

                                            global_size_property_pointer = json_pointer;

                                            if (rapidjson::Value* temp_json_value3 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                            {
                                                global_size.x = temp_json_value3->GetObject()["x"].GetFloat();
                                                global_size.y = temp_json_value3->GetObject()["y"].GetFloat();
                                                global_size.z = temp_json_value3->GetObject()["z"].GetFloat();

                                                if (!util::floats_equal(global_size.x, 0.0f) && !util::floats_equal(global_size.y, 0.0f) && !util::floats_equal(global_size.z, 0.0f))
                                                {
                                                    has_global_size = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues";
                    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                    {
                        postInitPropertyValues_count = temp_json_value->GetArray().Size();
                    }

                    for (uint32_t p = 0; p < propertyValues_count; p++)
                    {
                        json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues/" + util::uint32_t_to_string(p) + "/nPropertyID";
                        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                        {
                            if (temp_json_value->IsString())
                            {
                                if (std::strcmp(temp_json_value->GetString(), "m_mTransform") == 0)
                                {
                                    has_transform = true;

                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues/" + util::uint32_t_to_string(p) + "/value/$val";

                                    transform_post_init_property_pointer = json_pointer;

                                    if (rapidjson::Value* temp_json_value2 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        m_mTransform.x_axis.x = temp_json_value2->GetObject()["XAxis"]["x"].GetFloat();
                                        m_mTransform.y_axis.x = temp_json_value2->GetObject()["YAxis"]["x"].GetFloat();
                                        m_mTransform.z_axis.x = temp_json_value2->GetObject()["ZAxis"]["x"].GetFloat();
                                        m_mTransform.x_axis.y = temp_json_value2->GetObject()["XAxis"]["y"].GetFloat();
                                        m_mTransform.y_axis.y = temp_json_value2->GetObject()["YAxis"]["y"].GetFloat();
                                        m_mTransform.z_axis.y = temp_json_value2->GetObject()["ZAxis"]["y"].GetFloat();
                                        m_mTransform.x_axis.z = temp_json_value2->GetObject()["XAxis"]["z"].GetFloat();
                                        m_mTransform.y_axis.z = temp_json_value2->GetObject()["YAxis"]["z"].GetFloat();
                                        m_mTransform.z_axis.z = temp_json_value2->GetObject()["ZAxis"]["z"].GetFloat();
                                        m_mTransform.transform.x = temp_json_value2->GetObject()["Trans"]["x"].GetFloat();
                                        m_mTransform.transform.y = temp_json_value2->GetObject()["Trans"]["y"].GetFloat();
                                        m_mTransform.transform.z = temp_json_value2->GetObject()["Trans"]["z"].GetFloat();
                                    }
                                }
                                else if (std::strcmp(temp_json_value->GetString(), "m_PrimitiveScale") == 0)
                                {
                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues/" + util::uint32_t_to_string(p) + "/value/$type";
                                    if (rapidjson::Value* temp_json_value2 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        if (std::strcmp(temp_json_value2->GetString(), "SVector3") == 0)
                                        {
                                            json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues/" + util::uint32_t_to_string(p) + "/value/$val";

                                            primitive_scale_post_init_property_pointer = json_pointer;

                                            if (rapidjson::Value* temp_json_value3 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                            {
                                                primitive_scale.x = temp_json_value3->GetObject()["x"].GetFloat();
                                                primitive_scale.y = temp_json_value3->GetObject()["y"].GetFloat();
                                                primitive_scale.z = temp_json_value3->GetObject()["z"].GetFloat();

                                                if (!util::floats_equal(primitive_scale.x, 0.0f) && !util::floats_equal(primitive_scale.y, 0.0f) && !util::floats_equal(primitive_scale.z, 0.0f))
                                                {
                                                    has_primitive_scale = true;
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (std::strcmp(temp_json_value->GetString(), "m_vGlobalSize") == 0)
                                {
                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues/" + util::uint32_t_to_string(p) + "/value/$type";
                                    if (rapidjson::Value* temp_json_value2 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        if (std::strcmp(temp_json_value2->GetString(), "SVector3") == 0)
                                        {
                                            json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/postInitPropertyValues/" + util::uint32_t_to_string(p) + "/value/$val";

                                            global_size_post_init_property_pointer = json_pointer;

                                            if (rapidjson::Value* temp_json_value3 = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                            {
                                                global_size.x = temp_json_value3->GetObject()["x"].GetFloat();
                                                global_size.y = temp_json_value3->GetObject()["y"].GetFloat();
                                                global_size.z = temp_json_value3->GetObject()["z"].GetFloat();

                                                if (!util::floats_equal(global_size.x, 0.0f) && !util::floats_equal(global_size.y, 0.0f) && !util::floats_equal(global_size.z, 0.0f))
                                                {
                                                    has_global_size = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (has_transform && has_primitive_scale)
                    {
                        temp_map.scale_transform(m_mTransform, primitive_scale.x, primitive_scale.y, primitive_scale.z);

                        matrix43 node_m_mTransform = temp_map.map_nodes.at(m).m_mTransform;

                        temp_map.scale_transform(node_m_mTransform, primitive_scale.x, primitive_scale.y, primitive_scale.z);

                        if (!temp_map.matrixes_equal(node_m_mTransform, m_mTransform))
                        {
                            std::unordered_map<uint64_t, bool>::iterator temps_modified_map_it = temps_modified_map.find(temp_map.map_nodes.at(m).temp_hash);

                            if (temps_modified_map_it == temps_modified_map.end())
                            {
                                temps_modified_map[temp_map.map_nodes.at(m).temp_hash] = true;

                                if (temp_map.qn_format == "patch")
                                {
                                    temp_map.map_temps.at(it->second).temp_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_temps.at(it->second).temp_json_document_original.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document, temp_map.map_temps.at(it->second).tblu_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).rt_json_to_qn_json();

                                    temp_map.map_temps.at(it->second).qn_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).qn_json_document, temp_map.map_temps.at(it->second).qn_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).temp_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).tblu_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).qn_json_document = rapidjson::Document();

                                    temp_map.map_temps.at(it->second).temp_json_document.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document_original, temp_map.map_temps.at(it->second).temp_json_document.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document_original, temp_map.map_temps.at(it->second).tblu_json_document.GetAllocator());
                                }
                            }

                            if (transform_property_pointer == "")
                            {
                                transform_property_pointer = transform_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(transform_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["XAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.x);
                                temp_json_value->GetObject()["YAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.x);
                                temp_json_value->GetObject()["ZAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.x);
                                temp_json_value->GetObject()["XAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.y);
                                temp_json_value->GetObject()["YAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.y);
                                temp_json_value->GetObject()["ZAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.y);
                                temp_json_value->GetObject()["XAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.z);
                                temp_json_value->GetObject()["YAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.z);
                                temp_json_value->GetObject()["ZAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.z);
                                temp_json_value->GetObject()["Trans"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.x);
                                temp_json_value->GetObject()["Trans"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.y);
                                temp_json_value->GetObject()["Trans"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.z);
                            }

                            if (primitive_scale_property_pointer == "")
                            {
                                primitive_scale_property_pointer = primitive_scale_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(primitive_scale_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["x"].SetFloat(temp_map.map_nodes.at(m).m_PrimitiveScale.x);
                                temp_json_value->GetObject()["y"].SetFloat(temp_map.map_nodes.at(m).m_PrimitiveScale.y);
                                temp_json_value->GetObject()["z"].SetFloat(temp_map.map_nodes.at(m).m_PrimitiveScale.z);
                            }
                        }
                    }
                    else if (has_transform && has_global_size)
                    {
                        temp_map.scale_transform(m_mTransform, global_size.x, global_size.y, global_size.z);

                        matrix43 node_m_mTransform = temp_map.map_nodes.at(m).m_mTransform;

                        temp_map.scale_transform(node_m_mTransform, global_size.x, global_size.y, global_size.z);

                        if (!temp_map.matrixes_equal(node_m_mTransform, m_mTransform))
                        {
                            std::unordered_map<uint64_t, bool>::iterator temps_modified_map_it = temps_modified_map.find(temp_map.map_nodes.at(m).temp_hash);

                            if (temps_modified_map_it == temps_modified_map.end())
                            {
                                temps_modified_map[temp_map.map_nodes.at(m).temp_hash] = true;

                                if (temp_map.qn_format == "patch")
                                {
                                    temp_map.map_temps.at(it->second).temp_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_temps.at(it->second).temp_json_document_original.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document, temp_map.map_temps.at(it->second).tblu_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).rt_json_to_qn_json();

                                    temp_map.map_temps.at(it->second).qn_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).qn_json_document, temp_map.map_temps.at(it->second).qn_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).temp_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).tblu_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).qn_json_document = rapidjson::Document();

                                    temp_map.map_temps.at(it->second).temp_json_document.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document_original, temp_map.map_temps.at(it->second).temp_json_document.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document_original, temp_map.map_temps.at(it->second).tblu_json_document.GetAllocator());
                                }
                            }

                            if (transform_property_pointer == "")
                            {
                                transform_property_pointer = transform_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(transform_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["XAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.x);
                                temp_json_value->GetObject()["YAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.x);
                                temp_json_value->GetObject()["ZAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.x);
                                temp_json_value->GetObject()["XAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.y);
                                temp_json_value->GetObject()["YAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.y);
                                temp_json_value->GetObject()["ZAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.y);
                                temp_json_value->GetObject()["XAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.z);
                                temp_json_value->GetObject()["YAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.z);
                                temp_json_value->GetObject()["ZAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.z);
                                temp_json_value->GetObject()["Trans"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.x);
                                temp_json_value->GetObject()["Trans"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.y);
                                temp_json_value->GetObject()["Trans"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.z);
                            }

                            if (global_size_property_pointer == "")
                            {
                                global_size_property_pointer = global_size_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(global_size_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["x"].SetFloat(temp_map.map_nodes.at(m).m_vGlobalSize.x);
                                temp_json_value->GetObject()["y"].SetFloat(temp_map.map_nodes.at(m).m_vGlobalSize.y);
                                temp_json_value->GetObject()["z"].SetFloat(temp_map.map_nodes.at(m).m_vGlobalSize.z);
                            }
                        }
                    }
                    else if (has_transform && !has_primitive_scale && !has_global_size && temp_map.map_nodes.at(m).scaled_on_import && !temp_map.map_nodes.at(m).is_volume_box)
                    {
                        if (!temp_map.matrixes_equal(temp_map.map_nodes.at(m).m_mTransform, m_mTransform))
                        {
                            std::unordered_map<uint64_t, bool>::iterator temps_modified_map_it = temps_modified_map.find(temp_map.map_nodes.at(m).temp_hash);

                            if (temps_modified_map_it == temps_modified_map.end())
                            {
                                temps_modified_map[temp_map.map_nodes.at(m).temp_hash] = true;

                                if (temp_map.qn_format == "patch")
                                {
                                    temp_map.map_temps.at(it->second).temp_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_temps.at(it->second).temp_json_document_original.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document, temp_map.map_temps.at(it->second).tblu_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).rt_json_to_qn_json();

                                    temp_map.map_temps.at(it->second).qn_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).qn_json_document, temp_map.map_temps.at(it->second).qn_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).temp_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).tblu_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).qn_json_document = rapidjson::Document();

                                    temp_map.map_temps.at(it->second).temp_json_document.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document_original, temp_map.map_temps.at(it->second).temp_json_document.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document_original, temp_map.map_temps.at(it->second).tblu_json_document.GetAllocator());
                                }
                            }

                            if (transform_property_pointer == "")
                            {
                                transform_property_pointer = transform_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(transform_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["XAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.x);
                                temp_json_value->GetObject()["YAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.x);
                                temp_json_value->GetObject()["ZAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.x);
                                temp_json_value->GetObject()["XAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.y);
                                temp_json_value->GetObject()["YAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.y);
                                temp_json_value->GetObject()["ZAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.y);
                                temp_json_value->GetObject()["XAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.z);
                                temp_json_value->GetObject()["YAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.z);
                                temp_json_value->GetObject()["ZAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.z);
                                temp_json_value->GetObject()["Trans"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.x);
                                temp_json_value->GetObject()["Trans"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.y);
                                temp_json_value->GetObject()["Trans"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.z);
                            }

                            json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(propertyValues_count);
                            std::string json_pointer_value = json_pointer + "/nPropertyID";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, "m_PrimitiveScale");
                            json_pointer_value = json_pointer + "/value/$type";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, "SVector3");
                            json_pointer_value = json_pointer + "/value/$val/x";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_nodes.at(m).m_PrimitiveScale.x);
                            json_pointer_value = json_pointer + "/value/$val/y";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_nodes.at(m).m_PrimitiveScale.y);
                            json_pointer_value = json_pointer + "/value/$val/z";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_nodes.at(m).m_PrimitiveScale.z);
                        }
                    }
                    else if (has_transform && !has_primitive_scale && !has_global_size && temp_map.map_nodes.at(m).scaled_on_import && temp_map.map_nodes.at(m).is_volume_box)
                    {
                        if (!temp_map.matrixes_equal(temp_map.map_nodes.at(m).m_mTransform, m_mTransform))
                        {
                            std::unordered_map<uint64_t, bool>::iterator temps_modified_map_it = temps_modified_map.find(temp_map.map_nodes.at(m).temp_hash);

                            if (temps_modified_map_it == temps_modified_map.end())
                            {
                                temps_modified_map[temp_map.map_nodes.at(m).temp_hash] = true;

                                if (temp_map.qn_format == "patch")
                                {
                                    temp_map.map_temps.at(it->second).temp_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_temps.at(it->second).temp_json_document_original.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document, temp_map.map_temps.at(it->second).tblu_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).rt_json_to_qn_json();

                                    temp_map.map_temps.at(it->second).qn_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).qn_json_document, temp_map.map_temps.at(it->second).qn_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).temp_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).tblu_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).qn_json_document = rapidjson::Document();

                                    temp_map.map_temps.at(it->second).temp_json_document.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document_original, temp_map.map_temps.at(it->second).temp_json_document.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document_original, temp_map.map_temps.at(it->second).tblu_json_document.GetAllocator());
                                }
                            }

                            if (transform_property_pointer == "")
                            {
                                transform_property_pointer = transform_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(transform_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["XAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.x);
                                temp_json_value->GetObject()["YAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.x);
                                temp_json_value->GetObject()["ZAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.x);
                                temp_json_value->GetObject()["XAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.y);
                                temp_json_value->GetObject()["YAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.y);
                                temp_json_value->GetObject()["ZAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.y);
                                temp_json_value->GetObject()["XAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.z);
                                temp_json_value->GetObject()["YAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.z);
                                temp_json_value->GetObject()["ZAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.z);
                                temp_json_value->GetObject()["Trans"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.x);
                                temp_json_value->GetObject()["Trans"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.y);
                                temp_json_value->GetObject()["Trans"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.z);
                            }

                            json_pointer = "/subEntities/" + util::uint32_t_to_string(it2->second) + "/propertyValues/" + util::uint32_t_to_string(propertyValues_count);
                            std::string json_pointer_value = json_pointer + "/nPropertyID";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, "m_vGlobalSize");
                            json_pointer_value = json_pointer + "/value/$type";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, "SVector3");
                            json_pointer_value = json_pointer + "/value/$val/x";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_nodes.at(m).m_vGlobalSize.x);
                            json_pointer_value = json_pointer + "/value/$val/y";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_nodes.at(m).m_vGlobalSize.y);
                            json_pointer_value = json_pointer + "/value/$val/z";
                            rapidjson::Pointer(json_pointer_value.c_str()).Set(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_nodes.at(m).m_vGlobalSize.z);
                        }
                    }
                    else if (has_transform)
                    {
                        if (!temp_map.matrixes_equal(temp_map.map_nodes.at(m).m_mTransform, m_mTransform))
                        {
                            std::unordered_map<uint64_t, bool>::iterator temps_modified_map_it = temps_modified_map.find(temp_map.map_nodes.at(m).temp_hash);

                            if (temps_modified_map_it == temps_modified_map.end())
                            {
                                temps_modified_map[temp_map.map_nodes.at(m).temp_hash] = true;

                                if (temp_map.qn_format == "patch")
                                {
                                    temp_map.map_temps.at(it->second).temp_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document, temp_map.map_temps.at(it->second).temp_json_document_original.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document, temp_map.map_temps.at(it->second).tblu_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).rt_json_to_qn_json();

                                    temp_map.map_temps.at(it->second).qn_json_document_original.CopyFrom(temp_map.map_temps.at(it->second).qn_json_document, temp_map.map_temps.at(it->second).qn_json_document_original.GetAllocator());

                                    temp_map.map_temps.at(it->second).temp_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).tblu_json_document = rapidjson::Document();
                                    temp_map.map_temps.at(it->second).qn_json_document = rapidjson::Document();

                                    temp_map.map_temps.at(it->second).temp_json_document.CopyFrom(temp_map.map_temps.at(it->second).temp_json_document_original, temp_map.map_temps.at(it->second).temp_json_document.GetAllocator());
                                    temp_map.map_temps.at(it->second).tblu_json_document.CopyFrom(temp_map.map_temps.at(it->second).tblu_json_document_original, temp_map.map_temps.at(it->second).tblu_json_document.GetAllocator());
                                }
                            }

                            if (transform_property_pointer == "")
                            {
                                transform_property_pointer = transform_post_init_property_pointer;
                            }

                            if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_map.map_temps.at(it->second).temp_json_document, rapidjson::Pointer(transform_property_pointer.c_str())))
                            {
                                temp_json_value->GetObject()["XAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.x);
                                temp_json_value->GetObject()["YAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.x);
                                temp_json_value->GetObject()["ZAxis"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.x);
                                temp_json_value->GetObject()["XAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.y);
                                temp_json_value->GetObject()["YAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.y);
                                temp_json_value->GetObject()["ZAxis"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.y);
                                temp_json_value->GetObject()["XAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.x_axis.z);
                                temp_json_value->GetObject()["YAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.y_axis.z);
                                temp_json_value->GetObject()["ZAxis"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.z_axis.z);
                                temp_json_value->GetObject()["Trans"]["x"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.x);
                                temp_json_value->GetObject()["Trans"]["y"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.y);
                                temp_json_value->GetObject()["Trans"]["z"].SetFloat(temp_map.map_nodes.at(m).m_mTransform.transform.z);
                            }
                        }
                    }
                }
            }
        }
    }

    temp_map.end_time = std::chrono::high_resolution_clock::now();
    ss.str(std::string(""));
    ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
    LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
    timing_string = ss.str();

    std::unordered_map<uint64_t, bool>::iterator temps_modified_map_it;

    task_map_status = MAP_WRITING_CHANGES_TO_QN;

    temp_map.start_time = std::chrono::high_resolution_clock::now();
    temp_map.stringstream_length = 80;
    temp_map.message = "Writing changes to QN (QuickEntity) JSON files: ";
    timing_string = temp_map.message + "0% done";

    uint32_t map_node_jsons_count = 0;

    for (temps_modified_map_it = temps_modified_map.begin(); temps_modified_map_it != temps_modified_map.end(); ++temps_modified_map_it)
    {
        if (gui_control == ABORT_CURRENT_TASK)
        {
            return;
        }

        if (map_node_jsons_count > 0)
        {
            temp_map.stringstream_length = console::update_console(temp_map.message, temps_modified_map.size(), map_node_jsons_count, temp_map.start_time, temp_map.stringstream_length);
        }

        map_node_jsons_count++;

        map_percent_progress_map_writing_changes_to_qn = percent_progress;

        //std::cout << "Modified: " << temps_modified_map_it->first << std::endl;

        std::unordered_map<uint64_t, uint64_t>::iterator it_temp = temp_map.map_temps_map.find(temps_modified_map_it->first);

        if (it_temp != temp_map.map_temps_map.end())
        {
            temp_map.map_temps.at(it_temp->second).rt_json_to_qn_json();

            std::string rpkg_file_short_name = util::to_lower_case(rpkgs.at(temp_map.map_temps.at(it_temp->second).temp_rpkg_index).rpkg_file_name);

            size_t pos = rpkg_file_short_name.find("patch");

            if (pos != std::string::npos)
            {
                rpkg_file_short_name = rpkg_file_short_name.substr(0, pos);
            }
            else
            {
                pos = rpkg_file_short_name.find(".");

                if (pos != std::string::npos)
                {
                    rpkg_file_short_name = rpkg_file_short_name.substr(0, pos);
                }
            }

            std::string output_rpkg_json_path = file::output_path_append(rpkg_file_short_name, output_path);

            file::create_directories(output_rpkg_json_path);

            std::string json_base_file_name = output_rpkg_json_path + "\\" + util::uint64_t_to_hex_string(rpkgs.at(temp_map.map_temps.at(it_temp->second).temp_rpkg_index).hash.at(temp_map.map_temps.at(it_temp->second).temp_hash_index).hash_value);

            //std::cout << file::output_path_append(json_base_file_name + ".entity.json", map_path) << std::endl;

            if (temp_map.qn_format == "entity")
            {
                temp_map.map_temps.at(it_temp->second).write_qn_json_to_file(json_base_file_name + ".entity.json");
            }
            else if (temp_map.qn_format == "patch")
            {
                temp_map.map_temps.at(it_temp->second).generate_qn_patch_json();

                //std::cout << file::output_path_append(json_base_file_name + ".entity.patch.json", map_path) << std::endl;

                temp_map.map_temps.at(it_temp->second).write_qn_patch_json_to_file(json_base_file_name + ".entity.patch.json");
            }
        }
    }

    temp_map.end_time = std::chrono::high_resolution_clock::now();
    ss.str(std::string(""));
    ss << temp_map.message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(temp_map.end_time - temp_map.start_time).count()) << "s";
    LOG("\r" << ss.str() << std::string((temp_map.stringstream_length - ss.str().length()), ' '));
    timing_string = ss.str();

    task_map_status = MAP_IMPORT_SUCCESSFUL;

    log_output = true;
}

void map::add_to_temp_hash_map_node_map(uint32_t map_node, uint64_t temp_hash)
{
    std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_map_node_map.find(temp_hash);

    if (it != temp_hash_map_node_map.end())
    {
        it->second.push_back(map_node);
    }
    else
    {
        std::vector<uint32_t> temp_map_node_vector;

        temp_map_node_vector.push_back(map_node);

        temp_hash_map_node_map[temp_hash] = temp_map_node_vector;
    }
}

void map::add_to_m_eidParent_local_temp_hash_entityIndex_map(uint32_t map_node, uint64_t temp_hash, uint32_t entityIndex)
{
    std::unordered_map<uint64_t, std::unordered_map<uint32_t, std::vector<uint32_t>>>::iterator it = m_eidParent_local_temp_hash_entityIndex_map.find(temp_hash);

    if (it != m_eidParent_local_temp_hash_entityIndex_map.end())
    {
        std::unordered_map<uint32_t, std::vector<uint32_t>>::iterator it2 = it->second.find(entityIndex);

        if (it2 != it->second.end())
        {
            it2->second.push_back(map_node);
        }
        else
        {
            std::vector<uint32_t> temp_map_node_vector;

            temp_map_node_vector.push_back(map_node);

            it->second[entityIndex] = temp_map_node_vector;
        }
    }
    else
    {
        std::unordered_map<uint32_t, std::vector<uint32_t>> temp_temp_hash_entityIndex_map;

        std::vector<uint32_t> temp_map_node_vector;

        temp_map_node_vector.push_back(map_node);

        temp_temp_hash_entityIndex_map[entityIndex] = temp_map_node_vector;

        m_eidParent_local_temp_hash_entityIndex_map[temp_hash] = temp_temp_hash_entityIndex_map;
    }
}

void map::add_to_m_eidParent_external_externalSceneIndex_hash_entityID_map(uint32_t map_node, uint64_t externalSceneIndex_hash, uint64_t entityID)
{
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::vector<uint32_t>>>::iterator it = m_eidParent_external_externalSceneIndex_hash_entityID_map.find(externalSceneIndex_hash);

    if (it != m_eidParent_external_externalSceneIndex_hash_entityID_map.end())
    {
        std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it2 = it->second.find(entityID);

        if (it2 != it->second.end())
        {
            it2->second.push_back(map_node);
        }
        else
        {
            std::vector<uint32_t> temp_map_node_vector;

            temp_map_node_vector.push_back(map_node);

            it->second[entityID] = temp_map_node_vector;
        }
    }
    else
    {
        std::unordered_map<uint64_t, std::vector<uint32_t>> temp_externalSceneIndex_hash_entityID_map;

        std::vector<uint32_t> temp_map_node_vector;

        temp_map_node_vector.push_back(map_node);

        temp_externalSceneIndex_hash_entityID_map[entityID] = temp_map_node_vector;

        m_eidParent_external_externalSceneIndex_hash_entityID_map[externalSceneIndex_hash] = temp_externalSceneIndex_hash_entityID_map;
    }
}

void map::add_to_temp_hash_is_top_level_Local_node_map(uint32_t map_node, uint64_t temp_hash)
{
    std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_is_top_level_Local_node_map.find(temp_hash);

    if (it != temp_hash_is_top_level_Local_node_map.end())
    {
        it->second.push_back(map_node);
    }
    else
    {
        std::vector<uint32_t> temp_map_node_vector;

        temp_map_node_vector.push_back(map_node);

        temp_hash_is_top_level_Local_node_map[temp_hash] = temp_map_node_vector;
    }
}

void map::extract_map_prims(std::string output_path, bool textured)
{
    for (uint64_t m = 0; m < map_nodes.size(); m++)
    {
        if (map_nodes.at(m).has_prim_resource)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((map_node_prim_count_current * (uint64_t)10000) / (uint64_t)map_node_prim_count) % (uint64_t)10 == 0 && map_node_prim_count_current > 0)
            {
                stringstream_length = console::update_console(message, map_node_prim_count, map_node_prim_count_current, start_time, stringstream_length);
            }

            map_node_prim_count_current++;

            map_percent_progress_map_nodes_prim = percent_progress;

            std::unordered_map<uint64_t, uint64_t>::iterator it = map_prims_map.find(map_nodes.at(m).prim_hash);

            if (it != map_prims_map.end())
            {
                //std::cout << map_nodes.at(m).prim_hash << " was already found/added." << std::endl;
            }
            else
            {
                uint32_t rpkg_index = rpkg_function::get_latest_hash(map_nodes.at(m).prim_hash);

                if (rpkg_index != UINT32_MAX)
                {
                    std::unordered_map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index).hash_map.find(map_nodes.at(m).prim_hash);

                    std::string prim_hash_string = util::uint64_t_to_hex_string(map_nodes.at(m).prim_hash);

                    if (it2 != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        if (textured)
                        {
                            rpkg_function::extract_prim_textured_from(rpkgs.at(rpkg_index).rpkg_file_path, prim_hash_string, output_path, false);
                        }
                        else
                        {
                            rpkg_function::extract_prim_from(rpkgs.at(rpkg_index).rpkg_file_path, prim_hash_string, output_path, GLB_SINGLE, false);
                        }
                    }

                    if (file::path_exists(file::output_path_append(prim_hash_string + ".PRIM.glb", output_path)))
                    {
                        map_prims_map[map_nodes.at(m).prim_hash] = map_prims_map.size();

                        map_prims.push_back(map_nodes.at(m).prim_hash);

                        map_nodes.at(m).map_prims_index = map_prims.size() - 1;
                    }
                    else
                    {
                        map_nodes.at(m).has_prim_resource = false;
                    }
                }
            }
        }
    }
}

void map::scale_transform(matrix43& transform, float x, float y, float z)
{
    transform.x_axis.x *= x;
    transform.x_axis.y *= x;
    transform.x_axis.z *= x;
    transform.y_axis.x *= y;
    transform.y_axis.y *= y;
    transform.y_axis.z *= y;
    transform.z_axis.x *= z;
    transform.z_axis.y *= z;
    transform.z_axis.z *= z;
}

bool map::extract_scale_from_transform(matrix43& transform, vector3& scale)
{
    bool scaled = false;

    DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();

    matrix.r[0].m128_f32[0] = transform.x_axis.x;
    matrix.r[0].m128_f32[1] = transform.x_axis.y;
    matrix.r[0].m128_f32[2] = transform.x_axis.z;
    matrix.r[1].m128_f32[0] = transform.y_axis.x;
    matrix.r[1].m128_f32[1] = transform.y_axis.y;
    matrix.r[1].m128_f32[2] = transform.y_axis.z;
    matrix.r[2].m128_f32[0] = transform.z_axis.x;
    matrix.r[2].m128_f32[1] = transform.z_axis.y;
    matrix.r[2].m128_f32[2] = transform.z_axis.z;
    matrix.r[3].m128_f32[0] = transform.transform.x;
    matrix.r[3].m128_f32[1] = transform.transform.y;
    matrix.r[3].m128_f32[2] = transform.transform.z;

    /*std::cout << "matrix.r[0].m128_f32[0]: " << matrix.r[0].m128_f32[0] << std::endl;
    std::cout << "matrix.r[0].m128_f32[1]: " << matrix.r[0].m128_f32[1] << std::endl;
    std::cout << "matrix.r[0].m128_f32[2]: " << matrix.r[0].m128_f32[2] << std::endl;
    std::cout << "matrix.r[1].m128_f32[0]: " << matrix.r[1].m128_f32[0] << std::endl;
    std::cout << "matrix.r[1].m128_f32[1]: " << matrix.r[1].m128_f32[1] << std::endl;
    std::cout << "matrix.r[1].m128_f32[2]: " << matrix.r[1].m128_f32[2] << std::endl;
    std::cout << "matrix.r[2].m128_f32[0]: " << matrix.r[2].m128_f32[0] << std::endl;
    std::cout << "matrix.r[2].m128_f32[1]: " << matrix.r[2].m128_f32[1] << std::endl;
    std::cout << "matrix.r[2].m128_f32[2]: " << matrix.r[2].m128_f32[2] << std::endl;
    std::cout << "matrix.r[3].m128_f32[0]: " << matrix.r[3].m128_f32[0] << std::endl;
    std::cout << "matrix.r[3].m128_f32[1]: " << matrix.r[3].m128_f32[1] << std::endl;
    std::cout << "matrix.r[3].m128_f32[2]: " << matrix.r[3].m128_f32[2] << std::endl;*/

    DirectX::XMVECTOR scale_vector;
    DirectX::XMVECTOR rotation_vector;
    DirectX::XMVECTOR transform_vector;

    DirectX::XMMatrixDecompose(&scale_vector, &rotation_vector, &transform_vector, matrix);

    scale.x = scale_vector.m128_f32[0];
    scale.y = scale_vector.m128_f32[1];
    scale.z = scale_vector.m128_f32[2];

    /*std::cout << "scale.x: " << scale.x << std::endl;
    std::cout << "scale.y: " << scale.y << std::endl;
    std::cout << "scale.z: " << scale.z << std::endl;

    std::cout << "scale_vector.m128_f32[0]: " << scale_vector.m128_f32[0] << std::endl;
    std::cout << "scale_vector.m128_f32[1]: " << scale_vector.m128_f32[1] << std::endl;
    std::cout << "scale_vector.m128_f32[2]: " << scale_vector.m128_f32[2] << std::endl;
    std::cout << "scale_vector.m128_f32[3]: " << scale_vector.m128_f32[3] << std::endl;*/

    scale_vector.m128_f32[0] = 1.0f;
    scale_vector.m128_f32[1] = 1.0f;
    scale_vector.m128_f32[2] = 1.0f;
    scale_vector.m128_f32[3] = 0.0f;

    matrix = DirectX::XMMatrixIdentity() * DirectX::XMMatrixScalingFromVector(scale_vector) * DirectX::XMMatrixRotationQuaternion(rotation_vector) * DirectX::XMMatrixTranslationFromVector(transform_vector);

    /*std::cout << "matrix.r[0].m128_f32[0]: " << matrix.r[0].m128_f32[0] << std::endl;
    std::cout << "matrix.r[0].m128_f32[1]: " << matrix.r[0].m128_f32[1] << std::endl;
    std::cout << "matrix.r[0].m128_f32[2]: " << matrix.r[0].m128_f32[2] << std::endl;
    std::cout << "matrix.r[1].m128_f32[0]: " << matrix.r[1].m128_f32[0] << std::endl;
    std::cout << "matrix.r[1].m128_f32[1]: " << matrix.r[1].m128_f32[1] << std::endl;
    std::cout << "matrix.r[1].m128_f32[2]: " << matrix.r[1].m128_f32[2] << std::endl;
    std::cout << "matrix.r[2].m128_f32[0]: " << matrix.r[2].m128_f32[0] << std::endl;
    std::cout << "matrix.r[2].m128_f32[1]: " << matrix.r[2].m128_f32[1] << std::endl;
    std::cout << "matrix.r[2].m128_f32[2]: " << matrix.r[2].m128_f32[2] << std::endl;
    std::cout << "matrix.r[3].m128_f32[0]: " << matrix.r[3].m128_f32[0] << std::endl;
    std::cout << "matrix.r[3].m128_f32[1]: " << matrix.r[3].m128_f32[1] << std::endl;
    std::cout << "matrix.r[3].m128_f32[2]: " << matrix.r[3].m128_f32[2] << std::endl;*/

    vector3 new_scale;

    new_scale.x = std::sqrt(transform.x_axis.x * transform.x_axis.x + transform.x_axis.y * transform.x_axis.y + transform.x_axis.z * transform.x_axis.z);
    new_scale.y = std::sqrt(transform.y_axis.x * transform.y_axis.x + transform.y_axis.y * transform.y_axis.y + transform.y_axis.z * transform.y_axis.z);
    new_scale.z = std::sqrt(transform.z_axis.x * transform.z_axis.x + transform.z_axis.y * transform.z_axis.y + transform.z_axis.z * transform.z_axis.z);

    scale.x = new_scale.x;
    scale.y = new_scale.y;
    scale.z = new_scale.z;

    if (!util::floats_equal(1.0f, scale.x) || !util::floats_equal(1.0f, scale.y) || !util::floats_equal(1.0f, scale.z))
    {
        if (!util::floats_equal(0.0f, scale.x) && !util::floats_equal(0.0f, scale.y) && !util::floats_equal(0.0f, scale.z))
        {
            scaled = true;
        }
    }

    /*std::cout << "new_scale.x: " << new_scale.x << std::endl;
    std::cout << "new_scale.y: " << new_scale.y << std::endl;
    std::cout << "new_scale.z: " << new_scale.z << std::endl;

    std::cout << "x_axis.x: " << transform.x_axis.x / new_scale.x << std::endl;
    std::cout << "y_axis.x: " << transform.y_axis.x / new_scale.y << std::endl;
    std::cout << "z_axis.x: " << transform.z_axis.x / new_scale.z << std::endl;
    std::cout << "x_axis.y: " << transform.x_axis.y / new_scale.x << std::endl;
    std::cout << "y_axis.y: " << transform.y_axis.y / new_scale.y << std::endl;
    std::cout << "z_axis.y: " << transform.z_axis.y / new_scale.z << std::endl;
    std::cout << "x_axis.z: " << transform.x_axis.z / new_scale.x << std::endl;
    std::cout << "y_axis.z: " << transform.y_axis.z / new_scale.y << std::endl;
    std::cout << "z_axis.z: " << transform.z_axis.z / new_scale.z << std::endl;
    std::cout << "transform.x: " << transform.transform.x << std::endl;
    std::cout << "transform.y: " << transform.transform.y << std::endl;
    std::cout << "transform.z: " << transform.transform.z << std::endl;*/

    if (scaled)
    {
        transform.x_axis.x /= new_scale.x;
        transform.y_axis.x /= new_scale.y;
        transform.z_axis.x /= new_scale.z;
        transform.x_axis.y /= new_scale.x;
        transform.y_axis.y /= new_scale.y;
        transform.z_axis.y /= new_scale.z;
        transform.x_axis.z /= new_scale.x;
        transform.y_axis.z /= new_scale.y;
        transform.z_axis.z /= new_scale.z;
    }

    /*transform.x_axis.x = matrix.r[0].m128_f32[0];
    transform.x_axis.y = matrix.r[0].m128_f32[1];
    transform.x_axis.z = matrix.r[0].m128_f32[2];
    transform.y_axis.x = matrix.r[1].m128_f32[0];
    transform.y_axis.y = matrix.r[1].m128_f32[1];
    transform.y_axis.z = matrix.r[1].m128_f32[2];
    transform.z_axis.x = matrix.r[2].m128_f32[0];
    transform.z_axis.y = matrix.r[2].m128_f32[1];
    transform.z_axis.z = matrix.r[2].m128_f32[2];
    transform.transform.x = matrix.r[3].m128_f32[0];
    transform.transform.y = matrix.r[3].m128_f32[1];
    transform.transform.z = matrix.r[3].m128_f32[2];*/

    return scaled;
}

void map::generate_map_node_strings(uint32_t root_map_node, uint32_t parent_map_node, std::string map_node_string, uint32_t level)
{
    if (gui_control == ABORT_CURRENT_TASK)
    {
        return;
    }

    if (((map_node_count_current * (uint64_t)10000) / (uint64_t)map_node_count) % (uint64_t)10 == 0 && map_node_count_current > 0)
    {
        if (map_node_count_current <= map_node_count)
        {
            stringstream_length = console::update_console(message, map_node_count, map_node_count_current, start_time, stringstream_length);
        }
    }

    map_node_count_current++;

    map_percent_progress_godot_files = percent_progress;

    if (level == 0)
    {
        map_nodes.at(root_map_node).type = "empty";
    }
    else
    {
        bool in_parent_node = false;

        for (uint64_t p = 0; p < map_nodes.at(root_map_node).parent_map_nodes.size(); p++)
        {
            if (parent_map_node == map_nodes.at(root_map_node).parent_map_nodes.at(p))
            {
                in_parent_node = true;
            }
        }

        if (!in_parent_node)
        {
            map_nodes.at(root_map_node).parent_map_nodes.push_back(parent_map_node);

            map_nodes.at(root_map_node).has_parent_map_node = true;
        }
    }

    bool has_aset_with_temp_resource = false;

    uint64_t temp_entityTypeResourceIndex_hash = 0;

    std::vector<uint64_t> aset_temp_hashes;

    if (map_nodes.at(root_map_node).entityTypeResourceIndex_hash_type == "ASET")
    {
        uint32_t rpkg_index = rpkg_function::get_latest_hash(map_nodes.at(root_map_node).entityTypeResourceIndex_hash);

        if (rpkg_index != UINT32_MAX)
        {
            std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(map_nodes.at(root_map_node).entityTypeResourceIndex_hash);

            if (it != rpkgs.at(rpkg_index).hash_map.end())
            {
                uint32_t temp_hash_reference_count = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint64_t a = 0; a < temp_hash_reference_count; a++)
                {
                    uint64_t hash_reference_value = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(a);

                    uint32_t rpkg_index2 = rpkg_function::get_latest_hash(hash_reference_value);

                    if (rpkg_index2 != UINT32_MAX)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index2).hash_map.find(hash_reference_value);

                        if (it2 != rpkgs.at(rpkg_index2).hash_map.end())
                        {
                            if (rpkgs.at(rpkg_index2).hash.at(it2->second).hash_resource_type == "TEMP")
                            {
                                //std::cout << util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type << " has TEMP reference file: " << rpkgs.at(rpkg_index2).hash.at(it2->second).hash_file_name << std::endl;

                                temp_entityTypeResourceIndex_hash = rpkgs.at(rpkg_index2).hash.at(it2->second).hash_value;

                                aset_temp_hashes.push_back(rpkgs.at(rpkg_index2).hash.at(it2->second).hash_value);

                                has_aset_with_temp_resource = true;
                            }
                        }
                    }
                }
            }
        }
    }

    {
        std::unordered_map<uint64_t, std::unordered_map<uint32_t, std::vector<uint32_t>>>::iterator it = m_eidParent_local_temp_hash_entityIndex_map.find(map_nodes.at(root_map_node).temp_hash);

        if (it != m_eidParent_local_temp_hash_entityIndex_map.end())
        {
            std::unordered_map<uint32_t, std::vector<uint32_t>>::iterator it2 = it->second.find(map_nodes.at(root_map_node).subEntity);

            if (it2 != it->second.end())
            {
                for (uint32_t i = 0; i < it2->second.size(); i++)
                {
                    uint32_t n = it2->second.at(i);

                    //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_local to " << map_nodes.at(n).entryName << std::endl;

                    //std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";
                    std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(n) + ")";

                    //std::cout << node_string << std::endl;

                    std::string transform_string = "";

                    if (map_nodes.at(n).has_m_mTransform)
                    {
                        transform_string = "";

                        matrix43 transform = map_nodes.at(n).m_mTransform;

                        if (map_nodes.at(n).has_m_PrimitiveScale && map_nodes.at(n).has_m_vGlobalSize)
                        {
                            scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                        }
                        else if (map_nodes.at(n).has_m_PrimitiveScale)
                        {
                            scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                        }
                        else if (map_nodes.at(n).has_m_vGlobalSize)
                        {
                            scale_transform(transform, map_nodes.at(n).m_vGlobalSize.x, map_nodes.at(n).m_vGlobalSize.y, map_nodes.at(n).m_vGlobalSize.z);
                        }

                        transform_string += util::float_to_string(transform.x_axis.x) + ",";
                        transform_string += util::float_to_string(transform.y_axis.x) + ",";
                        transform_string += util::float_to_string(transform.z_axis.x) + ",";
                        transform_string += util::float_to_string(transform.x_axis.y) + ",";
                        transform_string += util::float_to_string(transform.y_axis.y) + ",";
                        transform_string += util::float_to_string(transform.z_axis.y) + ",";
                        transform_string += util::float_to_string(transform.x_axis.z) + ",";
                        transform_string += util::float_to_string(transform.y_axis.z) + ",";
                        transform_string += util::float_to_string(transform.z_axis.z) + ",";
                        transform_string += util::float_to_string(transform.transform.x) + ",";
                        transform_string += util::float_to_string(transform.transform.y) + ",";
                        transform_string += util::float_to_string(transform.transform.z);
                    }
                    else
                    {
                        transform_string = "1,0,0,0,1,0,0,0,1,0,0,0";
                    }

                    //std::cout << transform_string << std::endl;

                    if (map_nodes.at(n).has_prim_resource)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                        if (it3 != map_prims_map.end())
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                            map_nodes.at(n).type = "glb";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else if (map_nodes.at(n).is_volume_box)
                    {
                        if (map_filter_volume_boxes)
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                            map_nodes.at(n).type = "box";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else if (map_nodes.at(n).is_volume_sphere)
                    {
                        if (map_filter_volume_spheres)
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                            map_nodes.at(n).type = "sphere";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else
                    {
                        if (level == 0)
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";
                        }
                        else
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\"" + map_node_string + "\"]\n";
                        }

                        godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        if (map_filter_visible)
                        {
                            if (map_nodes.at(root_map_node).has_m_bVisible)
                            {
                                if (!map_nodes.at(root_map_node).m_bVisible)
                                {
                                    godot_tscn_file += "visible = false\n";
                                }
                            }
                        }

                        godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                        map_nodes.at(n).type = "empty";

                        map_nodes.at(n).m_mTransform_string = transform_string;

                        if (level == 0)
                        {
                            generate_map_node_strings(n, root_map_node, node_string, level + 1);
                        }
                        else
                        {
                            generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                        }
                    }
                }
            }
        }
    }

    {
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::vector<uint32_t>>>::iterator it = m_eidParent_external_externalSceneIndex_hash_entityID_map.find(map_nodes.at(root_map_node).temp_hash);

        if (it != m_eidParent_external_externalSceneIndex_hash_entityID_map.end())
        {
            std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it2 = it->second.find(map_nodes.at(root_map_node).entityId);

            if (it2 != it->second.end())
            {
                for (uint32_t i = 0; i < it2->second.size(); i++)
                {
                    uint32_t n = it2->second.at(i);

                    //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_external to " << map_nodes.at(n).entryName << std::endl;

                    //std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";
                    std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(n) + ")";
                    //std::cout << node_string << std::endl;

                    std::string transform_string = "";

                    if (map_nodes.at(n).has_m_mTransform)
                    {
                        transform_string = "";

                        matrix43 transform = map_nodes.at(n).m_mTransform;

                        if (map_nodes.at(n).has_m_PrimitiveScale && map_nodes.at(n).has_m_vGlobalSize)
                        {
                            scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                        }
                        else if (map_nodes.at(n).has_m_PrimitiveScale)
                        {
                            scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                        }
                        else if (map_nodes.at(n).has_m_vGlobalSize)
                        {
                            scale_transform(transform, map_nodes.at(n).m_vGlobalSize.x, map_nodes.at(n).m_vGlobalSize.y, map_nodes.at(n).m_vGlobalSize.z);
                        }

                        transform_string += util::float_to_string(transform.x_axis.x) + ",";
                        transform_string += util::float_to_string(transform.y_axis.x) + ",";
                        transform_string += util::float_to_string(transform.z_axis.x) + ",";
                        transform_string += util::float_to_string(transform.x_axis.y) + ",";
                        transform_string += util::float_to_string(transform.y_axis.y) + ",";
                        transform_string += util::float_to_string(transform.z_axis.y) + ",";
                        transform_string += util::float_to_string(transform.x_axis.z) + ",";
                        transform_string += util::float_to_string(transform.y_axis.z) + ",";
                        transform_string += util::float_to_string(transform.z_axis.z) + ",";
                        transform_string += util::float_to_string(transform.transform.x) + ",";
                        transform_string += util::float_to_string(transform.transform.y) + ",";
                        transform_string += util::float_to_string(transform.transform.z);
                    }
                    else
                    {
                        transform_string = "1,0,0,0,1,0,0,0,1,0,0,0";
                    }

                    //std::cout << transform_string << std::endl;

                    if (map_nodes.at(n).has_prim_resource)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                        if (it3 != map_prims_map.end())
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                            map_nodes.at(n).type = "glb";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else if (map_nodes.at(n).is_volume_box)
                    {
                        if (map_filter_volume_boxes)
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                            map_nodes.at(n).type = "box";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else if (map_nodes.at(n).is_volume_sphere)
                    {
                        if (map_filter_volume_spheres)
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                            map_nodes.at(n).type = "sphere";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else
                    {
                        if (level == 0)
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";
                        }
                        else
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\"" + map_node_string + "\"]\n";
                        }

                        godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        if (map_filter_visible)
                        {
                            if (map_nodes.at(root_map_node).has_m_bVisible)
                            {
                                if (!map_nodes.at(root_map_node).m_bVisible)
                                {
                                    godot_tscn_file += "visible = false\n";
                                }
                            }
                        }

                        godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",1\"\n";

                        map_nodes.at(n).type = "empty";

                        map_nodes.at(n).m_mTransform_string = transform_string;

                        if (level == 0)
                        {
                            generate_map_node_strings(n, root_map_node, node_string, level + 1);
                        }
                        else
                        {
                            generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                        }
                    }
                }
            }
        }
    }

    if (map_nodes.at(root_map_node).entityTypeResourceIndex_hash_type == "TEMP")
    {
        std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_is_top_level_Local_node_map.find(map_nodes.at(root_map_node).entityTypeResourceIndex_hash);

        if (it != temp_hash_is_top_level_Local_node_map.end())
        {
            for (uint32_t i = 0; i < it->second.size(); i++)
            {
                uint32_t n = it->second.at(i);

                //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_local to " << map_nodes.at(n).entryName << std::endl;

                    //std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";
                std::string node_string = "*DONTUSE* " + map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(n) + ")";
                //std::cout << node_string << std::endl;

                std::string transform_string = "";

                /*if (map_nodes.at(n).has_m_mTransform)
                {
                    transform_string = "";

                    matrix43 transform = map_nodes.at(n).m_mTransform;

                    if (map_nodes.at(n).has_m_PrimitiveScale && map_nodes.at(n).has_m_vGlobalSize)
                    {
                        scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                    }
                    else if (map_nodes.at(n).has_m_PrimitiveScale)
                    {
                        scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                    }
                    else if (map_nodes.at(n).has_m_vGlobalSize)
                    {
                        scale_transform(transform, map_nodes.at(n).m_vGlobalSize.x, map_nodes.at(n).m_vGlobalSize.y, map_nodes.at(n).m_vGlobalSize.z);
                    }

                    transform_string += util::float_to_string(transform.x_axis.x) + ",";
                    transform_string += util::float_to_string(transform.x_axis.y) + ",";
                    transform_string += util::float_to_string(transform.x_axis.z) + ",";
                    transform_string += util::float_to_string(transform.y_axis.x) + ",";
                    transform_string += util::float_to_string(transform.y_axis.y) + ",";
                    transform_string += util::float_to_string(transform.y_axis.z) + ",";
                    transform_string += util::float_to_string(transform.z_axis.x) + ",";
                    transform_string += util::float_to_string(transform.z_axis.y) + ",";
                    transform_string += util::float_to_string(transform.z_axis.z) + ",";
                    transform_string += util::float_to_string(transform.transform.x) + ",";
                    transform_string += util::float_to_string(transform.transform.y) + ",";
                    transform_string += util::float_to_string(transform.transform.z);
                }
                else
                {*/
                transform_string = "1,0,0,0,1,0,0,0,1,0,0,0";
                //}

                //std::cout << transform_string << std::endl;

                if (map_nodes.at(n).has_prim_resource)
                {
                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                    if (it3 != map_prims_map.end())
                    {
                        if (level == 0)
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                        }
                        else
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                        }

                        godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        if (map_filter_visible)
                        {
                            if (map_nodes.at(root_map_node).has_m_bVisible)
                            {
                                if (!map_nodes.at(root_map_node).m_bVisible)
                                {
                                    godot_tscn_file += "visible = false\n";
                                }
                            }
                        }

                        godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",0\"\n";

                        map_nodes.at(n).type = "glb";

                        map_nodes.at(n).m_mTransform_string = transform_string;

                        if (level == 0)
                        {
                            generate_map_node_strings(n, root_map_node, node_string, level + 1);
                        }
                        else
                        {
                            generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                        }
                    }
                }
                else if (map_nodes.at(n).is_volume_box)
                {
                    if (map_filter_volume_boxes)
                    {
                        if (level == 0)
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                            godot_tscn_file += "transparency = 0.98\n";
                            godot_tscn_file += "cast_shadow = 0\n";
                            godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }
                        else
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                            godot_tscn_file += "transparency = 0.98\n";
                            godot_tscn_file += "cast_shadow = 0\n";
                            godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }

                        godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        if (map_filter_visible)
                        {
                            if (map_nodes.at(root_map_node).has_m_bVisible)
                            {
                                if (!map_nodes.at(root_map_node).m_bVisible)
                                {
                                    godot_tscn_file += "visible = false\n";
                                }
                            }
                        }

                        godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",0\"\n";

                        map_nodes.at(n).type = "box";

                        map_nodes.at(n).m_mTransform_string = transform_string;

                        if (level == 0)
                        {
                            generate_map_node_strings(n, root_map_node, node_string, level + 1);
                        }
                        else
                        {
                            generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                        }
                    }
                }
                else if (map_nodes.at(n).is_volume_sphere)
                {
                    if (map_filter_volume_spheres)
                    {
                        if (level == 0)
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                            godot_tscn_file += "transparency = 0.98\n";
                            godot_tscn_file += "cast_shadow = 0\n";
                            godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }
                        else
                        {
                            godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                            godot_tscn_file += "transparency = 0.98\n";
                            godot_tscn_file += "cast_shadow = 0\n";
                            godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }

                        godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        if (map_filter_visible)
                        {
                            if (map_nodes.at(root_map_node).has_m_bVisible)
                            {
                                if (!map_nodes.at(root_map_node).m_bVisible)
                                {
                                    godot_tscn_file += "visible = false\n";
                                }
                            }
                        }

                        godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",0\"\n";

                        map_nodes.at(n).type = "sphere";

                        map_nodes.at(n).m_mTransform_string = transform_string;

                        if (level == 0)
                        {
                            generate_map_node_strings(n, root_map_node, node_string, level + 1);
                        }
                        else
                        {
                            generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                        }
                    }
                }
                else
                {
                    if (level == 0)
                    {
                        generate_map_node_strings(n, root_map_node, ".", level + 1);
                    }
                    else
                    {
                        generate_map_node_strings(n, root_map_node, map_node_string, level + 1);
                    }
                }
            }
        }
    }

    if (has_aset_with_temp_resource)
    {
        for (uint32_t a = 0; a < aset_temp_hashes.size(); a++)
        {
            std::unordered_map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_is_top_level_Local_node_map.find(aset_temp_hashes.at(a));

            if (it != temp_hash_is_top_level_Local_node_map.end())
            {
                for (uint32_t i = 0; i < it->second.size(); i++)
                {
                    uint32_t n = it->second.at(i);

                    //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_local to " << map_nodes.at(n).entryName << std::endl;

                        //std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";
                    std::string node_string = "*DONTUSE* " + map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(n) + ")";
                    //std::cout << node_string << std::endl;

                    std::string transform_string = "";

                    /*if (map_nodes.at(n).has_m_mTransform)
                    {
                        transform_string = "";

                        matrix43 transform = map_nodes.at(n).m_mTransform;

                        if (map_nodes.at(n).has_m_PrimitiveScale && map_nodes.at(n).has_m_vGlobalSize)
                        {
                            scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                        }
                        else if (map_nodes.at(n).has_m_PrimitiveScale)
                        {
                            scale_transform(transform, map_nodes.at(n).m_PrimitiveScale.x, map_nodes.at(n).m_PrimitiveScale.y, map_nodes.at(n).m_PrimitiveScale.z);
                        }
                        else if (map_nodes.at(n).has_m_vGlobalSize)
                        {
                            scale_transform(transform, map_nodes.at(n).m_vGlobalSize.x, map_nodes.at(n).m_vGlobalSize.y, map_nodes.at(n).m_vGlobalSize.z);
                        }

                        transform_string += util::float_to_string(transform.x_axis.x) + ",";
                        transform_string += util::float_to_string(transform.x_axis.y) + ",";
                        transform_string += util::float_to_string(transform.x_axis.z) + ",";
                        transform_string += util::float_to_string(transform.y_axis.x) + ",";
                        transform_string += util::float_to_string(transform.y_axis.y) + ",";
                        transform_string += util::float_to_string(transform.y_axis.z) + ",";
                        transform_string += util::float_to_string(transform.z_axis.x) + ",";
                        transform_string += util::float_to_string(transform.z_axis.y) + ",";
                        transform_string += util::float_to_string(transform.z_axis.z) + ",";
                        transform_string += util::float_to_string(transform.transform.x) + ",";
                        transform_string += util::float_to_string(transform.transform.y) + ",";
                        transform_string += util::float_to_string(transform.transform.z);
                    }
                    else
                    {*/
                    transform_string = "1,0,0,0,1,0,0,0,1,0,0,0";
                    //}

                    //std::cout << transform_string << std::endl;

                    if (map_nodes.at(n).has_prim_resource)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                        if (it3 != map_prims_map.end())
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",0\"\n";

                            map_nodes.at(n).type = "glb";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else if (map_nodes.at(n).is_volume_box)
                    {
                        if (map_filter_volume_boxes)
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",0\"\n";

                            map_nodes.at(n).type = "box";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else if (map_nodes.at(n).is_volume_sphere)
                    {
                        if (map_filter_volume_spheres)
                        {
                            if (level == 0)
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }
                            else
                            {
                                godot_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                                godot_tscn_file += "transparency = 0.98\n";
                                godot_tscn_file += "cast_shadow = 0\n";
                                godot_tscn_file += "size = Vector3(1, 1, 1)\n";
                            }

                            godot_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            if (map_filter_visible)
                            {
                                if (map_nodes.at(root_map_node).has_m_bVisible)
                                {
                                    if (!map_nodes.at(root_map_node).m_bVisible)
                                    {
                                        godot_tscn_file += "visible = false\n";
                                    }
                                }
                            }

                            godot_tscn_file += "editor_description = \"" + map_nodes.at(n).temp_hash_string + "," + util::uint64_t_to_hex_string(map_nodes.at(n).entityId) + ",0\"\n";

                            map_nodes.at(n).type = "sphere";

                            map_nodes.at(n).m_mTransform_string = transform_string;

                            if (level == 0)
                            {
                                generate_map_node_strings(n, root_map_node, node_string, level + 1);
                            }
                            else
                            {
                                generate_map_node_strings(n, root_map_node, map_node_string + "/" + node_string, level + 1);
                            }
                        }
                    }
                    else
                    {
                        if (level == 0)
                        {
                            generate_map_node_strings(n, root_map_node, ".", level + 1);
                        }
                        else
                        {
                            generate_map_node_strings(n, root_map_node, map_node_string, level + 1);
                        }
                    }
                }
            }
        }
    }
}

void map::get_map_node(temp& temp_temp)
{
    const rapidjson::Value& temp_json_subEntities = temp_temp.temp_json_document["subEntities"];

    for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++)
    {
        if (gui_control == ABORT_CURRENT_TASK)
        {
            return;
        }

        if (((map_node_count_current * (uint64_t)10000) / (uint64_t)map_node_count) % (uint64_t)10 == 0 && map_node_count_current > 0)
        {
            stringstream_length = console::update_console(message, map_node_count, map_node_count_current, start_time, stringstream_length);
        }

        map_node_count_current++;

        map_percent_progress_map_nodes = percent_progress;

        map_node temp_map_node;

        temp_map_node.temp_hash = std::strtoull(temp_temp.temp_file_name.c_str(), nullptr, 16);

        temp_map_node.temp_hash_string = util::uint64_t_to_hex_string(temp_map_node.temp_hash);

        temp_map_node.entityId = temp_temp.tblu_entityId.at(s);

        temp_map_node.subEntity = s;

        temp_map_node.entryName = temp_temp.tblu_entityName.at(s);

        rapidjson::Value::ConstMemberIterator it2 = temp_json_subEntities[s].FindMember("entityTypeResourceIndex");

        if (it2 != temp_json_subEntities[s].MemberEnd())
        {
            temp_map_node.entityTypeResourceIndex = it2->value.GetInt();

            uint32_t temp_hash_reference_count = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            if (temp_map_node.entityTypeResourceIndex < temp_hash_reference_count)
            {
                temp_map_node.entityTypeResourceIndex_hash = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(temp_map_node.entityTypeResourceIndex);

                uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_map_node.entityTypeResourceIndex_hash);

                if (rpkg_index != UINT32_MAX)
                {
                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(rpkg_index).hash_map.find(temp_map_node.entityTypeResourceIndex_hash);

                    if (it3 != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        temp_map_node.entityTypeResourceIndex_hash_type = rpkgs.at(rpkg_index).hash.at(it3->second).hash_resource_type;
                    }

                    if (temp_map_node.entityTypeResourceIndex_hash == 0x0054667393764C74)
                    {
                        temp_map_node.is_volume_box = true;
                    }
                    else if (temp_map_node.entityTypeResourceIndex_hash == 0x00B86A9EE991EFB2)
                    {
                        temp_map_node.is_volume_sphere = true;
                    }
                }
            }
        }

        it2 = temp_json_subEntities[s].FindMember("propertyValues");

        if (it2 != temp_json_subEntities[s].MemberEnd())
        {
            for (uint64_t p = 0; p < it2->value.Size(); p++)
            {
                rapidjson::Value::ConstMemberIterator it3 = it2->value[p].FindMember("nPropertyID");

                if (it3 != it2->value[p].MemberEnd())
                {
                    if (it3->value.IsString())
                    {
                        if (std::strcmp(it3->value.GetString(), "m_ResourceID") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_ResourceID in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$type");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.IsString())
                                    {
                                        if (std::strcmp(it5->value.GetString(), "ZRuntimeResourceID") == 0)
                                        {
                                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_ResourceID ZRuntimeResourceID" << std::endl;

                                            rapidjson::Value::ConstMemberIterator it6 = it4->value.FindMember("$val");

                                            if (it6 != it4->value.MemberEnd())
                                            {
                                                uint32_t resource_id_low = 999999;
                                                uint32_t resource_id_high = 999999;

                                                rapidjson::Value::ConstMemberIterator it7 = it6->value.FindMember("m_IDHigh");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " ZRuntimeResourceID m_IDHigh is " << it7->value.GetUint() << std::endl;

                                                    resource_id_high = it7->value.GetUint();
                                                }

                                                it7 = it6->value.FindMember("m_IDLow");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " ZRuntimeResourceID m_IDLow is " << it7->value.GetUint() << std::endl;

                                                    resource_id_low = it7->value.GetUint();
                                                }

                                                temp_map_node.m_IDHigh = resource_id_high;
                                                temp_map_node.m_IDLow = resource_id_low;

                                                uint32_t temp_hash_reference_count = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                                                if (temp_map_node.m_IDHigh == 0 && temp_map_node.m_IDLow < temp_hash_reference_count)
                                                {
                                                    //std::cout << "PRIM file: " << util::uint64_t_to_hex_string(rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(temp_map_node.m_IDLow)) << std::endl;

                                                    temp_map_node.prim_hash = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(temp_map_node.m_IDLow);

                                                    temp_map_node.has_prim_resource = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_mTransform") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_mTransform in propertyValues" << std::endl;

                            temp_map_node.has_m_mTransform = true;

                            const rapidjson::Value& value = it2->value[p]["value"];

                            if (!value.IsNull())
                            {
                                const rapidjson::Value& val = value["$val"];

                                if (!val.IsNull())
                                {
                                    ////std::cout << val["XAxis"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["XAxis"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["XAxis"]["z"].GetFloat() << std::endl;
                                    ////std::cout << val["YAxis"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["YAxis"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["YAxis"]["z"].GetFloat() << std::endl;
                                    ////std::cout << val["ZAxis"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["ZAxis"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["ZAxis"]["z"].GetFloat() << std::endl;
                                    ////std::cout << val["Trans"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["Trans"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["Trans"]["z"].GetFloat() << std::endl;

                                    temp_map_node.m_mTransform.x_axis.x = val["XAxis"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.y_axis.x = val["YAxis"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.z_axis.x = val["ZAxis"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.x_axis.y = val["XAxis"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.y_axis.y = val["YAxis"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.z_axis.y = val["ZAxis"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.x_axis.z = val["XAxis"]["z"].GetFloat();
                                    temp_map_node.m_mTransform.y_axis.z = val["YAxis"]["z"].GetFloat();
                                    temp_map_node.m_mTransform.z_axis.z = val["ZAxis"]["z"].GetFloat();
                                    temp_map_node.m_mTransform.transform.x = val["Trans"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.transform.y = val["Trans"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.transform.z = val["Trans"]["z"].GetFloat();
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_PrimitiveScale") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_PrimitiveScale in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$type");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.IsString())
                                    {
                                        if (std::strcmp(it5->value.GetString(), "SVector3") == 0)
                                        {
                                            rapidjson::Value::ConstMemberIterator it6 = it4->value.FindMember("$val");

                                            if (it6 != it4->value.MemberEnd())
                                            {
                                                rapidjson::Value::ConstMemberIterator it7 = it6->value.FindMember("x");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_PrimitiveScale.x = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("y");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_PrimitiveScale.y = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("z");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_PrimitiveScale.z = it7->value.GetFloat();
                                                }

                                                if (!util::floats_equal(temp_map_node.m_PrimitiveScale.x, 0.0f) && !util::floats_equal(temp_map_node.m_PrimitiveScale.y, 0.0f) && !util::floats_equal(temp_map_node.m_PrimitiveScale.z, 0.0f))
                                                {
                                                    temp_map_node.has_m_PrimitiveScale = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_vGlobalSize") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_vGlobalSize in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$type");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.IsString())
                                    {
                                        if (std::strcmp(it5->value.GetString(), "SVector3") == 0)
                                        {
                                            rapidjson::Value::ConstMemberIterator it6 = it4->value.FindMember("$val");

                                            if (it6 != it4->value.MemberEnd())
                                            {
                                                rapidjson::Value::ConstMemberIterator it7 = it6->value.FindMember("x");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_vGlobalSize.x = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("y");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_vGlobalSize.y = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("z");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_vGlobalSize.z = it7->value.GetFloat();
                                                }

                                                if (!util::floats_equal(temp_map_node.m_vGlobalSize.x, 0.0f) && !util::floats_equal(temp_map_node.m_vGlobalSize.y, 0.0f) && !util::floats_equal(temp_map_node.m_vGlobalSize.z, 0.0f))
                                                {
                                                    temp_map_node.has_m_vGlobalSize = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_eidParent") == 0)
                        {
                            temp_map_node.has_m_eidParent = true;

                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_eidParent in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$val");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    rapidjson::Value::ConstMemberIterator it6 = it5->value.FindMember("entityID");

                                    if (it6 != it5->value.MemberEnd())
                                    {
                                        //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_eidParent entityID is " << it6->value.GetUint64() << std::endl;

                                        temp_map_node.entityID = it6->value.GetUint64();
                                    }

                                    it6 = it5->value.FindMember("externalSceneIndex");

                                    if (it6 != it5->value.MemberEnd())
                                    {
                                        //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_eidParent externalSceneIndex is " << it6->value.GetInt() << std::endl;

                                        temp_map_node.externalSceneIndex = it6->value.GetInt();
                                    }

                                    it6 = it5->value.FindMember("entityIndex");

                                    if (it6 != it5->value.MemberEnd())
                                    {
                                        //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_eidParent entityIndex is " << it6->value.GetInt() << std::endl;

                                        temp_map_node.entityIndex = it6->value.GetInt();
                                    }

                                    if (temp_map_node.entityID == 18446744073709551615 && temp_map_node.externalSceneIndex == -1)
                                    {
                                        //std::cout << "m_eidParent reference is local!" << std::endl;

                                        temp_map_node.has_m_eidParent_local = true;
                                    }
                                    else if (temp_map_node.entityIndex == -2 && temp_map_node.externalSceneIndex >= 0)
                                    {
                                        //std::cout << "m_eidParent reference is external!" << std::endl;

                                        temp_map_node.has_m_eidParent_external = true;

                                        uint32_t temp_hash_reference_count = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                                        if (temp_map_node.externalSceneIndex < temp_temp.temp_externalSceneTypeIndicesInResourceHeader.size())
                                        {
                                            int32_t externalScene = temp_temp.temp_externalSceneTypeIndicesInResourceHeader.at(temp_map_node.externalSceneIndex);

                                            if (externalScene < temp_hash_reference_count)
                                            {
                                                temp_map_node.externalSceneIndex_hash = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(externalScene);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        //std::cout << "m_eidParent reference is unknown!" << std::endl;
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_bVisible") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_mTransform in propertyValues" << std::endl;

                            temp_map_node.has_m_bVisible = true;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$val");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.GetBool())
                                    {
                                        temp_map_node.m_bVisible = true;
                                    }
                                    else
                                    {
                                        temp_map_node.m_bVisible = false;
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
                        if (std::strcmp(it3->value.GetString(), "m_ResourceID") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_ResourceID in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$type");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.IsString())
                                    {
                                        if (std::strcmp(it5->value.GetString(), "ZRuntimeResourceID") == 0)
                                        {
                                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_ResourceID ZRuntimeResourceID" << std::endl;

                                            rapidjson::Value::ConstMemberIterator it6 = it4->value.FindMember("$val");

                                            if (it6 != it4->value.MemberEnd())
                                            {
                                                uint32_t resource_id_low = 999999;
                                                uint32_t resource_id_high = 999999;

                                                rapidjson::Value::ConstMemberIterator it7 = it6->value.FindMember("m_IDHigh");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " ZRuntimeResourceID m_IDHigh is " << it7->value.GetUint() << std::endl;

                                                    resource_id_high = it7->value.GetUint();
                                                }

                                                it7 = it6->value.FindMember("m_IDLow");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " ZRuntimeResourceID m_IDLow is " << it7->value.GetUint() << std::endl;

                                                    resource_id_low = it7->value.GetUint();
                                                }

                                                temp_map_node.m_IDHigh = resource_id_high;
                                                temp_map_node.m_IDLow = resource_id_low;

                                                uint32_t temp_hash_reference_count = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                                                if (temp_map_node.m_IDHigh == 0 && temp_map_node.m_IDLow < temp_hash_reference_count)
                                                {
                                                    //std::cout << "PRIM file: " << util::uint64_t_to_hex_string(rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(temp_map_node.m_IDLow)) << std::endl;

                                                    temp_map_node.prim_hash = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(temp_map_node.m_IDLow);

                                                    temp_map_node.has_prim_resource = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_mTransform") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_mTransform in propertyValues" << std::endl;

                            temp_map_node.has_m_mTransform = true;

                            const rapidjson::Value& value = it2->value[p]["value"];

                            if (!value.IsNull())
                            {
                                const rapidjson::Value& val = value["$val"];

                                if (!val.IsNull())
                                {
                                    ////std::cout << val["XAxis"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["XAxis"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["XAxis"]["z"].GetFloat() << std::endl;
                                    ////std::cout << val["YAxis"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["YAxis"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["YAxis"]["z"].GetFloat() << std::endl;
                                    ////std::cout << val["ZAxis"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["ZAxis"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["ZAxis"]["z"].GetFloat() << std::endl;
                                    ////std::cout << val["Trans"]["x"].GetFloat() << std::endl;
                                    ////std::cout << val["Trans"]["y"].GetFloat() << std::endl;
                                    ////std::cout << val["Trans"]["z"].GetFloat() << std::endl;

                                    temp_map_node.m_mTransform.x_axis.x = val["XAxis"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.y_axis.x = val["YAxis"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.z_axis.x = val["ZAxis"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.x_axis.y = val["XAxis"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.y_axis.y = val["YAxis"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.z_axis.y = val["ZAxis"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.x_axis.z = val["XAxis"]["z"].GetFloat();
                                    temp_map_node.m_mTransform.y_axis.z = val["YAxis"]["z"].GetFloat();
                                    temp_map_node.m_mTransform.z_axis.z = val["ZAxis"]["z"].GetFloat();
                                    temp_map_node.m_mTransform.transform.x = val["Trans"]["x"].GetFloat();
                                    temp_map_node.m_mTransform.transform.y = val["Trans"]["y"].GetFloat();
                                    temp_map_node.m_mTransform.transform.z = val["Trans"]["z"].GetFloat();
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_PrimitiveScale") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_PrimitiveScale in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$type");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.IsString())
                                    {
                                        if (std::strcmp(it5->value.GetString(), "SVector3") == 0)
                                        {
                                            rapidjson::Value::ConstMemberIterator it6 = it4->value.FindMember("$val");

                                            if (it6 != it4->value.MemberEnd())
                                            {
                                                rapidjson::Value::ConstMemberIterator it7 = it6->value.FindMember("x");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_PrimitiveScale.x = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("y");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_PrimitiveScale.y = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("z");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_PrimitiveScale.z = it7->value.GetFloat();
                                                }

                                                if (!util::floats_equal(temp_map_node.m_PrimitiveScale.x, 0.0f) && !util::floats_equal(temp_map_node.m_PrimitiveScale.y, 0.0f) && !util::floats_equal(temp_map_node.m_PrimitiveScale.z, 0.0f))
                                                {
                                                    temp_map_node.has_m_PrimitiveScale = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_vGlobalSize") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_vGlobalSize in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$type");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.IsString())
                                    {
                                        if (std::strcmp(it5->value.GetString(), "SVector3") == 0)
                                        {
                                            rapidjson::Value::ConstMemberIterator it6 = it4->value.FindMember("$val");

                                            if (it6 != it4->value.MemberEnd())
                                            {
                                                rapidjson::Value::ConstMemberIterator it7 = it6->value.FindMember("x");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_vGlobalSize.x = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("y");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_vGlobalSize.y = it7->value.GetFloat();
                                                }

                                                it7 = it6->value.FindMember("z");

                                                if (it7 != it6->value.MemberEnd())
                                                {
                                                    temp_map_node.m_vGlobalSize.z = it7->value.GetFloat();
                                                }

                                                if (!util::floats_equal(temp_map_node.m_vGlobalSize.x, 0.0f) && !util::floats_equal(temp_map_node.m_vGlobalSize.y, 0.0f) && !util::floats_equal(temp_map_node.m_vGlobalSize.z, 0.0f))
                                                {
                                                    temp_map_node.has_m_vGlobalSize = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_eidParent") == 0)
                        {
                            temp_map_node.has_m_eidParent = true;

                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_eidParent in propertyValues" << std::endl;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$val");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    rapidjson::Value::ConstMemberIterator it6 = it5->value.FindMember("entityID");

                                    if (it6 != it5->value.MemberEnd())
                                    {
                                        //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_eidParent entityID is " << it6->value.GetUint64() << std::endl;

                                        temp_map_node.entityID = it6->value.GetUint64();
                                    }

                                    it6 = it5->value.FindMember("externalSceneIndex");

                                    if (it6 != it5->value.MemberEnd())
                                    {
                                        //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_eidParent externalSceneIndex is " << it6->value.GetInt() << std::endl;

                                        temp_map_node.externalSceneIndex = it6->value.GetInt();
                                    }

                                    it6 = it5->value.FindMember("entityIndex");

                                    if (it6 != it5->value.MemberEnd())
                                    {
                                        //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " m_eidParent entityIndex is " << it6->value.GetInt() << std::endl;

                                        temp_map_node.entityIndex = it6->value.GetInt();
                                    }

                                    if (temp_map_node.entityID == 18446744073709551615 && temp_map_node.externalSceneIndex == -1)
                                    {
                                        //std::cout << "m_eidParent reference is local!" << std::endl;

                                        temp_map_node.has_m_eidParent_local = true;
                                    }
                                    else if (temp_map_node.entityIndex == -2 && temp_map_node.externalSceneIndex >= 0)
                                    {
                                        //std::cout << "m_eidParent reference is external!" << std::endl;

                                        temp_map_node.has_m_eidParent_external = true;

                                        uint32_t temp_hash_reference_count = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                                        if (temp_map_node.externalSceneIndex < temp_temp.temp_externalSceneTypeIndicesInResourceHeader.size())
                                        {
                                            int32_t externalScene = temp_temp.temp_externalSceneTypeIndicesInResourceHeader.at(temp_map_node.externalSceneIndex);

                                            if (externalScene < temp_hash_reference_count)
                                            {
                                                temp_map_node.externalSceneIndex_hash = rpkgs.at(temp_temp.temp_rpkg_index).hash.at(temp_temp.temp_hash_index).hash_reference_data.hash_reference.at(externalScene);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        //std::cout << "m_eidParent reference is unknown!" << std::endl;
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_bVisible") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_mTransform in propertyValues" << std::endl;

                            temp_map_node.has_m_bVisible = true;

                            rapidjson::Value::ConstMemberIterator it4 = it2->value[p].FindMember("value");

                            if (it4 != it2->value[p].MemberEnd())
                            {
                                rapidjson::Value::ConstMemberIterator it5 = it4->value.FindMember("$val");

                                if (it5 != it4->value.MemberEnd())
                                {
                                    if (it5->value.GetBool())
                                    {
                                        temp_map_node.m_bVisible = true;
                                    }
                                    else
                                    {
                                        temp_map_node.m_bVisible = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (temp_map_node.has_m_eidParent_local)
        {
            add_to_m_eidParent_local_temp_hash_entityIndex_map(map_nodes.size(), temp_map_node.temp_hash, temp_map_node.entityIndex);
        }

        if (temp_map_node.has_m_eidParent_external)
        {
            add_to_m_eidParent_external_externalSceneIndex_hash_entityID_map(map_nodes.size(), temp_map_node.externalSceneIndex_hash, temp_map_node.entityID);
        }

        map_nodes.push_back(temp_map_node);
    }
}

void map::get_root_scenes()
{
    for (uint64_t t = 0; t < map_temps.size(); t++)
    {
        //std::cout << "TEMP file: " << map_temps.at(t).temp_file_name << std::endl;

        for (uint64_t s = 0; s < map_temps.at(t).temp_externalSceneTypeIndicesInResourceHeader.size(); s++)
        {
            //std::cout << "External scene TEMP file index: " << map_temps.at(t).temp_externalSceneTypeIndicesInResourceHeader.at(s) << std::endl;
        }

        for (uint64_t s = 0; s < map_temps.at(t).temp_externalSceneHashes.size(); s++)
        {
            //std::cout << "External scene TEMP file: " << util::uint64_t_to_hex_string(map_temps.at(t).temp_externalSceneHashes.at(s)) << std::endl;

            std::unordered_map<uint64_t, uint64_t>::iterator it = root_scenes.find(map_temps.at(t).temp_externalSceneHashes.at(s));

            if (it == root_scenes.end())
            {
                root_scenes[map_temps.at(t).temp_externalSceneHashes.at(s)] = root_scenes.size();

                get_root_scenes();
            }
        }
    }
}

void map::map_recursive_temp_loader(uint32_t rpkg_index, uint64_t hash_value)
{
    std::unordered_map<uint64_t, uint64_t>::iterator it = map_temps_map.find(hash_value);

    if (it != map_temps_map.end())
    {
        //std::cout << hash_value << " was already found/added." << std::endl;

        return;
    }

    map_temps_map[hash_value] = map_temps_map.size();

    it = rpkgs.at(rpkg_index).hash_map.find(hash_value);

    if (it != rpkgs.at(rpkg_index).hash_map.end())
    {
        map_temps.emplace_back(temp(rpkg_index, it->second, 3));

        uint32_t temp_index = map_temps.size() - 1;

        //map_temps.at(temp_index).load_data();

        if (map_temps.at(temp_index).tblu_return_value == TEMP_TBLU_FOUND)
        {
            //std::cout << std::endl << "Processing TEMP file: " << map_temps.at(temp_index).temp_file_name << std::endl;

            std::vector<uint32_t> prims_index_in_hash_depends;
            std::vector<std::string> prims_index_in_hash_depends_file_name;

            uint32_t temp_hash_reference_count = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint64_t p = 0; p < temp_hash_reference_count; p++)
            {
                uint64_t hash_reference_value = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(p);

                uint32_t rpkg_index2 = rpkg_function::get_latest_hash(hash_reference_value);

                if (rpkg_index2 != UINT32_MAX)
                {
                    std::unordered_map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index2).hash_map.find(hash_reference_value);

                    if (it2 != rpkgs.at(rpkg_index2).hash_map.end())
                    {
                        if (rpkgs.at(rpkg_index2).hash.at(it2->second).hash_resource_type == "TEMP")
                        {
                            map_recursive_temp_loader(rpkg_index2, rpkgs.at(rpkg_index2).hash.at(it2->second).hash_value);
                        }
                        else if (rpkgs.at(rpkg_index2).hash.at(it2->second).hash_resource_type == "ASET")
                        {
                            //std::cout << map_temps.at(temp_index).temp_file_name << " has ASET reference file: " << rpkgs.at(rpkg_index2).hash.at(it2->second).hash_file_name << std::endl;

                            uint32_t temp_hash_reference_count2 = rpkgs.at(rpkg_index2).hash.at(it2->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                            for (uint64_t a = 0; a < temp_hash_reference_count2; a++)
                            {
                                uint64_t hash_reference_value2 = rpkgs.at(rpkg_index2).hash.at(it2->second).hash_reference_data.hash_reference.at(a);

                                uint32_t rpkg_index3 = rpkg_function::get_latest_hash(hash_reference_value2);

                                if (rpkg_index3 != UINT32_MAX)
                                {
                                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(rpkg_index3).hash_map.find(hash_reference_value2);

                                    if (it3 != rpkgs.at(rpkg_index3).hash_map.end())
                                    {
                                        if (rpkgs.at(rpkg_index3).hash.at(it3->second).hash_resource_type == "TEMP")
                                        {
                                            //std::cout << rpkgs.at(rpkg_index2).hash.at(it2->second).hash_file_name << " has TEMP reference file: " << rpkgs.at(rpkg_index3).hash.at(it3->second).hash_file_name << std::endl;

                                            map_recursive_temp_loader(rpkg_index3, rpkgs.at(rpkg_index3).hash.at(it3->second).hash_value);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool map::matrixes_equal(matrix43& matrix1, matrix43& matrix2)
{
    if (!util::floats_equal(matrix1.x_axis.x, matrix2.x_axis.x))
    {
        std::cout << matrix1.x_axis.x << ", " << matrix2.x_axis.x << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.y_axis.x, matrix2.y_axis.x))
    {
        std::cout << matrix1.y_axis.x << ", " << matrix2.y_axis.x << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.z_axis.x, matrix2.z_axis.x))
    {
        std::cout << matrix1.z_axis.x << ", " << matrix2.z_axis.x << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.x_axis.y, matrix2.x_axis.y))
    {
        std::cout << matrix1.x_axis.y << ", " << matrix2.x_axis.y << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.y_axis.y, matrix2.y_axis.y))
    {
        std::cout << matrix1.y_axis.y << ", " << matrix2.y_axis.y << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.z_axis.y, matrix2.z_axis.y))
    {
        std::cout << matrix1.z_axis.y << ", " << matrix2.z_axis.y << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.x_axis.z, matrix2.x_axis.z))
    {
        std::cout << matrix1.x_axis.z << ", " << matrix2.x_axis.z << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.y_axis.z, matrix2.y_axis.z))
    {
        std::cout << matrix1.y_axis.z << ", " << matrix2.y_axis.z << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.z_axis.z, matrix2.z_axis.z))
    {
        std::cout << matrix1.z_axis.z << ", " << matrix2.z_axis.z << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.transform.x, matrix2.transform.x))
    {
        std::cout << matrix1.transform.x << ", " << matrix2.transform.x << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.transform.y, matrix2.transform.y))
    {
        std::cout << matrix1.transform.y << ", " << matrix2.transform.y << std::endl;
        return false;
    }
    if (!util::floats_equal(matrix1.transform.z, matrix2.transform.z))
    {
        std::cout << matrix1.transform.z << ", " << matrix2.transform.z << std::endl;
        return false;
    }

    return true;
}