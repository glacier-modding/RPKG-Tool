#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/zhmtools/ResourceLib_HM3.h"
#include "thirdparty/json/json.hpp"

using json = nlohmann::ordered_json;

class temp {
public:
    temp();

    temp(uint64_t rpkgs_index, uint64_t hash_index);

    temp(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version);

    void load_data();

    void load_hash_depends();

    void load_temp_data();

    void load_tblu_data();

    void temp_version_check();

    void get_top_level_logical_parents() const;

    void get_entries_with_logical_parent(uint32_t logical_parent);

    void get_entries_hash_references(uint32_t entry_index);

    void get_entries_hash_reference_data(uint32_t entry_index);

    void get_temp_entries_data(std::string value_type, std::string type_string);

    void json_temp_node_scan(const rapidjson::Value& node, std::string& propertyValues_string,
                             std::string& nPropertyID_string, std::string& type_string, std::string json_pointer,
                             std::string json_type);

    void get_entries_data(uint32_t entry_index, std::string value_type);

    void
    json_node_scan(const rapidjson::Value& node, std::string& propertyValues_string, std::string& nPropertyID_string,
                   std::string& type_string, std::string json_pointer, std::string json_type);

    void get_enum_values(std::string& property_type_string) const;

    void get_all_bricks() const;

    void get_entry_name_string(int entry_index);

    bool rt_json_to_qn_json();

    void convert_to_qn_reference(rapidjson::Document& json_document, rapidjson::Value& reference,
                                 std::string pointer_string);

    void convert_to_qn_property(rapidjson::Document& json_document, rapidjson::Value& property);

    void write_qn_json_to_file(std::string output_path);

    void generate_qn_patch_json();

    void write_qn_patch_json_to_file(std::string output_path);

    std::string temp_file_name = "";
    uint32_t temp_temps_index = 0;
    uint32_t temp_rpkg_index = 0;
    uint32_t temp_hash_index = 0;
    uint32_t tblu_rpkg_index = 0;
    uint32_t tblu_hash_index = 0;
    uint32_t temp_file_version = 3;
    uint32_t temp_subentity_count = 0;
    uint32_t tblu_subentity_count = 0;
    uint32_t parent = 0;
    std::vector<uint32_t> parents;
    std::vector<uint32_t> children;
    std::vector<char> temp_input_data;
    std::vector<char> temp_output_data;
    std::vector<char> temp_data;
    std::vector<char> tblu_input_data;
    std::vector<char> tblu_output_data;
    std::vector<char> tblu_data;
    std::vector<std::string> temp_meta_strings;
    std::vector<std::string> temp_meta_flags;
    std::vector<std::string> tblu_meta_strings;
    std::vector<std::string> tblu_meta_flags;
    std::vector<uint32_t> temp_logicalParent;
    std::vector<std::string> tblu_entityName;
    std::vector<uint64_t> tblu_entityId;
    std::unordered_map<uint64_t, uint64_t> tblu_entityId_map;
    std::vector<uint32_t> temp_entityTypeResourceIndex;
    std::vector<uint32_t> tblu_entityTypeResourceIndex;
    std::vector<int32_t> temp_externalSceneTypeIndicesInResourceHeader;
    std::vector<uint64_t> temp_externalSceneHashes;
    JsonString* temp_json_input = nullptr;
    JsonString* tblu_json_input = nullptr;
    rapidjson::Document temp_json_document;
    rapidjson::Document temp_json_document_original;
    rapidjson::Document tblu_json_document;
    rapidjson::Document tblu_json_document_original;
    rapidjson::Document qn_json_document;
    rapidjson::Document qn_json_document_original;
    json qn_patch_json;
    std::vector<std::string> temp_depends_file_name;
    std::vector<std::string> temp_depends_hash_string;
    std::vector<uint64_t> temp_depends_hash_value;
    std::vector<uint32_t> temp_depends_index;
    std::vector<std::vector<std::string>> temp_depends_in_rpkgs;
    std::vector<uint32_t> temp_depends_in_rpkgs_index;
    std::vector<std::vector<uint32_t>> temp_depends_rpkg_index;
    std::vector<uint32_t> temp_depends_rpkg_index_index;
    std::vector<std::vector<uint32_t>> temp_depends_hash_index;
    std::vector<uint32_t> temp_depends_hash_index_index;
    int tblu_return_value = 0;
    std::vector<std::string> tblu_depends_file_name;
    std::vector<std::string> tblu_depends_hash_string;
    std::vector<uint64_t> tblu_depends_hash_value;
    std::vector<uint32_t> tblu_depends_index;
    std::vector<std::vector<std::string>> tblu_depends_in_rpkgs;
    std::vector<uint32_t> tblu_depends_in_rpkgs_index;
    std::vector<std::vector<uint32_t>> tblu_depends_rpkg_index;
    std::vector<uint32_t> tblu_depends_rpkg_index_index;
    std::vector<std::vector<uint32_t>> tblu_depends_hash_index;
    std::vector<uint32_t> tblu_depends_hash_index_index;
    std::vector<std::string> prim_depends_file_name;
    std::vector<std::string> prim_depends_hash_string;
    std::vector<uint64_t> prim_depends_hash_value;
    std::vector<uint32_t> prim_depends_index;
    std::vector<std::vector<std::string>> prim_depends_in_rpkgs;
    std::vector<uint32_t> prim_depends_in_rpkgs_index;
    std::vector<std::vector<uint32_t>> prim_depends_rpkg_index;
    std::vector<uint32_t> prim_depends_rpkg_index_index;
    std::vector<std::vector<uint32_t>> prim_depends_hash_index;
    std::vector<uint32_t> prim_depends_hash_index_index;
    ResourceConverter* resource_tool_converter;
    ResourceGenerator* resource_tool_generator;
};
