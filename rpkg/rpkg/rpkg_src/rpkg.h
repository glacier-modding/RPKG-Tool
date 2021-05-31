#pragma once
#include "hash.h"
#include <string>
#include <vector>
#include <map>

class rpkg
{
public:
	rpkg();

    std::string rpkg_file_path = "";
    std::string rpkg_file_name = "";
    int rpkg_file_version = 0;
    int rpkgv2_chunk_number = 0;
    int rpkgv2_chunk_patch_number = 0;
    uint32_t rpkg_file_count = 0;
    uint32_t rpkg_table_offset = 0;
    uint32_t rpkg_table_size = 0;
    bool is_patch_file = false;
    uint32_t patch_entry_count = 0;
    std::vector<uint64_t> patch_entry_list;
    std::vector<hash> hash;
    std::map<uint64_t, uint64_t> hash_map;
    std::vector<uint64_t> meta_hash_value;
    std::vector<uint64_t> meta_hash_offset;
    std::vector<uint32_t> meta_hash_size;
    std::vector<std::string> hash_resource_types;
    std::vector<uint64_t> hash_resource_types_data_size;
    std::vector<std::vector<std::string>> hashes_based_on_resource_types;
    std::vector<std::vector<uint64_t>> hashes_indexes_based_on_resource_types;
};
