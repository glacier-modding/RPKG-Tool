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
#include <Windows.h>
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

void map::export_map(std::string& input_path, std::string& filter, std::string& output_path, bool textured)
{
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        LOG("Loading Hash List...");

        generic_function::load_hash_list(false);

        LOG("Loading Hash List: Done");

        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        std::vector<std::string> filters = util::parse_input_filter(filter);

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        if (filters.size() != 1)
        {
            LOG_AND_RETURN("Error: This function only takes one hash filter value, and it should be the root map entity TEMP hash value.");
        }

        uint64_t hash_value = std::strtoull(filters.at(0).c_str(), nullptr, 16);

        uint32_t rpkg_index = rpkg_function::get_latest_hash(hash_value, false);

        //std::cout << rpkg_index << std::endl;

        //std::cout << rpkgs.at(rpkg_index).rpkg_file_name << std::endl;

        map temp_map(hash_value);

        temp_map.map_recursive_temp_loader(rpkg_index, hash_value);

        temp_map.root_scenes[hash_value] = temp_map.root_scenes.size();

        temp_map.get_root_scenes();

        std::map<uint64_t, uint64_t>::iterator it;

        for (it = temp_map.root_scenes.begin(); it != temp_map.root_scenes.end(); ++it)
        {
            //std::cout << "Root scenes (TEMPs): " << util::uint64_t_to_hex_string(it->first) << std::endl;
        }

        for (uint64_t t = 0; t < temp_map.map_temps.size(); t++)
        {
            temp_map.get_map_node(temp_map.map_temps.at(t));
        }

        temp_map.extract_map_prims(output_path, textured);

        for (uint64_t m = 0; m < temp_map.map_nodes.size(); m++)
        {
            std::cout << std::endl;
            std::cout << "temp_hash: " << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).temp_hash) << std::endl;
            std::cout << "entityId: " << temp_map.map_nodes.at(m).entityId << std::endl;
            std::cout << "subEntity: " << temp_map.map_nodes.at(m).subEntity << std::endl;
            std::cout << "entryName: " << temp_map.map_nodes.at(m).entryName << std::endl;
            std::cout << "entityTypeResourceIndex: " << temp_map.map_nodes.at(m).entityTypeResourceIndex << std::endl;
            std::cout << "entityTypeResourceIndex_hash: " << util::uint64_t_to_hex_string(temp_map.map_nodes.at(m).entityTypeResourceIndex_hash) << std::endl;
            std::cout << "entityTypeResourceIndex_hash_type: " << temp_map.map_nodes.at(m).entityTypeResourceIndex_hash_type << std::endl;

            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(temp_map.map_nodes.at(m).entityTypeResourceIndex_hash);

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

                    std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(temp_map.map_nodes.at(m).externalSceneIndex_hash);

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
        }

        for (uint64_t m = 0; m < temp_map.map_nodes.size(); m++)
        {
            bool is_parent_node_local = false;
            bool temp_has_m_eidParents = false;
            uint32_t local_temp_entity_count = 0;

            for (uint64_t n = 0; n < temp_map.map_nodes.size(); n++)
            {
                if (temp_map.map_nodes.at(m).temp_hash == temp_map.map_nodes.at(n).temp_hash)
                {
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

            for (it = temp_map.root_scenes.begin(); it != temp_map.root_scenes.end(); ++it)
            {
                if (temp_map.map_nodes.at(m).temp_hash == it->first)
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

        temp_map.godot_v3_tscn_file = "[gd_scene load_steps=" + util::uint32_t_to_string(temp_map.map_prims.size() + 3) + " format=2]\n\n";
        temp_map.godot_v3_tscn_file += "[sub_resource type=\"SpatialMaterial\" id=1]\n";
        temp_map.godot_v3_tscn_file += "flags_unshaded=true\n";
        temp_map.godot_v3_tscn_file += "params_blend_mode=3\n";

        temp_map.godot_v4_tscn_file = "[gd_scene load_steps=" + util::uint32_t_to_string(temp_map.map_prims.size() + 2) + " format=3]\n\n";

        for (uint64_t p = 0; p < temp_map.map_prims.size(); p++)
        {
            std::string prim_hash_string = util::uint64_t_to_hex_string(temp_map.map_prims.at(p));

            std::string prim_path = prim_hash_string + ".PRIM/" + prim_hash_string + ".PRIM.glb";

            temp_map.godot_v3_tscn_file += "[ext_resource type=\"PackedScene\" path=\"res://" + prim_path + "\" id=" + util::uint32_t_to_string(p + 2) + "]\n";

            temp_map.godot_v4_tscn_file += "[ext_resource type=\"PackedScene\" path=\"res://" + prim_path + "\" id=" + util::uint32_t_to_string(p) + "]\n";
        }

        temp_map.godot_v3_tscn_file += "[node name=\"Spatial\" type=\"Spatial\"]\n\n";
        temp_map.godot_v3_tscn_file += "transform = Transform(1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0)\n\n";

        temp_map.godot_v4_tscn_file += "[node name=\"Node3D\" type=\"Node3D\"]\n";
        temp_map.godot_v4_tscn_file += "transform = Transform3D(1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0)\n\n";

        for (uint32_t m = 0; m < temp_map.map_root_nodes.size(); m++)
        {
            //std::cout << "Map root node: " << temp_map.map_root_nodes.at(m) << std::endl;

            /*std::string node_string = temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).entryName + " (" + util::uint32_t_to_string(temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).subEntity) + ")";

            std::string transform_string = "";

            if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).has_m_mTransform)
            {
                transform_string = "";

                matrix43 transform = temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_mTransform;

                if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).has_m_PrimitiveScale && temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).has_m_vGlobalSize)
                {
                    temp_map.scale_transform(transform, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_PrimitiveScale.x, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_PrimitiveScale.y, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_PrimitiveScale.z);
                }
                else if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).has_m_PrimitiveScale)
                {
                    temp_map.scale_transform(transform, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_PrimitiveScale.x, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_PrimitiveScale.y, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_PrimitiveScale.z);
                }
                else if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).has_m_vGlobalSize)
                {
                    temp_map.scale_transform(transform, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_vGlobalSize.x, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_vGlobalSize.y, temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).m_vGlobalSize.z);
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

            if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).has_prim_resource)
            {
                it = temp_map.map_prims_map.find(temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).prim_hash);

                if (it != temp_map.map_prims_map.end())
                {
                    temp_map.godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it->second) + " )]\n\n";

                    temp_map.godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }
            }
            else if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).is_volume_box)
            {
                temp_map.godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                temp_map.godot_v4_tscn_file += "transparency = 0.98\n";

                temp_map.godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                map_nodes.at(n).m_mTransform_string = transform_string;
            }
            else if (temp_map.map_nodes.at(temp_map.map_root_nodes.at(m)).is_volume_sphere)
            {
                temp_map.godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                temp_map.godot_v4_tscn_file += "transparency = 0.98\n";

                temp_map.godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                map_nodes.at(n).m_mTransform_string = transform_string;
            }
            else
            {
                temp_map.godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";

                temp_map.godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                map_nodes.at(n).m_mTransform_string = transform_string;
            }*/

            //temp_map.generate_map_node_strings(temp_map.map_root_nodes.at(m), node_string, 1);
            temp_map.generate_map_node_strings(temp_map.map_root_nodes.at(m), 0, "", 0);
        }

        std::string UE5_file = "map_root_nodes=";

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
        }

        //std::cout << temp_map.godot_v4_tscn_file << std::endl;

        std::string godot_v3_godot_file = "config_version = 4\n\n";
        godot_v3_godot_file += "[application]\n\n";
        godot_v3_godot_file += "config/name=\"map\"\n";
        godot_v3_godot_file += "run/main_scene=\"res://map.tscn\"\n";
        godot_v3_godot_file += "config/icon=\"res://icon.png\"\n";
        godot_v3_godot_file += "[physics]\n";
        godot_v3_godot_file += "common/enable_pause_aware_picking=true\n";
        godot_v3_godot_file += "[rendering]\n";
        godot_v3_godot_file += "environment/default_environment=\"res://default_env.tres\"\n";
        godot_v3_godot_file += "[memory]\n";
        godot_v3_godot_file += "limits/message_queue/max_size_kb = 2048000\n";

        std::string godot_v3_default_env_file = "[gd_resource type=\"Environment\" load_steps=2 format=2]\n";
        godot_v3_default_env_file += "[sub_resource type=\"ProceduralSky\" id=1]\n";
        godot_v3_default_env_file += "[resource]\n";
        godot_v3_default_env_file += "background_mode = 2\n";
        godot_v3_default_env_file += "background_sky = SubResource( 1 )\n";

        std::string godot_v4_godot_file = "config_version = 5\n\n";
        godot_v4_godot_file += "[application]\n\n";
        godot_v4_godot_file += "config/name=\"map\"\n";
        godot_v4_godot_file += "run/main_scene=\"res://map.tscn\"\n";
        godot_v4_godot_file += "config/icon=\"res://icon.png\"\n";
        godot_v4_godot_file += "config/features=PackedStringArray(\"4.0\", \"Vulkan Clustered\")\n";
        godot_v4_godot_file += "[memory]\n";
        godot_v4_godot_file += "limits/message_queue/max_size_kb = 2048000\n";

        std::string godot_v3_path = file::output_path_append("godot_v3", output_path);
        std::string godot_v4_path = file::output_path_append("godot_v4", output_path);

        file::create_directories(godot_v3_path);
        file::create_directories(godot_v4_path);

        

        if (!file::write_to_file(godot_v3_path + "\\default_env.tres", godot_v3_default_env_file))
        {
            LOG("Error: File " + godot_v3_path + "\\default_env.tres could not be created.");
        }

        if (!file::write_to_file(godot_v3_path + "\\map.tscn", temp_map.godot_v3_tscn_file))
        {
            LOG("Error: File " + godot_v3_path + "\\map.tscn could not be created.");
        }

        if (!file::write_to_file(godot_v4_path + "\\map.tscn", temp_map.godot_v4_tscn_file))
        {
            LOG("Error: File " + godot_v4_path + "\\map.tscn could not be created.");
        }

        if (!file::write_to_file(godot_v3_path + "\\project.godot", godot_v3_godot_file))
        {
            LOG("Error: File " + godot_v3_path + "\\project.godot could not be created.");
        }

        if (!file::write_to_file(godot_v4_path + "\\project.godot", godot_v4_godot_file))
        {
            LOG("Error: File " + godot_v4_path + "\\project.godot could not be created.");
        }
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for map editor mode does not exist.");
    }
}

void map::import_map(std::string& input_path, std::string& filter, std::string& output_path)
{
    std::ifstream input_file = std::ifstream(input_path);

    if (!input_file.good())
    {
        LOG("Error: Godot tscn file " + input_path + " could not be read.");
    }

    input_file.seekg(0, input_file.end);

    uint64_t input_file_size = (uint64_t)input_file.tellg();

    input_file.seekg(0, input_file.beg);

    std::string godot_scene_file(input_file_size, '\0');

    input_file.read(&godot_scene_file[0], input_file_size);

    input_file.close();

    std::string_view godot_scene_file_string_view(godot_scene_file.c_str(), input_file_size);

    response_string = "";

    bool done = false;

    size_t pos1 = godot_scene_file_string_view.find("[n");
    size_t pos2, pos3, pos4, pos5;

    pos2 = pos1;

    if (pos1 != std::string::npos)
    {

    }

    int count = 0;

    while (pos2 != std::string::npos)
    {
        pos1++;

        pos2 = godot_scene_file_string_view.substr(pos1).find("[n");

        if (pos2 != std::string::npos)
        {
            pos3 = godot_scene_file_string_view.substr(pos1, pos2).find("Transform3D(");

            if (pos3 != std::string::npos)
            {
                pos3 += pos1;
                pos3 += 12;

                pos4 = godot_scene_file_string_view.substr(pos3, pos2 - pos3 - 12).find(")");

                if (pos4 != std::string::npos)
                {
                    bool matrix_found = false;

                    matrix43 temp_matrix43;
                    pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                    if (pos5 != std::string::npos)
                    {
                        pos4 -= pos5;
                        pos4--;

                        temp_matrix43.x_axis.x = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                        pos3 += pos5;
                        pos3++;

                        pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                        if (pos5 != std::string::npos)
                        {
                            pos4 -= pos5;
                            pos4--;

                            temp_matrix43.y_axis.x = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                            pos3 += pos5;
                            pos3++;

                            pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                            if (pos5 != std::string::npos)
                            {
                                pos4 -= pos5;
                                pos4--;

                                temp_matrix43.z_axis.x = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                pos3 += pos5;
                                pos3++;

                                pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                if (pos5 != std::string::npos)
                                {
                                    pos4 -= pos5;
                                    pos4--;

                                    temp_matrix43.x_axis.y = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                    pos3 += pos5;
                                    pos3++;

                                    pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                    if (pos5 != std::string::npos)
                                    {
                                        pos4 -= pos5;
                                        pos4--;

                                        temp_matrix43.y_axis.y = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                        pos3 += pos5;
                                        pos3++;

                                        pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                        if (pos5 != std::string::npos)
                                        {
                                            pos4 -= pos5;
                                            pos4--;

                                            temp_matrix43.z_axis.y = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                            pos3 += pos5;
                                            pos3++;

                                            pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                            if (pos5 != std::string::npos)
                                            {
                                                pos4 -= pos5;
                                                pos4--;

                                                temp_matrix43.x_axis.z = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                                pos3 += pos5;
                                                pos3++;

                                                pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                                if (pos5 != std::string::npos)
                                                {
                                                    pos4 -= pos5;
                                                    pos4--;

                                                    temp_matrix43.y_axis.z = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                                    pos3 += pos5;
                                                    pos3++;

                                                    pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                                    if (pos5 != std::string::npos)
                                                    {
                                                        pos4 -= pos5;
                                                        pos4--;

                                                        temp_matrix43.z_axis.z = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                                        pos3 += pos5;
                                                        pos3++;

                                                        pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                                        if (pos5 != std::string::npos)
                                                        {
                                                            pos4 -= pos5;
                                                            pos4--;

                                                            temp_matrix43.transform.x = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                                            pos3 += pos5;
                                                            pos3++;

                                                            pos5 = godot_scene_file_string_view.substr(pos3, pos4).find(',');

                                                            if (pos5 != std::string::npos)
                                                            {
                                                                pos4 -= pos5;
                                                                pos4--;

                                                                temp_matrix43.transform.y = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos5)).c_str(), nullptr);

                                                                pos3 += pos5;
                                                                pos3++;

                                                                temp_matrix43.transform.z = std::strtof(std::string(godot_scene_file_string_view.substr(pos3, pos4)).c_str(), nullptr);

                                                                pos3 = godot_scene_file_string_view.substr(pos1, pos2).find("name=\"");

                                                                if (pos3 != std::string::npos)
                                                                {
                                                                    pos4 = pos1 + pos3 + 6;

                                                                    pos5 = godot_scene_file_string_view.substr(pos4, pos2).find("\"");

                                                                    if (pos5 != std::string::npos)
                                                                    {
                                                                        matrix_found = true;

                                                                        pos3 = godot_scene_file_string_view.substr(pos4, pos5).find("(");

                                                                        if (pos3 != std::string::npos)
                                                                        {
                                                                            //std::cout << godot_scene_file_string_view.substr(pos4, pos5);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.x_axis.x);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.y_axis.x);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.z_axis.x);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.x_axis.y);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.y_axis.y);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.z_axis.y);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.x_axis.z);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.y_axis.z);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.z_axis.z);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.transform.x);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.transform.y);
                                                                            //std::cout << ',';
                                                                            //std::cout << util::float_to_string(temp_matrix43.transform.z);
                                                                            //std::cout << '|' << std::endl;
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
                            }
                        }
                    }

                    if (!matrix_found)
                    {
                        //std::cout << "No matrix found!" << std::endl;
                    }
                }
            }

            count++;
        }

        pos1 += pos2;
    }
}

void map::add_to_m_eidParent_local_temp_hash_entityIndex_map(uint32_t map_node, uint64_t temp_hash, uint32_t entityIndex)
{
    std::map<uint64_t, std::map<uint32_t, std::vector<uint32_t>>>::iterator it = m_eidParent_local_temp_hash_entityIndex_map.find(temp_hash);

    if (it != m_eidParent_local_temp_hash_entityIndex_map.end())
    {
        std::map<uint32_t, std::vector<uint32_t>>::iterator it2 = it->second.find(entityIndex);

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
        std::map<uint32_t, std::vector<uint32_t>> temp_temp_hash_entityIndex_map;

        std::vector<uint32_t> temp_map_node_vector;

        temp_map_node_vector.push_back(map_node);

        temp_temp_hash_entityIndex_map[entityIndex] = temp_map_node_vector;

        m_eidParent_local_temp_hash_entityIndex_map[temp_hash] = temp_temp_hash_entityIndex_map;
    }
}

void map::add_to_m_eidParent_external_externalSceneIndex_hash_entityID_map(uint32_t map_node, uint64_t externalSceneIndex_hash, uint32_t entityID)
{
    std::map<uint64_t, std::map<uint64_t, std::vector<uint32_t>>>::iterator it = m_eidParent_external_externalSceneIndex_hash_entityID_map.find(externalSceneIndex_hash);

    if (it != m_eidParent_external_externalSceneIndex_hash_entityID_map.end())
    {
        std::map<uint64_t, std::vector<uint32_t>>::iterator it2 = it->second.find(entityID);

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
        std::map<uint64_t, std::vector<uint32_t>> temp_externalSceneIndex_hash_entityID_map;

        std::vector<uint32_t> temp_map_node_vector;

        temp_map_node_vector.push_back(map_node);

        temp_externalSceneIndex_hash_entityID_map[entityID] = temp_map_node_vector;

        m_eidParent_external_externalSceneIndex_hash_entityID_map[externalSceneIndex_hash] = temp_externalSceneIndex_hash_entityID_map;
    }
}

void map::add_to_temp_hash_is_top_level_Local_node_map(uint32_t map_node, uint64_t temp_hash)
{
    std::map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_is_top_level_Local_node_map.find(temp_hash);

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
            std::map<uint64_t, uint64_t>::iterator it = map_prims_map.find(map_nodes.at(m).prim_hash);

            if (it != map_prims_map.end())
            {
                //std::cout << map_nodes.at(m).prim_hash << " was already found/added." << std::endl;
            }
            else
            {
                uint32_t rpkg_index = rpkg_function::get_latest_hash(map_nodes.at(m).prim_hash, false);

                if (rpkg_index != UINT32_MAX)
                {
                    std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index).hash_map.find(map_nodes.at(m).prim_hash);

                    std::string prim_hash_string = util::uint64_t_to_hex_string(map_nodes.at(m).prim_hash);

                    std::string prim_path = file::output_path_append(prim_hash_string + ".PRIM", output_path);

                    file::create_directories(prim_path);

                    if (it2 != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        if (textured)
                        {
                            rpkg_function::extract_prim_textured_from(rpkgs.at(rpkg_index).rpkg_file_path, prim_hash_string, prim_path, false);
                        }
                        else
                        {
                            rpkg_function::extract_prim_from(rpkgs.at(rpkg_index).rpkg_file_path, prim_hash_string, prim_path, GLB_SINGLE, false);
                        }
                    }

                    if (file::path_exists(file::output_path_append(prim_hash_string + ".PRIM.glb", prim_path)))
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

void map::generate_map_node_strings(uint32_t root_map_node, uint32_t parent_map_node, std::string map_node_string, uint32_t level)
{
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

    if (map_nodes.at(root_map_node).entityTypeResourceIndex_hash_type == "ASET")
    {
        uint32_t rpkg_index = rpkg_function::get_latest_hash(map_nodes.at(root_map_node).entityTypeResourceIndex_hash, false);

        if (rpkg_index != UINT32_MAX)
        {
            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(map_nodes.at(root_map_node).entityTypeResourceIndex_hash);

            if (it != rpkgs.at(rpkg_index).hash_map.end())
            {
                uint32_t temp_hash_reference_count = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint64_t a = 0; a < temp_hash_reference_count; a++)
                {
                    uint64_t hash_reference_value = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(a);

                    uint32_t rpkg_index2 = rpkg_function::get_latest_hash(hash_reference_value, false);

                    if (rpkg_index2 != UINT32_MAX)
                    {
                        std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index2).hash_map.find(hash_reference_value);

                        if (it2 != rpkgs.at(rpkg_index2).hash_map.end())
                        {
                            if (rpkgs.at(rpkg_index2).hash.at(it2->second).hash_resource_type == "TEMP")
                            {
                                //std::cout << rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name << " has TEMP reference file: " << rpkgs.at(rpkg_index2).hash.at(it2->second).hash_file_name << std::endl;

                                temp_entityTypeResourceIndex_hash = rpkgs.at(rpkg_index2).hash.at(it2->second).hash_value;

                                has_aset_with_temp_resource = true;
                            }
                        }
                    }
                }
            }
        }
    }

    {
        std::map<uint64_t, std::map<uint32_t, std::vector<uint32_t>>>::iterator it = m_eidParent_local_temp_hash_entityIndex_map.find(map_nodes.at(root_map_node).temp_hash);

        if (it != m_eidParent_local_temp_hash_entityIndex_map.end())
        {
            std::map<uint32_t, std::vector<uint32_t>>::iterator it2 = it->second.find(map_nodes.at(root_map_node).subEntity);

            if (it2 != it->second.end())
            {
                for (uint32_t i = 0; i < it2->second.size(); i++)
                {
                    uint32_t n = it2->second.at(i);

                    //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_local to " << map_nodes.at(n).entryName << std::endl;

                    std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";

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
                        std::map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                        if (it3 != map_prims_map.end())
                        {
                            if (level == 0)
                            {
                                godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                                godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                            }
                            else
                            {
                                godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                                godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                            }

                            godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                            godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            map_nodes.at(n).type = "glb";

                            map_nodes.at(n).m_mTransform_string = transform_string;
                        }
                    }
                    else if (map_nodes.at(n).is_volume_box)
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\".\"]\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "width = 1.0\n";
                            godot_v3_tscn_file += "height = 1.0\n";
                            godot_v3_tscn_file += "depth = 1.0\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\"" + map_node_string + "\"]\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "width = 1.0\n";
                            godot_v3_tscn_file += "height = 1.0\n";
                            godot_v3_tscn_file += "depth = 1.0\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "box";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }
                    else if (map_nodes.at(n).is_volume_sphere)
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\".\"]\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "radius = 0.5\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\"" + map_node_string + "\"]\n";
                            godot_v3_tscn_file += "transform = Transform(1, 0, 0, 0, 1, 0, 0, 0, 1, 1.40402, 0, 0)\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "radius = 0.5\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "sphere";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }
                    else
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\".\"]\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\"" + map_node_string + "\"]\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\"" + map_node_string + "\"]\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "empty";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }

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

    {
        std::map<uint64_t, std::map<uint64_t, std::vector<uint32_t>>>::iterator it = m_eidParent_external_externalSceneIndex_hash_entityID_map.find(map_nodes.at(root_map_node).temp_hash);

        if (it != m_eidParent_external_externalSceneIndex_hash_entityID_map.end())
        {
            std::map<uint64_t, std::vector<uint32_t>>::iterator it2 = it->second.find(map_nodes.at(root_map_node).entityId);

            if (it2 != it->second.end())
            {
                for (uint32_t i = 0; i < it2->second.size(); i++)
                {
                    uint32_t n = it2->second.at(i);

                    //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_external to " << map_nodes.at(n).entryName << std::endl;

                    std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";

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
                        std::map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                        if (it3 != map_prims_map.end())
                        {
                            if (level == 0)
                            {
                                godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                                godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                            }
                            else
                            {
                                godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                                godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                            }

                            godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                            godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                            map_nodes.at(n).type = "glb";

                            map_nodes.at(n).m_mTransform_string = transform_string;
                        }
                    }
                    else if (map_nodes.at(n).is_volume_box)
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\".\"]\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "width = 1.0\n";
                            godot_v3_tscn_file += "height = 1.0\n";
                            godot_v3_tscn_file += "depth = 1.0\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\"" + map_node_string + "\"]\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "width = 1.0\n";
                            godot_v3_tscn_file += "height = 1.0\n";
                            godot_v3_tscn_file += "depth = 1.0\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "box";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }
                    else if (map_nodes.at(n).is_volume_sphere)
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\".\"]\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "radius = 0.5\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\"" + map_node_string + "\"]\n";
                            godot_v3_tscn_file += "transform = Transform(1, 0, 0, 0, 1, 0, 0, 0, 1, 1.40402, 0, 0)\n";
                            godot_v3_tscn_file += "cast_shadow = 0\n";
                            godot_v3_tscn_file += "radius = 0.5\n";
                            godot_v3_tscn_file += "material = SubResource(1)\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                            godot_v4_tscn_file += "transparency = 0.98\n";
                            godot_v4_tscn_file += "cast_shadow = 0\n";
                            godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "sphere";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }
                    else
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\".\"]\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\"" + map_node_string + "\"]\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\"" + map_node_string + "\"]\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "empty";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }

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

    if (map_nodes.at(root_map_node).entityTypeResourceIndex_hash_type == "TEMP")
    {
        std::map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_is_top_level_Local_node_map.find(map_nodes.at(root_map_node).entityTypeResourceIndex_hash);

        if (it != temp_hash_is_top_level_Local_node_map.end())
        {
            for (uint32_t i = 0; i < it->second.size(); i++)
            {
                uint32_t n = it->second.at(i);

                //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_local to " << map_nodes.at(n).entryName << std::endl;

                std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";

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
                    std::map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                    if (it3 != map_prims_map.end())
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "glb";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }
                }
                else if (map_nodes.at(n).is_volume_box)
                {
                    if (level == 0)
                    {
                        godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\".\"]\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "width = 1.0\n";
                        godot_v3_tscn_file += "height = 1.0\n";
                        godot_v3_tscn_file += "depth = 1.0\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }
                    else
                    {
                        godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\"" + map_node_string + "\"]\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "width = 1.0\n";
                        godot_v3_tscn_file += "height = 1.0\n";
                        godot_v3_tscn_file += "depth = 1.0\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }

                    godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                    godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).type = "box";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }
                else if (map_nodes.at(n).is_volume_sphere)
                {
                    if (level == 0)
                    {
                        godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\".\"]\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "radius = 0.5\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }
                    else
                    {
                        godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\"" + map_node_string + "\"]\n";
                        godot_v3_tscn_file += "transform = Transform(1, 0, 0, 0, 1, 0, 0, 0, 1, 1.40402, 0, 0)\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "radius = 0.5\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }

                    godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                    godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).type = "sphere";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }
                else
                {
                    if (level == 0)
                    {
                        godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\".\"]\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";
                    }
                    else
                    {
                        godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\"" + map_node_string + "\"]\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\"" + map_node_string + "\"]\n";
                    }

                    godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                    godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).type = "empty";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }

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

    if (has_aset_with_temp_resource)
    {
        std::map<uint64_t, std::vector<uint32_t>>::iterator it = temp_hash_is_top_level_Local_node_map.find(temp_entityTypeResourceIndex_hash);

        if (it != temp_hash_is_top_level_Local_node_map.end())
        {
            for (uint32_t i = 0; i < it->second.size(); i++)
            {
                uint32_t n = it->second.at(i);

                //std::cout << map_nodes.at(root_map_node).entryName << "(" << util::uint64_t_to_hex_string(map_nodes.at(root_map_node).temp_hash) << ")" << " is an m_eidParent_local to " << map_nodes.at(n).entryName << std::endl;

                std::string node_string = map_nodes.at(n).entryName + " (" + util::uint32_t_to_string(map_nodes.at(n).subEntity) + ")";

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
                    std::map<uint64_t, uint64_t>::iterator it3 = map_prims_map.find(map_nodes.at(n).prim_hash);

                    if (it3 != map_prims_map.end())
                    {
                        if (level == 0)
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                        }
                        else
                        {
                            godot_v3_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second + 2) + " )]\n\n";

                            godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" instance=ExtResource( " + util::uint32_t_to_string(it3->second) + " )]\n\n";
                        }

                        godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                        godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                        map_nodes.at(n).type = "glb";

                        map_nodes.at(n).m_mTransform_string = transform_string;
                    }
                }
                else if (map_nodes.at(n).is_volume_box)
                {
                    if (level == 0)
                    {
                        godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\".\"]\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "width = 1.0\n";
                        godot_v3_tscn_file += "height = 1.0\n";
                        godot_v3_tscn_file += "depth = 1.0\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGBox3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }
                    else
                    {
                        godot_v3_tscn_file += "[node name=\"CSGBox\" type=\"CSGBox\" parent=\"" + map_node_string + "\"]\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "width = 1.0\n";
                        godot_v3_tscn_file += "height = 1.0\n";
                        godot_v3_tscn_file += "depth = 1.0\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGBox3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }

                    godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                    godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).type = "box";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }
                else if (map_nodes.at(n).is_volume_sphere)
                {
                    if (level == 0)
                    {
                        godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\".\"]\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "radius = 0.5\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\".\" type=\"CSGSphere3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }
                    else
                    {
                        godot_v3_tscn_file += "[node name=\"CSGSphere\" type=\"CSGSphere\" parent=\"" + map_node_string + "\"]\n";
                        godot_v3_tscn_file += "transform = Transform(1, 0, 0, 0, 1, 0, 0, 0, 1, 1.40402, 0, 0)\n";
                        godot_v3_tscn_file += "cast_shadow = 0\n";
                        godot_v3_tscn_file += "radius = 0.5\n";
                        godot_v3_tscn_file += "material = SubResource(1)\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" parent=\"" + map_node_string + "\" type=\"CSGSphere3D\"]\n";
                        godot_v4_tscn_file += "transparency = 0.98\n";
                        godot_v4_tscn_file += "cast_shadow = 0\n";
                        godot_v4_tscn_file += "size = Vector3(1, 1, 1)\n";
                    }

                    godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                    godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).type = "sphere";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }
                else
                {
                    if (level == 0)
                    {
                        godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\".\"]\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\".\"]\n";
                    }
                    else
                    {
                        godot_v3_tscn_file += "[node name=\"" + node_string + "\" type=\"Spatial\" parent=\"" + map_node_string + "\"]\n";

                        godot_v4_tscn_file += "[node name=\"" + node_string + "\" type=\"Node3D\" parent=\"" + map_node_string + "\"]\n";
                    }

                    godot_v3_tscn_file += "transform = Transform(" + transform_string + ")\n";

                    godot_v4_tscn_file += "transform = Transform3D(" + transform_string + ")\n";

                    map_nodes.at(n).type = "empty";

                    map_nodes.at(n).m_mTransform_string = transform_string;
                }

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

void map::get_map_node(temp& temp_temp)
{
    const rapidjson::Value& temp_json_subEntities = temp_temp.temp_json_document["subEntities"];

    for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++)
    {
        map_node temp_map_node;

        temp_map_node.temp_hash = std::strtoull(temp_temp.temp_file_name.c_str(), nullptr, 16);

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

                uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_map_node.entityTypeResourceIndex_hash, false);

                if (rpkg_index != UINT32_MAX)
                {
                    std::map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(rpkg_index).hash_map.find(temp_map_node.entityTypeResourceIndex_hash);

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

                            temp_map_node.has_m_PrimitiveScale = true;

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
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_vGlobalSize") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_vGlobalSize in propertyValues" << std::endl;

                            temp_map_node.has_m_vGlobalSize = true;

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

                            temp_map_node.has_m_PrimitiveScale = true;

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
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (std::strcmp(it3->value.GetString(), "m_vGlobalSize") == 0)
                        {
                            //std::cout << temp_temp.temp_file_name << ": " << temp_temp.tblu_entityName.at(s) << " has a m_vGlobalSize in propertyValues" << std::endl;

                            temp_map_node.has_m_vGlobalSize = true;

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
            add_to_m_eidParent_external_externalSceneIndex_hash_entityID_map(map_nodes.size(), temp_map_node.entityTypeResourceIndex_hash, temp_map_node.entityID);
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

            std::map<uint64_t, uint64_t>::iterator it = root_scenes.find(map_temps.at(t).temp_externalSceneHashes.at(s));

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
    std::map<uint64_t, uint64_t>::iterator it = map_temps_map.find(hash_value);

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

        map_temps.at(temp_index).load_data();

        if (map_temps.at(temp_index).tblu_return_value == TEMP_TBLU_FOUND)
        {
            //std::cout << std::endl << "Processing TEMP file: " << map_temps.at(temp_index).temp_file_name << std::endl;

            std::vector<uint32_t> prims_index_in_hash_depends;
            std::vector<std::string> prims_index_in_hash_depends_file_name;

            uint32_t temp_hash_reference_count = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint64_t p = 0; p < temp_hash_reference_count; p++)
            {
                uint64_t hash_reference_value = rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(p);

                uint32_t rpkg_index2 = rpkg_function::get_latest_hash(hash_reference_value, false);

                if (rpkg_index2 != UINT32_MAX)
                {
                    std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index2).hash_map.find(hash_reference_value);

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

                                uint32_t rpkg_index3 = rpkg_function::get_latest_hash(hash_reference_value2, false);

                                if (rpkg_index3 != UINT32_MAX)
                                {
                                    std::map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(rpkg_index3).hash_map.find(hash_reference_value2);

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
