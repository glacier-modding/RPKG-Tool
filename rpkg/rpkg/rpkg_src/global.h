#pragma once
#include "rpkg.h"
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
    PRIM_OBJECT_IS_NOT_A_MESH_TYPE
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
extern std::vector<uint32_t> temp_entry_index;
extern std::vector<uint32_t> temp_logicalParent;
extern std::vector<uint32_t> temp_entityTypeResourceIndex;
extern std::vector<uint32_t> temp_propertyValues_start_offsets;
extern std::vector<uint32_t> temp_propertyValues_end_offsets;
extern std::vector<uint32_t> temp_postInitPropertyValues_start_offsets;
extern std::vector<uint32_t> temp_postInitPropertyValues_end_offsets;
extern std::vector<uint32_t> temp_platformSpecificPropertyValues_start_offsets;
extern std::vector<uint32_t> temp_platformSpecificPropertyValues_end_offsets;
extern std::vector<std::string> temp_property_types;
extern std::vector<std::vector<bool>> temp_property_types_shared;
extern std::vector<std::vector<uint32_t>> temp_property_types_shared_count;
extern std::vector<std::vector<uint32_t>> temp_property_types_offsets;
extern std::vector<std::vector<std::string>> temp_property_types_values;
extern std::vector<std::map<uint32_t, uint32_t>> temp_property_types_offsets_map;
extern std::vector<std::vector<uint32_t>> property_crc32_values;
extern std::vector<std::vector<uint32_t>> property_type_indexes;
extern std::vector<std::vector<uint32_t>> property_offsets;
extern std::vector<std::vector<uint32_t>> property_pointer_offsets;
extern std::vector<uint32_t> tblu_entry_index;
extern std::vector<uint32_t> tblu_logicalParent;
extern std::vector<uint32_t> tblu_entityTypeResourceIndex;
extern std::vector<uint64_t> tblu_entityId;
extern std::vector<uint32_t> tblu_editorOnly;
extern std::vector<std::string> tblu_entityName;
extern std::vector<char> temp_input_data;
extern std::vector<char> temp_output_data;
extern std::vector<char>* temp_data;
extern uint64_t temp_rpkg_index_1;
extern uint64_t temp_rpkg_index_2;
extern std::vector<char> tblu_input_data;
extern std::vector<char> tblu_output_data;
extern std::vector<char>* tblu_data;
extern uint64_t tblu_rpkg_index_1;
extern uint64_t tblu_rpkg_index_2;
extern std::vector<hash_depends_variables> temp_hash_depends_data;
extern std::vector<hash_depends_variables> tblu_hash_depends_data;