#include "rpkg_function.h"
#include "global.h"
#include "generic_function.h"
#include "file.h"
#include "util.h"
#include <fstream>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

std::vector<char> rpkg_function::json_to_hash_meta(std::string& json_string) {
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::vector<char> meta_data;

    char char1[1];
    char char4[4];
    char char8[8];

    rapidjson::Document document;

    document.Parse(json_string.c_str());

    const rapidjson::Value& hash_value = document["hash_value"];

    if (hash_value.IsNull()) {
        LOG("Error: hash_value is missing from the JSON.");

        response_string = "Error: hash_value is missing from the JSON.";

        return {};
    }

    std::cout << hash_value.GetString() << std::endl;

    std::string temp_string = hash_value.GetString();

    if (!util::is_valid_hash(temp_string)) {
        temp_string = generic_function::compute_ioi_hash(temp_string);
    }

    uint64_t temp_uint64_t = std::strtoull(temp_string.c_str(), nullptr, 16);

    std::memcpy(&char8, &temp_uint64_t, 0x8);

    for (char& j : char8) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_offset = document["hash_offset"];

    if (hash_offset.IsNull()) {
        LOG("Error: hash_offset is missing from the JSON.");

        response_string = "Error: hash_offset is missing from the JSON.";

        return {};
    }

    std::cout << hash_offset.GetUint64() << std::endl;

    temp_uint64_t = hash_offset.GetUint64();

    std::memcpy(&char8, &temp_uint64_t, 0x8);

    for (char& j : char8) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_size = document["hash_size"];

    if (hash_size.IsNull()) {
        LOG("Error: hash_size is missing from the JSON.");

        response_string = "Error: hash_size is missing from the JSON.";

        return {};
    }

    std::cout << hash_size.GetUint() << std::endl;

    uint32_t temp_uint32_t = hash_size.GetUint();

    std::memcpy(&char4, &temp_uint32_t, 0x4);

    for (char& j : char4) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_resource_type = document["hash_resource_type"];

    if (hash_resource_type.IsNull()) {
        LOG("Error: hash_resource_type is missing from the JSON.");

        response_string = "Error: hash_resource_type is missing from the JSON.";

        return {};
    }

    std::cout << hash_resource_type.GetString() << std::endl;

    temp_string = hash_resource_type.GetString();

    for (char& j : temp_string) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_reference_data = document["hash_reference_data"];

    if (hash_reference_data.IsNull()) {
        LOG("Error: hash_reference_data is missing from the JSON.");

        response_string = "Error: hash_reference_data is missing from the JSON.";

        return {};
    }

    if (!hash_reference_data.IsArray()) {
        LOG("Error: hash_reference_data is in the JSON but not an array.");

        response_string = "Error: hash_reference_data is in the JSON but not an array.";

        return {};
    }

    uint32_t hash_reference_data_size = hash_reference_data.Size();

    const rapidjson::Value& hash_reference_table_size = document["hash_reference_table_size"];

    if (hash_reference_table_size.IsNull()) {
        LOG("Error: hash_reference_table_size is missing from the JSON.");

        response_string = "Error: hash_reference_table_size is missing from the JSON.";

        return {};
    }

    std::cout << hash_reference_table_size.GetUint() << std::endl;

    if (hash_reference_data_size == 0) {
        temp_uint32_t = 0;
    } else {
        temp_uint32_t = hash_reference_data_size * static_cast<uint32_t>(0x9) + static_cast<uint32_t>(0x4);
    }

    std::memcpy(&char4, &temp_uint32_t, 0x4);

    for (char& j : char4) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_reference_table_dummy = document["hash_reference_table_dummy"];

    if (hash_reference_table_dummy.IsNull()) {
        LOG("Error: hash_reference_table_dummy is missing from the JSON.");

        response_string = "Error: hash_reference_table_dummy is missing from the JSON.";

        return {};
    }

    std::cout << hash_reference_table_dummy.GetUint() << std::endl;

    temp_uint32_t = hash_reference_table_dummy.GetUint();

    std::memcpy(&char4, &temp_uint32_t, 0x4);

    for (char& j : char4) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_size_final = document["hash_size_final"];

    if (hash_size_final.IsNull()) {
        LOG("Error: hash_size_final is missing from the JSON.");

        response_string = "Error: hash_size_final is missing from the JSON.";

        return {};
    }

    std::cout << hash_size_final.GetUint() << std::endl;

    temp_uint32_t = hash_size_final.GetUint();

    std::memcpy(&char4, &temp_uint32_t, 0x4);

    for (char& j : char4) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_size_in_memory = document["hash_size_in_memory"];

    if (hash_size_in_memory.IsNull()) {
        LOG("Error: hash_size_in_memory is missing from the JSON.");

        response_string = "Error: hash_size_in_memory is missing from the JSON.";

        return {};
    }

    std::cout << hash_size_in_memory.GetUint() << std::endl;

    temp_uint32_t = hash_size_in_memory.GetUint();

    std::memcpy(&char4, &temp_uint32_t, 0x4);

    for (char& j : char4) {
        meta_data.push_back(j);
    }

    const rapidjson::Value& hash_size_in_video_memory = document["hash_size_in_video_memory"];

    if (hash_size_in_video_memory.IsNull()) {
        LOG("Error: hash_size_in_video_memory is missing from the JSON.");

        response_string = "Error: hash_size_in_video_memory is missing from the JSON.";

        return {};
    }

    std::cout << hash_size_in_video_memory.GetUint() << std::endl;

    temp_uint32_t = hash_size_in_video_memory.GetUint();

    std::memcpy(&char4, &temp_uint32_t, 0x4);

    for (char& j : char4) {
        meta_data.push_back(j);
    }

    if (hash_reference_data_size > 0) {
        temp_uint32_t = hash_reference_data_size | 0xC0000000;

        std::memcpy(&char4, &temp_uint32_t, 0x4);

        for (char& j : char4) {
            meta_data.push_back(j);
        }

        for (rapidjson::SizeType i = 0; i < hash_reference_data.Size(); i++) {
            if (hash_reference_data[i]["flag"].IsNull()) {
                LOG("Error: hash_reference_data[" + std::to_string(i) + "][\"flag\"] is missing from the JSON.");

                response_string =
                        "Error: hash_reference_data[" + std::to_string(i) + "][\"flag\"] is missing from the JSON.";

                return {};
            }

            std::cout << hash_reference_data[i]["flag"].GetString() << std::endl;

            uint8_t temp_uint8_t = std::strtol(hash_reference_data[i]["flag"].GetString(), nullptr, 16);

            std::memcpy(&char1, &temp_uint8_t, 0x1);

            meta_data.push_back(char1[0]);
        }

        for (rapidjson::SizeType i = 0; i < hash_reference_data.Size(); i++) {
            if (hash_reference_data[i]["hash"].IsNull()) {
                LOG("Error: hash_reference_data[" + std::to_string(i) + "][\"hash\"] is missing from the JSON.");

                response_string =
                        "Error: hash_reference_data[" + std::to_string(i) + "][\"hash\"] is missing from the JSON.";

                return {};
            }

            std::cout << hash_reference_data[i]["hash"].GetString() << std::endl;

            std::string hash_value_string = hash_reference_data[i]["hash"].GetString();

            if (hash_value_string.length() == 0) {
                LOG("Error: hash_reference_data[" + std::to_string(i) + "][\"hash\"] is empty in the JSON.");

                response_string =
                    "Error: hash_reference_data[" + std::to_string(i) + "][\"hash\"] is empty in the JSON.";

                return {};
            } else if (util::is_valid_hash(hash_value_string)) {
                temp_string = hash_value_string;
            } else {
                temp_string = generic_function::compute_ioi_hash(hash_value_string);
            }

            temp_uint64_t = std::strtoull(temp_string.c_str(), nullptr, 16);

            std::memcpy(&char8, &temp_uint64_t, 0x8);

            for (char& j : char8) {
                meta_data.push_back(j);
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;

    return meta_data;
}