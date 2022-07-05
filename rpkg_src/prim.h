#pragma once
#include "global.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class prim
{
public:
	prim(uint64_t rpkgs_index, uint64_t hash_index);
    prim(std::string prim_file_path);

	void load_hash_depends();
    void extract_meta(std::string output_path);

    class prim_header
    {
    public:
        prim_header();
        prim_header(std::vector<char>& prim_data, uint32_t& prim_position);

        enum class header_type : uint16_t
        {
            NONE = 0,
            OBJECT = 1,
            MESH = 2,
            SHAPE = 5,
        };

        uint32_t offset = 0;
        uint8_t draw = 0;
        uint8_t pack = 0;
        uint16_t type = (uint16_t)header_type::NONE;
    };

    class prim_object_header
    {
    public:
        prim_object_header();
        prim_object_header(std::vector<char>& prim_data, uint32_t& prim_position);

        enum class PROPERTY_FLAGS : uint32_t
        {
            BONES = 0x01,
            FRAMES = 0x02,
            LINKED = 0x04,
            WEIGHTED = 0x08,
            BOUNDS = 0x0100,
            HIGH_RESOLUTION = 0x0200,
        };

        prim_header prim_header_instance;
        uint32_t property_flags = 0;
        bool flag_bones = false;
        bool flag_frames = false;
        bool flag_linked = false;
        bool flag_weighted = false;
        bool flag_bounds = false;
        bool flag_high_resolution = false;
        uint32_t borg_index = 0;
        uint32_t object_count = 0;
        uint32_t object_offset = 0;
        vector3 bounding_box_min;
        vector3 bounding_box_max;
    };

    class prim_object
    {
    public:
        prim_object();
        prim_object(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number);

        enum class SUBTYPE : char {
            STANDARD = 0,
            LINKED = 1,
            WEIGHTED = 2,
            STANDARDUV2 = 3,
            STANDARDUV3 = 4,
            STANDARDUV4 = 5,
            SPEEDTREE = 6,
        };

        enum class PROPERTY_FLAGS : char {
            NONE = 0,
            X_AXIS_LOCKED = 0x01,
            Y_AXIS_LOCKED = 0x02,
            Z_AXIS_LOCKED = 0x04,
            HIGH_RESOLUTION = 0x08,
            PS3 = 0x10,
            COLOR1 = 0x20,
            NO_PHYSICS = 0x40,
        };

        prim_header prim_header_instance;
        uint32_t object = 0;
        uint8_t sub_type = 0;
        uint8_t property_flags = 0;
        uint8_t lod = 0;
        uint8_t variant = 0;
        uint8_t bias = 0;
        uint8_t offset = 0;
        uint16_t material_id = 0;
        uint32_t wire_color = 0;
        uint32_t color = 0;
        vector3 bounding_box_min;
        vector3 bounding_box_max;
    };

    class prim_mesh
    {
    public:
        prim_mesh();
        prim_mesh(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number);

        enum class CLOTH_FLAGS : uint32_t
        {
            SMALL = 0x80,
        };

        prim_object prim_object_instance;
        uint32_t sub_mesh_table_offset_pointer = 0;
        uint32_t sub_mesh_table_offset = 0;
        vector4 vertex_position_scale;
        vector4 vertex_position_bias;
        vector2 uv_position_scale;
        vector2 uv_position_bias;
        uint32_t cloth_flags = 0;
    };

    class prim_weighted_mesh
    {
    public:
        prim_weighted_mesh();
        prim_weighted_mesh(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number);

        prim_mesh prim_mesh_instance;
        uint32_t bones_nodes_count = 0;
        uint32_t bones_nodes_offset = 0;
        uint32_t bone_indices_offset = 0;
        uint32_t bone_info_offset = 0;
    };

    class prim_sub_mesh
    {
    public:
        prim_sub_mesh();
        prim_sub_mesh(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number, prim_mesh& temp_prim_mesh, std::vector<char>& meta_data, bool flag_high_resolution, bool& success);

        prim_object prim_object_instance;
        uint32_t vertex_count = 0;
        uint32_t vertex_buffer_offset = 0;
        uint32_t index_count = 0;
        uint32_t index_count_additional = 0;
        uint32_t index_buffer_offset = 0;
        uint32_t collision_offset = 0;
        uint32_t cloth_offset = 0;
        uint32_t uv_channel_count = 0;
        uint32_t unknown_offset = 0;
        std::vector<std::string> names;
        std::vector<uint16_t> indexes;
        std::vector<float> vertexes;
        std::vector<float> vertexes_weighted_weights_0;
        std::vector<float> vertexes_weighted_weights_1;
        std::vector<uint8_t> vertexes_weighted_bone_ids_0;
        std::vector<uint8_t> vertexes_weighted_bone_ids_1;
        std::vector<float> vertexes_normals;
        std::vector<float> vertexes_tangents;
        std::vector<float> vertexes_bitangents;
        std::vector<float> vertexes_uvs;
        std::vector<uint8_t> vertexes_colors;
        std::vector<char> cloth_data;
        std::vector<char> collision_data;
        std::vector<uint32_t> bones_nodes_data;
        std::vector<uint8_t> bones_info_data;
        std::vector<uint16_t> bones_indices_data;
    };

    class prim_weighted_sub_mesh
    {
    public:
        prim_weighted_sub_mesh();
        prim_weighted_sub_mesh(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number, prim_weighted_mesh& temp_prim_mesh, std::vector<char>& meta_data, bool flag_high_resolution, bool& success);

        prim_object prim_object_instance;
        uint32_t vertex_count = 0;
        uint32_t vertex_buffer_offset = 0;
        uint32_t index_count = 0;
        uint32_t index_count_additional = 0;
        uint32_t index_buffer_offset = 0;
        uint32_t collision_offset = 0;
        uint32_t cloth_offset = 0;
        uint32_t uv_channel_count = 0;
        uint32_t unknown_offset = 0;
        std::vector<std::string> names;
        std::vector<uint16_t> indexes;
        std::vector<float> vertexes;
        std::vector<float> vertexes_weighted_weights_0;
        std::vector<float> vertexes_weighted_weights_1;
        std::vector<uint8_t> vertexes_weighted_bone_ids_0;
        std::vector<uint8_t> vertexes_weighted_bone_ids_1;
        std::vector<float> vertexes_normals;
        std::vector<float> vertexes_tangents;
        std::vector<float> vertexes_bitangents;
        std::vector<float> vertexes_uvs;
        std::vector<uint8_t> vertexes_colors;
        std::vector<char> cloth_data;
        std::vector<char> collision_data;
        std::vector<uint32_t> bones_nodes_data;
        std::vector<uint8_t> bones_info_data;
        std::vector<uint16_t> bones_indices_data;
    };

	std::string prim_file_name = "";
	uint32_t prim_rpkg_index = 0;
	uint32_t prim_hash_index = 0;
	std::vector<char> prim_input_data;
	std::vector<char> prim_output_data;
	std::vector<char> prim_data;
	std::vector<std::string> mati_depends_file_name;
	std::vector<std::string> mati_depends_hash_string;
	std::vector<uint64_t> mati_depends_hash_value;
	std::vector<uint32_t> mati_depends_index;
	std::vector<std::vector<std::string>> mati_depends_in_rpkgs;
	std::vector<uint32_t> mati_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> mati_depends_rpkg_index;
	std::vector<uint32_t> mati_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> mati_depends_hash_index;
	std::vector<uint32_t> mati_depends_hash_index_index;
	std::vector<std::string> borg_depends_file_name;
	std::vector<std::string> borg_depends_hash_string;
	std::vector<uint64_t> borg_depends_hash_value;
	std::vector<uint32_t> borg_depends_index;
	std::vector<std::vector<std::string>> borg_depends_in_rpkgs;
	std::vector<uint32_t> borg_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> borg_depends_rpkg_index;
	std::vector<uint32_t> borg_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> borg_depends_hash_index;
	std::vector<uint32_t> borg_depends_hash_index_index;
    std::map<std::string, uint32_t> borg_bone_name_map;
    uint32_t prim_primary_header_offset = 0;
    std::vector<uint32_t> prim_object_offsets;
    std::vector<bool> prim_object_is_weighted;
    std::vector<prim_object_header> prim_object_headers;
    std::vector<prim_mesh> prim_meshes;
    std::vector<prim_weighted_mesh> prim_weighted_meshes;
    std::vector<prim_sub_mesh> prim_sub_meshes;
    std::vector<prim_weighted_sub_mesh> prim_weighted_sub_meshes;
    std::vector<char> prim_meta_data;
    std::string rpkg_output_file = "";
    bool success = true;
    asset3ds asset3ds_data;
};