#include "temp.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/prettywriter.h"
#include "thirdparty/rapidjson/writer.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/pointer.h"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <set>

#pragma comment(lib, "../rpkg_src/thirdparty/zhmtools/ResourceLib_HM2.lib")
#pragma comment(lib, "../rpkg_src/thirdparty/zhmtools/ResourceLib_HM3.lib")

temp::temp()
{
    
}

temp::temp(uint64_t rpkgs_index, uint64_t hash_index)
{
#ifdef RPKG_DLL
    //util::initialize_resource_tool();
#endif
    
    initialize_enum_map_h2();
    initialize_enum_map_h3();

    initialize_type_map_h2();
    initialize_type_map_h3();

	temp_rpkg_index = rpkgs_index;
	temp_hash_index = hash_index;

    temp_file_name = rpkgs.at(rpkgs_index).hash.at(hash_index).hash_file_name;

    tblu_return_value = TEMP_TBLU_NOT_FOUND_IN_DEPENDS;

    load_hash_depends();
}

temp::temp(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version)
{
    temp_file_version = temp_version;

#ifdef RPKG_DLL
    //util::initialize_resource_tool();
#endif

    initialize_enum_map_h2();
    initialize_enum_map_h3();

    initialize_type_map_h2();
    initialize_type_map_h3();

    temp_rpkg_index = rpkgs_index;
    temp_hash_index = hash_index;

    temp_file_name = rpkgs.at(rpkgs_index).hash.at(hash_index).hash_file_name;

    tblu_return_value = TEMP_TBLU_NOT_FOUND_IN_DEPENDS;

    load_hash_depends();
}

void temp::load_data()
{
    load_temp_data();

#ifdef RPKG_DLL
    const rapidjson::Value& temp_json_subEntities = temp_json_document["subEntities"];

    temp_subentity_count = temp_json_subEntities.Size();

    for (rapidjson::SizeType i = 0; i < temp_json_subEntities.Size(); i++)
    {
        rapidjson::Value::ConstMemberIterator it1 = temp_json_subEntities[i].FindMember("logicalParent");

        if (it1 != temp_json_subEntities[i].MemberEnd())
        {
            rapidjson::Value::ConstMemberIterator it2 = it1->value.FindMember("entityIndex");

            if (it2 != it1->value.MemberEnd())
            {
                temp_logicalParent.push_back(it2->value.GetInt());
            }
        }

        tblu_entityName.push_back("");

        rapidjson::Value::ConstMemberIterator it2 = temp_json_subEntities[i].FindMember("entityTypeResourceIndex");

        if (it2 != temp_json_subEntities[i].MemberEnd())
        {
            temp_entityTypeResourceIndex.push_back(it2->value.GetInt());

            tblu_entityTypeResourceIndex.push_back(0);
        }
    }
#endif

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        load_tblu_data();

#ifdef RPKG_DLL
        const rapidjson::Value& tblu_json_subEntities = tblu_json_document["subEntities"];

        tblu_subentity_count = tblu_json_subEntities.Size();

        for (rapidjson::SizeType i = 0; i < tblu_json_subEntities.Size(); i++)
        {
            if (i < temp_logicalParent.size())
            {
                rapidjson::Value::ConstMemberIterator it1 = tblu_json_subEntities[i].FindMember("entityName");

                if (it1 != tblu_json_subEntities[i].MemberEnd())
                {
                    tblu_entityName.at(i) = it1->value.GetString();
                }

                rapidjson::Value::ConstMemberIterator it2 = tblu_json_subEntities[i].FindMember("entityTypeResourceIndex");

                if (it2 != tblu_json_subEntities[i].MemberEnd())
                {
                    tblu_entityTypeResourceIndex.at(i) = it2->value.GetInt();
                }
            }
        }
#endif

    }
}

void temp::load_temp_data()
{
    //std::cout << rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name << std::endl;
    //std::cout << rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_file_name << std::endl;

    uint64_t temp_hash_size;

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_lz4ed == 1)
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size;

        if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_xored == 1)
        {
            temp_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size_final;
    }

    temp_input_data = std::vector<char>(temp_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(temp_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(temp_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_offset, file.beg);
    file.read(temp_input_data.data(), temp_hash_size);
    file.close();

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_xored == 1)
    {
        crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
    }

    uint32_t temp_decompressed_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size_final;

    temp_output_data = std::vector<char>(temp_decompressed_size, 0);

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_lz4ed)
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

    std::string type = "TEMP";

    //if (temp_file_version == 2)
    //{
        //type = "TEMPH2";
    //}

#ifdef RPKG_DLL
    if (temp_file_version == 2)
    {
        resource_tool_converter = HM2_GetConverterForResource("TEMP");
    }
    else
    {
        resource_tool_converter = HM3_GetConverterForResource("TEMP");
    }

    temp_json_input = resource_tool_converter->FromMemoryToJsonString((const void*)temp_data.data(), (size_t)temp_data.size(), true);

    //resource_tool_ConvertMemoryResourceToJson(&type[0], (void*)temp_data.data(), (uint64_t)temp_data.size());

    //temp_json_input = resource_tool_GetJsonFromMemory();

    rapidjson::StringStream ss(temp_json_input->JsonData);

    temp_json_document.ParseStream(ss);

    resource_tool_converter->FreeJsonString(temp_json_input);
#endif
}

void temp::load_tblu_data()
{
    std::string tblu_hash_file_name = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_file_name;

    uint64_t tblu_hash_size;

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_lz4ed == 1)
    {
        tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_size;

        if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_xored == 1)
        {
            tblu_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_size_final;
    }

    tblu_input_data = std::vector<char>(tblu_hash_size, 0);

    std::ifstream file2 = std::ifstream(rpkgs.at(tblu_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file2.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(tblu_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file2.seekg(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_offset, file2.beg);
    file2.read(tblu_input_data.data(), tblu_hash_size);
    file2.close();

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_xored == 1)
    {
        crypto::xor_data(tblu_input_data.data(), (uint32_t)tblu_hash_size);
    }

    uint32_t tblu_decompressed_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_size_final;

    tblu_output_data = std::vector<char>(tblu_decompressed_size, 0);

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_lz4ed)
    {
        LZ4_decompress_safe(tblu_input_data.data(), tblu_output_data.data(), (int)tblu_hash_size, tblu_decompressed_size);

        tblu_data = tblu_output_data;
    }
    else
    {
        tblu_data = tblu_input_data;
    }

    std::vector<char>().swap(tblu_output_data);
    std::vector<char>().swap(tblu_input_data);

    std::string type = "TBLU";

#ifdef RPKG_DLL
    if (temp_file_version == 2)
    {
        resource_tool_converter = HM2_GetConverterForResource("TBLU");
    }
    else
    {
        resource_tool_converter = HM3_GetConverterForResource("TBLU");
    }

    tblu_json_input = resource_tool_converter->FromMemoryToJsonString((const void*)tblu_data.data(), (size_t)tblu_data.size(), true);

    //resource_tool_ConvertMemoryResourceToJson(&type[0], (void*)tblu_data.data(), (uint64_t)tblu_data.size());

    //tblu_json_input = resource_tool_GetJsonFromMemory();

    rapidjson::StringStream ss(tblu_json_input->JsonData);

    tblu_json_document.ParseStream(ss);

    resource_tool_converter->FreeJsonString(tblu_json_input);
#endif
}

void temp::load_hash_depends()
{
    uint32_t temp_hash_reference_count = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    //LOG(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(temp_rpkg_index).rpkg_file_path);

    if (temp_hash_reference_count > 0)
    {
        for (uint64_t i = 0; i < temp_hash_reference_count; i++)
        {
            std::vector<std::string> temp_temp_depends_in_rpkgs;
            std::vector<uint32_t> temp_temp_depends_rpkg_index;
            std::vector<uint32_t> temp_temp_depends_hash_index;

            std::vector<std::string> temp_tblu_depends_in_rpkgs;
            std::vector<uint32_t> temp_tblu_depends_rpkg_index;
            std::vector<uint32_t> temp_tblu_depends_hash_index;

            std::vector<std::string> temp_prim_depends_in_rpkgs;
            std::vector<uint32_t> temp_prim_depends_rpkg_index;
            std::vector<uint32_t> temp_prim_depends_hash_index;

            bool temp_found = false;
            bool tblu_found = false;
            bool prim_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end())
                {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TEMP")
                    {
                        if (!temp_found)
                        {
                            temp_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            temp_depends_index.push_back(i);

                            ////LOG("Found TEMP hash depend:");
                            ////LOG("  - File Name: " + temp_depends_file_name.back());
                        }

                        temp_found = true;

                        temp_temp_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_temp_depends_rpkg_index.push_back(j);
                        temp_temp_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_temp_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_temp_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_temp_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TBLU")
                    {
                        if (!tblu_found)
                        {
                            tblu_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            tblu_depends_index.push_back(i);

                            tblu_return_value = TEMP_TBLU_FOUND;

                            ////LOG("Found TEMP hash depend:");
                            ////LOG("  - File Name: " + temp_depends_file_name.back());
                        }
                        else
                        {
                            if (rpkgs.at(j).hash.at(it->second).hash_file_name != tblu_depends_file_name.back())
                            {
                                tblu_return_value = TEMP_TBLU_TOO_MANY;
                            }
                        }

                        tblu_found = true;

                        temp_tblu_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_tblu_depends_rpkg_index.push_back(j);
                        temp_tblu_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_tblu_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_tblu_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_tblu_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "PRIM")
                    {
                        if (!prim_found)
                        {
                            prim_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            prim_depends_index.push_back(i);

                            ////LOG("Found PRIM hash depend:");
                            ////LOG("  - File Name: " + prim_depends_file_name.back());
                        }

                        prim_found = true;

                        temp_prim_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_prim_depends_rpkg_index.push_back(j);
                        temp_prim_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_prim_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_prim_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_prim_depends_hash_index.back()));
                    }
                }
            }

            uint32_t temp_value = 0;

            if (temp_found)
            {
                temp_depends_in_rpkgs.push_back(temp_temp_depends_in_rpkgs);
                temp_depends_rpkg_index.push_back(temp_temp_depends_rpkg_index);
                temp_depends_hash_index.push_back(temp_temp_depends_hash_index);
                temp_depends_in_rpkgs_index.push_back(temp_value);
                temp_depends_rpkg_index_index.push_back(temp_value);
                temp_depends_hash_index_index.push_back(temp_value);
            }

            if (tblu_found)
            {
                tblu_depends_in_rpkgs.push_back(temp_tblu_depends_in_rpkgs);
                tblu_depends_rpkg_index.push_back(temp_tblu_depends_rpkg_index);
                tblu_depends_hash_index.push_back(temp_tblu_depends_hash_index);                
                tblu_depends_in_rpkgs_index.push_back(temp_value);
                tblu_depends_rpkg_index_index.push_back(temp_value);
                tblu_depends_hash_index_index.push_back(temp_value);
            }

            if (prim_found)
            {
                prim_depends_in_rpkgs.push_back(temp_prim_depends_in_rpkgs);
                prim_depends_rpkg_index.push_back(temp_prim_depends_rpkg_index);
                prim_depends_hash_index.push_back(temp_prim_depends_hash_index);
                prim_depends_in_rpkgs_index.push_back(temp_value);
                prim_depends_rpkg_index_index.push_back(temp_value);
                prim_depends_hash_index_index.push_back(temp_value);
            }
        }
    }

    for (uint64_t k = 0; k < temp_depends_file_name.size(); k++)
    {
        //LOG("  - TEMP File Name: " + temp_depends_file_name.at(k));

        bool temp_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < temp_depends_in_rpkgs.at(k).size(); d++)
        {
            std::string temp_depends_in_rpkgs_upper_case = util::to_upper_case(temp_depends_in_rpkgs.at(k).at(d));

            std::string_view temp_depends_in_rpkgs_string_view(temp_depends_in_rpkgs_upper_case.c_str(), temp_depends_in_rpkgs_upper_case.length());

            size_t pos1 = temp_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos)
            {
                size_t pos2 = temp_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos)
                {
                    temp_patch_name_found = true;

                    long new_patch_level = std::strtol(std::string(temp_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level)
                    {
                        patch_level = new_patch_level;

                        temp_depends_in_rpkgs_index.at(k) = d;
                        temp_depends_rpkg_index_index.at(k) = d;
                        temp_depends_hash_index_index.at(k) = d;
                    }
                }
            }
            else
            {
                if (!temp_patch_name_found)
                {
                    temp_depends_in_rpkgs_index.at(k) = d;
                    temp_depends_rpkg_index_index.at(k) = d;
                    temp_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - TEMP File Name In RPKG: " + temp_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - TEMP File Name In RPKG Used: " + temp_depends_in_rpkgs.at(k).at(temp_depends_rpkg_index_index.at(k)));
    }

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        //LOG("  - TBLU Return Value: TEMP_TBLU_FOUND");

        for (uint64_t k = 0; k < tblu_depends_file_name.size(); k++)
        {
            //LOG("  - TBLU File Name: " + tblu_depends_file_name.at(k));

            bool tblu_patch_name_found = false;

            long patch_level = 0;

            for (uint64_t d = 0; d < tblu_depends_in_rpkgs.at(k).size(); d++)
            {
                std::string tblu_depends_in_rpkgs_upper_case = util::to_upper_case(tblu_depends_in_rpkgs.at(k).at(d));

                std::string_view tblu_depends_in_rpkgs_string_view(tblu_depends_in_rpkgs_upper_case.c_str(), tblu_depends_in_rpkgs_upper_case.length());

                size_t pos1 = tblu_depends_in_rpkgs_string_view.find("PATCH");

                if (pos1 != std::string::npos)
                {
                    size_t pos2 = tblu_depends_in_rpkgs_string_view.substr(pos1).find(".");

                    if (pos2 != std::string::npos)
                    {
                        tblu_patch_name_found = true;

                        long new_patch_level = std::strtol(std::string(tblu_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                        if (new_patch_level > patch_level)
                        {
                            patch_level = new_patch_level;

                            tblu_depends_in_rpkgs_index.at(k) = d;
                            tblu_depends_rpkg_index_index.at(k) = d;
                            tblu_depends_hash_index_index.at(k) = d;
                        }
                    }
                }
                else
                {
                    if (!tblu_patch_name_found)
                    {
                        tblu_depends_in_rpkgs_index.at(k) = d;
                        tblu_depends_rpkg_index_index.at(k) = d;
                        tblu_depends_hash_index_index.at(k) = d;
                    }
                }

                //LOG("  - TBLU File Name In RPKG: " + tblu_depends_in_rpkgs.at(k).at(d));
            }

            //LOG("  - TBLU File Name In RPKG Used: " + tblu_depends_in_rpkgs.at(k).at(tblu_depends_rpkg_index_index.at(k)));

            tblu_rpkg_index = tblu_depends_rpkg_index.at(k).at(tblu_depends_rpkg_index_index.at(k));
            tblu_hash_index = tblu_depends_hash_index.at(k).at(tblu_depends_hash_index_index.at(k));
        }
    }

    for (uint64_t k = 0; k < prim_depends_file_name.size(); k++)
    {
        //LOG("  - PRIM File Name: " + prim_depends_file_name.at(k));

        bool prim_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < prim_depends_in_rpkgs.at(k).size(); d++)
        {
            std::string prim_depends_in_rpkgs_upper_case = util::to_upper_case(prim_depends_in_rpkgs.at(k).at(d));

            std::string_view prim_depends_in_rpkgs_string_view(prim_depends_in_rpkgs_upper_case.c_str(), prim_depends_in_rpkgs_upper_case.length());

            size_t pos1 = prim_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos)
            {
                size_t pos2 = prim_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos)
                {
                    prim_patch_name_found = true;

                    long new_patch_level = std::strtol(std::string(prim_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level)
                    {
                        patch_level = new_patch_level;

                        prim_depends_in_rpkgs_index.at(k) = d;
                        prim_depends_rpkg_index_index.at(k) = d;
                        prim_depends_hash_index_index.at(k) = d;
                    }
                }
            }
            else
            {
                if (!prim_patch_name_found)
                {
                    prim_depends_in_rpkgs_index.at(k) = d;
                    prim_depends_rpkg_index_index.at(k) = d;
                    prim_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - PRIM File Name In RPKG: " + prim_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - PRIM File Name In RPKG Used: " + prim_depends_in_rpkgs.at(k).at(prim_depends_rpkg_index_index.at(k)));
    }
}

void temp::get_prim_from_temp(uint32_t entry_index)
{
    response_string = "";

    std::map<uint64_t, uint32_t>::iterator it = temps_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(entry_index)));

    if (it != temps_map.end())
    {
        for (uint64_t k = 0; k < temps.at(it->second).prim_depends_file_name.size(); k++)
        {
            if (temps.at(it->second).prim_depends_in_rpkgs.at(k).size() > 0)
            {
                response_string.append(temps.at(it->second).prim_depends_file_name.at(k));
                response_string.push_back('|');
                response_string.append(temps.at(it->second).prim_depends_in_rpkgs.at(k).at(temps.at(it->second).prim_depends_in_rpkgs_index.at(k)));
                response_string.push_back(',');
            }
        }
    }
}

void temp::temp_version_check()
{
    uint64_t temp_hash_size;

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_lz4ed == 1)
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size;

        if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_xored == 1)
        {
            temp_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size_final;
    }

    temp_input_data = std::vector<char>(temp_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(temp_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(temp_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_offset, file.beg);
    file.read(temp_input_data.data(), temp_hash_size);
    file.close();

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_xored == 1)
    {
        crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
    }

    uint32_t temp_decompressed_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size_final;

    temp_output_data = std::vector<char>(temp_decompressed_size, 0);

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_lz4ed)
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

    uint64_t temp_position = 0;

    uint32_t temp_sub_entity_table_offset = 0;
    uint32_t temp_after_sub_entity_table_offset = 0;
    uint32_t entity_count = 0;

    temp_position = 0x20;

    std::memcpy(&temp_sub_entity_table_offset, &temp_data.data()[temp_position], 0x4);

    temp_position = 0x28;

    std::memcpy(&temp_after_sub_entity_table_offset, &temp_data.data()[temp_position], 0x4);

    temp_position = 0x6C;

    std::memcpy(&entity_count, &temp_data.data()[temp_position], 0x4);

    if (temp_sub_entity_table_offset == 0x60 && entity_count != 0xFFFFFFFF)
    {
        uint32_t temp_version_check = temp_after_sub_entity_table_offset - temp_sub_entity_table_offset;

        if (temp_version_check == 0x58 * entity_count)
        {
            temp_file_version = 2;

            LOG("TEMP version: H1/H2");
        }
        else if (temp_version_check == 0x70 * entity_count)
        {
            temp_file_version = 3;

            LOG("TEMP version: H3");
        }
        else
        {
            temp_file_version = 4;

            LOG("TEMP version: Entry count found by still unknown");
        }
    }
    else if (temp_sub_entity_table_offset == 0x58)
    {
        uint32_t temp_version_check = temp_after_sub_entity_table_offset - temp_sub_entity_table_offset;

        if ((temp_version_check % 0x58 == 0) && (temp_version_check % 0x70 == 0))
        {
            temp_file_version = 5;

            LOG("TEMP version: Entry count not found and still Unknown");
        }
        else if (temp_version_check % 0x58 == 0)
        {
            temp_file_version = 2;

            LOG("TEMP version: H1/H2");
        }
        else if (temp_version_check % 0x70 == 0)
        {
            temp_file_version = 3;

            LOG("TEMP version: H3");
        }
    }
    else
    {
        temp_file_version = 6;

        LOG("TEMP version: Unknown");
    }
}

void temp::set_temp_version(uint32_t temp_version)
{
    temp_file_version = temp_version;
}

#ifdef RPKG_DLL
void temp::get_top_level_logical_parents()
{
    std::set<std::string> logical_parents_set;

    for (uint32_t e = 0; e < temp_logicalParent.size(); e++)
    {
        if (temp_logicalParent.at(e) >= temp_logicalParent.size())
        {
            logical_parents_set.insert(util::uint32_t_to_string(temp_logicalParent.at(e)));
        }
    }

    for (std::set<std::string>::iterator it = logical_parents_set.begin(); it != logical_parents_set.end(); it++)
    {
        response_string.append(*it);
        response_string.push_back(',');
    }
}

void temp::get_entries_with_logical_parent(uint32_t logical_parent)
{
    for (uint32_t e = 0; e < temp_logicalParent.size(); e++)
    {
        if (temp_logicalParent.at(e) == logical_parent)
        {
            char char4[4];

            std::memcpy(&char4, &e, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            uint32_t entityName_string_length = tblu_entityName.at(e).length();

            std::memcpy(&char4, &entityName_string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < entityName_string_length; i++)
            {
                response_data.push_back(tblu_entityName.at(e)[i]);
            }
        }
    }
}

void temp::get_entries_hash_references(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < tblu_entityName.size(); e++)
    {
        if (e == entry_index)
        {
            std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
            }
            else
            {
                response_string.append(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_string.at(temp_entityTypeResourceIndex.at(e)));
            }

            response_string.append(",");

            it = hash_list_hash_map.find(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
            }
            else
            {
                response_string.append(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference_string.at(tblu_entityTypeResourceIndex.at(e)));
            }

        }
    }
}

void temp::get_entries_hash_reference_data(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < tblu_entityName.size(); e++)
    {
        if (e == entry_index)
        {
            response_string.append("  - ");

            std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
                response_string.push_back(' ');
                response_string.append(hash_list_hash_strings.at(it->second));
            }
            else
            {
                response_string.append(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_string.at(temp_entityTypeResourceIndex.at(e)));
            }

            response_string.append("\n  - ");

            it = hash_list_hash_map.find(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
                response_string.push_back(' ');
                response_string.append(hash_list_hash_strings.at(it->second));
            }
            else
            {
                response_string.append(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference_string.at(tblu_entityTypeResourceIndex.at(e)));
            }

        }
    }
}

void temp::get_temp_entries_data(std::string value_type, std::string type_string)
{
    std::vector<char>().swap(response_data);

    std::string propertyValues_string = "0";

    std::string nPropertyID_string = value_type;

    rapidjson::Value::ConstMemberIterator it1 = temp_json_document.FindMember(value_type.c_str());

    if (it1 != temp_json_document.MemberEnd())
    {
        if (it1->value.IsArray() || it1->value.IsObject())
        {
            json_temp_node_scan(it1->value, propertyValues_string, nPropertyID_string, type_string, "/" + value_type, type_string);
        }
        else
        {
            std::stringstream ss;

            std::string json_pointer = "/" + value_type;

            //std::cout << it4->value.GetString() << std::endl;

            std::map<std::string, uint32_t>::iterator it;

            if (temp_file_version == 2)
            {
                it = type_map_h2->find("STemplateEntityFactory/" + type_string);

                if (it != type_map_h2->end())
                {
                    //std::cout << "Type: " << it->second << std::endl;

                    if (it->second == TYPE_INT8)
                    {
                        ss << json_pointer << " int8 " << it1->value.GetInt();
                    }
                    else if (it->second == TYPE_UINT8)
                    {
                        ss << json_pointer << " uint8 " << it1->value.GetUint();
                    }
                    else if (it->second == TYPE_INT16)
                    {
                        ss << json_pointer << " int16 " << it1->value.GetInt();
                    }
                    else if (it->second == TYPE_UINT16)
                    {
                        ss << json_pointer << " uint16 " << it1->value.GetUint();
                    }
                    else if (it->second == TYPE_INT32)
                    {
                        ss << json_pointer << " int32 " << it1->value.GetInt();
                    }
                    else if (it->second == TYPE_UINT32)
                    {
                        ss << json_pointer << " uint32 " << it1->value.GetUint();
                    }
                    else if (it->second == TYPE_INT64)
                    {
                        ss << json_pointer << " int64 " << it1->value.GetInt64();
                    }
                    else if (it->second == TYPE_UINT64)
                    {
                        ss << json_pointer << " uint64 " << it1->value.GetUint64();
                    }
                    else if (it->second == TYPE_FLOAT32)
                    {
                        ss << json_pointer << " float32 " << it1->value.GetFloat();
                    }
                    else if (it->second == TYPE_FLOAT64)
                    {
                        ss << json_pointer << " float64 " << it1->value.GetDouble();
                    }
                    else if (it->second == TYPE_BOOL)
                    {
                        ss << json_pointer << " bool " << it1->value.GetBool();
                    }
                    else if (it->second == TYPE_ZSTRING)
                    {
                        ss << json_pointer << " ZString " << it1->value.GetString();
                    }
                    else if (it->second == TYPE_ENUM)
                    {
                        ss << json_pointer << " enum " << it1->value.GetString();
                    }
                    else
                    {
                        ss << json_pointer << " none none";

                        response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                    }
                    /*if (it->second == 12)
                    {
                        ss << json_pointer << " ZVariant " << node.GetBool();
                    }
                    if (it->second == 13)
                    {
                        ss << json_pointer << " enum " << node.GetBool();
                    }*/
                }
                else
                {
                    if (it1->value.IsInt64())
                    {
                        ss << json_pointer << " int64 " << it1->value.GetInt64();
                    }
                    else if (it1->value.IsUint64())
                    {
                        ss << json_pointer << " uint64 " << it1->value.GetUint64();
                    }
                    else if (it1->value.IsDouble())
                    {
                        ss << json_pointer << " float64 " << it1->value.GetDouble();
                    }
                    else if (it1->value.IsInt())
                    {
                        ss << json_pointer << " int32 " << it1->value.GetInt();
                    }
                    else if (it1->value.IsUint())
                    {
                        ss << json_pointer << " uint32 " << it1->value.GetUint();
                    }
                    else if (it1->value.IsFloat())
                    {
                        ss << json_pointer << " float32 " << it1->value.GetFloat();
                    }
                    else if (it1->value.IsBool())
                    {
                        ss << json_pointer << " bool " << it1->value.GetBool();
                    }
                    else if (it1->value.IsString())
                    {
                        ss << json_pointer << " ZString " << it1->value.GetString();
                    }
                    else if (it1->value.IsString())
                    {
                        ss << json_pointer << " enum " << it1->value.GetString();
                    }
                    else
                    {
                        ss << json_pointer << " none none";

                        response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                    }
                }
            }
            else
            {
                it = type_map_h3->find("STemplateEntityFactory/" + type_string);

                if (it != type_map_h3->end())
                {
                    //std::cout << "Type: " << it->second << std::endl;

                    if (it->second == TYPE_INT8)
                    {
                        ss << json_pointer << " int8 " << it1->value.GetInt();
                    }
                    else if (it->second == TYPE_UINT8)
                    {
                        ss << json_pointer << " uint8 " << it1->value.GetUint();
                    }
                    else if (it->second == TYPE_INT16)
                    {
                        ss << json_pointer << " int16 " << it1->value.GetInt();
                    }
                    else if (it->second == TYPE_UINT16)
                    {
                        ss << json_pointer << " uint16 " << it1->value.GetUint();
                    }
                    else if (it->second == TYPE_INT32)
                    {
                        ss << json_pointer << " int32 " << it1->value.GetInt();
                    }
                    else if (it->second == TYPE_UINT32)
                    {
                        ss << json_pointer << " uint32 " << it1->value.GetUint();
                    }
                    else if (it->second == TYPE_INT64)
                    {
                        ss << json_pointer << " int64 " << it1->value.GetInt64();
                    }
                    else if (it->second == TYPE_UINT64)
                    {
                        ss << json_pointer << " uint64 " << it1->value.GetUint64();
                    }
                    else if (it->second == TYPE_FLOAT32)
                    {
                        ss << json_pointer << " float32 " << it1->value.GetFloat();
                    }
                    else if (it->second == TYPE_FLOAT64)
                    {
                        ss << json_pointer << " float64 " << it1->value.GetDouble();
                    }
                    else if (it->second == TYPE_BOOL)
                    {
                        ss << json_pointer << " bool " << it1->value.GetBool();
                    }
                    else if (it->second == TYPE_ZSTRING)
                    {
                        ss << json_pointer << " ZString " << it1->value.GetString();
                    }
                    else if (it->second == TYPE_ENUM)
                    {
                        ss << json_pointer << " enum " << it1->value.GetString();
                    }
                    else
                    {
                        ss << json_pointer << " none none";

                        response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                    }
                    /*if (it->second == 12)
                    {
                        ss << json_pointer << " ZVariant " << node.GetBool();
                    }
                    if (it->second == 13)
                    {
                        ss << json_pointer << " enum " << node.GetBool();
                    }*/
                }
                else
                {
                    if (it1->value.IsInt64())
                    {
                        ss << json_pointer << " int64 " << it1->value.GetInt64();
                    }
                    else if (it1->value.IsUint64())
                    {
                        ss << json_pointer << " uint64 " << it1->value.GetUint64();
                    }
                    else if (it1->value.IsDouble())
                    {
                        ss << json_pointer << " float64 " << it1->value.GetDouble();
                    }
                    else if (it1->value.IsInt())
                    {
                        ss << json_pointer << " int32 " << it1->value.GetInt();
                    }
                    else if (it1->value.IsUint())
                    {
                        ss << json_pointer << " uint32 " << it1->value.GetUint();
                    }
                    else if (it1->value.IsFloat())
                    {
                        ss << json_pointer << " float32 " << it1->value.GetFloat();
                    }
                    else if (it1->value.IsBool())
                    {
                        ss << json_pointer << " bool " << it1->value.GetBool();
                    }
                    else if (it1->value.IsString())
                    {
                        ss << json_pointer << " ZString " << it1->value.GetString();
                    }
                    else if (it1->value.IsString())
                    {
                        ss << json_pointer << " enum " << it1->value.GetString();
                    }
                    else
                    {
                        ss << json_pointer << " none none";

                        response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                    }
                }
            }

            char char4[4];

            uint32_t string_length = propertyValues_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(propertyValues_string[i]);
            }

            string_length = nPropertyID_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(nPropertyID_string[i]);
            }

            string_length = type_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(type_string[i]);
            }

            std::string ss_string = ss.str();

            string_length = ss_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(ss_string[i]);
            }
        }
    }
    else
    {
        response_string += "Error: " + value_type + " could not be found.";
    }
}

void temp::json_temp_node_scan(const rapidjson::Value& node, std::string& propertyValues_string, std::string& nPropertyID_string, std::string& type_string, std::string json_pointer, std::string json_type)
{
    bool output = true;

    std::stringstream ss;

    if (node.IsArray())
    {
        output = false;

        for (rapidjson::SizeType i = 0; i < node.Size(); ++i)
        {
            json_temp_node_scan(node[i], propertyValues_string, nPropertyID_string, type_string, json_pointer + "/" + std::to_string(i), json_type);
        }
    }
    else if (node.IsObject())
    {
        output = false;

        for (rapidjson::Value::ConstMemberIterator it = node.MemberBegin(); it != node.MemberEnd(); it++)
        {
            json_temp_node_scan(it->value, propertyValues_string, nPropertyID_string, type_string, json_pointer + "/" + it->name.GetString(), json_type + "/" + it->name.GetString());
        }
    }
    else
    {
        //std::cout << json_type << "/" << last_name << std::endl;

        std::map<std::string, uint32_t>::iterator it;

        if (temp_file_version == 2)
        {
            it = type_map_h2->find("STemplateEntityFactory/" + type_string);

            if (it != type_map_h2->end())
            {
                //std::cout << "Type: " << it->second << std::endl;

                if (it->second == TYPE_INT8)
                {
                    ss << json_pointer << " int8 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT8)
                {
                    ss << json_pointer << " uint8 " << node.GetUint();
                }
                else if (it->second == TYPE_INT16)
                {
                    ss << json_pointer << " int16 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT16)
                {
                    ss << json_pointer << " uint16 " << node.GetUint();
                }
                else if (it->second == TYPE_INT32)
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT32)
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (it->second == TYPE_INT64)
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (it->second == TYPE_UINT64)
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (it->second == TYPE_FLOAT32)
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (it->second == TYPE_FLOAT64)
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (it->second == TYPE_BOOL)
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (it->second == TYPE_ZSTRING)
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (it->second == TYPE_ENUM)
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                }
                /*if (it->second == 12)
                {
                    ss << json_pointer << " ZVariant " << node.GetBool();
                }
                if (it->second == 13)
                {
                    ss << json_pointer << " enum " << node.GetBool();
                }*/
            }
            else
            {
                if (node.IsInt64())
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (node.IsUint64())
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (node.IsDouble())
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (node.IsInt())
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (node.IsUint())
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (node.IsFloat())
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (node.IsBool())
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                }
            }
        }
        else
        {
            it = type_map_h3->find("STemplateEntityFactory/" + type_string);

            if (it != type_map_h3->end())
            {
                //std::cout << "Type: " << it->second << std::endl;

                if (it->second == TYPE_INT8)
                {
                    ss << json_pointer << " int8 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT8)
                {
                    ss << json_pointer << " uint8 " << node.GetUint();
                }
                else if (it->second == TYPE_INT16)
                {
                    ss << json_pointer << " int16 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT16)
                {
                    ss << json_pointer << " uint16 " << node.GetUint();
                }
                else if (it->second == TYPE_INT32)
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT32)
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (it->second == TYPE_INT64)
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (it->second == TYPE_UINT64)
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (it->second == TYPE_FLOAT32)
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (it->second == TYPE_FLOAT64)
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (it->second == TYPE_BOOL)
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (it->second == TYPE_ZSTRING)
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (it->second == TYPE_ENUM)
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                }
                /*if (it->second == 12)
                {
                    ss << json_pointer << " ZVariant " << node.GetBool();
                }
                if (it->second == 13)
                {
                    ss << json_pointer << " enum " << node.GetBool();
                }*/
            }
            else
            {
                if (node.IsInt64())
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (node.IsUint64())
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (node.IsDouble())
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (node.IsInt())
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (node.IsUint())
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (node.IsFloat())
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (node.IsBool())
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                }
            }
        }

        if (output)
        {
            char char4[4];

            uint32_t string_length = propertyValues_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(propertyValues_string[i]);
            }

            string_length = nPropertyID_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(nPropertyID_string[i]);
            }

            string_length = type_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(type_string[i]);
            }

            std::string ss_string = ss.str();

            string_length = ss_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(ss_string[i]);
            }
        }
    }
}

void temp::get_entries_data(uint32_t entry_index, std::string value_type)
{
    std::vector<char>().swap(response_data);

    std::string propertyValues_string = "";

    std::string nPropertyID_string = "";

    std::string type_string = "";

    const rapidjson::Value& temp_json_subEntities = temp_json_document["subEntities"];

    rapidjson::Value::ConstMemberIterator it1 = temp_json_subEntities[entry_index].FindMember(value_type.c_str());

    if (it1 != temp_json_subEntities[entry_index].MemberEnd())
    {
        for (rapidjson::SizeType p = 0; p < it1->value.Size(); p++)
        {
            propertyValues_string = util::uint32_t_to_string(p);

            rapidjson::Value::ConstMemberIterator it2 = it1->value[p].FindMember("nPropertyID");

            if (it2 != it1->value[p].MemberEnd())
            {
                if (it2->value.IsString())
                {
                    //std::cout << it2->value.GetString() << std::endl;

                    nPropertyID_string = it2->value.GetString();
                }
                else if (it2->value.IsNumber())
                {
                    //std::cout << it2->value.GetUint() << std::endl;

                    nPropertyID_string = util::uint32_t_to_string(it2->value.GetUint());
                }
                else
                {
                    response_string += "Error: nPropertyID's type for " + value_type + " (" + util::uint32_t_to_string(p) + ") could not be determined.";
                }
            }
            else
            {
                response_string += "Error: nPropertyID for " + value_type + " (" + util::uint32_t_to_string(p) + ") could not be found.";
            }

            rapidjson::Value::ConstMemberIterator it3 = it1->value[p].FindMember("value");

            if (it3 != it1->value[p].MemberEnd())
            {
                rapidjson::Value::ConstMemberIterator it4 = it3->value.FindMember("$type");

                if (it4 != it3->value.MemberEnd())
                {
                    //std::cout << it4->value.GetString() << std::endl;

                    type_string = it4->value.GetString();
                }
                else
                {
                    response_string += "Error: $type for " + value_type + " (" + util::uint32_t_to_string(p) + ") could not be found.";
                }

                rapidjson::Value::ConstMemberIterator it5 = it3->value.FindMember("$val");

                if (it5 != it3->value.MemberEnd())
                {
                    if (it5->value.IsArray() || it5->value.IsObject())
                    {
                        json_node_scan(it5->value, propertyValues_string, nPropertyID_string, type_string, "/subEntities/" + std::to_string(entry_index) + "/" + value_type + "/" + std::to_string(p) + "/value/$val", type_string);
                    }
                    else
                    {
                        std::stringstream ss;

                        std::string json_pointer = "/subEntities/" + std::to_string(entry_index) + "/" + value_type + "/" + std::to_string(p) + "/value/$val";

                        //std::cout << it4->value.GetString() << std::endl;

                        std::map<std::string, uint32_t>::iterator it;

                        if (temp_file_version == 2)
                        {
                            it = type_map_h2->find(type_string);

                            if (it != type_map_h2->end())
                            {
                                //std::cout << "Type: " << it->second << std::endl;

                                if (it->second == TYPE_INT8)
                                {
                                    ss << json_pointer << " int8 " << it5->value.GetInt();
                                }
                                else if (it->second == TYPE_UINT8)
                                {
                                    ss << json_pointer << " uint8 " << it5->value.GetUint();
                                }
                                else if (it->second == TYPE_INT16)
                                {
                                    ss << json_pointer << " int16 " << it5->value.GetInt();
                                }
                                else if (it->second == TYPE_UINT16)
                                {
                                    ss << json_pointer << " uint16 " << it5->value.GetUint();
                                }
                                else if (it->second == TYPE_INT32)
                                {
                                    ss << json_pointer << " int32 " << it5->value.GetInt();
                                }
                                else if (it->second == TYPE_UINT32)
                                {
                                    ss << json_pointer << " uint32 " << it5->value.GetUint();
                                }
                                else if (it->second == TYPE_INT64)
                                {
                                    ss << json_pointer << " int64 " << it5->value.GetInt64();
                                }
                                else if (it->second == TYPE_UINT64)
                                {
                                    ss << json_pointer << " uint64 " << it5->value.GetUint64();
                                }
                                else if (it->second == TYPE_FLOAT32)
                                {
                                    ss << json_pointer << " float32 " << it5->value.GetFloat();
                                }
                                else if (it->second == TYPE_FLOAT64)
                                {
                                    ss << json_pointer << " float64 " << it5->value.GetDouble();
                                }
                                else if (it->second == TYPE_BOOL)
                                {
                                    ss << json_pointer << " bool " << it5->value.GetBool();
                                }
                                else if (it->second == TYPE_ZSTRING)
                                {
                                    ss << json_pointer << " ZString " << it5->value.GetString();
                                }
                                else if (it->second == TYPE_ENUM)
                                {
                                    ss << json_pointer << " enum " << it5->value.GetString();
                                }
                                else
                                {
                                    ss << json_pointer << " none none";

                                    response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                                }
                                /*if (it->second == 12)
                                {
                                    ss << json_pointer << " ZVariant " << node.GetBool();
                                }
                                if (it->second == 13)
                                {
                                    ss << json_pointer << " enum " << node.GetBool();
                                }*/
                            }
                            else
                            {
                                if (it5->value.IsInt64())
                                {
                                    ss << json_pointer << " int64 " << it5->value.GetInt64();
                                }
                                else if (it5->value.IsUint64())
                                {
                                    ss << json_pointer << " uint64 " << it5->value.GetUint64();
                                }
                                else if (it5->value.IsDouble())
                                {
                                    ss << json_pointer << " float64 " << it5->value.GetDouble();
                                }
                                else if (it5->value.IsInt())
                                {
                                    ss << json_pointer << " int32 " << it5->value.GetInt();
                                }
                                else if (it5->value.IsUint())
                                {
                                    ss << json_pointer << " uint32 " << it5->value.GetUint();
                                }
                                else if (it5->value.IsFloat())
                                {
                                    ss << json_pointer << " float32 " << it5->value.GetFloat();
                                }
                                else if (it5->value.IsBool())
                                {
                                    ss << json_pointer << " bool " << it5->value.GetBool();
                                }
                                else if (it5->value.IsString())
                                {
                                    ss << json_pointer << " ZString " << it5->value.GetString();
                                }
                                else if (it5->value.IsString())
                                {
                                    ss << json_pointer << " enum " << it5->value.GetString();
                                }
                                else
                                {
                                    ss << json_pointer << " none none";

                                    response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                                }
                            }
                        }
                        else
                        {
                            it = type_map_h3->find(type_string);

                            if (it != type_map_h3->end())
                            {
                                //std::cout << "Type: " << it->second << std::endl;

                                if (it->second == TYPE_INT8)
                                {
                                    ss << json_pointer << " int8 " << it5->value.GetInt();
                                }
                                else if (it->second == TYPE_UINT8)
                                {
                                    ss << json_pointer << " uint8 " << it5->value.GetUint();
                                }
                                else if (it->second == TYPE_INT16)
                                {
                                    ss << json_pointer << " int16 " << it5->value.GetInt();
                                }
                                else if (it->second == TYPE_UINT16)
                                {
                                    ss << json_pointer << " uint16 " << it5->value.GetUint();
                                }
                                else if (it->second == TYPE_INT32)
                                {
                                    ss << json_pointer << " int32 " << it5->value.GetInt();
                                }
                                else if (it->second == TYPE_UINT32)
                                {
                                    ss << json_pointer << " uint32 " << it5->value.GetUint();
                                }
                                else if (it->second == TYPE_INT64)
                                {
                                    ss << json_pointer << " int64 " << it5->value.GetInt64();
                                }
                                else if (it->second == TYPE_UINT64)
                                {
                                    ss << json_pointer << " uint64 " << it5->value.GetUint64();
                                }
                                else if (it->second == TYPE_FLOAT32)
                                {
                                    ss << json_pointer << " float32 " << it5->value.GetFloat();
                                }
                                else if (it->second == TYPE_FLOAT64)
                                {
                                    ss << json_pointer << " float64 " << it5->value.GetDouble();
                                }
                                else if (it->second == TYPE_BOOL)
                                {
                                    ss << json_pointer << " bool " << it5->value.GetBool();
                                }
                                else if (it->second == TYPE_ZSTRING)
                                {
                                    ss << json_pointer << " ZString " << it5->value.GetString();
                                }
                                else if (it->second == TYPE_ENUM)
                                {
                                    ss << json_pointer << " enum " << it5->value.GetString();
                                }
                                else
                                {
                                    ss << json_pointer << " none none";

                                    response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                                }
                                /*if (it->second == 12)
                                {
                                    ss << json_pointer << " ZVariant " << node.GetBool();
                                }
                                if (it->second == 13)
                                {
                                    ss << json_pointer << " enum " << node.GetBool();
                                }*/
                            }
                            else
                            {
                                if (it5->value.IsInt64())
                                {
                                    ss << json_pointer << " int64 " << it5->value.GetInt64();
                                }
                                else if (it5->value.IsUint64())
                                {
                                    ss << json_pointer << " uint64 " << it5->value.GetUint64();
                                }
                                else if (it5->value.IsDouble())
                                {
                                    ss << json_pointer << " float64 " << it5->value.GetDouble();
                                }
                                else if (it5->value.IsInt())
                                {
                                    ss << json_pointer << " int32 " << it5->value.GetInt();
                                }
                                else if (it5->value.IsUint())
                                {
                                    ss << json_pointer << " uint32 " << it5->value.GetUint();
                                }
                                else if (it5->value.IsFloat())
                                {
                                    ss << json_pointer << " float32 " << it5->value.GetFloat();
                                }
                                else if (it5->value.IsBool())
                                {
                                    ss << json_pointer << " bool " << it5->value.GetBool();
                                }
                                else if (it5->value.IsString())
                                {
                                    ss << json_pointer << " ZString " << it5->value.GetString();
                                }
                                else if (it5->value.IsString())
                                {
                                    ss << json_pointer << " enum " << it5->value.GetString();
                                }
                                else
                                {
                                    ss << json_pointer << " none none";

                                    response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                                }
                            }
                        }

                        char char4[4];

                        uint32_t string_length = propertyValues_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (uint32_t i = 0; i < 0x4; i++)
                        {
                            response_data.push_back(char4[i]);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(propertyValues_string[i]);
                        }

                        string_length = nPropertyID_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (uint32_t i = 0; i < 0x4; i++)
                        {
                            response_data.push_back(char4[i]);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(nPropertyID_string[i]);
                        }

                        string_length = type_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (uint32_t i = 0; i < 0x4; i++)
                        {
                            response_data.push_back(char4[i]);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(type_string[i]);
                        }

                        std::string ss_string = ss.str();

                        string_length = ss_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (uint32_t i = 0; i < 0x4; i++)
                        {
                            response_data.push_back(char4[i]);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(ss_string[i]);
                        }
                    }
                }
                else
                {
                    response_string += "Error: $val for " + value_type + " (" + util::uint32_t_to_string(p) + ") could not be found.";
                }
            }
            else
            {
                response_string += "Error: value for " + value_type + " (" + util::uint32_t_to_string(p) + ") could not be found.";
            }
        }
    }
    else
    {
        response_string += "Error: propertyValues for " + value_type + " (" + util::uint32_t_to_string(entry_index) + ") could not be found.";
    }
}

void temp::json_node_scan(const rapidjson::Value& node, std::string& propertyValues_string, std::string& nPropertyID_string, std::string& type_string, std::string json_pointer, std::string json_type)
{
    bool output = true;

    std::stringstream ss;

    if (node.IsArray())
    {
        output = false;

        for (rapidjson::SizeType i = 0; i < node.Size(); ++i)
        {
            json_node_scan(node[i], propertyValues_string, nPropertyID_string, type_string, json_pointer + "/" + std::to_string(i), json_type);
        }
    }
    else if (node.IsObject())
    {
        output = false;

        for (rapidjson::Value::ConstMemberIterator it = node.MemberBegin(); it != node.MemberEnd(); it++)
        {
            json_node_scan(it->value, propertyValues_string, nPropertyID_string, type_string, json_pointer + "/" + it->name.GetString(), json_type + "/" + it->name.GetString());
        }
    }
    else
    {
        //std::cout << json_type << "/" << last_name << std::endl;

        std::map<std::string, uint32_t>::iterator it;

        if (temp_file_version == 2)
        {
            it = type_map_h2->find(json_type);

            if (it != type_map_h2->end())
            {
                //std::cout << "Type: " << it->second << std::endl;

                if (it->second == TYPE_INT8)
                {
                    ss << json_pointer << " int8 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT8)
                {
                    ss << json_pointer << " uint8 " << node.GetUint();
                }
                else if (it->second == TYPE_INT16)
                {
                    ss << json_pointer << " int16 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT16)
                {
                    ss << json_pointer << " uint16 " << node.GetUint();
                }
                else if (it->second == TYPE_INT32)
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT32)
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (it->second == TYPE_INT64)
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (it->second == TYPE_UINT64)
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (it->second == TYPE_FLOAT32)
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (it->second == TYPE_FLOAT64)
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (it->second == TYPE_BOOL)
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (it->second == TYPE_ZSTRING)
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (it->second == TYPE_ENUM)
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                }
                /*if (it->second == 12)
                {
                    ss << json_pointer << " ZVariant " << node.GetBool();
                }
                if (it->second == 13)
                {
                    ss << json_pointer << " enum " << node.GetBool();
                }*/
            }
            else
            {
                if (node.IsInt64())
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (node.IsUint64())
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (node.IsDouble())
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (node.IsInt())
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (node.IsUint())
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (node.IsFloat())
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (node.IsBool())
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                }
            }
        }
        else
        {
            it = type_map_h3->find(json_type);

            if (it != type_map_h3->end())
            {
                //std::cout << "Type: " << it->second << std::endl;

                if (it->second == TYPE_INT8)
                {
                    ss << json_pointer << " int8 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT8)
                {
                    ss << json_pointer << " uint8 " << node.GetUint();
                }
                else if (it->second == TYPE_INT16)
                {
                    ss << json_pointer << " int16 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT16)
                {
                    ss << json_pointer << " uint16 " << node.GetUint();
                }
                else if (it->second == TYPE_INT32)
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (it->second == TYPE_UINT32)
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (it->second == TYPE_INT64)
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (it->second == TYPE_UINT64)
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (it->second == TYPE_FLOAT32)
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (it->second == TYPE_FLOAT64)
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (it->second == TYPE_BOOL)
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (it->second == TYPE_ZSTRING)
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (it->second == TYPE_ENUM)
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += util::uint32_t_to_string(it->second) + "Error: The type for " + json_pointer + " could not be determined.\n";
                }
                /*if (it->second == 12)
                {
                    ss << json_pointer << " ZVariant " << node.GetBool();
                }
                if (it->second == 13)
                {
                    ss << json_pointer << " enum " << node.GetBool();
                }*/
            }
            else
            {
                if (node.IsInt64())
                {
                    ss << json_pointer << " int64 " << node.GetInt64();
                }
                else if (node.IsUint64())
                {
                    ss << json_pointer << " uint64 " << node.GetUint64();
                }
                else if (node.IsDouble())
                {
                    ss << json_pointer << " float64 " << node.GetDouble();
                }
                else if (node.IsInt())
                {
                    ss << json_pointer << " int32 " << node.GetInt();
                }
                else if (node.IsUint())
                {
                    ss << json_pointer << " uint32 " << node.GetUint();
                }
                else if (node.IsFloat())
                {
                    ss << json_pointer << " float32 " << node.GetFloat();
                }
                else if (node.IsBool())
                {
                    ss << json_pointer << " bool " << node.GetBool();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " ZString " << node.GetString();
                }
                else if (node.IsString())
                {
                    ss << json_pointer << " enum " << node.GetString();
                }
                else
                {
                    ss << json_pointer << " none none";

                    response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
                }
            }
        }

        if (output)
        {
            char char4[4];

            uint32_t string_length = propertyValues_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(propertyValues_string[i]);
            }

            string_length = nPropertyID_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(nPropertyID_string[i]);
            }

            string_length = type_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(type_string[i]);
            }

            std::string ss_string = ss.str();

            string_length = ss_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (uint32_t i = 0; i < 0x4; i++)
            {
                response_data.push_back(char4[i]);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(ss_string[i]);
            }
        }
    }
}

void temp::get_enum_values(std::string& property_type_string)
{
    response_string = "";

    std::map<std::string, std::map<int32_t, std::string>>::iterator it;

    if (temp_file_version == 2)
    {
        it = enum_map_h2->find(property_type_string);

        if (it != enum_map_h2->end())
        {
            for (std::map<int32_t, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                response_string.append(it2->second);
                response_string.push_back(',');
            }
        }
    }
    else
    {
        it = enum_map_h3->find(property_type_string);

        if (it != enum_map_h3->end())
        {
            for (std::map<int32_t, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                response_string.append(it2->second);
                response_string.push_back(',');
            }
        }
    }
}

void temp::update_temp_file(uint32_t entry_index, char* update_data, uint32_t update_data_size)
{
    //std::ofstream file = std::ofstream("R:\\outputdata.txt", std::ofstream::binary);

    file_has_been_changed = true;

    char input[1024];

    uint32_t pointer = 0;

    while (pointer < update_data_size)
    {
        uint32_t string_length = 0;

        std::string json_pointer = "";
        std::string json_data_type = "";
        std::string json_data = "";

        std::memcpy(&string_length, &update_data[pointer], 0x4);
        pointer += 0x4;

        std::memcpy(&input, &update_data[pointer], string_length);
        input[string_length] = 0;
        pointer += string_length;

        json_pointer = std::string(input);

        //file.write(json_pointer.c_str(), json_pointer.length());

        std::memcpy(&string_length, &update_data[pointer], 0x4);
        pointer += 0x4;

        std::memcpy(&input, &update_data[pointer], string_length);
        input[string_length] = 0;
        pointer += string_length;

        json_data_type = std::string(input);

        //file.write(json_data_type.c_str(), json_data_type.length());

        std::memcpy(&string_length, &update_data[pointer], 0x4);
        pointer += 0x4;

        std::memcpy(&input, &update_data[pointer], string_length);
        input[string_length] = 0;
        pointer += string_length;

        json_data = std::string(input);

        //file.write(json_data.c_str(), json_data.length());

        if (json_data_type == "int8")
        {
            int32_t temp_int32_t = std::strtol(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_int32_t);
        }
        else if (json_data_type == "uint8")
        {
            uint32_t temp_uint32_t = std::strtoul(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_uint32_t);
        }
        else if (json_data_type == "int16")
        {
            int32_t temp_int32_t = std::strtol(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_int32_t);
        }
        else if (json_data_type == "uint16")
        {
            uint32_t temp_uint32_t = std::strtoul(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_uint32_t);
        }
        else if (json_data_type == "int32")
        {
            int32_t temp_int32_t = std::strtol(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_int32_t);
        }
        else if (json_data_type == "uint32")
        {
            uint32_t temp_uint32_t = std::strtoul(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_uint32_t);
        }
        else if (json_data_type == "int64")
        {
            int64_t temp_int64_t = std::strtoll(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_int64_t);
        }
        else if (json_data_type == "uint64")
        {
            uint64_t temp_uint64_t = std::strtoull(json_data.c_str(), nullptr, 10);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_uint64_t);
        }
        else if (json_data_type == "float32")
        {
            float temp_float = std::strtof(json_data.c_str(), nullptr);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_float);
        }
        else if (json_data_type == "float64")
        {
            double temp_double = std::strtod(json_data.c_str(), nullptr);

            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), temp_double);
        }
        else if (json_data_type == "bool")
        {
            uint32_t bool_value = std::strtoul(json_data.c_str(), nullptr, 10);

            if (json_data == "True" || bool_value == 1)
            {
                rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), true);
            }
            else if (json_data == "False" || bool_value == 0)
            {
                rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), false);
            }
            else
            {
                response_string += "Error: The bool value for " + json_pointer + " could not be determined.\n";
            }
        }
        else if (json_data_type == "ZString")
        {
            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), json_data.c_str());
        }
        else if (json_data_type == "enum")
        {
            rapidjson::SetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str()), json_data.c_str());
        }
        else
        {
            response_string += "Error: The type for " + json_pointer + " could not be determined.\n";
        }
    }

    //rapidjson::StringBuffer buffer;
    //rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    //temp_json_document.Accept(writer);

    //std::cout << buffer.GetString() << std::endl;

    //file.write(buffer.GetString(), buffer.GetSize());

    //file.close();
}

void temp::export_json_files(std::string& json_file_path)
{
    std::ofstream file_temp = std::ofstream(json_file_path + rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name + ".JSON", std::ofstream::binary);

    rapidjson::StringBuffer buffer_temp;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer_temp(buffer_temp);
    temp_json_document.Accept(writer_temp);

    //std::cout << buffer.GetString() << std::endl;

    file_temp.write(buffer_temp.GetString(), buffer_temp.GetSize());

    file_temp.close();

    std::ofstream file_tblu = std::ofstream(json_file_path + rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_file_name + ".JSON", std::ofstream::binary);

    rapidjson::StringBuffer buffer_tblu;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer_tblu(buffer_tblu);
    tblu_json_document.Accept(writer_tblu);

    //std::cout << buffer.GetString() << std::endl;

    file_tblu.write(buffer_tblu.GetString(), buffer_tblu.GetSize());

    file_tblu.close();
}

int temp::generate_temp_file_from_data(std::string temp_path)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    temp_json_document.Accept(writer);

    //std::cout << buffer.GetString() << std::endl;

    std::string type = "TEMP";

    //if (temp_file_version == 2)
    //{
        //type = "TEMPH2";
    //}

    if (temp_file_version == 2)
    {
        resource_tool_generator = HM2_GetGeneratorForResource("TEMP");
    }
    else
    {
        resource_tool_generator = HM3_GetGeneratorForResource("TEMP");
    }

    resource_tool_generator->FromJsonStringToResourceFile(buffer.GetString(), buffer.GetSize(), &temp_path[0], true, true);

    //resource_tool_ConvertMemoryJsonToResource(&type[0], buffer.GetString(), buffer.GetSize(), &temp_path[0]);

    rpkg_function::extract_hash_meta(temp_rpkg_index, temp_hash_index, temp_path);

    return 0;
}

void temp::get_all_bricks()
{
    response_string = "";

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        for (uint32_t i = 0; i < temp_depends_file_name.size(); i++)
        {
            if (temp_depends_index.at(i) > tblu_depends_index.at(0))
            {
                uint64_t temp_hash_value = std::strtoull(temp_depends_file_name.at(i).c_str(), nullptr, 16);

                std::map<uint64_t, uint32_t>::iterator it = temps_map.find(temp_hash_value);

                if (it != temps_map.end())
                {
                    if (temps.at(it->second).tblu_return_value == TEMP_TBLU_FOUND)
                    {
                        std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_depends_index.at(i)));

                        if (it2 != hash_list_hash_map.end())
                        {
                            response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it2->second)));
                            response_string.push_back(' ');
                            response_string.append(hash_list_hash_strings.at(it2->second));
                            response_string.push_back(',');
                        }
                        else
                        {
                            response_string.append(temp_depends_file_name.at(i));
                            response_string.push_back(' ');
                            response_string.push_back(',');
                        }
                    }
                }
            }
        }
    }
}

void temp::get_entry_name_string(int entry_index)
{
    if (entry_index < 0 || entry_index >= tblu_entityName.size())
    {
        response_string.append("");
    }
    else
    {
        response_string.append(tblu_entityName.at(entry_index));
    }
}
#endif