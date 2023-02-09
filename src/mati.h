#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "hash.h"
#include "thirdparty/json/json.hpp"

struct mati_property {
    std::string name = "AAAA";
    uint32_t data;
    uint32_t size;
    uint32_t type;
    uint32_t int_value;
    std::vector<float> float_values;
    std::string string_value = "";
    int32_t parent = -1;
};

enum class MATI_PROPERTY_TYPE {
    FLOAT = 0,
    STRING = 1,
    INT = 2,
    PROPERTY = 3,
    UNKNOWN = 4
};

class mati {
public:
    mati();

    mati(uint64_t rpkgs_index, uint64_t hash_index);

    mati(std::string json_path, uint64_t hash_value, std::string output_path, bool output_path_is_file);

    mati(std::string mati_path, std::string mati_meta_path, uint64_t hash_value, std::string output_path,
         bool output_path_is_file);

    void load_hash_depends();

    void map_textures();

    void read_properties(uint32_t position, uint32_t parent);

    void generate_json(std::string output_path);

    nlohmann::ordered_json
    read_properties_json(nlohmann::ordered_json json, uint32_t position, uint32_t parent, bool from_mati_file);

    void rebuild_properties(nlohmann::ordered_json json, std::string key);

    MATI_PROPERTY_TYPE get_property_type(std::string key);

    void align();

    void write_uint32_t(uint32_t data);

    void write_string(std::string data);

    void write_float(float data);

    void write_name(std::string name);

    void write_property_type(std::string name);

    std::string mati_file_name = "";
    uint32_t mati_rpkg_index = 0;
    uint32_t mati_hash_index = 0;
    std::vector<char> mati_input_data;
    std::vector<char> mati_output_data;
    std::vector<char> mati_data;
    hash meta_data;
    uint32_t header_offset = 0;
    uint32_t type_offset = 0;
    uint32_t texture_count = 0;
    uint32_t unknown_flag_1 = 0;
    uint32_t unknown_flag_2 = 0;
    int32_t eres_index = -1;
    int32_t mate_index = -1;
    uint32_t instance_offset = 0;
    bool has_diffuse_texture = false;
    uint64_t diffuse_texture_hash = 0;
    bool has_normal_texture = false;
    uint64_t normal_texture_hash = 0;
    bool has_specular_texture = false;
    uint64_t specular_texture_hash = 0;
    bool has_emissive_texture = false;
    uint64_t emissive_texture_hash = 0;
    bool texture_found = false;
    nlohmann::ordered_json json;
    std::string json_error = "";
    std::string type = "";
    std::vector<mati_property> mati_properties;
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
    std::unordered_map<std::string, std::string> mati_property_name_map = {{"AREF", "Alpha Reference"},
                                                                           {"ATST", "Alpha Test Enabled"},
                                                                           {"BENA", "Blend Enabled"},
                                                                           {"BIND", "Binder"},
                                                                           {"BMOD", "Blend Mode"},
                                                                           {"COLO", "Color"},
                                                                           {"CULL", "Culling Mode"},
                                                                           {"DBDE", "Decal Blend Diffuse"},
                                                                           {"DBEE", "Decal Blend Emission"},
                                                                           {"DBNE", "Decal Blend Normal"},
                                                                           {"DBRE", "Decal Blend Roughness"},
                                                                           {"DBSE", "Decal Blend Specular"},
                                                                           {"ENAB", "Enabled"},
                                                                           {"FENA", "Fog Enabled"},
                                                                           {"FLTV", "Float Value"},
                                                                           {"INST", "Instance"},
                                                                           {"NAME", "Name"},
                                                                           {"OPAC", "Opacity"},
                                                                           {"RSTA", "Render State"},
                                                                           {"SSBW", "Subsurface Value"},
                                                                           {"SSVB", "Subsurface Blue"},
                                                                           {"SSVG", "Subsurface Green"},
                                                                           {"SSVR", "Subsurface Red"},
                                                                           {"TAGS", "Tags"},
                                                                           {"TEXT", "Texture"},
                                                                           {"TILU", "Tiling U"},
                                                                           {"TILV", "Tiling V"},
                                                                           {"TXID", "Texture Id"},
                                                                           {"TYPE", "Type"},
                                                                           {"VALU", "Value"},
                                                                           {"ZBIA", "Z Bias"},
                                                                           {"ZOFF", "Z Offset"}};
    std::unordered_map<std::string, MATI_PROPERTY_TYPE> mati_property_type_map = {{"AREF", MATI_PROPERTY_TYPE::INT},
                                                                                  {"ATST", MATI_PROPERTY_TYPE::INT},
                                                                                  {"BENA", MATI_PROPERTY_TYPE::INT},
                                                                                  {"BIND", MATI_PROPERTY_TYPE::PROPERTY},
                                                                                  {"BMOD", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"COLO", MATI_PROPERTY_TYPE::PROPERTY},
                                                                                  {"CULL", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"DBDE", MATI_PROPERTY_TYPE::INT},
                                                                                  {"DBEE", MATI_PROPERTY_TYPE::INT},
                                                                                  {"DBNE", MATI_PROPERTY_TYPE::INT},
                                                                                  {"DBRE", MATI_PROPERTY_TYPE::INT},
                                                                                  {"DBSE", MATI_PROPERTY_TYPE::INT},
                                                                                  {"ENAB", MATI_PROPERTY_TYPE::INT},
                                                                                  {"FENA", MATI_PROPERTY_TYPE::INT},
                                                                                  {"FLTV", MATI_PROPERTY_TYPE::PROPERTY},
                                                                                  {"INST", MATI_PROPERTY_TYPE::PROPERTY},
                                                                                  {"NAME", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"OPAC", MATI_PROPERTY_TYPE::FLOAT},
                                                                                  {"RSTA", MATI_PROPERTY_TYPE::PROPERTY},
                                                                                  {"SSBW", MATI_PROPERTY_TYPE::FLOAT},
                                                                                  {"SSVB", MATI_PROPERTY_TYPE::FLOAT},
                                                                                  {"SSVG", MATI_PROPERTY_TYPE::FLOAT},
                                                                                  {"SSVR", MATI_PROPERTY_TYPE::FLOAT},
                                                                                  {"TAGS", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"TEXT", MATI_PROPERTY_TYPE::PROPERTY},
                                                                                  {"TILU", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"TILV", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"TXID", MATI_PROPERTY_TYPE::INT},
                                                                                  {"TYPE", MATI_PROPERTY_TYPE::STRING},
                                                                                  {"VALU", MATI_PROPERTY_TYPE::FLOAT},
                                                                                  {"ZBIA", MATI_PROPERTY_TYPE::INT},
                                                                                  {"ZOFF", MATI_PROPERTY_TYPE::FLOAT}};
};