#include "temp.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/rapidjson/writer.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/pointer.h"
#include "thirdparty/directxmath/DirectXMath.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include <iomanip>

#pragma comment(lib, "../thirdparty/zhmtools/ResourceLib_HM2.lib")
#pragma comment(lib, "../thirdparty/zhmtools/ResourceLib_HM3.lib")

temp::temp()
{

}

temp::temp(uint64_t rpkgs_index, uint64_t hash_index)
{
    initialize_enum_map_h2();
    initialize_enum_map_h3();

    initialize_type_map_h2();
    initialize_type_map_h3();

    temp_rpkg_index = rpkgs_index;
    temp_hash_index = hash_index;

    temp_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." + rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    tblu_return_value = TEMP_TBLU_NOT_FOUND_IN_DEPENDS;

    load_hash_depends();
}

temp::temp(const uint64_t rpkgs_index, const uint64_t hash_index, const uint32_t temp_version)
{
    temp_file_version = temp_version;

    initialize_enum_map_h2();
    initialize_enum_map_h3();

    initialize_type_map_h2();
    initialize_type_map_h3();

    temp_rpkg_index = rpkgs_index;
    temp_hash_index = hash_index;

    temp_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." + rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    tblu_return_value = TEMP_TBLU_NOT_FOUND_IN_DEPENDS;

    load_hash_depends();
}

void temp::load_data()
{
    load_temp_data();

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

        tblu_entityId.push_back(0);

        rapidjson::Value::ConstMemberIterator it2 = temp_json_subEntities[i].FindMember("entityTypeResourceIndex");

        if (it2 != temp_json_subEntities[i].MemberEnd())
        {
            temp_entityTypeResourceIndex.push_back(it2->value.GetInt());

            tblu_entityTypeResourceIndex.push_back(0);
        }
    }

    const rapidjson::Value& temp_json_externalSceneTypeIndicesInResourceHeader = temp_json_document["externalSceneTypeIndicesInResourceHeader"];

    for (rapidjson::SizeType e = 0; e < temp_json_externalSceneTypeIndicesInResourceHeader.Size(); e++)
    {
        temp_externalSceneTypeIndicesInResourceHeader.push_back(temp_json_externalSceneTypeIndicesInResourceHeader[e].GetInt());

        temp_externalSceneHashes.push_back(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_externalSceneTypeIndicesInResourceHeader.back()));

        //std::cout << "temp_externalSceneTypeIndicesInResourceHeader: " << temp_externalSceneTypeIndicesInResourceHeader.back() << std::endl;
        //std::cout << "temp_externalSceneHashes: " << temp_externalSceneHashes.back() << std::endl;
    }

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        load_tblu_data();

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

                rapidjson::Value::ConstMemberIterator it2 = tblu_json_subEntities[i].FindMember("entityId");

                if (it2 != tblu_json_subEntities[i].MemberEnd())
                {
                    tblu_entityId.at(i) = it2->value.GetUint64();

                    tblu_entityId_map[tblu_entityId.at(i)] = i;
                }

                rapidjson::Value::ConstMemberIterator it3 = tblu_json_subEntities[i].FindMember("entityTypeResourceIndex");

                if (it3 != tblu_json_subEntities[i].MemberEnd())
                {
                    tblu_entityTypeResourceIndex.at(i) = it3->value.GetInt();
                }
            }
        }
    }
}

void temp::load_temp_data()
{
    //std::cout << util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) + "." + rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type << std::endl;
    //std::cout << util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_value) + "." + rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_resource_type << std::endl;

    uint64_t temp_hash_size;

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.lz4ed)
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.header.data_size;

        if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.xored)
        {
            temp_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.resource.size_final;
    }

    temp_input_data = std::vector<char>(temp_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(temp_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(temp_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.header.data_offset, std::ifstream::beg);
    file.read(temp_input_data.data(), temp_hash_size);
    file.close();

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.xored)
    {
        crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
    }

    uint32_t temp_decompressed_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.resource.size_final;

    temp_output_data = std::vector<char>(temp_decompressed_size, 0);

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.lz4ed)
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

    if (temp_file_version == 2)
    {
        resource_tool_converter = HM2_GetConverterForResource("TEMP");
    }
    else
    {
        resource_tool_converter = HM3_GetConverterForResource("TEMP");
    }

    temp_json_input = resource_tool_converter->FromMemoryToJsonString((const void*)temp_data.data(), temp_data.size());

    //resource_tool_ConvertMemoryResourceToJson(&type[0], (void*)temp_data.data(), (uint64_t)temp_data.size());

    //temp_json_input = resource_tool_GetJsonFromMemory();

    rapidjson::StringStream ss(temp_json_input->JsonData);

    temp_json_document.ParseStream(ss);

    resource_tool_converter->FreeJsonString(temp_json_input);
}

void temp::load_tblu_data()
{
    std::string tblu_hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_value) + "." + rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_resource_type;

    uint64_t tblu_hash_size;

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.lz4ed)
    {
        tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.header.data_size;

        if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.xored)
        {
            tblu_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.resource.size_final;
    }

    tblu_input_data = std::vector<char>(tblu_hash_size, 0);

    std::ifstream file2 = std::ifstream(rpkgs.at(tblu_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file2.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(tblu_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file2.seekg(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.header.data_offset, file2.beg);
    file2.read(tblu_input_data.data(), tblu_hash_size);
    file2.close();

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.xored)
    {
        crypto::xor_data(tblu_input_data.data(), (uint32_t)tblu_hash_size);
    }

    uint32_t tblu_decompressed_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.resource.size_final;

    tblu_output_data = std::vector<char>(tblu_decompressed_size, 0);

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.lz4ed)
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

    if (temp_file_version == 2)
    {
        resource_tool_converter = HM2_GetConverterForResource("TBLU");
    }
    else
    {
        resource_tool_converter = HM3_GetConverterForResource("TBLU");
    }

    tblu_json_input = resource_tool_converter->FromMemoryToJsonString((const void*)tblu_data.data(), tblu_data.size());

    //resource_tool_ConvertMemoryResourceToJson(&type[0], (void*)tblu_data.data(), (uint64_t)tblu_data.size());

    //tblu_json_input = resource_tool_GetJsonFromMemory();

    rapidjson::StringStream ss(tblu_json_input->JsonData);

    tblu_json_document.ParseStream(ss);

    resource_tool_converter->FreeJsonString(tblu_json_input);
}

void temp::load_hash_depends()
{
    uint32_t temp_hash_reference_count = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    //LOG(util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) + "." + rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(temp_rpkg_index).rpkg_file_path);

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
                std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end())
                {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TEMP")
                    {
                        if (!temp_found)
                        {
                            temp_depends_file_name.push_back(util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." + rpkgs.at(j).hash.at(it->second).hash_resource_type);

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
                            tblu_depends_file_name.push_back(util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." + rpkgs.at(j).hash.at(it->second).hash_resource_type);

                            tblu_depends_index.push_back(i);

                            tblu_return_value = TEMP_TBLU_FOUND;

                            ////LOG("Found TEMP hash depend:");
                            ////LOG("  - File Name: " + temp_depends_file_name.back());
                        }
                        else
                        {
                            if (util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." + rpkgs.at(j).hash.at(it->second).hash_resource_type != tblu_depends_file_name.back())
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
                            prim_depends_file_name.push_back(util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." + rpkgs.at(j).hash.at(it->second).hash_resource_type);

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

            std::string hash_value_string = util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

            std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

            if (it2 != hash_list_hash_map.end())
            {
                if (hash_value_string == generic_function::compute_ioi_hash(hash_list_hash_strings.at(it2->second)))
                {
                    temp_meta_strings.push_back(hash_list_hash_strings.at(it2->second));
                }
                else
                {
                    temp_meta_strings.push_back(util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i)));
                }
            }
            else
            {
                temp_meta_strings.push_back(util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i)));
            }

            temp_meta_flags.push_back(util::uint8_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_type.at(i)));
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

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        uint32_t tblu_hash_reference_count = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        //LOG(util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) + "." + rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(temp_rpkg_index).rpkg_file_path);

        if (tblu_hash_reference_count > 0)
        {
            for (uint64_t k = 0; k < tblu_hash_reference_count; k++)
            {
                std::string hash_value_string = util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(k));

                std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(k));

                if (it2 != hash_list_hash_map.end())
                {
                    if (hash_value_string == generic_function::compute_ioi_hash(hash_list_hash_strings.at(it2->second)))
                    {
                        tblu_meta_strings.push_back(hash_list_hash_strings.at(it2->second));
                    }
                    else
                    {
                        tblu_meta_strings.push_back(util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(k)));
                    }
                }
                else
                {
                    tblu_meta_strings.push_back(util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(k)));
                }

                tblu_meta_flags.push_back(util::uint8_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference_type.at(k)));
            }
        }
    }
}

void temp::get_prim_from_temp(uint32_t entry_index)
{
    response_string = "";

    std::unordered_map<uint64_t, uint32_t>::iterator it = temps_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(entry_index)));

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

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.lz4ed)
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.header.data_size;

        if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.xored)
        {
            temp_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.resource.size_final;
    }

    temp_input_data = std::vector<char>(temp_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(temp_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(temp_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.header.data_offset, file.beg);
    file.read(temp_input_data.data(), temp_hash_size);
    file.close();

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.xored)
    {
        crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
    }

    uint32_t temp_decompressed_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.resource.size_final;

    temp_output_data = std::vector<char>(temp_decompressed_size, 0);

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.lz4ed)
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
            std::unordered_map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
            }
            else
            {
                response_string.append(util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e))));
            }

            response_string.append(",");

            it = hash_list_hash_map.find(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
            }
            else
            {
                response_string.append(util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e))));
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

            std::unordered_map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
                response_string.push_back(' ');
                response_string.append(hash_list_hash_strings.at(it->second));
            }
            else
            {
                response_string.append(util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e))));
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
                response_string.append(util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e))));
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

            std::unordered_map<std::string, uint32_t>::iterator it;

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

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(propertyValues_string[i]);
            }

            string_length = nPropertyID_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(nPropertyID_string[i]);
            }

            string_length = type_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(type_string[i]);
            }

            std::string ss_string = ss.str();

            string_length = ss_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
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

        std::unordered_map<std::string, uint32_t>::iterator it;

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

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(propertyValues_string[i]);
            }

            string_length = nPropertyID_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(nPropertyID_string[i]);
            }

            string_length = type_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(type_string[i]);
            }

            std::string ss_string = ss.str();

            string_length = ss_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
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

                        std::unordered_map<std::string, uint32_t>::iterator it;

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

                        for (char i : char4)
                        {
                            response_data.push_back(i);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(propertyValues_string[i]);
                        }

                        string_length = nPropertyID_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (char i : char4)
                        {
                            response_data.push_back(i);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(nPropertyID_string[i]);
                        }

                        string_length = type_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (char i : char4)
                        {
                            response_data.push_back(i);
                        }

                        for (uint32_t i = 0; i < string_length; i++)
                        {
                            response_data.push_back(type_string[i]);
                        }

                        std::string ss_string = ss.str();

                        string_length = ss_string.length();

                        std::memcpy(&char4, &string_length, 0x4);

                        for (char i : char4)
                        {
                            response_data.push_back(i);
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

        std::unordered_map<std::string, uint32_t>::iterator it;

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

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(propertyValues_string[i]);
            }

            string_length = nPropertyID_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(nPropertyID_string[i]);
            }

            string_length = type_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
            }

            for (uint32_t i = 0; i < string_length; i++)
            {
                response_data.push_back(type_string[i]);
            }

            std::string ss_string = ss.str();

            string_length = ss_string.length();

            std::memcpy(&char4, &string_length, 0x4);

            for (char i : char4)
            {
                response_data.push_back(i);
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

    std::unordered_map<std::string, std::unordered_map<int32_t, std::string>>::iterator it;

    if (temp_file_version == 2)
    {
        it = enum_map_h2->find(property_type_string);

        if (it != enum_map_h2->end())
        {
            for (std::unordered_map<int32_t, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
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
            for (std::unordered_map<int32_t, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
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
    std::ofstream file_temp = std::ofstream(json_file_path + util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) + "." + rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type + ".JSON", std::ofstream::binary);

    rapidjson::StringBuffer buffer_temp;
    rapidjson::Writer<rapidjson::StringBuffer> writer_temp(buffer_temp);
    temp_json_document.Accept(writer_temp);

    //std::cout << buffer.GetString() << std::endl;

    file_temp.write(buffer_temp.GetString(), buffer_temp.GetSize());

    file_temp.close();

    std::ofstream file_tblu = std::ofstream(json_file_path + util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_value) + "." + rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_resource_type + ".JSON", std::ofstream::binary);

    rapidjson::StringBuffer buffer_tblu;
    rapidjson::Writer<rapidjson::StringBuffer> writer_tblu(buffer_tblu);
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

    resource_tool_generator->FromJsonStringToResourceFile(buffer.GetString(), buffer.GetSize(), &temp_path[0], true);

    //resource_tool_ConvertMemoryJsonToResource(&type[0], buffer.GetString(), buffer.GetSize(), &temp_path[0]);

    rpkg_function::extract_hash_meta(temp_rpkg_index, temp_hash_index, temp_path);

    return 0;
}

void temp::get_all_bricks() const
{
    response_string = "";

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        for (uint32_t i = 0; i < temp_depends_file_name.size(); i++)
        {
            if (temp_depends_index.at(i) > tblu_depends_index.at(0))
            {
                uint64_t temp_hash_value = std::strtoull(temp_depends_file_name.at(i).c_str(), nullptr, 16);

                std::unordered_map<uint64_t, uint32_t>::iterator it = temps_map.find(temp_hash_value);

                if (it != temps_map.end())
                {
                    if (temps.at(it->second).tblu_return_value == TEMP_TBLU_FOUND)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_depends_index.at(i)));

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

bool temp::rt_json_to_qn_json()
{
    if (tblu_entityId.size() != tblu_entityId_map.size())
    {
        LOG("Error: The TBLU contains duplicate entityIds.");

        return false;
    }

    rapidjson::Pointer("/tempHash").Set(qn_json_document, util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value).c_str());
    rapidjson::Pointer("/tbluHash").Set(qn_json_document, util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_value).c_str());

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer("/rootEntityIndex")))
    {
        rapidjson::Pointer("/rootEntity").Set(qn_json_document, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetInt())).c_str());
    }

    rapidjson::Pointer("/entities").Set(qn_json_document, rapidjson::Value().SetObject());

    rapidjson::Value& tblu_json_subEntities = tblu_json_document["subEntities"];

    for (uint32_t s = 0; s < temp_subentity_count; s++)
    {
        rapidjson::Document subEntity_json(&qn_json_document.GetAllocator());

        std::string json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/logicalParent";

        if (rapidjson::Value* tblu_json_logicalParent = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            convert_to_qn_reference(subEntity_json, tblu_json_logicalParent->GetObject(), "/parent");
        }

        rapidjson::Pointer("/name").Set(subEntity_json, tblu_entityName.at(s).c_str());

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/entityTypeResourceIndex";

        if (rapidjson::Value* temp_json_entityTypeResourceIndex = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            rapidjson::Pointer("/template").Set(subEntity_json, temp_meta_strings.at(temp_json_entityTypeResourceIndex->GetInt()).c_str());

            if (temp_meta_flags.at(temp_json_entityTypeResourceIndex->GetInt()) != "1F")
            {
                rapidjson::Pointer("/templateFlag").Set(subEntity_json, temp_meta_flags.at(temp_json_entityTypeResourceIndex->GetInt()).c_str());
            }
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/entityTypeResourceIndex";

        if (rapidjson::Value* tblu_json_entityTypeResourceIndex = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            rapidjson::Pointer("/blueprint").Set(subEntity_json, tblu_meta_strings.at(tblu_json_entityTypeResourceIndex->GetInt()).c_str());
        }

        rapidjson::Pointer("/properties").Set(subEntity_json, rapidjson::Value().SetObject());

        rapidjson::Pointer("/postInitProperties").Set(subEntity_json, rapidjson::Value().SetObject());

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/editorOnly";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            rapidjson::Pointer("/editorOnly").Set(subEntity_json, temp_json_value->GetBool());
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/propertyValues";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            rapidjson::Pointer("/propertyValues").Set(subEntity_json, temp_json_value->GetArray());
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/postInitPropertyValues";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            rapidjson::Pointer("/postInitPropertyValues").Set(subEntity_json, temp_json_value->GetArray());
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/platformSpecificPropertyValues";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            rapidjson::Pointer("/platformSpecificPropertyValues").Set(subEntity_json, temp_json_value->GetArray());
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/propertyAliases";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            if (temp_json_value->GetArray().Size() > 0)
            {
                for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
                {
                    json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/propertyAliases/" + util::uint32_t_to_string(a);

                    if (rapidjson::Value* tblu_json_subEntity2 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                    {
                        rapidjson::Document json_object(&subEntity_json.GetAllocator());

                        rapidjson::Pointer("/exposeProperty").Set(json_object, tblu_json_subEntity2->GetObject()["sAliasName"].GetString());
                        rapidjson::Pointer("/onEntity").Set(json_object, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(tblu_json_subEntity2->GetObject()["entityID"].GetInt())).c_str());
                        rapidjson::Pointer("/asProperty").Set(json_object, tblu_json_subEntity2->GetObject()["sPropertyName"].GetString());

                        //std::cout << "propertyAliases sAliasName: " << tblu_json_subEntity2->GetObject()["sAliasName"].GetString() << std::endl;
                        //std::cout << "propertyAliases entityID: " << tblu_json_subEntity2->GetObject()["entityID"].GetInt() << std::endl;
                        //std::cout << "propertyAliases sPropertyName: " << tblu_json_subEntity2->GetObject()["sPropertyName"].GetString() << std::endl;

                        rapidjson::Pointer(json_pointer.c_str()).Set(tblu_json_document, json_object);
                    }
                }

                json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/propertyAliases";

                if (rapidjson::Value* tblu_json_subEntity2 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer("/propertyAliases").Set(subEntity_json, tblu_json_subEntity2->GetArray());
                }
            }
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedEntities";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            if (temp_json_value->GetArray().Size() > 0)
            {
                for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
                {
                    json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedEntities/" + util::uint32_t_to_string(a);

                    if (rapidjson::Value* tblu_json_subEntity2 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                    {
                        rapidjson::Document json_object(&subEntity_json.GetAllocator());

                        rapidjson::Pointer("/name").Set(json_object, tblu_json_subEntity2->GetObject()["sName"].GetString());
                        rapidjson::Pointer("/isArray").Set(json_object, tblu_json_subEntity2->GetObject()["bIsArray"].GetBool());

                        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedEntities/" + util::uint32_t_to_string(a) + "/aTargets";

                        if (rapidjson::Value* tblu_json_subEntity3 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                        {
                            if (tblu_json_subEntity3->GetArray().Size() > 0)
                            {
                                for (uint32_t t = 0; t < tblu_json_subEntity3->GetArray().Size(); t++)
                                {
                                    json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedEntities/" + util::uint32_t_to_string(a) + "/aTargets/" + util::uint32_t_to_string(t);

                                    if (rapidjson::Value* tblu_json_aTargets = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                                    {
                                        convert_to_qn_reference(json_object, tblu_json_aTargets->GetObject(), "/targets/" + util::uint32_t_to_string(t));
                                    }
                                }
                            }
                            else
                            {
                                rapidjson::Pointer("/targets").Set(json_object, rapidjson::Value().SetArray());
                            }
                        }

                        //std::cout << "exposedEntities name: " << tblu_json_subEntity2->GetObject()["sName"].GetString() << std::endl;
                        //std::cout << "exposedEntities isArray: " << tblu_json_subEntity2->GetObject()["bIsArray"].GetBool() << std::endl;

                        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedEntities/" + util::uint32_t_to_string(a);

                        rapidjson::Pointer(json_pointer.c_str()).Set(tblu_json_document, json_object);
                    }
                }

                json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedEntities";

                if (rapidjson::Value* tblu_json_subEntity2 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer("/exposedEntities").Set(subEntity_json, tblu_json_subEntity2->GetArray());
                }
            }
        }

        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedInterfaces";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
        {
            if (temp_json_value->GetArray().Size() > 0)
            {
                for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
                {
                    json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedInterfaces/" + util::uint32_t_to_string(a);

                    if (rapidjson::Value* tblu_json_subEntity2 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                    {
                        rapidjson::Document json_object(&subEntity_json.GetAllocator());

                        //std::cout << "exposedInterfaces 0: " << tblu_json_subEntity2->GetArray()[0].GetString() << std::endl;
                        //std::cout << "exposedInterfaces 1: " << tblu_json_subEntity2->GetArray()[1].GetInt() << std::endl;

                        json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedInterfaces/" + util::uint32_t_to_string(a) + "/1";

                        rapidjson::Pointer(json_pointer.c_str()).Set(tblu_json_document, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(tblu_json_subEntity2->GetArray()[1].GetInt())).c_str());
                    }
                }

                json_pointer = "/subEntities/" + util::uint32_t_to_string(s) + "/exposedInterfaces";

                if (rapidjson::Value* tblu_json_subEntity2 = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer("/exposedInterfaces").Set(subEntity_json, tblu_json_subEntity2->GetArray());
                }
            }
        }

        rapidjson::Value::MemberIterator it = tblu_json_subEntities[s].FindMember("entitySubsets");

        if (it != tblu_json_subEntities[s].MemberEnd())
        {
            for (uint64_t e = 0; e < it->value.Size(); e++)
            {
                for (uint64_t a = 0; a < it->value[e][1]["entities"].Size(); a++)
                {
                    //std::cout << it->value[e][1]["entities"][a].GetInt() << std::endl;

                    std::string entityId_string = util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(it->value[e][1]["entities"][a].GetInt()));

                    it->value[e][1]["entities"][a].SetString(entityId_string.c_str(), entityId_string.length(), qn_json_document.GetAllocator());
                }
            }

            if (it->value.Size() > 0)
            {
                rapidjson::Pointer("/entitySubsets").Set(subEntity_json, it->value);
            }
        }

        json_pointer = "/propertyValues";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(subEntity_json, rapidjson::Pointer(json_pointer.c_str())))
        {
            if (temp_json_value->GetArray().Size() > 0)
            {
                for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
                {
                    convert_to_qn_property(subEntity_json, temp_json_value->GetArray()[a]);

                    std::string nPropertyID = "";

                    if (temp_json_value->GetArray()[a]["nPropertyID"].IsString())
                    {
                        nPropertyID = temp_json_value->GetArray()[a]["nPropertyID"].GetString();
                    }
                    else if (temp_json_value->GetArray()[a]["nPropertyID"].IsUint())
                    {
                        nPropertyID = util::uint32_t_to_string(temp_json_value->GetArray()[a]["nPropertyID"].GetUint());
                    }

                    std::string string_replace = "/";
                    std::string string_replace_with = "~1";

                    util::replace_all_string_in_string(nPropertyID, string_replace, string_replace_with);

                    json_pointer = "/properties/" + nPropertyID + "/type";

                    rapidjson::Pointer(json_pointer.c_str()).Set(subEntity_json, temp_json_value->GetArray()[a]["value"]["$type"]);

                    json_pointer = "/properties/" + nPropertyID + "/value";

                    rapidjson::Pointer(json_pointer.c_str()).Set(subEntity_json, temp_json_value->GetArray()[a]["value"]["$val"]);
                }
            }
        }

        rapidjson::Pointer("/propertyValues").Erase(subEntity_json);

        json_pointer = "/postInitPropertyValues";

        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(subEntity_json, rapidjson::Pointer(json_pointer.c_str())))
        {
            if (temp_json_value->GetArray().Size() > 0)
            {
                for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
                {
                    convert_to_qn_property(subEntity_json, temp_json_value->GetArray()[a]);

                    std::string nPropertyID = "";

                    if (temp_json_value->GetArray()[a]["nPropertyID"].IsString())
                    {
                        nPropertyID = temp_json_value->GetArray()[a]["nPropertyID"].GetString();
                    }
                    else if (temp_json_value->GetArray()[a]["nPropertyID"].IsUint())
                    {
                        nPropertyID = util::uint32_t_to_string(temp_json_value->GetArray()[a]["nPropertyID"].GetUint());
                    }

                    json_pointer = "/postInitProperties/" + nPropertyID + "/type";

                    rapidjson::Pointer(json_pointer.c_str()).Set(subEntity_json, temp_json_value->GetArray()[a]["value"]["$type"]);

                    json_pointer = "/postInitProperties/" + nPropertyID + "/value";

                    rapidjson::Pointer(json_pointer.c_str()).Set(subEntity_json, temp_json_value->GetArray()[a]["value"]["$val"]);
                }
            }
        }

        rapidjson::Pointer("/postInitPropertyValues").Erase(subEntity_json);

        json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(s));

        rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, subEntity_json);
    }

    std::string json_pointer = "/pinConnections";

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                //std::cout << "fromID: " << temp_json_value->GetArray()[a]["fromID"].GetInt() << std::endl;
                //std::cout << "toID: " << temp_json_value->GetArray()[a]["toID"].GetInt() << std::endl;
                //std::cout << "fromPinName: " << temp_json_value->GetArray()[a]["fromPinName"].GetString() << std::endl;
                //std::cout << "toPinName: " << temp_json_value->GetArray()[a]["toPinName"].GetString() << std::endl;
                //std::cout << "constantPinValue: " << temp_json_value->GetArray()[a].HasMember("constantPinValue") << std::endl;
                //std::cout << "constantPinValue $type: " << temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString() << std::endl;
                //std::cout << "constantPinValue $val: " << temp_json_value->GetArray()[a]["constantPinValue"]["$val"].GetString() << std::endl;

                rapidjson::Document json_object(&qn_json_document.GetAllocator());

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt()));

                rapidjson::Pointer("/onEvent").Set(json_object, temp_json_value->GetArray()[a]["fromPinName"].GetString());
                rapidjson::Pointer("/shouldTrigger").Set(json_object, temp_json_value->GetArray()[a]["toPinName"].GetString());
                rapidjson::Pointer("/onEntity").Set(json_object, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["toID"].GetInt())).c_str());

                if (temp_json_value->GetArray()[a].HasMember("constantPinValue"))
                {
                    if (std::strcmp(temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString(), "void") != 0)
                    {
                        rapidjson::Pointer("/value/type").Set(json_object, temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString());
                        rapidjson::Pointer("/value/value").Set(json_object, temp_json_value->GetArray()[a]["constantPinValue"]["$val"]);
                    }
                }

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt())) + "/events";

                if (!rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, rapidjson::Value().SetArray());
                }

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt())) + "/events/-";

                //std::cout << "json_pointer: " << json_pointer << std::endl;

                rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, json_object);

                //property["value"]["$val"].Swap(json_object);
            }
        }
    }

    json_pointer = "/inputPinForwardings";

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                //std::cout << "fromID: " << temp_json_value->GetArray()[a]["fromID"].GetInt() << std::endl;
                //std::cout << "toID: " << temp_json_value->GetArray()[a]["toID"].GetInt() << std::endl;
                //std::cout << "fromPinName: " << temp_json_value->GetArray()[a]["fromPinName"].GetString() << std::endl;
                //std::cout << "toPinName: " << temp_json_value->GetArray()[a]["toPinName"].GetString() << std::endl;
                //std::cout << "constantPinValue: " << temp_json_value->GetArray()[a].HasMember("constantPinValue") << std::endl;
                //std::cout << "constantPinValue $type: " << temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString() << std::endl;
                //std::cout << "constantPinValue $val: " << temp_json_value->GetArray()[a]["constantPinValue"]["$val"].GetString() << std::endl;

                rapidjson::Document json_object(&qn_json_document.GetAllocator());

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt()));

                rapidjson::Pointer("/whenTriggered").Set(json_object, temp_json_value->GetArray()[a]["fromPinName"].GetString());
                rapidjson::Pointer("/alsoTrigger").Set(json_object, temp_json_value->GetArray()[a]["toPinName"].GetString());
                rapidjson::Pointer("/onEntity").Set(json_object, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["toID"].GetInt())).c_str());

                if (temp_json_value->GetArray()[a].HasMember("constantPinValue"))
                {
                    if (std::strcmp(temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString(), "void") != 0)
                    {
                        rapidjson::Pointer("/value/type").Set(json_object, temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString());
                        rapidjson::Pointer("/value/value").Set(json_object, temp_json_value->GetArray()[a]["constantPinValue"]["$val"]);
                    }
                }

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt())) + "/inputCopying";

                if (!rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, rapidjson::Value().SetArray());
                }

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt())) + "/inputCopying/-";

                //std::cout << "json_pointer: " << json_pointer << std::endl;

                rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, json_object);

                //property["value"]["$val"].Swap(json_object);
            }
        }
    }

    json_pointer = "/outputPinForwardings";

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer(json_pointer.c_str())))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                //std::cout << "fromID: " << temp_json_value->GetArray()[a]["fromID"].GetInt() << std::endl;
                //std::cout << "toID: " << temp_json_value->GetArray()[a]["toID"].GetInt() << std::endl;
                //std::cout << "fromPinName: " << temp_json_value->GetArray()[a]["fromPinName"].GetString() << std::endl;
                //std::cout << "toPinName: " << temp_json_value->GetArray()[a]["toPinName"].GetString() << std::endl;
                //std::cout << "constantPinValue: " << temp_json_value->GetArray()[a].HasMember("constantPinValue") << std::endl;
                //std::cout << "constantPinValue $type: " << temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString() << std::endl;
                //std::cout << "constantPinValue $val: " << temp_json_value->GetArray()[a]["constantPinValue"]["$val"].GetString() << std::endl;

                rapidjson::Document json_object(&qn_json_document.GetAllocator());

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt()));

                rapidjson::Pointer("/onEvent").Set(json_object, temp_json_value->GetArray()[a]["fromPinName"].GetString());
                rapidjson::Pointer("/propagateEvent").Set(json_object, temp_json_value->GetArray()[a]["toPinName"].GetString());
                rapidjson::Pointer("/onEntity").Set(json_object, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["toID"].GetInt())).c_str());

                if (temp_json_value->GetArray()[a].HasMember("constantPinValue"))
                {
                    if (std::strcmp(temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString(), "void") != 0)
                    {
                        rapidjson::Pointer("/value/type").Set(json_object, temp_json_value->GetArray()[a]["constantPinValue"]["$type"].GetString());
                        rapidjson::Pointer("/value/value").Set(json_object, temp_json_value->GetArray()[a]["constantPinValue"]["$val"]);
                    }
                }

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt())) + "/outputCopying";

                if (!rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, rapidjson::Value().SetArray());
                }

                json_pointer = "/entities/" + util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(temp_json_value->GetArray()[a]["fromID"].GetInt())) + "/outputCopying/-";

                //std::cout << "json_pointer: " << json_pointer << std::endl;

                rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, json_object);

                //property["value"]["$val"].Swap(json_object);
            }
        }
    }

    json_pointer = "/propertyOverrides";

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer(json_pointer.c_str())))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                rapidjson::Document property_overrides_json_object(&qn_json_document.GetAllocator());

                convert_to_qn_reference(property_overrides_json_object, temp_json_value->GetArray()[a].GetObject()["propertyOwner"].GetObject(), "/entities");

                convert_to_qn_property(property_overrides_json_object, temp_json_value->GetArray()[a].GetObject()["propertyValue"]);

                std::string nPropertyID = "";

                if (temp_json_value->GetArray()[a].GetObject()["propertyValue"]["nPropertyID"].IsString())
                {
                    nPropertyID = temp_json_value->GetArray()[a].GetObject()["propertyValue"]["nPropertyID"].GetString();
                }
                else if (temp_json_value->GetArray()[a].GetObject()["propertyValue"]["nPropertyID"].IsUint())
                {
                    nPropertyID = util::uint32_t_to_string(temp_json_value->GetArray()[a].GetObject()["propertyValue"]["nPropertyID"].GetUint());
                }

                json_pointer = "/properties";

                if (!rapidjson::GetValueByPointer(property_overrides_json_object, rapidjson::Pointer(json_pointer.c_str())))
                {
                    rapidjson::Pointer(json_pointer.c_str()).Set(property_overrides_json_object, rapidjson::Value().SetObject());
                }

                json_pointer = "/properties/" + nPropertyID + "/type";

                rapidjson::Pointer(json_pointer.c_str()).Set(property_overrides_json_object, temp_json_value->GetArray()[a].GetObject()["propertyValue"]["value"]["$type"]);

                json_pointer = "/properties/" + nPropertyID + "/value";

                rapidjson::Pointer(json_pointer.c_str()).Set(property_overrides_json_object, temp_json_value->GetArray()[a].GetObject()["propertyValue"]["value"]["$val"]);

                json_pointer = "/propertyOverrides/" + util::uint32_t_to_string(a);

                rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, property_overrides_json_object);
            }
        }
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer("/propertyOverrides")))
    {
        //rapidjson::Pointer("/propertyOverrides").Set(qn_json_document, temp_json_value->GetArray());
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer("/overrideDeletes")))
    {
        rapidjson::Pointer("/overrideDeletes").Set(qn_json_document, temp_json_value->GetArray());
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer("/pinConnectionOverrides")))
    {
        rapidjson::Pointer("/pinConnectionOverrides").Set(qn_json_document, temp_json_value->GetArray());
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(tblu_json_document, rapidjson::Pointer("/pinConnectionOverrideDeletes")))
    {
        rapidjson::Pointer("/pinConnectionOverrideDeletes").Set(qn_json_document, temp_json_value->GetArray());
    }

    rapidjson::Document entities_json_object(&qn_json_document.GetAllocator());
    rapidjson::Pointer("").Set(entities_json_object, rapidjson::Value().SetArray());

    rapidjson::Document properties_json_object(&qn_json_document.GetAllocator());
    rapidjson::Pointer("").Set(properties_json_object, rapidjson::Value().SetArray());

    if (rapidjson::Value* entities_json_value = rapidjson::GetValueByPointer(entities_json_object, rapidjson::Pointer("")))
    {
        if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer("/propertyOverrides")))
        {
            if (temp_json_value->GetArray().Size() > 0)
            {
                for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
                {
                    bool in_entities = false;

                    for (uint32_t e = 0; e < entities_json_value->GetArray().Size(); e++)
                    {
                        if (temp_json_value->GetArray()[a].GetObject()["entities"] == entities_json_value->GetArray()[e]["entities"])
                        {
                            in_entities = true;

                            json_pointer = "/" + util::uint32_t_to_string(e) + "/properties";

                            if (rapidjson::Value* properties_json_value = rapidjson::GetValueByPointer(entities_json_object, rapidjson::Pointer(json_pointer.c_str())))
                            {
                                std::string property_name = "";

                                for (rapidjson::Value::ConstMemberIterator iter = temp_json_value->GetArray()[a].GetObject()["properties"].MemberBegin(); iter != temp_json_value->GetArray()[a].GetObject()["properties"].MemberEnd(); ++iter)
                                {
                                    property_name = std::string(iter->name.GetString());
                                }

                                rapidjson::Value key(property_name.c_str(), entities_json_object.GetAllocator());

                                properties_json_value->AddMember(key, temp_json_value->GetArray()[a].GetObject()["properties"].GetObject()[key], entities_json_object.GetAllocator());
                            }
                        }
                    }

                    if (!in_entities)
                    {
                        //std::cout << temp_json_value->GetArray()[a].GetObject()["entities"]["ref"].GetString() << std::endl;

                        uint32_t array_index = entities_json_value->GetArray().Size();

                        json_pointer = "/" + util::uint32_t_to_string(array_index) + "/entities";
                        rapidjson::Pointer(json_pointer.c_str()).Set(entities_json_object, temp_json_value->GetArray()[a].GetObject()["entities"]);

                        json_pointer = "/" + util::uint32_t_to_string(array_index) + "/properties";
                        rapidjson::Pointer(json_pointer.c_str()).Set(entities_json_object, temp_json_value->GetArray()[a].GetObject()["properties"]);
                    }
                }
            }
        }

        if (rapidjson::Value* properties_json_value = rapidjson::GetValueByPointer(properties_json_object, rapidjson::Pointer("")))
        {
            for (uint32_t e = 0; e < entities_json_value->GetArray().Size(); e++)
            {
                bool in_properties = false;

                for (uint32_t p = 0; p < properties_json_value->GetArray().Size(); p++)
                {
                    if (entities_json_value->GetArray()[e].GetObject()["properties"] == properties_json_value->GetArray()[p]["properties"])
                    {
                        in_properties = true;

                        json_pointer = "/" + util::uint32_t_to_string(p) + "/entities";

                        if (rapidjson::Value* properties_json = rapidjson::GetValueByPointer(properties_json_object, rapidjson::Pointer(json_pointer.c_str())))
                        {
                            properties_json->PushBack(entities_json_value->GetArray()[e].GetObject()["entities"], properties_json_object.GetAllocator());
                        }
                    }
                }

                if (!in_properties)
                {
                    uint32_t array_index = properties_json_value->GetArray().Size();

                    json_pointer = "/" + util::uint32_t_to_string(array_index) + "/entities/0";
                    rapidjson::Pointer(json_pointer.c_str()).Set(properties_json_object, entities_json_value->GetArray()[e].GetObject()["entities"]);

                    json_pointer = "/" + util::uint32_t_to_string(array_index) + "/properties";
                    rapidjson::Pointer(json_pointer.c_str()).Set(properties_json_object, entities_json_value->GetArray()[e].GetObject()["properties"]);
                }
            }
        }
    }

    rapidjson::Pointer("/propertyOverrides").Set(qn_json_document, properties_json_object.GetArray());

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer("/overrideDeletes")))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                json_pointer = "/overrideDeletes/" + util::uint32_t_to_string(a);

                convert_to_qn_reference(qn_json_document, temp_json_value->GetArray()[a].GetObject(), json_pointer);
            }
        }
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer("/pinConnectionOverrides")))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                rapidjson::Document json_object1(&qn_json_document.GetAllocator());

                convert_to_qn_reference(json_object1, temp_json_value->GetArray()[a].GetObject()["fromEntity"], "");

                temp_json_value->GetArray()[a].GetObject()["fromEntity"].Swap(json_object1);

                rapidjson::Document json_object2(&qn_json_document.GetAllocator());

                convert_to_qn_reference(json_object2, temp_json_value->GetArray()[a].GetObject()["toEntity"], "");

                temp_json_value->GetArray()[a].GetObject()["toEntity"].Swap(json_object2);
            }
        }
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(qn_json_document, rapidjson::Pointer("/pinConnectionOverrideDeletes")))
    {
        if (temp_json_value->GetArray().Size() > 0)
        {
            for (uint32_t a = 0; a < temp_json_value->GetArray().Size(); a++)
            {
                rapidjson::Document json_object1(&qn_json_document.GetAllocator());

                convert_to_qn_reference(json_object1, temp_json_value->GetArray()[a].GetObject()["fromEntity"], "");

                temp_json_value->GetArray()[a].GetObject()["fromEntity"].Swap(json_object1);

                rapidjson::Document json_object2(&qn_json_document.GetAllocator());

                convert_to_qn_reference(json_object2, temp_json_value->GetArray()[a].GetObject()["toEntity"], "");

                temp_json_value->GetArray()[a].GetObject()["toEntity"].Swap(json_object2);
            }
        }
    }

    json_pointer = "/externalScenes";

    rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, rapidjson::Value().SetArray());

    for (uint32_t s = 0; s < temp_externalSceneTypeIndicesInResourceHeader.size(); s++)
    {
        json_pointer = "/externalScenes/" + util::uint32_t_to_string(s);

        rapidjson::Pointer(json_pointer.c_str()).Set(qn_json_document, temp_meta_strings.at(temp_externalSceneTypeIndicesInResourceHeader.at(s)).c_str());
    }

    if (rapidjson::Value* temp_json_value = rapidjson::GetValueByPointer(temp_json_document, rapidjson::Pointer("/subType")))
    {
        rapidjson::Pointer("/subType").Set(qn_json_document, temp_json_value->GetInt());
    }

    rapidjson::Pointer("/quickEntityVersion").Set(qn_json_document, 2.1);
}

void temp::convert_to_qn_reference(rapidjson::Document& json_document, rapidjson::Value& reference, std::string pointer_string)
{
    uint64_t entityID = reference["entityID"].GetUint64();
    int32_t externalSceneIndex = reference["externalSceneIndex"].GetInt();
    int32_t entityIndex = reference["entityIndex"].GetInt();
    std::string exposedEntity = reference["exposedEntity"].GetString();

    //std::cout << "entityID: " << entityID << std::endl;
    //std::cout << "externalSceneIndex: " << externalSceneIndex << std::endl;
    //std::cout << "entityIndex: " << entityIndex << std::endl;
    //std::cout << "exposedEntity: " << exposedEntity << std::endl;

    if (externalSceneIndex != -1 || exposedEntity.length() > 0)
    {
        rapidjson::Document json_object(&json_document.GetAllocator());

        if (entityIndex == -1)
        {
            rapidjson::Pointer("/ref").Set(json_object, rapidjson::kNullType);
        }
        else if (entityIndex == -2)
        {
            rapidjson::Pointer("/ref").Set(json_object, util::uint64_t_to_hex_string_for_qn(entityID).c_str());
        }
        else
        {
            rapidjson::Pointer("/ref").Set(json_object, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(entityIndex)).c_str());
        }

        if (externalSceneIndex == -1)
        {
            rapidjson::Pointer("/externalScene").Set(json_object, rapidjson::kNullType);
        }
        else
        {
            rapidjson::Pointer("/externalScene").Set(json_object, temp_meta_strings.at(temp_externalSceneTypeIndicesInResourceHeader.at(externalSceneIndex)).c_str());
        }

        if (exposedEntity != "")
        {
            rapidjson::Pointer("/exposedEntity").Set(json_object, exposedEntity.c_str());
        }

        rapidjson::Pointer(pointer_string.c_str()).Set(json_document, json_object);
    }
    else
    {
        if (entityIndex == -1)
        {
            rapidjson::Pointer(pointer_string.c_str()).Set(json_document, rapidjson::kNullType);

        }
        else
        {
            rapidjson::Pointer(pointer_string.c_str()).Set(json_document, util::uint64_t_to_hex_string_for_qn(tblu_entityId.at(entityIndex)).c_str());
        }
    }
}

void temp::convert_to_qn_property(rapidjson::Document& json_document, rapidjson::Value& property)
{
    if (property["nPropertyID"].IsString())
    {
        //std::cout << property["nPropertyID"].GetString() << std::endl;
        //std::cout << property["value"]["$type"].GetString() << std::endl;

        std::string nPropertyID = property["nPropertyID"].GetString();
        std::string type = property["value"]["$type"].GetString();

        if (type.length() > 8)
        {
            if (type.substr(0, 7) == "TArray<")
            {
                if (property["value"]["$val"].GetArray().Size() > 0)
                {
                    for (uint32_t a = 0; a < property["value"]["$val"].GetArray().Size(); a++)
                    {
                        rapidjson::Document json_object(&json_document.GetAllocator());

                        rapidjson::Pointer("/nPropertyID").Set(json_object, "");
                        rapidjson::Pointer("/value").Set(json_object, rapidjson::Value().SetObject());
                        rapidjson::Pointer("/value/$type").Set(json_object, type.substr(7, (type.length() - 8)).c_str());
                        rapidjson::Pointer("/value/$val").Set(json_object, property["value"]["$val"].GetArray()[a]);

                        convert_to_qn_property(json_document, json_object);

                        property["value"]["$val"][a].Swap(json_object["value"]["$val"]);
                    }
                }
            }
        }

        if (type == "SEntityTemplateReference")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            convert_to_qn_reference(json_object, property["value"]["$val"], "/$val");

            property["value"]["$val"].Swap(json_object["$val"]);
        }
        else if (type == "ZRuntimeResourceID")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            if (property["value"]["$val"]["m_IDLow"].GetUint() == 4294967295 && property["value"]["$val"]["m_IDHigh"].GetUint() == 4294967295)
            {
                rapidjson::Pointer("").Set(json_object, rapidjson::kNullType);
            }
            else
            {
                if (temp_meta_flags.at(property["value"]["$val"]["m_IDLow"].GetUint()) != "1F")
                {
                    rapidjson::Pointer("/resource").Set(json_object, temp_meta_strings.at(property["value"]["$val"]["m_IDLow"].GetUint()).c_str());
                    rapidjson::Pointer("/flag").Set(json_object, temp_meta_flags.at(property["value"]["$val"]["m_IDLow"].GetUint()).c_str());
                }
                else
                {
                    rapidjson::Pointer("").Set(json_object, temp_meta_strings.at(property["value"]["$val"]["m_IDLow"].GetUint()).c_str());
                }
            }

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "SMatrix43")
        {
            matrix43 matrix;

            matrix.x_axis.x = property["value"]["$val"]["XAxis"]["x"].GetFloat();
            matrix.y_axis.x = property["value"]["$val"]["YAxis"]["x"].GetFloat();
            matrix.z_axis.x = property["value"]["$val"]["ZAxis"]["x"].GetFloat();
            matrix.x_axis.y = property["value"]["$val"]["XAxis"]["y"].GetFloat();
            matrix.y_axis.y = property["value"]["$val"]["YAxis"]["y"].GetFloat();
            matrix.z_axis.y = property["value"]["$val"]["ZAxis"]["y"].GetFloat();
            matrix.x_axis.z = property["value"]["$val"]["XAxis"]["z"].GetFloat();
            matrix.y_axis.z = property["value"]["$val"]["YAxis"]["z"].GetFloat();
            matrix.z_axis.z = property["value"]["$val"]["ZAxis"]["z"].GetFloat();
            matrix.transform.x = property["value"]["$val"]["Trans"]["x"].GetFloat();
            matrix.transform.y = property["value"]["$val"]["Trans"]["y"].GetFloat();
            matrix.transform.z = property["value"]["$val"]["Trans"]["z"].GetFloat();

            vector3 euler_angles;

            float radians_to_degrees = 180.0f / DirectX::XM_PI;

            euler_angles.y = std::asin(std::clamp(matrix.x_axis.z, -1.0f, 1.0f));

            if (std::abs(matrix.x_axis.z) < 0.9999999)
            {
                euler_angles.x = std::atan2(-matrix.y_axis.z, matrix.z_axis.z);
                euler_angles.z = std::atan2(-matrix.x_axis.y, matrix.x_axis.x);
            }
            else
            {
                euler_angles.x = std::atan2(matrix.z_axis.y, matrix.y_axis.y);
                euler_angles.z = 0;
            }

            if (euler_angles.x == 0.0f && std::signbit(euler_angles.x))
                euler_angles.x = 0.0f;
            if (euler_angles.y == 0.0f && std::signbit(euler_angles.y))
                euler_angles.y = 0.0f;
            if (euler_angles.z == 0.0f && std::signbit(euler_angles.z))
                euler_angles.z = 0.0f;

            //std::cout << "euler_angles.x: " << euler_angles.x * radians_to_degrees << std::endl;
            //std::cout << "euler_angles.y: " << euler_angles.y * radians_to_degrees << std::endl;
            //std::cout << "euler_angles.z: " << euler_angles.z * radians_to_degrees << std::endl;

            rapidjson::Document json_object(&json_document.GetAllocator());

            rapidjson::Pointer("/rotation/x").Set(json_object, (euler_angles.x * radians_to_degrees));
            rapidjson::Pointer("/rotation/y").Set(json_object, (euler_angles.y * radians_to_degrees));
            rapidjson::Pointer("/rotation/z").Set(json_object, (euler_angles.z * radians_to_degrees));

            rapidjson::Pointer("/position/x").Set(json_object, matrix.transform.x);
            rapidjson::Pointer("/position/y").Set(json_object, matrix.transform.y);
            rapidjson::Pointer("/position/z").Set(json_object, matrix.transform.z);

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "ZGuid")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            std::stringstream ss;

            ss << std::hex << std::setw(8) << std::setfill('0') << property["value"]["$val"]["_a"].GetUint() << "-";
            ss << std::hex << std::setw(4) << std::setfill('0') << property["value"]["$val"]["_b"].GetUint() << "-";
            ss << std::hex << std::setw(4) << std::setfill('0') << property["value"]["$val"]["_c"].GetUint() << "-";
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_d"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_e"].GetUint() << "-";
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_f"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_g"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_h"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_i"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_j"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_k"].GetUint();

            rapidjson::Pointer("").Set(json_object, ss.str().c_str());

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "SColorRGB")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            std::stringstream ss;

            ss << "#" << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["r"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["g"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["b"].GetFloat() * 255.0f);

            rapidjson::Pointer("").Set(json_object, ss.str().c_str());

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "SColorRGBA")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            std::stringstream ss;

            ss << "#" << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["r"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["g"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["b"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["a"].GetFloat() * 255.0f);

            rapidjson::Pointer("").Set(json_object, ss.str().c_str());

            property["value"]["$val"].Swap(json_object);
        }
    }
    else if (property["nPropertyID"].IsUint())
    {
        //std::cout << property["nPropertyID"].GetUint() << std::endl;

        std::string type = property["value"]["$type"].GetString();

        if (type.length() > 8)
        {
            if (type.substr(0, 7) == "TArray<")
            {
                if (property["value"]["$val"].GetArray().Size() > 0)
                {
                    for (uint32_t a = 0; a < property["value"]["$val"].GetArray().Size(); a++)
                    {
                        rapidjson::Document json_object(&json_document.GetAllocator());

                        rapidjson::Pointer("/nPropertyID").Set(json_object, "");
                        rapidjson::Pointer("/value").Set(json_object, rapidjson::Value().SetObject());
                        rapidjson::Pointer("/value/$type").Set(json_object, type.substr(7, (type.length() - 8)).c_str());
                        rapidjson::Pointer("/value/$val").Set(json_object, property["value"]["$val"].GetArray()[a]);

                        convert_to_qn_property(json_document, json_object);

                        property["value"]["$val"][a].Swap(json_object["value"]["$val"]);
                    }
                }
            }
        }

        if (type == "SEntityTemplateReference")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            convert_to_qn_reference(json_object, property["value"]["$val"], "/$val");

            property["value"]["$val"].Swap(json_object["$val"]);
        }
        else if (type == "ZRuntimeResourceID")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            if (property["value"]["$val"]["m_IDLow"].GetUint() == 4294967295 && property["value"]["$val"]["m_IDHigh"].GetUint() == 4294967295)
            {
                rapidjson::Pointer("").Set(json_object, rapidjson::kNullType);
            }
            else
            {
                if (temp_meta_flags.at(property["value"]["$val"]["m_IDLow"].GetUint()) != "1F")
                {
                    rapidjson::Pointer("/resource").Set(json_object, temp_meta_strings.at(property["value"]["$val"]["m_IDLow"].GetUint()).c_str());
                    rapidjson::Pointer("/flag").Set(json_object, temp_meta_flags.at(property["value"]["$val"]["m_IDLow"].GetUint()).c_str());
                }
                else
                {
                    rapidjson::Pointer("").Set(json_object, temp_meta_strings.at(property["value"]["$val"]["m_IDLow"].GetUint()).c_str());
                }
            }

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "SMatrix43")
        {
            matrix43 matrix;

            matrix.x_axis.x = property["value"]["$val"]["XAxis"]["x"].GetFloat();
            matrix.y_axis.x = property["value"]["$val"]["YAxis"]["x"].GetFloat();
            matrix.z_axis.x = property["value"]["$val"]["ZAxis"]["x"].GetFloat();
            matrix.x_axis.y = property["value"]["$val"]["XAxis"]["y"].GetFloat();
            matrix.y_axis.y = property["value"]["$val"]["YAxis"]["y"].GetFloat();
            matrix.z_axis.y = property["value"]["$val"]["ZAxis"]["y"].GetFloat();
            matrix.x_axis.z = property["value"]["$val"]["XAxis"]["z"].GetFloat();
            matrix.y_axis.z = property["value"]["$val"]["YAxis"]["z"].GetFloat();
            matrix.z_axis.z = property["value"]["$val"]["ZAxis"]["z"].GetFloat();
            matrix.transform.x = property["value"]["$val"]["Trans"]["x"].GetFloat();
            matrix.transform.y = property["value"]["$val"]["Trans"]["y"].GetFloat();
            matrix.transform.z = property["value"]["$val"]["Trans"]["z"].GetFloat();

            vector3 euler_angles;

            float radians_to_degrees = 180.0f / DirectX::XM_PI;

            euler_angles.y = std::asin(std::clamp(matrix.x_axis.z, -1.0f, 1.0f));

            if (std::abs(matrix.x_axis.z) < 0.9999999)
            {
                euler_angles.x = std::atan2(-matrix.y_axis.z, matrix.z_axis.z);
                euler_angles.z = std::atan2(-matrix.x_axis.y, matrix.x_axis.x);
            }
            else
            {
                euler_angles.x = std::atan2(matrix.z_axis.y, matrix.y_axis.y);
                euler_angles.z = 0;
            }

            if (euler_angles.x == 0.0f && std::signbit(euler_angles.x))
                euler_angles.x = 0.0f;
            if (euler_angles.y == 0.0f && std::signbit(euler_angles.y))
                euler_angles.y = 0.0f;
            if (euler_angles.z == 0.0f && std::signbit(euler_angles.z))
                euler_angles.z = 0.0f;

            //std::cout << "euler_angles.x: " << euler_angles.x * radians_to_degrees << std::endl;
            //std::cout << "euler_angles.y: " << euler_angles.y * radians_to_degrees << std::endl;
            //std::cout << "euler_angles.z: " << euler_angles.z * radians_to_degrees << std::endl;

            rapidjson::Document json_object(&json_document.GetAllocator());

            rapidjson::Pointer("/rotation/x").Set(json_object, (euler_angles.x * radians_to_degrees));
            rapidjson::Pointer("/rotation/y").Set(json_object, (euler_angles.y * radians_to_degrees));
            rapidjson::Pointer("/rotation/z").Set(json_object, (euler_angles.z * radians_to_degrees));

            rapidjson::Pointer("/position/x").Set(json_object, matrix.transform.x);
            rapidjson::Pointer("/position/y").Set(json_object, matrix.transform.y);
            rapidjson::Pointer("/position/z").Set(json_object, matrix.transform.z);

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "ZGuid")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            std::stringstream ss;

            ss << std::hex << std::setw(8) << std::setfill('0') << property["value"]["$val"]["_a"].GetUint() << "-";
            ss << std::hex << std::setw(4) << std::setfill('0') << property["value"]["$val"]["_b"].GetUint() << "-";
            ss << std::hex << std::setw(4) << std::setfill('0') << property["value"]["$val"]["_c"].GetUint() << "-";
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_d"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_e"].GetUint() << "-";
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_f"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_g"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_h"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_i"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_j"].GetUint();
            ss << std::hex << std::setw(2) << std::setfill('0') << property["value"]["$val"]["_k"].GetUint();

            rapidjson::Pointer("").Set(json_object, ss.str().c_str());

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "SColorRGB")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            std::stringstream ss;

            ss << "#" << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["r"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["g"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["b"].GetFloat() * 255.0f);

            rapidjson::Pointer("").Set(json_object, ss.str().c_str());

            property["value"]["$val"].Swap(json_object);
        }
        else if (type == "SColorRGBA")
        {
            rapidjson::Document json_object(&json_document.GetAllocator());

            std::stringstream ss;

            ss << "#" << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["r"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["g"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["b"].GetFloat() * 255.0f);
            ss << std::hex << std::setw(2) << std::setfill('0') << (uint32_t)std::round(property["value"]["$val"]["a"].GetFloat() * 255.0f);

            rapidjson::Pointer("").Set(json_object, ss.str().c_str());

            property["value"]["$val"].Swap(json_object);
        }
    }
}

void temp::write_qn_json_to_file(std::string output_path)
{
    std::ofstream file_qn = std::ofstream(output_path, std::ofstream::binary);

    rapidjson::StringBuffer buffer_qn;
    rapidjson::Writer<rapidjson::StringBuffer> writer_qn(buffer_qn);
    qn_json_document.Accept(writer_qn);

    //std::cout << buffer.GetString() << std::endl;

    file_qn.write(buffer_qn.GetString(), buffer_qn.GetSize());

    file_qn.close();
}

void temp::generate_qn_patch_json()
{
    rapidjson::StringBuffer buffer_qn_original;
    rapidjson::Writer<rapidjson::StringBuffer> writer_qn_original(buffer_qn_original);
    qn_json_document_original.Accept(writer_qn_original);

    rapidjson::StringBuffer buffer_qn;
    rapidjson::Writer<rapidjson::StringBuffer> writer_qn(buffer_qn);
    qn_json_document.Accept(writer_qn);

    json qn_json_original;
    json qn_json;

    try
    {
        qn_json_original = json::parse(buffer_qn_original.GetString(), buffer_qn_original.GetString() + buffer_qn_original.GetSize());

        qn_json = json::parse(buffer_qn.GetString(), buffer_qn.GetString() + buffer_qn.GetSize());

        qn_patch_json["tempHash"] = util::uint64_t_to_hex_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value);
        qn_patch_json["tbluHash"] = util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_value);
        qn_patch_json["patch"] = json::diff(qn_json_original, qn_json);
        qn_patch_json["patchVersion"] = 4;
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;
        ss << "Error: " << temp_file_name << "\n"
           << "Error message: " << e.what() << '\n'
           << "Error exception id: " << e.id << '\n'
           << "Error byte position of error: " << e.byte;
        LOG_AND_EXIT(ss.str());
    }
}

void temp::write_qn_patch_json_to_file(std::string output_path)
{
    std::ofstream file_qn_patch = std::ofstream(output_path, std::ofstream::binary);

    if (!file_qn_patch.good())
    {
        LOG_AND_EXIT("Error: QN Patch JSON file " + output_path + " could not be created.");
    }

    file_qn_patch << std::setw(4) << qn_patch_json << std::endl;

    file_qn_patch.close();
}
