#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include "hash.h"
#include "global.h"
#include "thirdparty/json/json.hpp"

struct mrtr_entry
{
	uint32_t index = 0;
	uint32_t parent = 0;
	std::string name = "";
	vector3 position;
	vector4 rotation;
};

class mrtr
{
public:
	mrtr();
	mrtr(uint64_t rpkgs_index, uint64_t hash_index);
	mrtr(std::string json_path, uint64_t hash_value, std::string output_path, bool output_path_is_file);
	mrtr(std::string mrtr_path, std::string mrtr_meta_path, uint64_t hash_value, std::string output_path, bool output_path_is_file);

	void generate_json();
	void write_json_to_file(std::string output_path);

	std::string mrtr_file_name = "";
	uint32_t mrtr_rpkg_index = 0;
	uint32_t mrtr_hash_index = 0;
	std::vector<char> mrtr_input_data;
	std::vector<char> mrtr_output_data;
	std::vector<char> mrtr_data;
	uint32_t mrtr_entry_count = 0;
	std::vector<mrtr_entry> mrtr_entries;
	nlohmann::ordered_json json;
	std::string json_error = "";
};