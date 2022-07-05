#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/prettywriter.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include "thirdparty/zhmtools/ResourceLib_HM2016.h"
#include "thirdparty/zhmtools/ResourceLib_HM2.h"
#include "thirdparty/zhmtools/ResourceLib_HM3.h"

#pragma comment(lib, "../rpkg_src/thirdparty/zhmtools/ResourceLib_HM2016.lib")
#pragma comment(lib, "../rpkg_src/thirdparty/zhmtools/ResourceLib_HM2.lib")
#pragma comment(lib, "../rpkg_src/thirdparty/zhmtools/ResourceLib_HM3.lib")

void rpkg_function::extract_to_rt_json(std::string& input_path, std::string& filter, std::string& version, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (!std::filesystem::is_regular_file(input_path))
    {
        LOG_AND_RETURN("Error: The input file path " + input_path + " is not a valid file.");
    }

    version = util::to_upper_case(version);

    if (version == "" || (version != "HM2016" && version != "HM2" && version != "HM3"))
    {
        LOG_AND_RETURN("Error: Version (HM2016, HM2, or HM3) must be passed via -version on the command line.");
    }

    std::vector<std::string> hm2016_valid_resources = { "TEMP", "AIRG", "TBLU", "CRMD", "ATMD", "CBLU", "CPPT", "DSWB", "GFXF", "GIDX", "VIDB", "WSGB" };
    std::vector<std::string> hm2_valid_resources = { "TEMP", "AIRG", "TBLU", "CRMD", "ATMD", "CBLU", "CPPT", "DSWB", "ECPB", "GFXF", "GIDX", "VIDB", "WSGB" };
    std::vector<std::string> hm3_valid_resources = { "TEMP", "AIRG", "TBLU", "CRMD", "ATMD", "CBLU", "CPPT", "DSWB", "ECPB", "GFXF", "GIDX", "VIDB", "WSGB" };

    if (file::path_exists(input_path))
    {
        rpkg_function::import_rpkg(input_path, true);

        std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t text_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            for (uint64_t i = 0; i < rpkgs.size(); i++)
            {
                uint64_t rpkg_index = i;

                if (rpkgs.at(i).rpkg_file_path == input_path)
                {
                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(text_hash_value);

                    if (it != rpkgs.at(rpkg_index).hash_map.end())
                    {
                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        if (version == "HM2016")
                        {
                            if (std::find(hm2016_valid_resources.begin(), hm2016_valid_resources.end(), rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type) == hm2016_valid_resources.end())
                            {
                                LOG_AND_RETURN("Error: " + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + " is not a valid resource type for ResourceLib for HM2016.");
                            }
                        }
                        else if (version == "HM2")
                        {
                            if (std::find(hm2_valid_resources.begin(), hm2_valid_resources.end(), rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type) == hm2_valid_resources.end())
                            {
                                LOG_AND_RETURN("Error: " + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + " is not a valid resource type for ResourceLib for HM2.");
                            }
                        }
                        else if (version == "HM3")
                        {
                            if (std::find(hm3_valid_resources.begin(), hm3_valid_resources.end(), rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type) == hm3_valid_resources.end())
                            {
                                LOG_AND_RETURN("Error: " + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + " is not a valid resource type for ResourceLib for HM3.");
                            }
                        }

                        uint64_t temp_hash_size;

                        if (rpkgs.at(rpkg_index).hash.at(it->second).is_lz4ed == 1)
                        {
                            temp_hash_size = rpkgs.at(rpkg_index).hash.at(it->second).hash_size;

                            if (rpkgs.at(rpkg_index).hash.at(it->second).is_xored == 1)
                            {
                                temp_hash_size &= 0x3FFFFFFF;
                            }
                        }
                        else
                        {
                            temp_hash_size = rpkgs.at(rpkg_index).hash.at(it->second).hash_size_final;
                        }

                        std::vector<char> temp_input_data = std::vector<char>(temp_hash_size, 0);

                        std::ifstream file = std::ifstream(rpkgs.at(rpkg_index).rpkg_file_path, std::ifstream::binary);

                        if (!file.good())
                        {
                            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(rpkg_index).rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkgs.at(rpkg_index).hash.at(it->second).hash_offset, file.beg);
                        file.read(temp_input_data.data(), temp_hash_size);
                        file.close();

                        if (rpkgs.at(rpkg_index).hash.at(it->second).is_xored == 1)
                        {
                            crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
                        }

                        uint32_t temp_decompressed_size = rpkgs.at(rpkg_index).hash.at(it->second).hash_size_final;

                        std::vector<char> temp_output_data = std::vector<char>(temp_decompressed_size, 0);

                        std::vector<char> temp_data;

                        if (rpkgs.at(rpkg_index).hash.at(it->second).is_lz4ed)
                        {
                            LZ4_decompress_safe(temp_input_data.data(), temp_output_data.data(), (int)temp_hash_size, temp_decompressed_size);

                            temp_data = temp_output_data;
                        }
                        else
                        {
                            temp_data = temp_input_data;
                        }

                        std::vector<char>().swap(temp_output_data);
                        std::vector<char>().swap(temp_input_data);

                        ResourceConverter* resource_tool_converter = nullptr;

                        if (version == "HM2016")
                        {
                            resource_tool_converter = HM2016_GetConverterForResource(rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type.c_str());
                        }
                        else if (version == "HM2")
                        {
                            resource_tool_converter = HM2_GetConverterForResource(rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type.c_str());
                        }
                        else if (version == "HM3")
                        {
                            resource_tool_converter = HM3_GetConverterForResource(rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type.c_str());
                        }

                        JsonString* temp_json_input = nullptr;

                        temp_json_input = resource_tool_converter->FromMemoryToJsonString((const void*)temp_data.data(), (size_t)temp_data.size());

                        rapidjson::StringStream ss(temp_json_input->JsonData);

                        rapidjson::Document temp_json_document;

                        temp_json_document.ParseStream(ss);

                        resource_tool_converter->FreeJsonString(temp_json_input);

                        std::ofstream file_temp = std::ofstream(file::output_path_append(rpkgs.at(rpkg_index).hash.at(it->second).hash_file_name + ".JSON", output_path), std::ofstream::binary);

                        rapidjson::StringBuffer buffer_temp;
                        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer_temp(buffer_temp);
                        temp_json_document.Accept(writer_temp);

                        file_temp.write(buffer_temp.GetString(), buffer_temp.GetSize());

                        file_temp.close();
                    }
                }
            }
        }

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The RPKG file " + input_path + " does not exist.");
    }
}