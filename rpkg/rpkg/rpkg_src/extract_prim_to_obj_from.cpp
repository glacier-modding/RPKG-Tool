#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

struct vector2
{
    float x = 0;
    float y = 0;
};

struct vector3
{
    float x = 0;
    float y = 0;
    float z = 0;
};

struct vector4
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;
};

struct matrix43
{
    vector3 x_axis;
    vector3 y_axis;
    vector3 z_axis;
    vector3 transform;
};

void rpkg_function::extract_prim_to_obj_from(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

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

        generic_function::load_hash_list(true);

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
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                    if (it != rpkgs.at(rpkg_index).hash_map.end())
                    {                        
                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        std::string temp_hash_file_name = rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name;

                        uint64_t hash_size;

                        if (rpkgs.at(i).hash.at(it->second).is_lz4ed == 1)
                        {
                            hash_size = rpkgs.at(i).hash.at(it->second).hash_size;

                            if (rpkgs.at(i).hash.at(it->second).is_xored == 1)
                            {
                                hash_size &= 0x3FFFFFFF;
                            }
                        }
                        else
                        {
                            hash_size = rpkgs.at(i).hash.at(it->second).hash_size_final;
                        }

                        std::vector<char> input_data(hash_size, 0);

                        std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                        if (!file.good())
                        {
                            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkgs.at(i).hash.at(it->second).hash_offset, file.beg);
                        file.read(input_data.data(), hash_size);
                        file.close();

                        if (rpkgs.at(i).hash.at(it->second).is_xored == 1)
                        {
                            crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                        }

                        uint32_t decompressed_size = rpkgs.at(i).hash.at(it->second).hash_size_final;

                        std::vector<char> output_data(decompressed_size, 0);

                        std::vector<char>* prim_data;

                        if (rpkgs.at(i).hash.at(it->second).is_lz4ed)
                        {
                            LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                            prim_data = &output_data;
                        }
                        else
                        {
                            prim_data = &input_data;
                        }

                        char input[1024];
                        char char4[4];
                        uint8_t bytes1 = 0;
                        uint16_t bytes2 = 0;
                        uint32_t bytes4 = 0;
                        uint64_t bytes8 = 0;

                        uint32_t prim_primary_header_offset = 0;
                        uint8_t prim_primary_header_draw = 0;
                        uint8_t prim_primary_header_pack = 0;
                        uint16_t prim_primary_header_type = 0;
                        uint32_t prim_primary_property_flags = 0;
                        bool prim_primary_flag_bones = false;
                        bool prim_primary_flag_frames = false;
                        bool prim_primary_flag_linked = false;
                        bool prim_primary_flag_weighted = false;
                        bool prim_primary_flag_bounds = false;
                        bool prim_primary_flag_32bitvertexdata = false;
                        uint32_t prim_primary_borg_index = 0;
                        uint32_t prim_primary_object_count = 0;
                        uint32_t prim_primary_object_offset = 0;
                        vector3 prim_primary_bounding_box_min;
                        vector3 prim_primary_bounding_box_max;
                        std::vector<uint32_t> prim_object_offsets;
                        std::vector<uint8_t> prim_object_header_draw;
                        std::vector<uint8_t> prim_object_header_pack;
                        std::vector<uint16_t> prim_object_header_type;
                        std::vector<uint8_t> prim_object_sub_type;
                        std::vector<uint8_t> prim_object_property_flags;
                        std::vector<uint8_t> prim_object_lod;
                        std::vector<uint8_t> prim_object_variant;
                        std::vector<uint8_t> prim_object_bias;
                        std::vector<uint8_t> prim_object_offset;
                        std::vector<uint16_t> prim_object_material_id;
                        std::vector<uint32_t> prim_object_wire_color;
                        std::vector<uint32_t> prim_object_color;
                        vector3 prim_object_bounding_box_min;
                        vector3 prim_object_bounding_box_max;

                        uint32_t prim_position = 0;

                        LOG("PRIM file: " + filters.at(f));

                        std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(rpkg_index).hash.at(it->second).hash_value);

                        if (it2 != hash_list_hash_map.end())
                        {
                            LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
                        }
                        else
                        {
                            LOG("  - IOI String: ");
                        }

                        std::memcpy(&prim_primary_header_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        LOG("  - PRIM primary header offset: " + util::uint32_t_to_hex_string(prim_primary_header_offset));

                        prim_position = prim_primary_header_offset;

                        std::memcpy(&prim_primary_header_draw, &prim_data->data()[prim_position], sizeof(bytes1));
                        prim_position += 0x1;

                        LOG("  - PRIM primary header draw value: " + util::uint8_t_to_hex_string(prim_primary_header_draw));

                        std::memcpy(&prim_primary_header_pack, &prim_data->data()[prim_position], sizeof(bytes1));
                        prim_position += 0x1;

                        LOG("  - PRIM primary header pack value: " + util::uint8_t_to_hex_string(prim_primary_header_draw));

                        std::memcpy(&prim_primary_header_type, &prim_data->data()[prim_position], sizeof(bytes2));
                        prim_position += 0x2;

                        LOG("  - PRIM primary header type value: " + util::uint16_t_to_hex_string(prim_primary_header_type));

                        if (prim_primary_header_type == 1)
                        {
                            LOG("  - PRIM primary header type: Object");
                        }
                        else if (prim_primary_header_type == 2)
                        {
                            LOG("  - PRIM primary header type: Mesh");

                            return;
                        }
                        else
                        {
                            LOG("  - PRIM primary header type: Unknown");

                            return;
                        }

                        std::memcpy(&prim_primary_property_flags, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        LOG("  - PRIM primary header property flags: " + util::uint32_t_to_hex_string(prim_primary_property_flags));

                        if ((prim_primary_property_flags & 0x1) == 0x1)
                        {
                            prim_primary_flag_bones = true;

                            LOG("  - PRIM primary header flag set: Bones");
                        }
                        if ((prim_primary_property_flags & 0x2) == 0x2)
                        {
                            prim_primary_flag_frames = true;

                            LOG("  - PRIM primary header flag set: Frames");
                        }
                        if ((prim_primary_property_flags & 0x4) == 0x4)
                        {
                            prim_primary_flag_linked = true;

                            LOG("  - PRIM primary header flag set: Linked");
                        }
                        if ((prim_primary_property_flags & 0x8) == 0x8)
                        {
                            prim_primary_flag_weighted = true;

                            LOG("  - PRIM primary header flag set: Weighted");
                        }
                        if ((prim_primary_property_flags & 0x100) == 0x100)
                        {
                            prim_primary_flag_bounds = true;

                            LOG("  - PRIM primary header flag set: Bounds");
                        }
                        if ((prim_primary_property_flags & 0x200) == 0x200)
                        {
                            prim_primary_flag_32bitvertexdata = true;

                            LOG("  - PRIM primary header flag set: 32BitVertexData");
                        }

                        std::memcpy(&prim_primary_borg_index, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        LOG("  - PRIM primary BORG index: " + util::uint32_t_to_hex_string(prim_primary_borg_index));

                        std::memcpy(&prim_primary_object_count, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        LOG("  - PRIM primary object count: " + util::uint32_t_to_hex_string(prim_primary_object_count));

                        std::memcpy(&prim_primary_object_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        LOG("  - PRIM primary object offset: " + util::uint32_t_to_hex_string(prim_primary_object_offset));

                        std::memcpy(&prim_primary_bounding_box_min.x, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;
                        std::memcpy(&prim_primary_bounding_box_min.y, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;
                        std::memcpy(&prim_primary_bounding_box_min.z, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        std::memcpy(&prim_primary_bounding_box_max.x, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;
                        std::memcpy(&prim_primary_bounding_box_max.y, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;
                        std::memcpy(&prim_primary_bounding_box_max.z, &prim_data->data()[prim_position], sizeof(bytes4));
                        prim_position += 0x4;

                        LOG("  - PRIM primary bounding box min:");
                        LOG("      - x: " + util::float_to_string(prim_primary_bounding_box_min.x));
                        LOG("      - y: " + util::float_to_string(prim_primary_bounding_box_min.y));
                        LOG("      - z: " + util::float_to_string(prim_primary_bounding_box_min.z));

                        LOG("  - PRIM primary bounding box max:");
                        LOG("      - x: " + util::float_to_string(prim_primary_bounding_box_max.x));
                        LOG("      - y: " + util::float_to_string(prim_primary_bounding_box_max.y));
                        LOG("      - z: " + util::float_to_string(prim_primary_bounding_box_max.z));

                        prim_position = prim_primary_object_offset;

                        for (uint32_t o = 0; o < prim_primary_object_count; o++)
                        {
                            std::memcpy(&bytes4, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;

                            prim_object_offsets.push_back(bytes4);

                            LOG("  - PRIM object (" + std::to_string(o) + ") offset:" + util::uint32_t_to_hex_string(prim_object_offsets.back()));
                        }

                        for (uint32_t o = 0; o < prim_object_offsets.size(); o++)
                        {
                            LOG("  - PRIM object (" + std::to_string(o) + ") offset:" + util::uint32_t_to_hex_string(prim_object_offsets.at(o)));

                            prim_position = prim_object_offsets.at(o);

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_header_draw.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") header draw value: " + util::uint8_t_to_hex_string(prim_object_header_draw.back()));

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_header_pack.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") header pack value: " + util::uint8_t_to_hex_string(prim_object_header_pack.back()));

                            std::memcpy(&bytes2, &prim_data->data()[prim_position], sizeof(bytes2));
                            prim_position += 0x2;

                            prim_object_header_type.push_back(bytes2);

                            LOG("      - PRIM object (" + std::to_string(o) + ") header type value: " + util::uint16_t_to_hex_string(prim_object_header_type.back()));

                            if (prim_object_header_type.back() == 1)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header type: Object");
                            }
                            else if (prim_object_header_type.back() == 2)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header type: Mesh");
                            }
                            else
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header type: Unknown");
                            }

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_sub_type.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") header sub type value: " + util::uint8_t_to_hex_string(prim_object_sub_type.back()));

                            if (prim_object_sub_type.back() == 0x0)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Standard");
                            }
                            else if (prim_object_sub_type.back() == 0x1)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Linked");
                            }
                            else if (prim_object_sub_type.back() == 0x2)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Weighted");
                            }
                            else if (prim_object_sub_type.back() == 0x3)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV2");
                            }
                            else if (prim_object_sub_type.back() == 0x4)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV3");
                            }
                            else if (prim_object_sub_type.back() == 0x5)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV4");
                            }
                            else if (prim_object_sub_type.back() == 0x6)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: SpeedTree");
                            }

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_property_flags.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") header property flags: " + util::uint8_t_to_hex_string(prim_object_property_flags.back()));

                            if (prim_object_property_flags.back() == 0x0)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: None");
                            }
                            if ((prim_object_property_flags.back() & 0x1) == 0x1)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: XAxisLocked");
                            }
                            if ((prim_object_property_flags.back() & 0x2) == 0x2)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: YAxisLocked");
                            }
                            if ((prim_object_property_flags.back() & 0x4) == 0x4)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: ZAxisLocked");
                            }
                            if ((prim_object_property_flags.back() & 0x8) == 0x8)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: 32BitVertexData");
                            }
                            if ((prim_object_property_flags.back() & 0x10) == 0x10)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: PS3");
                            }
                            if ((prim_object_property_flags.back() & 0x20) == 0x20)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: Color1");
                            }
                            if ((prim_object_property_flags.back() & 0x40) == 0x40)
                            {
                                LOG("      - PRIM object (" + std::to_string(o) + ") header flag set: NoPhysics");
                            }

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_lod.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") LOD: " + util::uint8_t_to_hex_string(prim_object_lod.back()));

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_variant.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") variant: " + util::uint8_t_to_hex_string(prim_object_variant.back()));

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_bias.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") bias: " + util::uint8_t_to_hex_string(prim_object_bias.back()));

                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                            prim_position += 0x1;

                            prim_object_offset.push_back(bytes1);

                            LOG("      - PRIM object (" + std::to_string(o) + ") offset: " + util::uint8_t_to_hex_string(prim_object_offset.back()));

                            std::memcpy(&bytes2, &prim_data->data()[prim_position], sizeof(bytes2));
                            prim_position += 0x2;

                            prim_object_material_id.push_back(bytes2);

                            LOG("      - PRIM object (" + std::to_string(o) + ") material id: " + util::uint16_t_to_hex_string(prim_object_material_id.back()));

                            std::memcpy(&bytes4, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;

                            prim_object_wire_color.push_back(bytes4);

                            LOG("      - PRIM object (" + std::to_string(o) + ") object wire color: " + util::uint32_t_to_hex_string(prim_object_wire_color.back()));

                            std::memcpy(&bytes4, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;

                            prim_object_color.push_back(bytes4);

                            LOG("      - PRIM object (" + std::to_string(o) + ") object color: " + util::uint32_t_to_hex_string(prim_object_color.back()));

                            std::memcpy(&prim_object_bounding_box_min.x, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;
                            std::memcpy(&prim_object_bounding_box_min.y, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;
                            std::memcpy(&prim_object_bounding_box_min.z, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;

                            std::memcpy(&prim_object_bounding_box_max.x, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;
                            std::memcpy(&prim_object_bounding_box_max.y, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;
                            std::memcpy(&prim_object_bounding_box_max.z, &prim_data->data()[prim_position], sizeof(bytes4));
                            prim_position += 0x4;

                            LOG("      - PRIM object (" + std::to_string(o) + ") bounding box min:");
                            LOG("          - x: " + util::float_to_string(prim_object_bounding_box_min.x));
                            LOG("          - y: " + util::float_to_string(prim_object_bounding_box_min.y));
                            LOG("          - z: " + util::float_to_string(prim_object_bounding_box_min.z));

                            LOG("      - PRIM object (" + std::to_string(o) + ") bounding box max:");
                            LOG("          - x: " + util::float_to_string(prim_object_bounding_box_max.x));
                            LOG("          - y: " + util::float_to_string(prim_object_bounding_box_max.y));
                            LOG("          - z: " + util::float_to_string(prim_object_bounding_box_max.z));

                            if (prim_object_header_type.back() == 2)
                            {
                                uint32_t prim_object_sub_mesh_table_offset_pointer = 0;
                                uint32_t prim_object_sub_mesh_table_offset = 0;
                                vector4 prim_object_mesh_vertex_position_scale;
                                vector4 prim_object_mesh_vertex_position_bias;
                                vector2 prim_object_mesh_uv_position_scale;
                                vector2 prim_object_mesh_uv_position_bias;
                                uint32_t prim_object_cloth_flags = 0;

                                std::memcpy(&prim_object_sub_mesh_table_offset_pointer, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") sub mesh table offset pointer:" + util::uint32_t_to_hex_string(prim_object_sub_mesh_table_offset_pointer));

                                std::memcpy(&prim_object_mesh_vertex_position_scale.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_vertex_position_scale.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_vertex_position_scale.z, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_vertex_position_scale.w, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") mesh vertex position scale:");
                                LOG("          - x: " + util::float_to_string(prim_object_mesh_vertex_position_scale.x));
                                LOG("          - y: " + util::float_to_string(prim_object_mesh_vertex_position_scale.y));
                                LOG("          - z: " + util::float_to_string(prim_object_mesh_vertex_position_scale.z));
                                LOG("          - w: " + util::float_to_string(prim_object_mesh_vertex_position_scale.w));

                                std::memcpy(&prim_object_mesh_vertex_position_bias.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_vertex_position_bias.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_vertex_position_bias.z, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_vertex_position_bias.w, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") mesh vertex position bias:");
                                LOG("          - x: " + util::float_to_string(prim_object_mesh_vertex_position_bias.x));
                                LOG("          - y: " + util::float_to_string(prim_object_mesh_vertex_position_bias.y));
                                LOG("          - z: " + util::float_to_string(prim_object_mesh_vertex_position_bias.z));
                                LOG("          - w: " + util::float_to_string(prim_object_mesh_vertex_position_bias.w));

                                std::memcpy(&prim_object_mesh_uv_position_scale.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_uv_position_scale.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") mesh uv position bias:");
                                LOG("          - x: " + util::float_to_string(prim_object_mesh_uv_position_scale.x));
                                LOG("          - y: " + util::float_to_string(prim_object_mesh_uv_position_scale.y));

                                std::memcpy(&prim_object_mesh_uv_position_bias.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_mesh_uv_position_bias.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") mesh uv position bias:");
                                LOG("          - x: " + util::float_to_string(prim_object_mesh_uv_position_bias.x));
                                LOG("          - y: " + util::float_to_string(prim_object_mesh_uv_position_bias.y));

                                std::memcpy(&prim_object_cloth_flags, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") mesh cloth flags:" + util::uint32_t_to_hex_string(prim_object_cloth_flags));

                                prim_position = prim_object_sub_mesh_table_offset_pointer;

                                std::memcpy(&prim_object_sub_mesh_table_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("      - PRIM object (" + std::to_string(o) + ") sub mesh table offset:" + util::uint32_t_to_hex_string(prim_object_sub_mesh_table_offset));

                                prim_position = prim_object_sub_mesh_table_offset;

                                std::vector<uint32_t> prim_object_sub_mesh_offsets;
                                std::vector<uint8_t> prim_object_sub_mesh_header_draw;
                                std::vector<uint8_t> prim_object_sub_mesh_header_pack;
                                std::vector<uint16_t> prim_object_sub_mesh_header_type;
                                std::vector<uint8_t> prim_object_sub_mesh_sub_type;
                                std::vector<uint8_t> prim_object_sub_mesh_property_flags;
                                std::vector<uint8_t> prim_object_sub_mesh_lod;
                                std::vector<uint8_t> prim_object_sub_mesh_variant;
                                std::vector<uint8_t> prim_object_sub_mesh_bias;
                                std::vector<uint8_t> prim_object_sub_mesh_offset;
                                std::vector<uint16_t> prim_object_sub_mesh_material_id;
                                std::vector<uint32_t> prim_object_sub_mesh_wire_color;
                                std::vector<uint32_t> prim_object_sub_mesh_color;
                                vector3 prim_object_sub_mesh_bounding_box_min;
                                vector3 prim_object_sub_mesh_bounding_box_max;

                                LOG("      - PRIM object (" + std::to_string(o) + ") sub mesh offset:" + util::uint32_t_to_hex_string(prim_position));

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_header_draw.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header draw value: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_header_draw.back()));

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_header_pack.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header pack value: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_header_pack.back()));

                                std::memcpy(&bytes2, &prim_data->data()[prim_position], sizeof(bytes2));
                                prim_position += 0x2;

                                prim_object_sub_mesh_header_type.push_back(bytes2);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header type value: " + util::uint16_t_to_hex_string(prim_object_sub_mesh_header_type.back()));

                                if (prim_object_sub_mesh_header_type.back() == 1)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header type: Object");
                                }
                                else if (prim_object_sub_mesh_header_type.back() == 2)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header type: Mesh");
                                }
                                else
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header type: Unknown");
                                }

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_sub_type.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type value: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_sub_type.back()));

                                if (prim_object_sub_mesh_sub_type.back() == 0x1)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: Standard");
                                }
                                else if (prim_object_sub_mesh_sub_type.back() == 0x2)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: Linked");
                                }
                                else if (prim_object_sub_mesh_sub_type.back() == 0x3)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: Weighted");
                                }
                                else if (prim_object_sub_mesh_sub_type.back() == 0x4)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: StandardUUV2");
                                }
                                else if (prim_object_sub_mesh_sub_type.back() == 0x5)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: StandardUUV3");
                                }
                                else if (prim_object_sub_mesh_sub_type.back() == 0x6)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: StandardUUV4");
                                }
                                else if (prim_object_sub_mesh_sub_type.back() == 0x7)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header sub type: SpeedTree");
                                }

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_property_flags.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header property flags: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_property_flags.back()));

                                if (prim_object_sub_mesh_property_flags.back() == 0x0)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: None");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x1) == 0x1)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: XAxisLocked");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x2) == 0x2)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: YAxisLocked");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x4) == 0x4)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: ZAxisLocked");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x8) == 0x8)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: 32BitVertexData");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x10) == 0x10)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: PS3");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x20) == 0x20)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: Color1");
                                }
                                if ((prim_object_sub_mesh_property_flags.back() & 0x40) == 0x40)
                                {
                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh header flag set: NoPhysics");
                                }

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_lod.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh LOD: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_lod.back()));

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_variant.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh variant: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_variant.back()));

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_bias.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh bias: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_bias.back()));

                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                prim_position += 0x1;

                                prim_object_sub_mesh_offset.push_back(bytes1);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh offset: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_offset.back()));

                                std::memcpy(&bytes2, &prim_data->data()[prim_position], sizeof(bytes2));
                                prim_position += 0x2;

                                prim_object_sub_mesh_material_id.push_back(bytes2);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh material id: " + util::uint16_t_to_hex_string(prim_object_sub_mesh_material_id.back()));

                                std::memcpy(&bytes4, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                prim_object_sub_mesh_wire_color.push_back(bytes4);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh object wire color: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_wire_color.back()));

                                std::memcpy(&bytes4, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                prim_object_sub_mesh_color.push_back(bytes4);

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh object color: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_color.back()));

                                std::memcpy(&prim_object_sub_mesh_bounding_box_min.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_sub_mesh_bounding_box_min.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_sub_mesh_bounding_box_min.z, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                std::memcpy(&prim_object_sub_mesh_bounding_box_max.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_sub_mesh_bounding_box_max.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;
                                std::memcpy(&prim_object_sub_mesh_bounding_box_max.z, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh bounding box min:");
                                LOG("              - x: " + util::float_to_string(prim_object_sub_mesh_bounding_box_min.x));
                                LOG("              - y: " + util::float_to_string(prim_object_sub_mesh_bounding_box_min.y));
                                LOG("              - z: " + util::float_to_string(prim_object_sub_mesh_bounding_box_min.z));

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh bounding box max:");
                                LOG("              - x: " + util::float_to_string(prim_object_sub_mesh_bounding_box_max.x));
                                LOG("              - y: " + util::float_to_string(prim_object_sub_mesh_bounding_box_max.y));
                                LOG("              - z: " + util::float_to_string(prim_object_sub_mesh_bounding_box_max.z));

                                uint32_t prim_object_sub_mesh_vertex_count;
                                uint32_t prim_object_sub_mesh_vertex_buffer_offset;
                                uint32_t prim_object_sub_mesh_index_count;
                                uint32_t prim_object_sub_mesh_index_count_additional;
                                uint32_t prim_object_sub_mesh_index_buffer_offset;
                                uint32_t prim_object_sub_mesh_collision_offset;
                                uint32_t prim_object_sub_mesh_cloth_offset;
                                uint32_t prim_object_sub_mesh_uv_channel_count;
                                uint32_t prim_object_sub_mesh_unknown_offset;
                                std::vector<uint16_t> prim_object_sub_mesh_indexes;
                                std::vector<vector4> prim_object_sub_mesh_vertexes;
                                std::vector<vector4> prim_object_sub_mesh_vertexes_normals;
                                std::vector<vector4> prim_object_sub_mesh_vertexes_tangents;
                                std::vector<vector4> prim_object_sub_mesh_vertexes_bitangents;
                                std::vector<vector2> prim_object_sub_mesh_vertexes_uvs;
                                std::vector<uint8_t> prim_object_sub_mesh_vertexes_colors;

                                std::memcpy(&prim_object_sub_mesh_vertex_count, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex count: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_vertex_count));

                                std::memcpy(&prim_object_sub_mesh_vertex_buffer_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex buffer offset: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_vertex_buffer_offset));

                                std::memcpy(&prim_object_sub_mesh_index_count, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh index count: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_index_count));

                                std::memcpy(&prim_object_sub_mesh_index_count_additional, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh index count additional: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_index_count_additional));

                                std::memcpy(&prim_object_sub_mesh_index_buffer_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh index buffer offset: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_index_buffer_offset));

                                std::memcpy(&prim_object_sub_mesh_collision_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh collision offset: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_collision_offset));

                                std::memcpy(&prim_object_sub_mesh_cloth_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh cloth offset: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_cloth_offset));

                                std::memcpy(&prim_object_sub_mesh_uv_channel_count, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh uv channel count: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_uv_channel_count));

                                std::memcpy(&prim_object_sub_mesh_unknown_offset, &prim_data->data()[prim_position], sizeof(bytes4));
                                prim_position += 0x4;

                                LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh unknown offset: " + util::uint32_t_to_hex_string(prim_object_sub_mesh_unknown_offset));

                                if (prim_object_sub_mesh_uv_channel_count == 1)
                                {
                                    prim_position = prim_object_sub_mesh_index_buffer_offset;

                                    uint32_t index_count_total = prim_object_sub_mesh_index_count + prim_object_sub_mesh_index_count_additional;

                                    LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh indexes at (" + util::uint32_t_to_hex_string(prim_object_sub_mesh_index_buffer_offset) + "): ");

                                    for (uint32_t x = 0; x < index_count_total; x++)
                                    {
                                        std::memcpy(&bytes2, &prim_data->data()[prim_position], sizeof(bytes2));
                                        prim_position += 0x2;

                                        prim_object_sub_mesh_indexes.push_back(bytes2);

                                        //LOG("              - PRIM object (" + std::to_string(o) + ") sub mesh index: " + util::uint16_t_to_hex_string(prim_object_sub_mesh_indexes.back()));
                                    }

                                    prim_position = prim_object_sub_mesh_vertex_buffer_offset;

                                    if (prim_primary_flag_32bitvertexdata)
                                    {
                                        for (uint32_t v = 0; v < prim_object_sub_mesh_vertex_count; v++)
                                        {
                                            vector4 temp_vector4;

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex (32bit float): ");

                                            std::memcpy(&temp_vector4.x, &prim_data->data()[prim_position], sizeof(bytes4));
                                            prim_position += 0x4;

                                            LOG("              - x: " + util::float_to_string(temp_vector4.x));

                                            std::memcpy(&temp_vector4.y, &prim_data->data()[prim_position], sizeof(bytes4));
                                            prim_position += 0x4;

                                            LOG("              - y: " + util::float_to_string(temp_vector4.y));

                                            std::memcpy(&temp_vector4.z, &prim_data->data()[prim_position], sizeof(bytes4));
                                            prim_position += 0x4;

                                            LOG("              - z: " + util::float_to_string(temp_vector4.z));

                                            prim_object_sub_mesh_vertexes.push_back(temp_vector4);
                                        }
                                    }
                                    else
                                    {
                                        for (uint32_t v = 0; v < prim_object_sub_mesh_vertex_count; v++)
                                        {
                                            short temp_short = 0;

                                            vector4 temp_vector4;

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex compressed (short): ");

                                            std::memcpy(&temp_short, &prim_data->data()[prim_position], sizeof(bytes2));
                                            prim_position += 0x2;

                                            LOG("              - x: " + util::short_to_string(temp_short));
                                            
                                            temp_vector4.x = (static_cast<float>(temp_short) * prim_object_mesh_vertex_position_scale.x) / std::numeric_limits<short>::max() + prim_object_mesh_vertex_position_bias.x;

                                            std::memcpy(&temp_short, &prim_data->data()[prim_position], sizeof(bytes2));
                                            prim_position += 0x2;

                                            LOG("              - y: " + util::short_to_string(temp_short));

                                            temp_vector4.y = (static_cast<float>(temp_short) * prim_object_mesh_vertex_position_scale.y) / std::numeric_limits<short>::max() + prim_object_mesh_vertex_position_bias.y;

                                            std::memcpy(&temp_short, &prim_data->data()[prim_position], sizeof(bytes2));
                                            prim_position += 0x2;

                                            LOG("              - z: " + util::short_to_string(temp_short));

                                            temp_vector4.z = (static_cast<float>(temp_short) * prim_object_mesh_vertex_position_scale.z) / std::numeric_limits<short>::max() + prim_object_mesh_vertex_position_bias.z;

                                            std::memcpy(&temp_short, &prim_data->data()[prim_position], sizeof(bytes2));
                                            prim_position += 0x2;

                                            LOG("              - w: " + util::short_to_string(temp_short));

                                            temp_vector4.w = (static_cast<float>(temp_short) * prim_object_mesh_vertex_position_scale.w) / std::numeric_limits<short>::max() + prim_object_mesh_vertex_position_bias.w;

                                            prim_object_sub_mesh_vertexes.push_back(temp_vector4);

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex decompressed (float):");
                                            LOG("              - x: " + util::float_to_string(temp_vector4.x));
                                            LOG("              - y: " + util::float_to_string(temp_vector4.y));
                                            LOG("              - z: " + util::float_to_string(temp_vector4.z));
                                            LOG("              - w: " + util::float_to_string(temp_vector4.w));

                                            //std::cout << "v " << util::float_to_string(temp_vector4.x) << " " << util::float_to_string(temp_vector4.y) << " " << util::float_to_string(temp_vector4.z) << " " << util::float_to_string(temp_vector4.w) << std::endl;
                                        }
                                    }

                                    if (prim_object_sub_type.at(o) != 0x2)
                                    {
                                        for (uint32_t v = 0; v < prim_object_sub_mesh_vertex_count; v++)
                                        {
                                            vector4 temp_vector4;

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex normal compressed (uint8_t):");

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

                                            prim_object_sub_mesh_vertexes_normals.push_back(temp_vector4);

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex normal decompressed (float):");
                                            LOG("              - x: " + util::float_to_string(temp_vector4.x));
                                            LOG("              - y: " + util::float_to_string(temp_vector4.y));
                                            LOG("              - z: " + util::float_to_string(temp_vector4.z));
                                            LOG("              - w: " + util::float_to_string(temp_vector4.w));

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex tangents compressed (uint8_t):");

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

                                            prim_object_sub_mesh_vertexes_tangents.push_back(temp_vector4);

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex tangents decompressed (float):");
                                            LOG("              - x: " + util::float_to_string(temp_vector4.x));
                                            LOG("              - y: " + util::float_to_string(temp_vector4.y));
                                            LOG("              - z: " + util::float_to_string(temp_vector4.z));
                                            LOG("              - w: " + util::float_to_string(temp_vector4.w));

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex bitangents compressed (uint8_t):");

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

                                            std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                            prim_position += 0x1;

                                            temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                                            LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

                                            prim_object_sub_mesh_vertexes_tangents.push_back(temp_vector4);

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex bitangents decompressed (float):");
                                            LOG("              - x: " + util::float_to_string(temp_vector4.x));
                                            LOG("              - y: " + util::float_to_string(temp_vector4.y));
                                            LOG("              - z: " + util::float_to_string(temp_vector4.z));
                                            LOG("              - w: " + util::float_to_string(temp_vector4.w));

                                            short temp_short = 0;

                                            vector2 temp_vector2;

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex uv compressed (short): ");

                                            std::memcpy(&temp_short, &prim_data->data()[prim_position], sizeof(bytes2));
                                            prim_position += 0x2;

                                            LOG("              - x: " + util::short_to_string(temp_short));

                                            temp_vector2.x = (static_cast<float>(temp_short) * prim_object_mesh_vertex_position_scale.x) / std::numeric_limits<short>::max() + prim_object_mesh_vertex_position_bias.x;

                                            std::memcpy(&temp_short, &prim_data->data()[prim_position], sizeof(bytes2));
                                            prim_position += 0x2;

                                            LOG("              - y: " + util::short_to_string(temp_short));

                                            temp_vector2.y = (static_cast<float>(temp_short) * prim_object_mesh_vertex_position_scale.y) / std::numeric_limits<short>::max() + prim_object_mesh_vertex_position_bias.y;

                                            prim_object_sub_mesh_vertexes_uvs.push_back(temp_vector2);

                                            LOG("          - PRIM object (" + std::to_string(o) + ") sub mesh vertex uv decompressed (float):");
                                            LOG("              - x: " + util::float_to_string(temp_vector2.x));
                                            LOG("              - y: " + util::float_to_string(temp_vector2.y));

                                        }

                                        if ((prim_object_property_flags.at(o) & 0x20) == 0x0)
                                        {
                                            for (uint32_t v = 0; v < (prim_object_sub_mesh_vertex_count * (uint32_t)0x4); v++)
                                            {
                                                std::memcpy(&bytes1, &prim_data->data()[prim_position], sizeof(bytes1));
                                                prim_position += 0x1;

                                                prim_object_sub_mesh_vertexes_colors.push_back(bytes1);

                                                LOG("      - PRIM object (" + std::to_string(o) + ") sub mesh vertex color: " + util::uint8_t_to_hex_string(prim_object_sub_mesh_vertexes_colors.back()));
                                            }
                                        }

                                        std::string obj_file_name = file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + "." + std::to_string(o) + ".obj", output_path);

                                        std::ofstream output_file = std::ofstream(obj_file_name, std::ofstream::binary);

                                        if (!output_file.good())
                                        {
                                            LOG_AND_EXIT("Error: OBJ file " + obj_file_name + " could not be created.");
                                        }

                                        std::string obj_file_data = "";

                                        for (uint32_t v = 0; v < prim_object_sub_mesh_vertexes.size(); v++)
                                        {
                                            obj_file_data.append("v ");
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes.at(v).x));
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes.at(v).y));
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes.at(v).z));
                                            //obj_file_data.push_back(' ');
                                            //obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes.at(v).w));
                                            obj_file_data.push_back('\n');
                                        }

                                        for (uint32_t v = 0; v < prim_object_sub_mesh_vertexes_normals.size(); v++)
                                        {
                                            obj_file_data.append("vn ");
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes_normals.at(v).x));
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes_normals.at(v).y));
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes_normals.at(v).z));
                                            //obj_file_data.push_back(' ');
                                            //obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes_normals.at(v).w));
                                            obj_file_data.push_back('\n');
                                        }

                                        for (uint32_t v = 0; v < prim_object_sub_mesh_vertexes_uvs.size(); v++)
                                        {
                                            obj_file_data.append("vt ");
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes_uvs.at(v).x));
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(util::float_to_string(prim_object_sub_mesh_vertexes_uvs.at(v).y));
                                            obj_file_data.push_back('\n');
                                        }

                                        for (uint32_t v = 0; v < (prim_object_sub_mesh_indexes.size() / 3); v++)
                                        {
                                            uint32_t one = prim_object_sub_mesh_indexes.at((uint64_t)v * (uint64_t)3);
                                            one++;
                                            uint32_t two = prim_object_sub_mesh_indexes.at((uint64_t)v * (uint64_t)3 + (uint64_t)1);
                                            two++;
                                            uint32_t three = prim_object_sub_mesh_indexes.at((uint64_t)v * (uint64_t)3 + (uint64_t)2);
                                            three++;
                                            //uint32_t four = prim_object_sub_mesh_indexes.at(v * (uint32_t)4 + (uint32_t)3);
                                            //four++;

                                            std::string one_string = util::uint32_t_to_string(one);
                                            std::string two_string = util::uint32_t_to_string(two);
                                            std::string three_string = util::uint32_t_to_string(three);
                                            //std::string four_string = util::uint32_t_to_string(four);

                                            obj_file_data.append("f ");
                                            obj_file_data.append(one_string);
                                            obj_file_data.push_back('/');
                                            obj_file_data.append(one_string);
                                            obj_file_data.push_back('/');
                                            obj_file_data.append(one_string);
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(two_string);
                                            obj_file_data.push_back('/');
                                            obj_file_data.append(two_string);
                                            obj_file_data.push_back('/');
                                            obj_file_data.append(two_string);
                                            obj_file_data.push_back(' ');
                                            obj_file_data.append(three_string);
                                            obj_file_data.push_back('/');
                                            obj_file_data.append(three_string);
                                            obj_file_data.push_back('/');
                                            obj_file_data.append(three_string);
                                            //obj_file_data.push_back(' ');
                                            //obj_file_data.append(four_string);
                                            //obj_file_data.push_back('/');
                                            //obj_file_data.append(four_string);
                                            //obj_file_data.push_back('/');
                                            //obj_file_data.append(four_string);
                                            obj_file_data.push_back('\n');
                                        }

                                        output_file.write(obj_file_data.c_str(), obj_file_data.length());

                                        output_file.close();
                                    }
                                    else
                                    {
                                        LOG_AND_EXIT_NOP("Error: PRIM_OBJECT_IS_NOT_A_MESH_TYPE");

                                        task_multiple_status = PRIM_OBJECT_IS_NOT_A_MESH_TYPE;

                                        return;
                                    }
                                }
                                else
                                {
                                    LOG_AND_EXIT_NOP("Error: PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1");

                                    task_multiple_status = PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1;

                                    return;
                                }
                            }
                            else
                            {
                                LOG_AND_EXIT_NOP("Error: PRIM_OBJECT_IS_NOT_A_MESH_TYPE");

                                task_multiple_status = PRIM_OBJECT_IS_NOT_A_MESH_TYPE;

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
    }
}