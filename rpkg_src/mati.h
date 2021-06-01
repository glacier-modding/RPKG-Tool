#pragma once
#include "hash.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class mati
{
public:
	mati();
	mati(uint64_t rpkgs_index, uint64_t hash_index);

	void load_hash_depends();

	std::string mati_file_name = "";
	uint32_t mati_rpkg_index = 0;
	uint32_t mati_hash_index = 0;
	std::vector<char> mati_input_data;
	std::vector<char> mati_output_data;
	std::vector<char> mati_data;
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
};