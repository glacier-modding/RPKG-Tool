#include "rpkg_function.h"
#include "file.h"
#include "hash.h"
#include "generic_function.h"
#include "file_reader.hpp"
#include "global.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

bool rpkg_function::import_hash_meta_json(hash& meta_data, const std::string& input_path) {
    std::string input_json_string = file::read_file_to_string(input_path);

    rapidjson::Document document;
    rapidjson::ParseResult pr = document.Parse(input_json_string.c_str());

    if (!pr) {
        std::stringstream ss;
        ss << "Error: Parsing JSON" << "\n" << "JSON parse error : " << rapidjson::GetParseError_En(pr.Code()) << ", At offset : " << pr.Offset() << std::endl;
        LOG(ss.str());
        return false;
    }

    // hash_valud
    const rapidjson::Value& hash_value = document["hash_value"];
    if (hash_value.IsNull()) {
        LOG("Error: hash_value is missing from the JSON.");
        return false;
    }
    std::string temp_string = hash_value.GetString();
    meta_data.hash_value = util::ioi_string_to_hash(temp_string);

    // hash_offset
    const rapidjson::Value& hash_offset = document["hash_offset"];
    if (hash_offset.IsNull()) {
        LOG("Error: hash_offset is missing from the JSON.");
        return false;
    }
    meta_data.data.header.data_offset = hash_offset.GetUint64();

    // hash_size
    const rapidjson::Value& hash_size = document["hash_size"];
    if (hash_size.IsNull()) {
        LOG("Error: hash_size is missing from the JSON.");
        return false;
    }
    meta_data.data.header.data_size = hash_size.GetUint();

    // calculate LZ4 and XOR states
    if ((meta_data.data.header.data_size & 0x3FFFFFFF) != 0)
        meta_data.data.lz4ed = true;
    else
        meta_data.data.lz4ed = false;

    if ((meta_data.data.header.data_size & 0x80000000) == 0x80000000)
        meta_data.data.xored = true;
    else
        meta_data.data.xored = false;

    // hash_resource_type
    const rapidjson::Value& hash_resource_type = document["hash_resource_type"];
    if (hash_resource_type.IsNull()) {
        LOG("Error: hash_resource_type is missing from the JSON.");
        return false;
    }
    meta_data.hash_resource_type = hash_resource_type.GetString();

    // hash_reference_data
    const rapidjson::Value& hash_reference_data = document["hash_reference_data"];
    if (hash_reference_data.IsNull()) {
        LOG("Error: hash_reference_data is missing from the JSON.");
        return false;
    }
    if (!hash_reference_data.IsArray()) {
        LOG("Error: hash_reference_data is in the JSON but not an array.");
        return false;
    }
    uint32_t hash_reference_data_size = hash_reference_data.Size();

    // hash_reference_table_size
    if (hash_reference_data_size == 0) {
        meta_data.data.resource.reference_table_size = 0;
    }
    else {
        meta_data.data.resource.reference_table_size = hash_reference_data_size * static_cast<uint32_t>(0x9) + static_cast<uint32_t>(0x4);
    }

    // hash_reference_table_dummy
    const rapidjson::Value& hash_reference_table_dummy = document["hash_reference_table_dummy"];
    if (hash_reference_table_dummy.IsNull()) {
        LOG("Error: hash_reference_table_dummy is missing from the JSON.");
        return false;
    }
    meta_data.data.resource.reference_table_dummy = hash_reference_table_dummy.GetUint();

    // hash_size_final
    const rapidjson::Value& hash_size_final = document["hash_size_final"];
    if (hash_size_final.IsNull()) {
        LOG("Error: hash_size_final is missing from the JSON.");
        return false;
    }
    meta_data.data.resource.size_final = hash_size_final.GetUint();

    // hash_size_in_memory
    const rapidjson::Value& hash_size_in_memory = document["hash_size_in_memory"];
    if (hash_size_in_memory.IsNull()) {
        LOG("Error: hash_size_in_memory is missing from the JSON.");
        return false;
    }
    meta_data.data.resource.size_in_memory = hash_size_in_memory.GetUint();

    // hash_size_in_video_memory
    const rapidjson::Value& hash_size_in_video_memory = document["hash_size_in_video_memory"];
    if (hash_size_in_video_memory.IsNull()) {
        LOG("Error: hash_size_in_video_memory is missing from the JSON.");
        return false;
    }
    meta_data.data.resource.size_in_video_memory = hash_size_in_video_memory.GetUint();

    // hash_reference_data
    hash_reference_variables temp_hash_reference_data;
    if (hash_reference_data_size > 0) {
        temp_hash_reference_data.hash_value = meta_data.hash_value;
        temp_hash_reference_data.hash_reference_count = hash_reference_data_size | 0xC0000000;
        for (rapidjson::SizeType i = 0; i < hash_reference_data.Size(); i++) {
            if (hash_reference_data[i]["flag"].IsNull()) {
                LOG("Error: hash_reference_data[" + std::to_string(i) + "][\"flag\"] is missing from the JSON.");
                return false;
            }
            temp_hash_reference_data.hash_reference_type.push_back(std::strtol(hash_reference_data[i]["flag"].GetString(), nullptr, 16));
        }
        for (rapidjson::SizeType i = 0; i < hash_reference_data.Size(); i++) {
            if (hash_reference_data[i]["hash"].IsNull()) {
                LOG("Error: hash_reference_data[" + std::to_string(i) + "][\"hash\"] is missing from the JSON.");
                return false;
            }
            std::string hash_value_string = hash_reference_data[i]["hash"].GetString();
            if (hash_value_string.length() == 0) {
                LOG("Error: hash_reference_data[" + std::to_string(i) + "][\"hash\"] is empty in the JSON.");
                return false;
            }
            else if (util::is_valid_hash(hash_value_string)) {
                temp_string = hash_value_string;
            }
            else {
                temp_string = generic_function::compute_ioi_hash(hash_value_string);
            }
            temp_hash_reference_data.hash_reference.push_back(std::strtoull(temp_string.c_str(), nullptr, 16));
        }
    } else {
        temp_hash_reference_data.hash_reference_count = 0x0;
    }

    meta_data.hash_reference_data = temp_hash_reference_data;

    return true;
}