#pragma once
#include "rpkg.h"
#include "temp.h"
#include <string>
#include <vector>
#include <stdint.h>

#ifdef RPKG_CLI
#define LOG(x) std::cout << x << std::endl
#define LOG_NO_ENDL(x) std::cout << x
#define LOG_AND_EXIT(x) std::cout << x << std::endl; std::exit(0)
#define LOG_AND_RETURN(x) std::cout << x << std::endl; return
#define LOG_AND_EXIT_NOP(x) std::cout << x << std::endl; std::exit(0)
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
    TBLU_VERSION_UNKNOWN
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

struct matrix43
{
    vector3 x_axis;
    vector3 y_axis;
    vector3 z_axis;
    vector3 transform;
};

struct asset3d
{
    std::vector<uint16_t>* indexes;
    std::vector<vector4>* vertexes;
    std::vector<vector4>* normals;
    std::vector<vector2>* uvs;
    std::vector<rgba>* colors;
};

struct asset3ds
{
    std::vector<std::vector<uint16_t>>* indexes;
    std::vector<std::vector<vector4>>* vertexes;
    std::vector<std::vector<vector4>>* normals;
    std::vector<std::vector<vector2>>* uvs;
    std::vector<std::vector<rgba>>* colors;
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
extern std::map<std::string, std::map<uint32_t, std::string>>* enum_map;
extern void initialize_property_map();
extern void initialize_enum_map();
extern bool property_map_initialized;
extern bool enum_map_initialized;
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
//extern void find_all_world_coordinates(std::string entry_name, uint32_t entry_index, matrix43 matrix43_value);
//extern void extract_world_coordinates_and_gltfs(std::string entry_name, uint32_t entry_index, matrix43 matrix43_value, std::string& output_path, std::ofstream& output_file);
//extern void extract_glbs_and_return_coordinates_for_entry(uint32_t entry_index);
//extern void extract_glb_and_return_coords(uint32_t entry_index, std::string parent_string);
//extern void get_entry_name_string(uint32_t entry_index);