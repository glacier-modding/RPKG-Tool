#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class ores
{
public:
	ores();
	ores(uint64_t rpkgs_index, uint64_t hash_index);

	uint32_t ores_rpkg_index = 0;
	uint32_t ores_hash_index = 0;
	std::vector<char> ores_data;
	std::map<std::string, uint64_t> ores_entries;
};