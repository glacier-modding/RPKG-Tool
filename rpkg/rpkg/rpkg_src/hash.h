#pragma once
#include <string>
#include <vector>
#include <map>

struct hash_depends_variables
{
    uint64_t hash_value = 0;
    std::string hash_string = "";
    std::string rpkg_file_name = "";
    std::vector<std::string> hash_dependency;
    std::vector<std::string> hash_dependency_file_name;
    std::map<uint64_t, uint64_t> hash_dependency_map;
    std::vector<std::vector<std::string>> hash_dependency_in_rpkg;
    std::vector<std::vector<std::string>> hash_dependency_patch_list;
};

struct hash_reference_variables
{
    uint64_t hash_value = 0;
    uint32_t hash_reference_count = 0;
    std::vector<uint8_t> hash_reference_type;
    std::vector<uint64_t> hash_reference;
    std::vector<std::string> hash_reference_string;
};

class hash
{
public:
	hash();

    uint64_t hash_value = 0;
    std::string hash_string = "";
    std::string hash_file_name = "";
    uint64_t hash_offset = 0;
    uint32_t hash_size = 0;
    int is_lz4ed = 0;
    int is_xored = 0;
    std::string hash_resource_type = "";
    uint32_t hash_reference_table_size = 0;
    uint32_t hash_reference_table_dummy = 0;
    uint32_t hash_size_final = 0;
    uint32_t hash_size_in_memory = 0;
    uint32_t hash_size_in_video_memory = 0;
    hash_reference_variables hash_reference_data;
};

