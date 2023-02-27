#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include "thirdparty/zhmtools/ResourceLib_HM2.h"
#include "thirdparty/zhmtools/ResourceLib_HM3.h"
#include "thirdparty/yyjson/yyjson.h"

class entity {
public:
    enum class search_type : int {
        DEFAULT,
        REGEX
    };

    enum class search_category : int {
        ALL,
        TEMPHASH,
        TBLUHASH,
        ROOTENTITY,
        ENTITIES,
        PROPERTYOVERRIDES,
        OVERRIDEDELETES,
        PINCONNECTIONOVERRIDES,
        PINCONNECTIONOVERRIDEDELETES,
        EXTERNALSCENES,
        SUBTYPE,
        EXTRAFACTORYDEPENDENCIES,
        EXTRABLUEPRINTDEPENDENCIES,
        COMMENTS
    };

    std::unordered_map<std::string, search_category> search_category_strings {
        {"ALL", search_category::ALL},
        {"tempHash", search_category::TEMPHASH},
        {"tbluHash", search_category::TBLUHASH},
        {"rootEntity", search_category::ROOTENTITY},
        {"entities", search_category::ENTITIES},
        {"propertyOverrides", search_category::PROPERTYOVERRIDES},
        {"overrideDeletes", search_category::OVERRIDEDELETES},
        {"pinConnectionOverrides", search_category::PINCONNECTIONOVERRIDES},
        {"pinConnectionOverrideDeletes", search_category::PINCONNECTIONOVERRIDEDELETES},
        {"externalScenes", search_category::EXTERNALSCENES},
        {"subType", search_category::SUBTYPE},
        {"extraFactoryDependencies", search_category::EXTRAFACTORYDEPENDENCIES},
        {"extraBlueprintDependencies", search_category::EXTRABLUEPRINTDEPENDENCIES},
        {"comments", search_category::COMMENTS}
    };

    struct search_item {
        std::string search = "";
        search_type type = search_type::DEFAULT;
        search_category category = search_category::ALL;
        bool found = false;
    };

    entity();

    entity(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version, std::string& output_path);

    uint32_t search(std::vector<search_item>& search_items,
                    uint32_t results_count,
                    uint32_t max_results);
    bool search_json(yyjson_val* json, std::string& search_string);
    bool search_value(const search_item& item, const char* value);
    void free_yyjson_doc();
    bool find_ci(const char* s1, const char* s2);
    bool search_hash_ioi_string(const uint64_t hash_value, const search_item& item);

    uint32_t temp_rpkg_index = 0;
    uint32_t temp_hash_index = 0;
    uint32_t tblu_rpkg_index = 0;
    uint32_t tblu_hash_index = 0;
    yyjson_doc* entity_yyjson_doc = nullptr;
};
