#pragma once

#include "global.h"
#include "console.h"
#include <string>
#include <vector>
#include <stdint.h>

struct map_node {
    uint64_t temp_hash = 0;
    std::string temp_hash_string = "";
    uint64_t entityId = 0;
    uint32_t subEntity = 0;
    std::string entryName = "";
    bool has_parent_map_node = false;
    std::vector<uint32_t> parent_map_nodes;
    bool is_top_level_root_node = false;
    bool is_top_level_Local_node = false;
    int32_t entityTypeResourceIndex = 0;
    uint64_t entityTypeResourceIndex_hash = 0;
    std::string entityTypeResourceIndex_hash_type = "";
    bool has_m_eidParent = false;
    bool has_m_eidParent_local = false;
    bool has_m_eidParent_external = false;
    uint64_t entityID = 0;
    int32_t externalSceneIndex = 0;
    uint64_t externalSceneIndex_hash = 0;
    int32_t entityIndex = 0;
    bool has_m_mTransform = false;
    bool use_m_mTransform = true;
    matrix43 m_mTransform;
    std::string m_mTransform_string = "";
    bool has_m_PrimitiveScale = false;
    vector3 m_PrimitiveScale;
    bool has_m_vGlobalSize = false;
    vector3 m_vGlobalSize;
    bool has_prim_resource = false;
    uint64_t prim_hash = 0;
    uint32_t map_prims_index = 0;
    std::string prim_glb_file = "";
    uint32_t m_IDHigh = 0;
    uint32_t m_IDLow = 0;
    bool is_volume_box = false;
    bool is_volume_sphere = false;
    std::string type = "";
    bool scaled_on_import = false;
    bool has_m_bVisible = false;
    bool m_bVisible = false;
};

class map {
public:
    map();

    map(uint64_t hash_value);

    void extract_map_prims(std::string output_path, bool textured);

    void scale_transform(matrix43& transform, float x, float y, float z);

    bool extract_scale_from_transform(matrix43& transform, vector3& scale);

    void generate_map_node_strings(uint32_t root_map_node, uint32_t parent_map_node, const std::string& map_node_string,
                                   uint32_t level);

    void get_map_node(temp& temp_temp);

    void get_root_scenes();

    void map_recursive_temp_loader(uint32_t rpkg_index, uint64_t hash_value);

    static void
    import_map(std::string& input_path, std::string& map_path, std::string& qn_format, std::string& output_path);

    static void
    export_map(std::string& input_path, std::string& filter, std::string& map_filter, std::string& output_path,
               bool textured);

    void add_to_temp_hash_map_node_map(uint32_t map_node, uint64_t temp_hash);

    void
    add_to_m_eidParent_local_temp_hash_entityIndex_map(uint32_t map_node, uint64_t temp_hash, uint32_t entityIndex);

    void add_to_m_eidParent_external_externalSceneIndex_hash_entityID_map(uint32_t map_node,
                                                                          uint64_t externalSceneIndex_hash,
                                                                          uint64_t entityID);

    void add_to_temp_hash_is_top_level_Local_node_map(uint32_t map_node, uint64_t temp_hash);

    bool matrixes_equal(matrix43& matrix1, matrix43& matrix2);

    std::vector<map_node> map_nodes;
    std::unordered_map<uint64_t, uint64_t> map_temps_map;
    std::vector<temp> map_temps;
    std::unordered_map<uint64_t, uint64_t> map_prims_map;
    std::vector<uint64_t> map_prims;
    std::string godot_tscn_file;
    std::unordered_map<uint64_t, uint64_t> root_scenes;
    std::vector<uint32_t> map_root_nodes;
    std::unordered_map<uint64_t, std::vector<uint32_t>> temp_hash_map_node_map;
    std::unordered_map<uint64_t, std::unordered_map<uint32_t, std::vector<uint32_t>>> m_eidParent_local_temp_hash_entityIndex_map;
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::vector<uint32_t>>> m_eidParent_external_externalSceneIndex_hash_entityID_map;
    std::unordered_map<uint64_t, std::vector<uint32_t>> temp_hash_is_top_level_Local_node_map;
    bool map_filter_volume_boxes = true;
    bool map_filter_volume_spheres = true;
    bool map_filter_visible = true;
    std::string qn_format = "entity";
    uint32_t map_node_count = 0;
    uint32_t map_node_count_current = 0;
    uint32_t map_node_prim_count = 0;
    uint32_t map_node_prim_count_current = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    int stringstream_length = 0;
    std::string message = "";
};