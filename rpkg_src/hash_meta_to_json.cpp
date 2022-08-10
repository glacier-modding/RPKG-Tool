#include "rpkg_function.h"
#include "global.h"
#include "generic_function.h"
#include "file.h"
#include "util.h"
#include "crypto.h"
#include "temp.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/writer.h"
#include "thirdparty/rapidjson/prettywriter.h"
#include "thirdparty/rapidjson/pointer.h"
#include "thirdparty/rapidjson/stringbuffer.h"

void rpkg_function::hash_meta_to_json(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    if (file::path_exists(input_path))
    {
        std::ifstream meta_file = std::ifstream(input_path, std::ifstream::binary);

        if (!meta_file.good())
        {
            LOG_AND_EXIT("Error: Hash meta file " + input_path + " could not be read.");
        }

        uint8_t bytes1 = 0;
        uint32_t bytes4 = 0;
        uint64_t bytes8 = 0;
        char input[1024];

        hash meta_data;

        std::string json_string = "{";

        meta_file.seekg(0, meta_file.beg);

        meta_file.read(input, sizeof(bytes8));
        std::memcpy(&bytes8, input, sizeof(bytes8));
        meta_data.hash_value = bytes8;
        json_string += "\"hash_value\":\"" + util::uint64_t_to_hex_string(meta_data.hash_value) + "\",";

        meta_file.read(input, sizeof(bytes8));
        std::memcpy(&bytes8, input, sizeof(bytes8));
        meta_data.data.header.data_offset = bytes8;
        json_string += "\"hash_offset\":" + util::uint64_t_to_string(meta_data.data.header.data_offset) + ",";

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        meta_data.data.header.data_size = bytes4;
        json_string += "\"hash_size\":" + util::uint32_t_to_string(meta_data.data.header.data_size) + ",";

        if ((meta_data.data.header.data_size & 0x3FFFFFFF) != 0)
        {
            meta_data.data.lz4ed = true;
        }
        else
        {
            meta_data.data.lz4ed = false;
        }

        if ((meta_data.data.header.data_size & 0x80000000) == 0x80000000)
        {
            meta_data.data.xored = true;
        }
        else
        {
            meta_data.data.xored = false;
        }

        meta_file.read(input, sizeof(bytes4));
        input[4] = 0x0;
        meta_data.hash_resource_type = std::string(input);
        json_string += "\"hash_resource_type\":\"" + meta_data.hash_resource_type + "\",";

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        meta_data.data.resource.reference_table_size = bytes4;
        json_string += "\"hash_reference_table_size\":" + util::uint32_t_to_string(meta_data.data.resource.reference_table_size) + ",";

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        meta_data.data.resource.reference_table_dummy = bytes4;
        json_string += "\"hash_reference_table_dummy\":" + util::uint32_t_to_string(meta_data.data.resource.reference_table_dummy) + ",";

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        meta_data.data.resource.size_final = bytes4;
        json_string += "\"hash_size_final\":" + util::uint32_t_to_string(meta_data.data.resource.size_final) + ",";

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        meta_data.data.resource.size_in_memory = bytes4;
        json_string += "\"hash_size_in_memory\":" + util::uint32_t_to_string(meta_data.data.resource.size_in_memory) + ",";

        meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        meta_data.data.resource.size_in_video_memory = bytes4;
        json_string += "\"hash_size_in_video_memory\":" + util::uint32_t_to_string(meta_data.data.resource.size_in_video_memory) + ",";

        json_string += "\"hash_reference_data\":[";

        hash_reference_variables temp_hash_reference_data;

        if (meta_data.data.resource.reference_table_size != 0x0)
        {
            temp_hash_reference_data.hash_value = meta_data.hash_value;

            meta_file.read(input, sizeof(bytes4));
            std::memcpy(&bytes4, input, sizeof(bytes4));
            temp_hash_reference_data.hash_reference_count = bytes4;

            uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint64_t j = 0; j < temp_hash_reference_count; j++)
            {
                meta_file.read(input, sizeof(bytes1));
                std::memcpy(&bytes1, input, sizeof(bytes1));
                temp_hash_reference_data.hash_reference_type.push_back(bytes1);
            }

            for (uint64_t j = 0; j < temp_hash_reference_count; j++)
            {
                meta_file.read(input, sizeof(bytes8));
                std::memcpy(&bytes8, input, sizeof(bytes8));
                temp_hash_reference_data.hash_reference.push_back(bytes8);

                std::string hash_value_string = util::uint64_t_to_hex_string(bytes8);

                std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(bytes8);

                if (it2 != hash_list_hash_map.end())
                {
                    if (hash_value_string == generic_function::compute_ioi_hash_string(hash_list_hash_strings.at(it2->second)))
                    {
                        json_string += "{\"hash\":\"" + hash_list_hash_strings.at(it2->second) + "\",";
                    }
                    else
                    {
                        json_string += "{\"hash\":\"" + util::uint64_t_to_hex_string(bytes8) + "\",";
                    }
                }
                else
                {
                    json_string += "{\"hash\":\"" + util::uint64_t_to_hex_string(bytes8) + "\",";
                }
                                
                json_string += "\"flag\":\"" + util::uint8_t_to_hex_string(temp_hash_reference_data.hash_reference_type.at(j)) + "\"}";

                if (j < temp_hash_reference_count - 1)
                {
                    json_string += ",";
                }
            }
        }
        else
        {
            temp_hash_reference_data.hash_reference_count = 0x0;
        }

        json_string += "]}";

        meta_data.hash_reference_data = temp_hash_reference_data;

        meta_file.close();

        rapidjson::Document document;
        document.Parse(json_string.c_str());

        rapidjson::StringBuffer strbuf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);

        document.Accept(writer);

        std::ofstream file_temp = std::ofstream(input_path + ".JSON", std::ofstream::binary);

        LOG("Success: " + input_path + " has been converted.");

        response_string = "Success: " + input_path + " has been converted.";

        file_temp.write(strbuf.GetString(), strbuf.GetSize());

        file_temp.close();
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}