#pragma once

#include "hash.h"
#include <string>
#include <vector>
#include <unordered_map>

class rpkg {
public:
#pragma pack(push, 1)
    struct Header {
        uint32_t hash_count = 0;
        uint32_t hash_header_table_size = 0;
        uint32_t hash_resource_table_size = 0;
        uint32_t patch_count = 0;
    };
#pragma pack(pop)

    rpkg();

    Header header;
    std::string rpkg_file_path = "";
    std::string rpkg_file_name = "";
    int rpkg_file_version = 0;
    std::vector<char> rpkgv2_header;
    bool is_patch_file = false;
    std::vector<uint64_t> patch_entry_list;
    std::vector<hash> hash;
    std::unordered_map<uint64_t, uint64_t> hash_map;
    std::vector<uint64_t> meta_hash_value;
    std::vector<uint64_t> meta_hash_offset;
    std::vector<uint32_t> meta_hash_size;
    std::vector<std::string> hash_resource_types;
    std::vector<uint64_t> hash_resource_types_data_size;
    std::vector<std::vector<uint64_t>> hashes_based_on_resource_types;
    std::vector<std::vector<uint64_t>> hashes_indexes_based_on_resource_types;
};
