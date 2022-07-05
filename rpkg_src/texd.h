#pragma once
#include <string>
#include <vector>
#include <stdint.h>

class texd
{
public:
	texd();

	void load(uint64_t rpkgs_index, uint64_t hash_index);

	std::string texd_file_name = "";
	uint32_t texd_rpkg_index = 0;
	uint32_t texd_hash_index = 0;
	std::vector<char> texd_input_data;
	std::vector<char> texd_output_data;
	std::vector<char> texd_data;
};