#pragma once
#include "hash.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class borg
{
public:
	borg();
	borg(uint64_t rpkgs_index, uint64_t hash_index);

	std::string borg_file_name = "";
	uint32_t borg_rpkg_index = 0;
	uint32_t borg_hash_index = 0;
	std::vector<char> borg_input_data;
	std::vector<char> borg_output_data;
	std::vector<char> borg_data;
};