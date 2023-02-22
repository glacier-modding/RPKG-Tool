#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include "thirdparty/zhmtools/ResourceLib_HM2.h"
#include "thirdparty/zhmtools/ResourceLib_HM3.h"
#include "thirdparty/yyjson/yyjson.h"

enum search_enums {
    DEFAULT,
    REGEX,
    NONE,
    AND,
    OR
};

struct search_item {
    std::string search = "";
    search_enums type = DEFAULT;
    search_enums operation = NONE;
};

class entity {
public:
    entity();

    entity(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version, std::string& output_path);

    uint32_t search(std::vector<search_item>& search_items_input,
                    uint32_t results_count,
                    uint32_t max_results);
    bool search_json(yyjson_val* json, std::string& search_string);
    void search_value(bool& found, const char* value);
    void free_yyjson_doc();
    bool find_ci(const char* s1, const char* s2);

    uint32_t temp_rpkg_index = 0;
    uint32_t temp_hash_index = 0;
    uint32_t tblu_rpkg_index = 0;
    uint32_t tblu_hash_index = 0;
    yyjson_doc* entity_yyjson_doc = nullptr;
    std::vector<search_item> search_items;
};
