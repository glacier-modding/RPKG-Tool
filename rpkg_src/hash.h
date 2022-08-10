#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct hash_depends_variables
{
    uint64_t hash_value = 0;
    std::string hash_string = "";
    std::string rpkg_file_name = "";
    std::vector<std::string> hash_dependency;
    std::vector<std::string> hash_dependency_file_name;
    std::vector<std::string> hash_dependency_ioi_string;
    std::unordered_map<uint64_t, uint64_t> hash_dependency_map;
    std::vector<std::vector<std::string>> hash_dependency_in_rpkg;
    std::vector<std::vector<std::string>> hash_dependency_patch_list;
};

struct hash_reference_variables
{
    uint64_t hash_value = 0;
    uint32_t hash_reference_count = 0;
    std::vector<uint8_t> hash_reference_type;
    std::vector<uint64_t> hash_reference;
};

class hash
{
public:
#pragma pack(push, 1)
    struct HashHeader
    {
        uint64_t hash = 0;
        uint64_t data_offset = 0;
        uint32_t data_size = 0;
    };

    struct HashDepends
    {
        std::vector<uint8_t> flag;
        std::vector<uint64_t> depend;
    };

    struct HashResource
    {
        char resource_type[4];
        uint32_t reference_table_size = 0;
        uint32_t reference_table_dummy = 0;
        uint32_t size_final = 0;
        uint32_t size_in_memory = 0;
        uint32_t size_in_video_memory = 0;
    };

    struct HashData
    {
        HashHeader header;
        HashResource resource;
        HashDepends depends;
        uint32_t size = 0;
        bool lz4ed = false;
        bool xored = false;
    };
#pragma pack(pop)

	hash();

    HashData data;
    uint64_t hash_value = 0;
    std::string hash_resource_type = "";
    hash_reference_variables hash_reference_data;
};

