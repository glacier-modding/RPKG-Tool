#pragma once
#include "rpkg.h"
#include "temp.h"
#include <string>
#include <vector>
#include <stdint.h>

#ifdef RPKG_CLI
#define LOG(x) std::cout << x << std::endl
#define LOG_NO_ENDL(x) std::cout << x
#define LOG_AND_EXIT(x) std::cout << x << std::endl; if (interative_mode) { return; } else std::exit(0)
#define LOG_AND_RETURN(x) std::cout << x << std::endl; return
#define LOG_AND_EXIT_NOP(x) std::cout << x << std::endl; if (!interative_mode) std::exit(0)
#elif RPKG_DLL
#define LOG(x)
#define LOG_NO_ENDL(x)
#define LOG_AND_EXIT(x) task_status_string = x; task_single_status = TASK_EXITED; task_multiple_status = TASK_EXITED; return
#define LOG_AND_RETURN(x) task_status_string = x; task_single_status = TASK_RETURNED; task_multiple_status = TASK_RETURNED; return
#define LOG_AND_EXIT_NOP(x)
#endif

enum RPKGStatus
{
    READY,
    TASK_EXECUTING,
    TASK_SUCCESSFUL,
    TASK_EXITED,
    TASK_RETURNED,
    RPKG_IMPORTING,
    RPKG_IMPORTED,
    RPKG_ALREADY_IMPORTED,
    SCANNING_DIRECTORY,
    SCANNING_DIRECTORY_DONE,
    ABORT_CURRENT_TASK,
    TEMP_TBLU_FOUND,
    TEMP_TBLU_NOT_FOUND_IN_DEPENDS,
    TEMP_TBLU_NOT_FOUND_IN_RPKG,
    TEMP_TBLU_TOO_MANY,
    TEMP_HEADER_NOT_FOUND,
    TEMP_TBLU_ENTRY_COUNT_MISMATCH,
    PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1,
    PRIM_OBJECT_IS_NOT_A_MESH_TYPE,
    TEMP_VERSION_UNKNOWN,
    TBLU_VERSION_UNKNOWN,
    PRIM_REBUILD_GLB_MESH_NAME_MALFORMED,
    PRIM_REBUILD_ONLY_ONE_MESH_ALLOWED,
    PRIM_REBUILD_VERTEX_NOT_MULTIPLE_OF_3,
    PRIM_REBUILD_MISSING_POSITION_DATA,
    PRIM_REBUILD_MISMATCHED_BONES,
    PRIM_REBUILD_WEIGHTED_DATA_DOES_NOT_CONFORM,
    PRIM_REBUILD_WEIGHTED_DATA_MISSING,
    PRIM_REBUILD_NORMALS_DO_NOT_MATCH_VERTICES,
    PRIM_REBUILD_MISSING_NORMAL_DATA,
    PRIM_REBUILD_UVS_DO_NOT_MATCH_VERTICES,
    PRIM_REBUILD_MISSING_UV_DATA,
    PRIM_REBUILD_COLORS_DO_NOT_MATCH_VERTICES,
    PRIM_REBUILD_COLORS_WRONG_FORMAT,
    PRIM_REBUILD_TOO_MANY_PRIMARY_OBJECT_HEADERS,
    PRIM_REBUILD_META_FILE_MISSING,
    PRIM_REBUILD_SUCCESSFUL,
    PRIM_MODEL_REBUILD_SUCCESSFUL,
    PRIM_MODEL_EXTRACT_SUCCESSFUL
};

enum FileType3D
{
    GLB_MULTIPLE,
    GLB_SINGLE,
    GLTF_MULTIPLE,
    GLTF_SINGLE,
    OBJ_MULTIPLE,
    OBJ_SINGLE
};

enum Types
{
    TYPE_INT8,
    TYPE_UINT8,
    TYPE_INT16,
    TYPE_UINT16,
    TYPE_INT32,
    TYPE_UINT32,
    TYPE_INT64,
    TYPE_UINT64,
    TYPE_FLOAT32,
    TYPE_FLOAT64,
    TYPE_BOOL,
    TYPE_ZSTRING,
    TYPE_ZVARIANT,
    TYPE_ENUM
};

extern std::string DirectXFormatsStrings[116];

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

struct vector6
{
    float a = 0;
    float b = 0;
    float c = 0;
    float d = 0;
    float e = 0;
    float f = 0;
};

struct rgba
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

struct uint8_t_6
{
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t d = 0;
    uint8_t e = 0;
    uint8_t f = 0;
};

struct matrix42
{
    vector4 x;
    vector4 y;
};

struct matrix43
{
    vector3 x_axis;
    vector3 y_axis;
    vector3 z_axis;
    vector3 transform;
};

struct matrix44
{
    vector4 x;
    vector4 y;
    vector4 z;
    vector4 w;
};

struct bone_data
{
    vector3 position;
    uint32_t parent_id;
    vector3 size;
    std::string name;
    uint16_t part;
};

struct asset3d
{
    std::vector<uint16_t>* indexes;
    std::vector<vector4>* vertexes;
    std::vector<vector4>* normals;
    std::vector<vector4>* tangents;
    std::vector<vector2>* uvs;
    std::vector<rgba>* colors;
};

struct asset3ds
{
    std::string root_name = "";
    std::vector<std::string> names;
    std::vector<std::vector<uint16_t>> indexes;
    std::vector<std::vector<float>> vertexes;
    std::vector<std::vector<float>> normals;
    std::vector<std::vector<float>> tangents;
    std::vector<std::vector<float>> uvs;
    std::vector<std::vector<uint8_t>> colors;
    std::vector<bool> weighted;
    std::vector<std::vector<float>> vertexes_weighted_weights_0;
    std::vector<std::vector<float>> vertexes_weighted_weights_1;
    std::vector<std::vector<uint8_t>> vertexes_weighted_bone_ids_0;
    std::vector<std::vector<uint8_t>> vertexes_weighted_bone_ids_1;
    std::vector<std::vector<char>> cloths;
    std::vector<std::vector<char>> collisions;
    std::vector<std::vector<uint32_t>> bones_nodes;
    std::vector<std::vector<uint8_t>> bones_infos;
    std::vector<std::vector<uint16_t>> bones_indices;
    std::vector<bone_data> bones_data;
    std::vector<float> bones_positions;
    std::vector<float> bones_inverse_matrices;
};

extern std::vector<rpkg> rpkgs;
extern int percent_progress;
extern std::string response_string;
extern std::vector<char> response_data;
extern std::string timing_string;
extern std::string task_status_string;
extern int task_single_status;
extern int task_multiple_status;
extern int gui_control;
extern std::string localization_string;
extern std::string hashes_with_no_reverse_depends;
extern std::string hash_direct_depends;
extern std::string hash_list_string;
extern std::string patch_deletion_list_string;
extern std::vector<std::string> hashes_depends_map_rpkg_file_paths;
extern std::vector<std::map<uint64_t, uint64_t>> hashes_depends_map;
extern int hash_list_version;
extern bool hash_list_loaded;
extern std::map<uint64_t, uint64_t> hash_list_hash_map;
extern std::vector<std::string> hash_list_hash_file_names;
extern std::vector<std::string> hash_list_hash_value_strings;
extern std::vector<std::string> hash_list_hash_strings;
extern std::string search_imported_hashes_string;
extern std::string search_hash_list_string;
extern std::string rpkg_file_paths_hash_is_in;
extern int pcm_sample_size;
extern int pcm_sample_rate;
extern int pcm_channels;
extern std::string extracted_wem_string;
extern std::map<uint32_t, std::string>* property_map;
extern std::map<std::string, uint32_t>* type_map;
extern std::map<std::string, std::map<uint32_t, std::string>>* enum_map;
extern void initialize_enum_map();
extern void initialize_type_map();
extern bool resource_tool_initialized;
extern bool property_map_initialized;
extern bool enum_map_initialized;
extern bool type_map_initialized;
extern std::vector<temp> temps;
extern std::map<uint64_t, uint32_t> temps_map;
extern std::vector<matrix43> temp_world_coordinates;
extern std::vector<std::string> temp_world_coordinates_property_names;
extern std::map<uint32_t, uint32_t> temp_world_coordinates_map;
extern std::vector<std::string> prim_asset_file_names;
extern uint32_t prim_asset_file_count;
extern std::string map_editor_output_path;
extern std::vector<std::string> map_editor_parents;
extern std::vector<std::string> map_editor_property_names;
extern std::vector<std::string> map_editor_matrixes;
extern std::vector<std::vector<std::string>> map_editor_glb_file_names;
extern std::map<std::string, uint32_t> map_editor_prim_file_names;
extern std::map<std::string, uint32_t> map_editor_resource_map;
extern std::string map_editor_godot_resources;
extern std::string map_editor_godot_nodes;
extern std::string map_editor_temp_matrix;
extern std::vector<std::string> map_editor_godot_import_property_strings;
extern std::map<std::string, uint32_t> map_editor_godot_import_property_strings_map;
extern std::vector<matrix43> map_editor_godot_import_matrixes;
extern std::vector<uint64_t> entity_id_master_list;
extern std::map<uint64_t, uint64_t> entity_id_master_list_map;
extern std::vector<uint32_t> entity_id_tblu_index;
extern std::vector<uint32_t> entity_id_temps_index;
extern std::vector<uint32_t> entity_id_logicalParent;
extern std::vector<std::string> entity_id_logicalParent_string;
extern std::vector<uint64_t> entity_id_m_eidParent;
extern std::vector<bool> entity_id_has_matrix;
extern bool interative_mode;
extern void* resource_tool_dll;
typedef int(__cdecl* ConvertMemoryResourceToJson)(char*, void*, uint64_t);
typedef int(__cdecl* ConvertMemoryJsonToResource)(char*, const char*, size_t, char*);
typedef char* (__cdecl* GetJsonFromMemory)();
extern ConvertMemoryResourceToJson resource_tool_ConvertMemoryResourceToJson;
extern ConvertMemoryJsonToResource resource_tool_ConvertMemoryJsonToResource;
extern GetJsonFromMemory resource_tool_GetJsonFromMemory;