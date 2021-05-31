#pragma once
#include "hash.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class temp
{
public:
	temp();
	temp(uint64_t rpkgs_index, uint64_t hash_index);

	void load_hash_depends();
	void load_temp_tblu_data();
	void get_entries_with_logical_parent(uint32_t logical_parent);
	void get_entries_data(uint32_t entry_index);
	void get_entries_hash_reference_data(uint32_t entry_index);
	void get_entries_hash_references(uint32_t entry_index);
	int update_temp_file(std::string& offset_string, std::string& type_string, std::string& value_string);
	int generate_temp_file_from_data(std::string& temp_path);
	int is_offset_shared(std::string& offset_string, std::string& property_type_index_string);
	void get_all_shared_values(std::string& offset_string, std::string& property_type_index_string);
	int get_shared_index(std::string& offset_string, std::string& property_type_index_string);
	int get_shared_count(std::string& offset_string, std::string& property_type_index_string);
	int update_temp_file_pointer(std::string& entry_index_string, std::string& property_index_string, std::string& offset_string);
	void get_enum_values(std::string& property_type_string);
	void get_prim_from_temp(uint32_t entry_index);
	void get_entry_name_string(uint32_t entry_index);
	void get_top_level_logical_parents();
	void get_all_bricks();

	void load_temp_hash_depends();
	void load_prim_hash_depends();

	std::string temp_file_name = "";
	uint32_t temp_temps_index = 0;
	uint32_t temp_rpkg_index = 0;
	uint32_t temp_hash_index = 0;
	uint32_t tblu_rpkg_index = 0;
	uint32_t tblu_hash_index = 0;
	uint32_t parent = 0;
	std::vector<uint32_t> parents;
	std::vector<uint32_t> children;
	bool file_has_been_changed = false;
	std::vector<uint32_t> temp_entry_index;
	std::vector<uint32_t> temp_logicalParent;
	std::vector<bool> temp_logicalParent_is_entityId;
	std::vector<uint64_t> temp_logicalParent_entityId;
	std::vector<uint32_t> temp_entityTypeResourceIndex;
	std::vector<uint32_t> temp_propertyValues_start_offsets;
	std::vector<uint32_t> temp_propertyValues_end_offsets;
	std::vector<uint32_t> temp_postInitPropertyValues_start_offsets;
	std::vector<uint32_t> temp_postInitPropertyValues_end_offsets;
	std::vector<uint32_t> temp_platformSpecificPropertyValues_start_offsets;
	std::vector<uint32_t> temp_platformSpecificPropertyValues_end_offsets;
	std::vector<std::string> temp_property_types;
	std::vector<std::vector<bool>> temp_property_types_shared;
	std::vector<std::vector<uint32_t>> temp_property_types_shared_count;
	std::vector<std::vector<uint32_t>> temp_property_types_offsets;
	std::vector<std::vector<std::string>> temp_property_types_values;
	std::vector<std::map<uint32_t, uint32_t>> temp_property_types_offsets_map;
	std::vector<std::vector<uint32_t>> property_crc32_values;
	std::vector<std::vector<uint32_t>> property_type_indexes;
	std::vector<std::vector<uint32_t>> property_offsets;
	std::vector<std::vector<uint32_t>> property_pointer_offsets;
	std::vector<uint32_t> tblu_entry_index;
	std::vector<uint32_t> tblu_logicalParent;
	std::vector<bool> tblu_logicalParent_is_entityId;
	std::vector<uint64_t> tblu_logicalParent_entityId;
	std::vector<uint32_t> tblu_entityTypeResourceIndex;
	std::vector<uint64_t> tblu_entityId;
	std::vector<uint32_t> tblu_editorOnly;
	std::vector<std::string> tblu_entityName;
	std::vector<char> temp_input_data;
	std::vector<char> temp_output_data;
	std::vector<char> temp_data;
	std::vector<char> tblu_input_data;
	std::vector<char> tblu_output_data;
	std::vector<char> tblu_data;
	std::vector<std::string> temp_depends_file_name;
	std::vector<std::string> temp_depends_hash_string;
	std::vector<uint64_t> temp_depends_hash_value;
	std::vector<uint32_t> temp_depends_index;
	std::vector<std::vector<std::string>> temp_depends_in_rpkgs;
	std::vector<uint32_t> temp_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> temp_depends_rpkg_index;
	std::vector<uint32_t> temp_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> temp_depends_hash_index;
	std::vector<uint32_t> temp_depends_hash_index_index;
	int tblu_return_value = 0;
	std::vector<std::string> tblu_depends_file_name;
	std::vector<std::string> tblu_depends_hash_string;
	std::vector<uint64_t> tblu_depends_hash_value;
	std::vector<uint32_t> tblu_depends_index;
	std::vector<std::vector<std::string>> tblu_depends_in_rpkgs;
	std::vector<uint32_t> tblu_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> tblu_depends_rpkg_index;
	std::vector<uint32_t> tblu_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> tblu_depends_hash_index;
	std::vector<uint32_t> tblu_depends_hash_index_index;
	std::vector<std::string> prim_depends_file_name;
	std::vector<std::string> prim_depends_hash_string;
	std::vector<uint64_t> prim_depends_hash_value;
	std::vector<uint32_t> prim_depends_index;
	std::vector<std::vector<std::string>> prim_depends_in_rpkgs;
	std::vector<uint32_t> prim_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> prim_depends_rpkg_index;
	std::vector<uint32_t> prim_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> prim_depends_hash_index;
	std::vector<uint32_t> prim_depends_hash_index_index;
};