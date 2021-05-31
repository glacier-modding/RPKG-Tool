#include "rpkg_function.h"
#include "dev_function.h"
#include "global.h"
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
#include "thirdparty/rapidjson/stringbuffer.h"

static void json_node_scan(const rapidjson::Value& node, std::string json_pointer, std::string json_type, std::string last_name)
{
    /*if (node.IsNull())
    {
        std::cout << json_pointer << " (null): " << node.GetBool() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsBool())
    {
        std::cout << json_pointer << " (bool): " << node.GetBool() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsInt())
    {
        std::cout << json_pointer << " (int): " << node.GetInt() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsInt64())
    {
        std::cout << json_pointer << " (int64): " << node.GetInt64() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsUint())
    {
        std::cout << json_pointer << " (uint): " << node.GetUint() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsUint64())
    {
        std::cout << json_pointer << " (uint64) : " << node.GetUint64() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsFloat())
    {
        std::cout << json_pointer << " (float): " << node.GetFloat() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsDouble())
    {
        std::cout << json_pointer << " (double): " << node.GetDouble() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }
    else if (node.IsString())
    {
        std::cout << json_pointer << " (string): " << node.GetString() << std::endl;
        std::cout << json_type << "/" << last_name << std::endl;
    }*/
    if (node.IsArray())
    {
        if (node.Size() == 0)
        {
            std::cout << json_pointer << " is empty array" << std::endl;
        }

        for (rapidjson::SizeType i = 0; i < node.Size(); ++i)
        {
            json_node_scan(node[i], json_pointer + "/" + std::to_string(i), json_type, "");
        }
    }
    else if (node.IsObject())
    {
        if (node.MemberCount() == 0)
        {
            std::cout << json_pointer << " is empty object" << std::endl;
        }

        for (rapidjson::Value::ConstMemberIterator it = node.MemberBegin(); it != node.MemberEnd(); it++)
        {
            json_node_scan(it->value, json_pointer + "/" + it->name.GetString(), json_type, it->name.GetString());
        }
    }
    else
    {
        std::cout << json_type << "/" << last_name << std::endl;

        std::map<std::string, uint32_t>::iterator it = type_map->find(json_type + "/" + last_name);

        if (it != type_map->end())
        {
            std::cout << "Type: " << it->second << std::endl;

            if (it->second == 0)
                std::cout << "TYPE_INT8" << std::endl;
            if (it->second == 1)
                std::cout << "TYPE_UINT8" << std::endl;
            if (it->second == 2)
                std::cout << "TYPE_INT16" << std::endl;
            if (it->second == 3)
                std::cout << "TYPE_UINT16" << std::endl;
            if (it->second == 4)
                std::cout << "TYPE_INT32" << std::endl;
            if (it->second == 5)
                std::cout << "TYPE_UINT32" << std::endl;
            if (it->second == 6)
                std::cout << "TYPE_INT64" << std::endl;
            if (it->second == 7)
                std::cout << "TYPE_UINT64" << std::endl;
            if (it->second == 8)
                std::cout << "TYPE_FLOAT32" << std::endl;
            if (it->second == 9)
                std::cout << "TYPE_FLOAT64" << std::endl;
            if (it->second == 10)
                std::cout << "TYPE_BOOL" << std::endl;
            if (it->second == 11)
                std::cout << "TYPE_ZSTRING" << std::endl;
            if (it->second == 12)
                std::cout << "TYPE_ZVARIANT" << std::endl;
            if (it->second == 13)
                std::cout << "TYPE_ENUM" << std::endl;
        }
        else
        {
            std::cout << "Type: Not found" << std::endl;
        }
    }
}

void dev_function::dev_resource_tool(std::string& input_path, std::string& filter, std::string& output_path)
{
    initialize_enum_map();

    initialize_type_map();

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path))
    {
        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    //temp temp_temp(i, it->second);

                    std::map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    rpkg_function::recursive_temp_loader(rpkg_index, it->second, parents_map, temps_indexes, 0, 0, 0);

                    for (uint64_t t = 0; t < temps.size(); t++)
                    {
                        //const rapidjson::Value& temp_json_subEntities = temps.at(t).temp_json_document["subEntities"];

                        //json_node_scan(temp_json_subEntities, "/subEntities");

                        const rapidjson::Value& temp_json_subEntities = temps.at(t).temp_json_document["subEntities"];

                        for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++)
                        {
                            std::string nPropertyID_string = "";

                            std::string type_string = "";

                            rapidjson::Value::ConstMemberIterator it1 = temp_json_subEntities[s].FindMember("propertyValues");

                            if (it1 != temp_json_subEntities[s].MemberEnd())
                            {
                                for (uint64_t p = 0; p < it1->value.Size(); p++)
                                {
                                    rapidjson::Value::ConstMemberIterator it2 = it1->value[p].FindMember("nPropertyID");

                                    if (it2 != it1->value[p].MemberEnd())
                                    {
                                        if (it2->value.IsString())
                                        {
                                            std::cout << it2->value.GetString() << std::endl;

                                            nPropertyID_string = it2->value.GetString();
                                        }
                                        else if (it2->value.IsNumber())
                                        {
                                            std::cout << it2->value.GetUint() << std::endl;

                                            nPropertyID_string = util::uint32_t_to_string(it2->value.GetUint());
                                        }
                                    }

                                    rapidjson::Value::ConstMemberIterator it3 = it1->value[p].FindMember("value");

                                    if (it3 != it1->value[p].MemberEnd())
                                    {
                                        rapidjson::Value::ConstMemberIterator it4 = it3->value.FindMember("$type");

                                        if (it4 != it3->value.MemberEnd())
                                        {
                                            std::cout << it4->value.GetString() << std::endl;

                                            type_string = it4->value.GetString();
                                        }

                                        rapidjson::Value::ConstMemberIterator it5 = it3->value.FindMember("$val");

                                        if (it5 != it3->value.MemberEnd())
                                        {
                                            if (it5->value.IsArray() || it5->value.IsObject())
                                            {
                                                json_node_scan(it5->value, "/subEntities/" + std::to_string(s) + "/propertyValues/" + std::to_string(p) + "/value/$val", it4->value.GetString(), "");
                                            }
                                            else
                                            {
                                                std::cout << it4->value.GetString() << std::endl;

                                                std::map<std::string, uint32_t>::iterator it = type_map->find(it4->value.GetString());

                                                if (it != type_map->end())
                                                {
                                                    std::cout << "Type: " << it->second << std::endl;

                                                    if (it->second == 0)
                                                        std::cout << "TYPE_INT8" << std::endl;
                                                    if (it->second == 1)
                                                        std::cout << "TYPE_UINT8" << std::endl;
                                                    if (it->second == 2)
                                                        std::cout << "TYPE_INT16" << std::endl;
                                                    if (it->second == 3)
                                                        std::cout << "TYPE_UINT16" << std::endl;
                                                    if (it->second == 4)
                                                        std::cout << "TYPE_INT32" << std::endl;
                                                    if (it->second == 5)
                                                        std::cout << "TYPE_UINT32" << std::endl;
                                                    if (it->second == 6)
                                                        std::cout << "TYPE_INT64" << std::endl;
                                                    if (it->second == 7)
                                                        std::cout << "TYPE_UINT64" << std::endl;
                                                    if (it->second == 8)
                                                        std::cout << "TYPE_FLOAT32" << std::endl;
                                                    if (it->second == 9)
                                                        std::cout << "TYPE_FLOAT64" << std::endl;
                                                    if (it->second == 10)
                                                        std::cout << "TYPE_BOOL" << std::endl;
                                                    if (it->second == 11)
                                                        std::cout << "TYPE_ZSTRING" << std::endl;
                                                    if (it->second == 12)
                                                        std::cout << "TYPE_ZVARIANT" << std::endl;
                                                    if (it->second == 13)
                                                        std::cout << "TYPE_ENUM" << std::endl;
                                                }
                                                else
                                                {
                                                    std::cout << "Type: Not found" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        






                        std::cout << "LOL!!!" << std::endl;

                        /*
                        {
                            const rapidjson::Value& temp_json_subEntities = temps.at(t).temp_json_document["subEntities"];

                            for (uint64_t s = 0; s < temp_json_subEntities.Size(); s++)
                            {
                                rapidjson::Value::ConstMemberIterator it1 = temp_json_subEntities[s].FindMember("propertyValues");

                                if (it1 != temp_json_subEntities[s].MemberEnd())
                                {
                                    for (uint64_t p = 0; p < it1->value.Size(); p++)
                                    {
                                        rapidjson::Value::ConstMemberIterator it2 = it1->value[p].FindMember("nPropertyID");

                                        if (it2 != it1->value[p].MemberEnd())
                                        {
                                            if (it2->value.IsString())
                                            {
                                                std::cout << it2->value.GetString() << std::endl;
                                            }
                                            else if (it2->value.IsNumber())
                                            {
                                                std::cout << it2->value.GetUint() << std::endl;
                                            }
                                        }

                                        rapidjson::Value::ConstMemberIterator it3 = it1->value[p].FindMember("value");

                                        if (it3 != it1->value[p].MemberEnd())
                                        {
                                            rapidjson::Value::ConstMemberIterator it4 = it3->value.FindMember("$type");

                                            if (it4 != it3->value.MemberEnd())
                                            {
                                                std::cout << it4->value.GetString() << std::endl;

                                                it4->value.
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        */

                        std::cout << "LOL!!!" << std::endl;

                        /*
                        {
                            rapidjson::Document d;

                            rapidjson::StringStream ss(temps.at(t).tblu_json_input_string.c_str());

                            d.ParseStream(ss);

                            const rapidjson::Value& a = d["subEntities"];
                            assert(a.IsArray());
                            for (rapidjson::SizeType z = 0; z < a.Size(); z++)
                            {
                                rapidjson::Value::ConstMemberIterator itr1 = a[z].FindMember("logicalParent");
                                if (itr1 != a[z].MemberEnd())
                                {
                                    rapidjson::Value::ConstMemberIterator itr2 = itr1->value.FindMember("entityIndex");
                                    if (itr2 != itr1->value.MemberEnd())
                                    {
                                        printf("subEntities %d: %d\n", z, itr2->value.GetInt());
                                    }
                                }

                                rapidjson::Value::ConstMemberIterator itr2 = a[z].FindMember("entityName");
                                if (itr2 != a[z].MemberEnd())
                                {
                                    printf("subEntities %d: %s\n", z, itr2->value.GetString());
                                }
                            }
                        }*/
                        // 3. Stringify the DOM
                        //rapidjson::StringBuffer buffer;
                        //rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        //d.Accept(writer);

                        // Output {"project":"rapidjson","stars":11}
                        //std::cout << buffer.GetString() << std::endl;

                        //util::initialize_resource_tool();

                        /*resource_tool_dll = LoadLibraryW(L"ResourceTool.dll");

                        resource_tool_ConvertResource = (ConvertResource)GetProcAddress((HMODULE)resource_tool_dll, "ConvertResource");

                        resource_tool_ConvertMemoryResource = (ConvertMemoryResource)GetProcAddress((HMODULE)resource_tool_dll, "ConvertMemoryResource");

                        resource_tool_ConvertMemoryJsonResource = (ConvertMemoryJsonResource)GetProcAddress((HMODULE)resource_tool_dll, "ConvertMemoryJsonResource");

                        resource_tool_GetTEMPEntities = (GetTEMPEntities)GetProcAddress((HMODULE)resource_tool_dll, "GetTEMPEntities");*/

                        //std::string convert = "convert_memory_to_json";
                        //std::string type = "TEMP";

                        //resource_tool_ConvertResource(&convert[0], &type[0], &input_path[0], &output_path[0]);

                        std::cout << temps.at(t).temp_file_name << std::endl;

                        //if (temps.at(t).tblu_return_value == TEMP_TBLU_FOUND)
                        //{
                            //type = "TBLU";

                            //resource_tool_ConvertMemoryResource(&convert[0], &type[0], (void*)temps.at(t).tblu_data.data(), (uint64_t)temps.at(t).tblu_data.size(), &output_path[0]);
                        //}

                        //resource_tool_ConvertMemoryResource(&convert[0], &type[0], (void*)temps.at(t).temp_data.data(), (uint64_t)temps.at(t).temp_data.size());
                        /*
                        size_t idx, max;

                        yyjson_mut_val* key, * val;

                        yyjson_mut_arr_foreach(temps.at(t).temp_json_root_subEntities, idx, max, val)
                        {
                            yyjson_mut_val* temp_json_root_logicalParent = yyjson_mut_obj_get(val, "logicalParent");

                            if (temp_json_root_logicalParent == NULL)
                            {
                                LOG("Error: TEMP JSON parsing error, could not locate logicalParent at subEntities index " + util::uint32_t_to_string(idx));

                                return;
                            }

                            yyjson_mut_val* entityIndex = yyjson_mut_obj_get(temp_json_root_logicalParent, "entityIndex");

                            if (entityIndex == NULL)
                            {
                                LOG("Error: TEMP JSON parsing error, could not locate entityIndex at subEntities index " + util::uint32_t_to_string(idx));

                                return;
                            }

                            uint32_t entityIndex_value = yyjson_mut_get_int(entityIndex);

                            yyjson_mut_val* tblu_entity = yyjson_mut_arr_get(temps.at(t).tblu_json_root_subEntities, idx);

                            yyjson_mut_val* entityName = yyjson_mut_obj_get(tblu_entity, "entityName");

                            const char* entityName_string = yyjson_mut_get_str(entityName);

                            temps.at(t).logical_parents.push_back(entityIndex_value);

                            temps.at(t).entity_names.push_back(entityName_string);
                        }

                        std::cout << temps.at(t).temp_file_name << std::endl;

                        //std::string temp_json_string = std::string(resource_tool_GetTEMPEntities());

                        //std::ofstream temp_json_string_file = std::ofstream("R:\\OUTPUT.JSON", std::ofstream::binary);

                        //if (!temp_json_string_file.good())
                        //{
                            //LOG_AND_EXIT("Error: Meta data file R:\\OUTPUT.JSON could not be created.");
                        //}

                        //temp_json_string_file.write(temp_json_string.c_str(), temp_json_string.length());

                        //temp_json_string_file.close();
                        */
                        /*
                            This is the default flag for JSON reader(RFC - 8259 or ECMA - 404 compliant):

                                Read positive integer as uint64_t.
                                Read negative integer as int64_t.
                                Read floating - point number as double with correct rounding.
                                Read integer which cannot fit in uint64_t or int64_t as double.
                                Report error if real number is infinity.
                                Report error if string contains invalid UTF - 8 character or BOM.
                                Report error on trailing commas, comments, infand nan literals.
                        */

                        //const char* json_data = resource_tool_GetTEMPEntities();
                        /*
                        yyjson_doc* doc = yyjson_read(json_data, strlen(json_data), 0);

                        //yyjson_val* json_root = yyjson_doc_get_root(doc);

                        yyjson_mut_doc* json_doc_mut = yyjson_doc_mut_copy(doc, NULL);

                        yyjson_mut_val* json_root_mut = yyjson_mut_doc_get_root(json_doc_mut);

                        yyjson_mut_val* json_root_subEntities = yyjson_mut_obj_get(json_root_mut, "subEntities");

                        size_t idx, max;

                        yyjson_mut_val* key, * val;

                        yyjson_mut_arr_foreach(json_root_subEntities, idx, max, val)
                        {
                            yyjson_mut_val* json_root_logicalParent = yyjson_mut_obj_get(val, "logicalParent");

                            if (json_root_logicalParent == NULL)
                            {
                                LOG("Error: TEMP JSON parsing error, could not locate logicalParent at subEntities index " + util::uint32_t_to_string(idx));

                                return;
                            }

                            yyjson_mut_val* entityIndex = yyjson_mut_obj_get(json_root_logicalParent, "entityIndex");

                            if (entityIndex == NULL)
                            {
                                LOG("Error: TEMP JSON parsing error, could not locate entityIndex at subEntities index " + util::uint32_t_to_string(idx));

                                return;
                            }

                            //printf("entityIndex: %d\n", (int)yyjson_mut_get_int(entityIndex));
                        }
                        */
                        //yyjson_mut_val* json_root_subEntities = yyjson_mut_doc_get_pointer(json_doc_mut, "/subEntities");
                        /*
                        size_t length = 0;

                        //char* json = yyjson_mut_write(json_doc_mut, YYJSON_WRITE_PRETTY, &length);

                        convert = "generate_from_memory_json";

                        //resource_tool_ConvertMemoryJsonResource(&convert[0], &type[0], json, length, &output_path[0]);

                        //printf("%s\n", json);

                        //free(json);

                        std::vector<yyjson_mut_val*> json_subEntities;
                        std::vector<yyjson_mut_val*> json_subEntities_values;
                        std::vector<std::string> json_subEntities_pointers;

                        //size_t idx, max;

                        //yyjson_mut_val* key, * val;

                        //yyjson_mut_arr_foreach(json_root_subEntities, idx, max, val)
                        //{
                            //json_subEntities.push_back(val);
                        //}

                        std::set<std::string> logical_parents_set;

                        size_t subEntities_count = yyjson_mut_arr_size(yyjson_mut_doc_get_pointer(json_doc_mut, "/subEntities"));

                        for (size_t s = 0; s < subEntities_count; s++)
                        {
                            std::string pointer = "/subEntities/" + std::to_string(s) + "/logicalParent/entityIndex";

                            size_t entityIndex = yyjson_mut_get_uint(yyjson_mut_doc_get_pointer(json_doc_mut, pointer.c_str()));

                            if (entityIndex >= subEntities_count)
                            {
                                logical_parents_set.insert(util::uint32_t_to_string(entityIndex));
                            }
                        }

                        for (uint64_t j = 0; j < json_subEntities.size(); j++)
                        {
                            std::cout << "subEntities (" << j << "): ";

                            std::string temp_path = "/subEntities/" + std::to_string(j) + "/propertyValues";

                            size_t temp_array_size = yyjson_mut_arr_size(yyjson_mut_doc_get_pointer(json_doc_mut, temp_path.c_str()));

                            for (uint64_t k = 0; k < temp_array_size; k++)
                            {
                                temp_path = "/subEntities/" + std::to_string(j) + "/propertyValues/" + std::to_string(k) + "/nPropertyID";

                                const char* temp_string = yyjson_mut_get_str(yyjson_mut_doc_get_pointer(json_doc_mut, temp_path.c_str()));

                                if (temp_string != NULL)
                                {
                                    std::cout << temp_string << ", ";
                                }

                                temp_path = "/subEntities/" + std::to_string(j) + "/propertyValues/" + std::to_string(k) + "/value/$type";

                                temp_string = yyjson_mut_get_str(yyjson_mut_doc_get_pointer(json_doc_mut, temp_path.c_str()));

                                if (temp_string != NULL)
                                {
                                    std::cout << temp_string;
                                }

                                std::cout << std::endl;

                                temp_path = "/subEntities/" + std::to_string(j) + "/propertyValues/" + std::to_string(k) + "/value/$val";

                                yyjson_mut_val* temp_val = yyjson_mut_doc_get_pointer(json_doc_mut, temp_path.c_str());

                                std::string pointer = temp_path = "/subEntities/" + std::to_string(j) + "/propertyValues/" + std::to_string(k) + "/value/$val";

                                get_values(temp_val, pointer, json_subEntities_values, json_subEntities_pointers);
                            }
                        }

                        for (uint64_t j = 0; j < json_subEntities_pointers.size(); j++)
                        {
                            yyjson_mut_val* temp_val = yyjson_mut_doc_get_pointer(json_doc_mut, json_subEntities_pointers.at(j).c_str());

                            if (temp_val == NULL)
                            {
                                //std::cout << json_subEntities_pointers.at(j) << " is NULL" << std::endl;
                            }
                            else
                            {
                                //std::cout << json_subEntities_pointers.at(j) << " value:" << std::endl;

                                //get_json_val(temp_val);
                            }
                        }

                        for (uint64_t j = 0; j < json_subEntities.size(); j++)
                        {
                            yyjson_mut_val* temp_propertyValues = yyjson_mut_obj_get(json_subEntities.at(j), "propertyValues");

                            yyjson_mut_arr_foreach(temp_propertyValues, idx, max, val)
                            {
                                size_t idx2, max2;

                                yyjson_mut_val* key2, * val2;

                                yyjson_mut_obj_foreach(val, idx2, max2, key2, val2)
                                {
                                    get_json_type(key2, val2);

                                    size_t idx3, max3;

                                    yyjson_mut_val* key3, * val3;

                                    yyjson_mut_obj_foreach(val2, idx3, max3, key3, val3)
                                    {
                                        get_json_type(key3, val3);
                                    }
                                }
                            }
                        }
                        */
                        std::cout << "LOL!!!" << std::endl;

                        /*yyjson_val* obj = yyjson_doc_get_root(doc);
                        yyjson_obj_iter iter;
                        yyjson_obj_iter_init(obj, &iter);
                        yyjson_val* key, * val;
                        while ((key = yyjson_obj_iter_next(&iter)))
                        {
                            val = yyjson_obj_iter_get_val(key);
                                
                            get_json_type(key, val);

                            if (yyjson_mut_is_arr(val))
                            {
                                yyjson_val* arr = val;

                                size_t idx, max;

                                yyjson_arr_foreach(arr, idx, max, val)
                                {
                                    std::cout << "1: " << idx << std::endl;

                                    get_json_val(val);

                                    size_t idx2, max2;

                                    yyjson_val* obj = val;

                                    yyjson_val* key2, * val2;

                                    yyjson_obj_foreach(obj, idx2, max2, key2, val2)
                                    {
                                        get_json_type(key2, val2);
                                    }
                                }
                            }
                        }*/

                        /*yyjson_val* subType = yyjson_obj_get(root, "subType");
                        printf("subType: %d\n", (int)yyjson_get_int(subType));

                        yyjson_val* blueprintIndexInResourceHeader = yyjson_obj_get(root, "blueprintIndexInResourceHeader");
                        printf("blueprintIndexInResourceHeader: %d\n", (int)yyjson_get_int(blueprintIndexInResourceHeader));

                        yyjson_val* srootEntityIndextar = yyjson_obj_get(root, "rootEntityIndex");
                        printf("srootEntityIndextar: %d\n", (int)yyjson_get_int(srootEntityIndextar));

                        yyjson_val* subEntities = yyjson_obj_get(root, "subEntities");
                        size_t idx, max;
                        yyjson_val* hit;
                        yyjson_arr_foreach(subEntities, idx, max, hit) {
                            printf("subEntities%d: %d\n", (int)idx, (int)yyjson_get_int(hit));
                        }*/
                    }
                }
            }
        }
    }
}