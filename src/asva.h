#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <variant>
#include "hash.h"
#include "global.h"
#include "thirdparty/json/json.hpp"

class asva
{
public:
#pragma pack(push, 1)
    struct Header
    {
        char bin1[4];
		uint8_t pad1;
		uint8_t type1;
		uint8_t type2;
		uint8_t pad2;
		uint32_t data_size = 0;
		uint32_t pad3;
		uint32_t table_offset = 0;
		uint32_t pad4;
		uint32_t strings_offset1 = 0;
		uint32_t pad5;
		uint32_t strings_offset2 = 0;
		uint64_t pad6;
		uint32_t entry_count = 0;
    };

	struct Table
	{
		uint32_t string_length = 0;
		uint32_t pad1;
		uint32_t string_offset = 0;
		uint64_t pad2;
		uint32_t depends_index = 0;
		uint64_t pad3;
	};
#pragma pack(pop)

	asva();	
	asva(uint64_t rpkgs_index, uint64_t hash_index);
	asva(std::string json_path, std::string output_path);
	
	void generate_json();
	void write_json_to_file(std::string output_path);
	
	std::string asva_file_name = "";
	uint32_t asva_rpkg_index = 0;
	uint32_t asva_hash_index = 0;
	std::vector<char> asva_input_data;
	std::vector<char> asva_output_data;
	std::vector<char> asva_data;
    asva::Header header;
	std::vector<asva::Table> table;
	nlohmann::ordered_json json;
	std::string json_error = "";
};