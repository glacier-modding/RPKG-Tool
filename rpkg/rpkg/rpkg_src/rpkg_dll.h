#pragma once

#include <stdint.h>

#define RPKG_EXPORT extern "C" __declspec(dllexport)

RPKG_EXPORT int load_hash_list();

RPKG_EXPORT char* get_hash_list_string(char* hash_string);

RPKG_EXPORT int task_execute(char* command, char* input_path, char* filter, char* search, char* search_type, char* output_path);

RPKG_EXPORT int get_current_percent();

RPKG_EXPORT int reset_task_status();

RPKG_EXPORT int get_task_single_status();

RPKG_EXPORT int reset_task_single_status();

RPKG_EXPORT int get_task_multiple_status();

RPKG_EXPORT int reset_task_multiple_status();

RPKG_EXPORT int set_gui_control(int gui_control_value);

RPKG_EXPORT int get_gui_control();

RPKG_EXPORT char* get_timing_string();

RPKG_EXPORT char* get_task_status_string();

RPKG_EXPORT uint32_t get_resource_types_count(char* rpkg_file);

RPKG_EXPORT char* get_resource_types_at(char* rpkg_file, uint32_t at_index);

RPKG_EXPORT uint64_t get_resource_types_data_size(char* rpkg_file, char* resource_type);

RPKG_EXPORT uint32_t get_hash_based_on_resource_type_count(char* rpkg_file, char* resource_type);

RPKG_EXPORT char* get_hash_based_on_resource_type_at(char* rpkg_file, char* resource_type, uint32_t at_index);

RPKG_EXPORT uint32_t get_all_hashes_in_rpkg_count(char* rpkg_file);

RPKG_EXPORT char* get_all_hashes_in_rpkg_at(char* rpkg_file, uint32_t at_index);

RPKG_EXPORT char* get_hash_details(char* rpkg_file, char* hash_string);

RPKG_EXPORT uint32_t get_hash_in_rpkg_size(char* rpkg_file, char* hash_string);

RPKG_EXPORT char* get_hash_in_rpkg_data(char* rpkg_file, char* hash_string);

RPKG_EXPORT uint64_t get_all_hashes_in_rpkg_data_size(char* rpkg_file);

RPKG_EXPORT int clear_hash_data_vector();

RPKG_EXPORT char* get_hash_in_rpkg_data_in_hex_view(char* rpkg_file, char* hash_string);

RPKG_EXPORT uint32_t generate_localization_string(char* rpkg_file, char* hash_string, char* resource_type);

RPKG_EXPORT char* get_localization_string();

RPKG_EXPORT int get_hashes_with_no_reverse_depends(char* rpkg_file);

RPKG_EXPORT char* get_hashes_with_no_reverse_depends_string();

RPKG_EXPORT int get_direct_hash_depends(char* rpkg_file, char* hash_string);

RPKG_EXPORT char* get_direct_hash_depends_string();

RPKG_EXPORT char* get_patch_deletion_list(char* rpkg_file);

RPKG_EXPORT int search_imported_hashes(char* search_str, char* rpkg_file, char* resource_type, int max_results);

RPKG_EXPORT char* get_search_imported_hashes();

RPKG_EXPORT int search_hash_list(char* search_str, int max_results);

RPKG_EXPORT char* get_search_hash_list();

RPKG_EXPORT char* get_rpkg_file_paths_hash_is_in(char* hash_string);