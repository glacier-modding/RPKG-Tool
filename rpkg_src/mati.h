#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

struct mati_property
{
	std::string name = "AAAA";
	uint32_t data;
	uint32_t size;
	uint32_t type;
	uint32_t value_int;
	float value_float;
	std::string value_string = "";
	int32_t parent = -1;
};

class mati
{
public:
	mati();
	mati(uint64_t rpkgs_index, uint64_t hash_index);

	void load_hash_depends();
	void map_textures();
	void read_properties(uint32_t position, uint32_t parent);

	std::string mati_file_name = "";
	uint32_t mati_rpkg_index = 0;
	uint32_t mati_hash_index = 0;
	std::vector<char> mati_input_data;
	std::vector<char> mati_output_data;
	std::vector<char> mati_data;
	uint32_t header_offset = 0;
	uint32_t type_offset = 0;
	uint32_t texture_count = 0;
	uint32_t instance_offset = 0;
	bool has_diffuse_texture = false;
	uint64_t diffuse_texture_hash = 0;
	bool has_normal_texture = false;
	uint64_t normal_texture_hash = 0;
	bool has_specular_texture = false;
	uint64_t specular_texture_hash = 0;
	bool has_emissive_texture = false;
	uint64_t emissive_texture_hash = 0;
	bool texture_found = false;
	std::string type = "";
	std::vector<mati_property> mati_properties;
	std::vector<std::string> text_depends_file_name;
	std::vector<std::string> text_depends_hash_string;
	std::vector<uint64_t> text_depends_hash_value;
	std::vector<uint32_t> text_depends_index;
	std::vector<std::vector<std::string>> text_depends_in_rpkgs;
	std::vector<uint32_t> text_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> text_depends_rpkg_index;
	std::vector<uint32_t> text_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> text_depends_hash_index;
	std::vector<uint32_t> text_depends_hash_index_index;
	std::vector<std::string> mate_depends_file_name;
	std::vector<std::string> mate_depends_hash_string;
	std::vector<uint64_t> mate_depends_hash_value;
	std::vector<uint32_t> mate_depends_index;
	std::vector<std::vector<std::string>> mate_depends_in_rpkgs;
	std::vector<uint32_t> mate_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> mate_depends_rpkg_index;
	std::vector<uint32_t> mate_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> mate_depends_hash_index;
	std::vector<uint32_t> mate_depends_hash_index_index;
	std::map<std::string, std::string> mati_property_name_map = { {"AREF", "Alpha Reference"},
																	{"ATST", "Alpha Test Enabled"},
																	{"BENA", "Blend Enabled"},
																	{"BIND", "Binder"},
																	{"BMOD", "Blend Mode"},
																	{"COLO", "Color"},
																	{"CULL", "Culling Mode"},
																	{"DBDE", "Decal Blend Diffuse"},
																	{"DBEE", "Decal Blend Emission"},
																	{"DBNE", "Decal Blend Normal"},
																	{"DBRE", "Decal Blend Roughness"},
																	{"DBSE", "Decal Blend Specular"},
																	{"ENAB", "Enabled"},
																	{"FENA", "Fog Enabled"},
																	{"FLTV", "Float Value"},
																	{"INST", "Instance"},
																	{"NAME", "Name"},
																	{"OPAC", "Opacity"},
																	{"RSTA", "Render State"},
																	{"SSBW", "Subsurface Value"},
																	{"SSVB", "Subsurface Blue"},
																	{"SSVG", "Subsurface Green"},
																	{"SSVR", "Subsurface Red"},
																	{"TAGS", "Tags"},
																	{"TEXT", "Texture"},
																	{"TILU", "Tiling U"},
																	{"TILV", "Tiling V"},
																	{"TXID", "Texture Id"},
																	{"TYPE", "Type"},
																	{"VALU", "Value"},
																	{"ZBIA", "Z Bias"},
																	{"ZOFF", "Z Offset"} };
};