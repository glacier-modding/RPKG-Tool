#pragma once

#include <cstdint>

#define RPKG_EXPORT extern "C" __declspec(dllexport)

RPKG_EXPORT int load_hash_list(char* path);

RPKG_EXPORT uint32_t load_hmla_hash_list(char* path);

RPKG_EXPORT char* get_hash_list_string(char* hash_string);

RPKG_EXPORT int
task_execute(char* command, char* input_path, char* filter, char* search, char* search_type, char* output_path);

RPKG_EXPORT int get_current_percent();

RPKG_EXPORT int reset_task_status();

RPKG_EXPORT int get_task_single_status();

RPKG_EXPORT int reset_task_single_status();

RPKG_EXPORT int get_task_multiple_status();

RPKG_EXPORT int reset_task_multiple_status();

RPKG_EXPORT int get_task_map_status();

RPKG_EXPORT int reset_task_map_status();

RPKG_EXPORT int get_map_percent_progress_recursive_temp();

RPKG_EXPORT int get_map_percent_progress_map_nodes();

RPKG_EXPORT int get_map_percent_progress_map_nodes_prim();

RPKG_EXPORT int get_map_percent_progress_godot_files();

RPKG_EXPORT int get_map_percent_progress_godot_map_nodes();

RPKG_EXPORT int get_map_percent_progress_map_node_temp_loading();

RPKG_EXPORT int get_map_percent_progress_map_node_changes_check();

RPKG_EXPORT int get_map_percent_progress_map_writing_changes_to_qn();

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

RPKG_EXPORT char* get_hash_details(char* rpkg_file, char* hash_string);

RPKG_EXPORT uint32_t get_hash_in_rpkg_size(char* rpkg_file, char* hash_string);

RPKG_EXPORT char* get_hash_in_rpkg_data(char* rpkg_file, char* hash_string);

RPKG_EXPORT uint64_t get_all_hashes_in_rpkg_data_size(char* rpkg_file);

RPKG_EXPORT int clear_hash_data_vector();

RPKG_EXPORT char* get_hash_in_rpkg_data_in_hex_view(char* rpkg_file, char* hash_string);

RPKG_EXPORT uint32_t generate_localization_string(char* rpkg_file, char* hash_string, char* resource_type);

RPKG_EXPORT uint32_t generate_localization_line_string(char* rpkg_file, char* hash_string, char* resource_type);

RPKG_EXPORT char* get_localization_string();

RPKG_EXPORT char* get_localization_line_string();

RPKG_EXPORT uint32_t generate_json_string(char* rpkg_file, char* hash_string);

RPKG_EXPORT char* get_json_string();

RPKG_EXPORT int get_hashes_with_no_reverse_depends(char* rpkg_file);

RPKG_EXPORT int get_direct_hash_depends(char* rpkg_file, char* hash_string);

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

RPKG_EXPORT char* get_entries_hash_reference_data(uint32_t temps_index, uint32_t entry_index);

RPKG_EXPORT char* get_entries_hash_references(uint32_t temps_index, uint32_t entry_index);

RPKG_EXPORT char* get_enum_values(uint32_t temps_index, char* property_type);

RPKG_EXPORT int get_temp_index(char* temp_hash_string);

RPKG_EXPORT int load_recursive_temps(char* temp_hash, char* rpkg_file_path, uint32_t temp_version);

RPKG_EXPORT int load_non_recursive_temps(char* temp_hash, char* rpkg_file_path, uint32_t temp_version);

RPKG_EXPORT char* get_top_level_logical_parents(uint32_t temps_index);

RPKG_EXPORT char* get_all_bricks(uint32_t temps_index);

RPKG_EXPORT int get_temp_version(char* temp_hash, char* rpkg_file_path);

RPKG_EXPORT int generate_png_from_text(char* rpkg_file, char* hash_string, char* png_path);

RPKG_EXPORT int unload_rpkg(char* rpkg_file);

RPKG_EXPORT int
modify_patch_deletion_list(char* rpkg_file, char* patch_list, uint32_t patch_count, uint32_t backup_rpkg);

RPKG_EXPORT int
modify_hash_depends(char* rpkg_file, char* hash_string, char* hash_list, char* hash_flag_list, uint32_t hash_count,
                    uint32_t backup_rpkg);

RPKG_EXPORT char* get_response_string();

RPKG_EXPORT int import_rpkgs(char* rpkgs_path, char* rpkgs_list);

RPKG_EXPORT int is_rpkg_valid(char* rpkg_file_path);

RPKG_EXPORT int get_hash_name_from_hash_value(uint64_t hash_value, char* hash_name);

RPKG_EXPORT int get_temp_subentity_count(uint32_t temps_index);

RPKG_EXPORT int get_tblu_subentity_count(uint32_t temps_index);

RPKG_EXPORT char* get_entry_name(uint32_t temp_index, int entry_index);

RPKG_EXPORT int
deep_search_localization(char* input_path, char* search_value, int search_dlge, int search_locr, int search_rtlv,
                         int max_results, char* version);

RPKG_EXPORT uint32_t get_localization_search_results_size();

RPKG_EXPORT char* get_localization_search_results();

RPKG_EXPORT int
deep_search_entities(char* input_path,
                     char** search_strings,
                     int* search_types,
                     int* search_categories,
                     int search_strings_count,
                     int max_results,
                     bool store_jsons,
                     bool use_latest_hashes);

RPKG_EXPORT uint32_t get_entities_search_results_size();

RPKG_EXPORT char* get_entities_search_results();

RPKG_EXPORT uint32_t is_repo_loaded();

RPKG_EXPORT uint32_t is_ores_loaded();

RPKG_EXPORT int load_repo();

RPKG_EXPORT int load_repo_from_file(char* repo_path);

RPKG_EXPORT int reset_repos();

RPKG_EXPORT uint32_t get_repo_response_data_size();

RPKG_EXPORT char* get_repo_response_data();

RPKG_EXPORT int get_repo_child_entries(char* id);

RPKG_EXPORT int get_repo_category(int category);

RPKG_EXPORT int get_repo_json(char* id);

RPKG_EXPORT char* get_repo_image_hash(char* id);

RPKG_EXPORT char* get_latest_hash_rpkg_path(char* hash);

RPKG_EXPORT int is_valid_json(const char* json);

RPKG_EXPORT char* check_json(const char* json);

RPKG_EXPORT int create_patch(const char* patch_path);

RPKG_EXPORT int import_patch(const char* patch_path);

RPKG_EXPORT int save_json(const char* id, const char* json);

RPKG_EXPORT int duplicate_repo_entry(const char* id);

RPKG_EXPORT int erase_repo_entry(const char* id);

RPKG_EXPORT int get_repo_entry(const char* id);

RPKG_EXPORT int update_json_at_pointer(const char* id, const char* json_pointer, const char* value);

RPKG_EXPORT int is_valid_regex(const char* regex_string);

RPKG_EXPORT int load_ioi_treeview();

RPKG_EXPORT int get_ioi_treeview_children(char* parent);

RPKG_EXPORT char* get_ioi_treeview();

RPKG_EXPORT int get_ioi_treeview_size();

RPKG_EXPORT char* get_prim_lod_data();

RPKG_EXPORT int set_prim_lods_to_export(int* prim_lods_array, int prim_lods_array_count);

RPKG_EXPORT int reset_prim_lods_to_export();