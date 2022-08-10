#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include "thirdparty/zhmtools/ResourceLib_HM2.h"
#include "thirdparty/zhmtools/ResourceLib_HM3.h"
#include "thirdparty/yyjson/yyjson.h"

class entity
{
public:
	entity();
	entity(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version);

	uint32_t search(std::string search_string, bool search_entity_ids, bool search_entity_names, bool search_property_names, bool search_property_values, uint32_t results_count, uint32_t max_results);

	uint32_t temp_rpkg_index = 0;
	uint32_t temp_hash_index = 0;
	uint32_t tblu_rpkg_index = 0;
	uint32_t tblu_hash_index = 0;
	yyjson_doc* temp_yyjson_doc = nullptr;
	yyjson_doc* tblu_yyjson_doc = nullptr;
};