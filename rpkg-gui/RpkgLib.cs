using System;
using System.Runtime.InteropServices;

namespace rpkg
{
    public static class RpkgLib
    {
        public delegate int execute_import_rpkgs(string rpkgs_path, string rpkgs_list);

        public delegate int execute_load_recursive_temps(string temp_hash, string rpkg_file_path, UInt32 temp_version);

        public delegate int execute_get_hashes_with_no_reverse_depends(string rpkg_file);

        public delegate int execute_get_direct_hash_depends(string rpkg_file, string hash_string);

        public delegate int execute_task(string csharp_command, string csharp_input_path, string csharp_filter,
            string search, string search_type, string csharp_output_path);

        public delegate int execute_generate_rpkg_files_from_data(string outputFolder);

        public delegate int execute_export_map_data_to_folder(UInt32 temps_index, string map_name, string output_path);

        [DllImport("rpkg.dll", EntryPoint = "task_execute", CallingConvention = CallingConvention.Cdecl)]
        public static extern int task_execute(string csharp_command, string csharp_input_path, string csharp_filter,
            string search, string search_type, string csharp_output_path);

        [DllImport("rpkg.dll", EntryPoint = "get_current_percent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_current_percent();

        [DllImport("rpkg.dll", EntryPoint = "reset_task_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_status();

        [DllImport("rpkg.dll", EntryPoint = "reset_task_single_status", CallingConvention = CallingConvention.Cdecl)]
        public static extern int reset_task_single_status();

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_resource_types_count(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_resource_types_at(string rpkg_file, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_resource_types_data_size",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 get_resource_types_data_size(string rpkg_file, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_based_on_resource_type_count",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_hash_based_on_resource_type_count(string rpkg_file, string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_based_on_resource_type_at",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_based_on_resource_type_at(string rpkg_file, string resource_type,
            UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_count",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_all_hashes_in_rpkg_count(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_at", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_hashes_in_rpkg_at(string rpkg_file, UInt32 at_index);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_details", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_details(string rpkg_file, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 get_hash_in_rpkg_size(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_in_rpkg_data(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_all_hashes_in_rpkg_data_size",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 get_all_hashes_in_rpkg_data_size(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "clear_hash_data_vector", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_hash_data_vector();

        [DllImport("rpkg.dll", EntryPoint = "get_hash_in_rpkg_data_in_hex_view",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_in_rpkg_data_in_hex_view(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "generate_localization_string",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 generate_localization_string(string rpkg_file_name, string hash_string,
            string resource_type);

        [DllImport("rpkg.dll", EntryPoint = "get_localization_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_localization_string();

        [DllImport("rpkg.dll", EntryPoint = "generate_json_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 generate_json_string(string rpkg_file_name, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_json_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_json_string();

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hashes_with_no_reverse_depends();

        [DllImport("rpkg.dll", EntryPoint = "load_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int load_hash_list();

        [DllImport("rpkg.dll", EntryPoint = "get_hash_list_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hash_list_string(string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_hashes_with_no_reverse_depends(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "get_hashes_with_no_reverse_depends_string",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_hashes_with_no_reverse_depends_string();

        [DllImport("rpkg.dll", EntryPoint = "get_direct_hash_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_direct_hash_depends(string rpkg_file, string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_direct_hash_depends_string",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_direct_hash_depends_string();

        [DllImport("rpkg.dll", EntryPoint = "get_patch_deletion_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_patch_deletion_list(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "search_imported_hashes", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_imported_hashes(string search_str, string rpkg_file, string resource_type,
            int max_results);

        [DllImport("rpkg.dll", EntryPoint = "get_search_imported_hashes", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_search_imported_hashes();

        [DllImport("rpkg.dll", EntryPoint = "search_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_hash_list(string search_str, int max_results);

        [DllImport("rpkg.dll", EntryPoint = "get_search_hash_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_search_hash_list();

        [DllImport("rpkg.dll", EntryPoint = "get_rpkg_file_paths_hash_is_in",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_rpkg_file_paths_hash_is_in(string hash_string);

        [DllImport("rpkg.dll", EntryPoint = "create_ogg_file_from_hash_in_rpkg",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern int create_ogg_file_from_hash_in_rpkg(string rpkg_file, string hash_string, int command,
            int wwev_number);

        [DllImport("rpkg.dll", EntryPoint = "convert_ogg_to_pcm", CallingConvention = CallingConvention.Cdecl)]
        public static extern int convert_ogg_to_pcm(string input_path, string output_path);

        [DllImport("rpkg.dll", EntryPoint = "get_pcm_sample_size", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_pcm_sample_size();

        [DllImport("rpkg.dll", EntryPoint = "get_pcm_sample_rate", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_pcm_sample_rate();

        [DllImport("rpkg.dll", EntryPoint = "get_pcm_channels", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_pcm_channels();

        [DllImport("rpkg.dll", EntryPoint = "clear_temp_tblu_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int clear_temp_tblu_data();

        [DllImport("resourcetool.dll", EntryPoint = "convert_temp_to_json",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern int convert_temp_to_json(string input_path, string output_path, string operating_mode);

        [DllImport("rpkg.dll", EntryPoint = "get_matrix_data_from_godot_scene",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_matrix_data_from_godot_scene(string input_path);

        [DllImport("rpkg.dll", EntryPoint = "load_recursive_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int load_recursive_temps(string temp_hash, string rpkg_file_path, UInt32 temp_version);

        [DllImport("rpkg.dll", EntryPoint = "load_non_recursive_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int load_non_recursive_temps(string temp_hash, string rpkg_file_path, UInt32 temp_version);

        [DllImport("rpkg.dll", EntryPoint = "generate_png_from_text", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_png_from_text(string rpkg_file, string hash_string, string png_path);

        [DllImport("rpkg.dll", EntryPoint = "unload_rpkg", CallingConvention = CallingConvention.Cdecl)]
        public static extern int unload_rpkg(string rpkg_file);

        [DllImport("rpkg.dll", EntryPoint = "modify_patch_deletion_list", CallingConvention = CallingConvention.Cdecl)]
        public static extern int modify_patch_deletion_list(string rpkg_file, string patch_list, UInt32 patch_count,
            UInt32 backup_rpkg);

        [DllImport("rpkg.dll", EntryPoint = "modify_hash_depends", CallingConvention = CallingConvention.Cdecl)]
        public static extern int modify_hash_depends(string rpkg_file, string hash_string, string hash_list,
            string hash_flag_list, UInt32 hash_count, UInt32 backup_rpkg);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_version", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_version(string temp_hash, string rpkg_file_path);

        [DllImport("rpkg.dll", EntryPoint = "set_temp_version", CallingConvention = CallingConvention.Cdecl)]
        public static extern int set_temp_version(UInt32 temps_index, UInt32 temp_version);

        [DllImport("rpkg.dll", EntryPoint = "import_rpkgs", CallingConvention = CallingConvention.Cdecl)]
        public static extern int import_rpkgs(string rpkgs_path, string rpkgs_list);

        [DllImport("rpkg.dll", EntryPoint = "is_rpkg_valid", CallingConvention = CallingConvention.Cdecl)]
        public static extern int is_rpkg_valid(string rpkg_file_path);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_entries_with_logical_parent(UInt32 temps_index, UInt32 logical_parent);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent_string(UInt32 vector_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_with_logical_parent_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_with_logical_parent_data();

        [DllImport("rpkg.dll", EntryPoint = "get_temp_entries", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_entries(UInt32 temps_index, string value_type, string type_string);

        [DllImport("rpkg.dll", EntryPoint = "get_entries", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_entries(UInt32 temps_index, UInt32 entry_index, string value_type);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_data();

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_reference_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_reference_data(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entries_hash_references", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entries_hash_references(UInt32 temps_index, UInt32 entry_index);

        [DllImport("rpkg.dll", EntryPoint = "update_temp_file", CallingConvention = CallingConvention.Cdecl)]
        public static extern int update_temp_file(UInt32 temps_index, UInt32 entry_index, IntPtr update_data, UInt32 update_data_size);

        [DllImport("rpkg.dll", EntryPoint = "generate_temp_file_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_temp_file_from_data(UInt32 temps_index, string temp_file_path);

        [DllImport("rpkg.dll", EntryPoint = "get_enum_values", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_enum_values(UInt32 temps_index, string property_type);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_index(string temp_hash_string);

        [DllImport("rpkg.dll", EntryPoint = "get_number_of_changed_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_number_of_changed_temps();

        [DllImport("rpkg.dll", EntryPoint = "generate_temp_files_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_temp_files_from_data(string temp_path);

        [DllImport("rpkg.dll", EntryPoint = "get_total_numer_of_temps", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_total_numer_of_temps();

        [DllImport("rpkg.dll", EntryPoint = "generate_rpkg_files_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_rpkg_files_from_data(string temp_path);

        [DllImport("rpkg.dll", EntryPoint = "get_top_level_logical_parents", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_top_level_logical_parents(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_all_bricks", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_all_bricks(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "search_temp_files", CallingConvention = CallingConvention.Cdecl)]
        public static extern int search_temp_files(UInt32 temps_index, string search_str, int max_results);

        [DllImport("rpkg.dll", EntryPoint = "get_search_temp_files", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_search_temp_files();

        [DllImport("rpkg.dll", EntryPoint = "get_response_string", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_response_string();

        [DllImport("rpkg.dll", EntryPoint = "get_temp_version", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_version(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_temp_subentity_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_temp_subentity_count(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_tblu_subentity_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_tblu_subentity_count(UInt32 temps_index);

        [DllImport("rpkg.dll", EntryPoint = "get_entry_name", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_entry_name(UInt32 temp_index, int entry_index);

        [DllImport("rpkg.dll", EntryPoint = "generate_json_files_from_data", CallingConvention = CallingConvention.Cdecl)]
        public static extern int generate_json_files_from_data(string temp_path);
    }
}