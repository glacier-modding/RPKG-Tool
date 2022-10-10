#pragma once
#include "rpkg_function.h"
#include <string>
#include <set>
#include <stdint.h>

class dev_function
{
public:
	static void dev_dlge_names(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_all_hashes_in_game(rpkg_extraction_vars& rpkg_vars);
	static void dev_hash_list_percent_found(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_map_editor(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_temp_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_resource_tool(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_temp_pointers(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_ores_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_diff_rpkg_supermetas(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_rpkg_supermetas(std::string& input_path, std::string& output_path);
	static void dev_extract_wwise_ids(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_all_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_wwem_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_wwes_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_wwev_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_materials_textures_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_all_entity_ids(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_all_parent_strings(uint32_t temps_index, uint32_t entry_index, std::string parent_string, std::set<std::string>& parent_string_set);
};