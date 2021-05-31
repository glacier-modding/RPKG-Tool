#include "rpkg_function.h"
#include "dev_function.h"
#include "global.h"
#include "file.h"
#include "crypto.h"
#include "temp.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include "thirdparty/yyjson/yyjson.h"

void dev_function::dev_resource_tool(std::string& input_path, std::string& filter, std::string& output_path)
{
    initialize_property_map();

    initialize_enum_map();

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    //else
    //{
        //input_path = file::parse_input_folder_path(input_path);
    //}

    if (file::path_exists(input_path) && input_path_is_rpkg_file)
    {
        /*if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path);
        }*/

        std::filesystem::path base_folder_path = input_path;

        std::string parent_path = base_folder_path.parent_path().string();

        rpkg_function::import_rpkg_files_in_folder(parent_path);

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

            if (rpkgs.at(i).rpkg_file_path == input_path)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    //temp temp_temp(i, it->second);

                    std::map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    rpkg_function::recursive_temp_loader(rpkg_index, it->second, parents_map, temps_indexes, 0, 0, 0);

                    for (uint64_t j = 0; j < temps.size(); j++)
                    {
                        resource_tool_dll = LoadLibraryW(L"ResourceTool.dll");

                        resource_tool_ConvertResource = (ConvertResource)GetProcAddress((HMODULE)resource_tool_dll, "ConvertResource");

                        resource_tool_ConvertMemoryResource = (ConvertMemoryResource)GetProcAddress((HMODULE)resource_tool_dll, "ConvertMemoryResource");

                        resource_tool_GetTEMPEntities = (GetTEMPEntities)GetProcAddress((HMODULE)resource_tool_dll, "GetTEMPEntities");

                        std::string convert = "convert_memory_to_json";
                        std::string type = "TEMP";

                        //resource_tool_ConvertResource(&convert[0], &type[0], &input_path[0], &output_path[0]);

                        std::cout << temps.at(j).temp_file_name << std::endl;

                        if (temps.at(j).tblu_return_value == TEMP_TBLU_FOUND && temps.at(j).temp_file_name != "00EE8145B2B6DB36.TEMP")
                        {
                            resource_tool_ConvertMemoryResource(&convert[0], &type[0], (void*)temps.at(j).temp_data.data(), (uint64_t)temps.at(j).temp_data.size(), &output_path[0]);

                            //std::string temp_json_string = std::string(resource_tool_GetTEMPEntities());

                            //std::ofstream temp_json_string_file = std::ofstream("R:\\OUTPUT.JSON", std::ofstream::binary);

                            //if (!temp_json_string_file.good())
                            //{
                                //LOG_AND_EXIT("Error: Meta data file R:\\OUTPUT.JSON could not be created.");
                            //}

                            //temp_json_string_file.write(temp_json_string.c_str(), temp_json_string.length());

                            //temp_json_string_file.close();

                            const char* json_data = resource_tool_GetTEMPEntities();

                            yyjson_doc* doc = yyjson_read(json_data, strlen(json_data), 0);

                            yyjson_val* root = yyjson_doc_get_root(doc);

                            yyjson_val* obj = yyjson_doc_get_root(doc);
                            yyjson_obj_iter iter;
                            yyjson_obj_iter_init(obj, &iter);
                            yyjson_val* key, * val;
                            while ((key = yyjson_obj_iter_next(&iter))) {
                                val = yyjson_obj_iter_get_val(key);
                                printf("%s: %s\n", yyjson_get_str(key), yyjson_get_type_desc(val));
                            }

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
}