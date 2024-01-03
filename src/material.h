#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <variant>
#include "hash.h"
#include "global.h"
#include <nlohmann/json.hpp>

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

class material {
public:
    material();

    material(uint64_t rpkgs_index, uint64_t hash_index);

    material(const std::string& json_path, const std::string& output_path);
    //material(std::string material_path, std::string material_meta_path, uint64_t hash_value, std::string output_path, bool output_path_is_file);

    void generate_json();

    void write_json_to_file(const std::string& output_path) const;

    nlohmann::ordered_json
    read_mati_properties_json(nlohmann::ordered_json temp_json, uint32_t position, uint32_t parent,
                              bool from_mati_file);

    [[nodiscard]] MATI_PROPERTY_TYPE get_property_type(const std::string& key) const;

    static void align(std::vector<char>& material_data);

    static void write_uint32_t(std::vector<char>& material_data, uint32_t data);

    static void write_string(std::vector<char>& material_data, const std::string& data);

    static void write_float(std::vector<char>& material_data, float data);

    static void write_name(std::vector<char>& material_data, std::string name);

    void write_property_type(std::vector<char>& material_data, const std::string& name);

    std::string get_property_name(std::string property);

    uint32_t encode_flags(std::map<uint32_t, std::vector<std::string>>& flags, nlohmann::ordered_json temp_json);

    nlohmann::ordered_json decode_flags(std::map<uint32_t, std::vector<std::string>>& flags, uint32_t value);

    std::string matt_file_name = "";
    uint32_t matt_rpkg_index = 0;
    uint32_t matt_hash_index = 0;
    uint32_t matb_rpkg_index = 0;
    uint32_t matb_hash_index = 0;
    uint32_t mati_rpkg_index = 0;
    uint32_t mati_hash_index = 0;
    uint64_t matt_hash_value = 0;
    uint64_t matb_hash_value = 0;
    std::vector<char> matt_data;
    std::vector<char> matb_data;
    std::vector<char> mati_data;
    std::vector<std::variant<int32_t, float, vector2, vector3, vector4>> matt_entries;
    std::vector<uint32_t> matt_types;
    std::vector<std::string> matb_entries;
    nlohmann::ordered_json json;
    nlohmann::ordered_json mati_json;
    std::string json_error = "";
    std::string type = "";
    std::vector<mati_property> mati_properties;

    std::map<uint32_t, std::vector<std::string>> ClassFlags = {
        {0x1, {"REFLECTION2D"}},
        {0x2, {"REFRACTION2D"}},
        {0x4, {"LIGHTING"}},
        {0x8, {"EMISSIVE"}},
        {0x10, {"DISCARD"}},
        {0x20, {"LM_SKIN"}},
        {0x40, {"PRIMCLASS_STANDARD"}},
        {0x80, {"PRIMCLASS_LINKED"}},
        {0x100, {"PRIMCLASS_WEIGHTED"}},
        {0x200, {"DOFOVERRIDE"}},
        {0x400, {"USES_DEFAULT_VS"}},
        {0x800, {"USES_SPRITE_SA_VS"}},
        {0x1000, {"USES_SPRITE_AO_VS"}},
        {0x2000, {"ALPHA"}},
        {0x4000, {"USES_SIMPLE_SHADER"}},
        {0x8000, {"DISABLE_INSTANCING"}},
        {0x10000, {"LM_HAIR"}},
        {0x20000, {"SAMPLE_LIGHTING", "UNKNOWN_1"}},
        {0x40000, {"HORIZONMAPPING", "UNKNOWN_2"}},
        {0x80000, {"UNKNOWN_3"}},
        {0x100000, {"UNKNOWN_4"}},
        {0x200000, {"UNKNOWN_5"}}};

    std::map<uint32_t, std::vector<std::string>> InstanceFlags = {
        {0x1, {"OPAQUE_EMISSIVE"}},
        {0x2, {"TRANS_EMISSIVE"}},
        {0x4, {"TRANSADD_EMISSIVE"}},
        {0x6, {"TRANSALL"}},
        {0x8, {"OPAQUE_LIT"}},
        {0x10, {"TRANS_LIT"}},
        {0x20, {"DECAL"}},
        {0x40, {"REFRACTIVE"}},
        {0x80, {"LM_SKIN"}},
        {0x100, {"LM_HAIR"}},
        {0x200, {"FORCE_EMISSIVE"}},
        {0x400, {"DISABLE_SHADER_LOD"}},
        {0x800, {"DISCARD"}},
        {0x1000, {"DECAL_EMISSIVE"}},
        {0x1020, {"DECAL_ALL"}},
        {0x2000, {"WATER_CLIPPING"}},
        {0x4000, {"SAMPLE_LIGHTING"}},
        {0x8000, {"EXCLUDE_GLOBAL_SHADOWS"}},
        {0xFFFFFFFF, {"ALL"}}};

    const std::unordered_map<std::string, std::string> mati_property_name_map = {
            {"AREF", "Alpha Reference"},
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

    const std::unordered_map<std::string, std::string> mati_property_name_map_reversed = {
            {"Alpha Reference",       "AREF"},
            {"Alpha Test Enabled",    "ATST"},
            {"Blend Enabled",         "BENA"},
            {"Binder",                "BIND"},
            {"Blend Mode",            "BMOD"},
            {"Color",                 "COLO"},
            {"Culling Mode",          "CULL"},
            {"Decal Blend Diffuse",   "DBDE"},
            {"Decal Blend Emission",  "DBEE"},
            {"Decal Blend Normal",    "DBNE"},
            {"Decal Blend Roughness", "DBRE"},
            {"Decal Blend Specular",  "DBSE"},
            {"Enabled",               "ENAB"},
            {"Fog Enabled",           "FENA"},
            {"Float Value",           "FLTV"},
            {"Instance",              "INST"},
            {"Name",                  "NAME"},
            {"Opacity",               "OPAC"},
            {"Render State",          "RSTA"},
            {"Subsurface Value",      "SSBW"},
            {"Subsurface Blue",       "SSVB"},
            {"Subsurface Green",      "SSVG"},
            {"Subsurface Red",        "SSVR"},
            {"Tags",                  "TAGS"},
            {"Texture",               "TEXT"},
            {"Tiling U",              "TILU"},
            {"Tiling V",              "TILV"},
            {"Texture Id",            "TXID"},
            {"Type",                  "TYPE"},
            {"Value",                 "VALU"},
            {"Z Bias",                "ZBIA"},
            {"Z Offset",              "ZOFF"}};

    const std::unordered_map<std::string, MATI_PROPERTY_TYPE> mati_property_type_map = {
            {"AREF", MATI_PROPERTY_TYPE::INT},
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