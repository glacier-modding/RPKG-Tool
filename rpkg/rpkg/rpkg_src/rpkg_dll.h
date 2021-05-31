#pragma once

#include <stdint.h>
#include <string>

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

RPKG_EXPORT uint32_t generate_json_string(char* rpkg_file, char* hash_string);

RPKG_EXPORT char* get_json_string();

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

RPKG_EXPORT int create_ogg_file_from_hash_in_rpkg(char* rpkg_file, char* hash_string, int command, int wwev_number);

RPKG_EXPORT int convert_ogg_to_pcm(char* input_path, char* output_path);

RPKG_EXPORT int get_pcm_sample_size();

RPKG_EXPORT int get_pcm_sample_rate();

RPKG_EXPORT int get_pcm_channels();

RPKG_EXPORT int clear_temp_tblu_data();

RPKG_EXPORT int get_entries_with_logical_parent(uint32_t temps_index, uint32_t logical_parent);

RPKG_EXPORT char* get_entries_with_logical_parent_data();

RPKG_EXPORT int get_temp_entries(uint32_t temps_index, char* value_string, char* value_type);

RPKG_EXPORT int get_entries(uint32_t temps_index, uint32_t entry_index, char* value_type);

RPKG_EXPORT char* get_entries_data();

RPKG_EXPORT char* get_entries_hash_reference_data(uint32_t temps_index, uint32_t entry_index);

RPKG_EXPORT char* get_entries_hash_references(uint32_t temps_index, uint32_t entry_index);

RPKG_EXPORT int update_temp_file(uint32_t temps_index, uint32_t entry_index, char* update_data, uint32_t update_data_size);

RPKG_EXPORT int generate_temp_file_from_data(uint32_t temps_index, char* temp_file_path);

RPKG_EXPORT char* get_enum_values(uint32_t temps_index, char* property_type);

RPKG_EXPORT char* get_prim_from_temp(uint32_t temps_index, uint32_t entry_index);

RPKG_EXPORT int get_temp_index(char* temp_hash_string);

RPKG_EXPORT int load_recursive_temps(char* temp_hash, char* rpkg_file_path, uint32_t temp_version);

RPKG_EXPORT int load_non_recursive_temps(char* temp_hash, char* rpkg_file_path, uint32_t temp_version);

RPKG_EXPORT int get_number_of_changed_temps();

RPKG_EXPORT char* get_changed_temp_data(uint32_t temp_changed_index);

RPKG_EXPORT int generate_temp_files_from_data(char* temp_path);

RPKG_EXPORT int get_total_numer_of_temps();

RPKG_EXPORT int generate_rpkg_files_from_data(char* temp_path);

RPKG_EXPORT char* get_top_level_logical_parents(uint32_t temps_index);

RPKG_EXPORT char* get_all_bricks(uint32_t temps_index);

RPKG_EXPORT int search_temp_files(uint32_t temps_index, char* search_str, int max_results);

RPKG_EXPORT char* get_search_temp_files();

RPKG_EXPORT int generate_png_from_text(char* rpkg_file, char* hash_string, char* png_path);

RPKG_EXPORT int unload_rpkg(char* rpkg_file);

RPKG_EXPORT int modify_patch_deletion_list(char* rpkg_file, char* patch_list, uint32_t patch_count, uint32_t backup_rpkg);

RPKG_EXPORT int modify_hash_depends(char* rpkg_file, char* hash_string, char* hash_list, char* hash_flag_list, uint32_t hash_count, uint32_t backup_rpkg);

RPKG_EXPORT char* get_response_string();

RPKG_EXPORT int get_temp_version(char* temp_hash, char* rpkg_file_path);

RPKG_EXPORT int set_temp_version(uint32_t temps_index, uint32_t temp_version);

RPKG_EXPORT int import_rpkgs(char* rpkgs_path, char* rpkgs_list);

RPKG_EXPORT int is_rpkg_valid(char* rpkg_file_path);

RPKG_EXPORT int search_hash_list_by_hash_value(char* hash_value);

RPKG_EXPORT int get_temp_subentity_count(uint32_t temps_index);

RPKG_EXPORT int get_tblu_subentity_count(uint32_t temps_index);

RPKG_EXPORT char* get_entry_name(uint32_t temp_index, int entry_index);

RPKG_EXPORT int generate_json_files_from_data(char* temp_path);