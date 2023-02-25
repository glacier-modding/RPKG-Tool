#include "entity.h"
#include "rpkg_function.h"
#include "global.h"
#include "crypto.h"
#include "file.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/directxmath/DirectXMath.h"
#include "thirdparty/quickentity-rs/quickentity_rs.dll.h"
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <regex>

#pragma comment(lib, "../thirdparty/zhmtools/ResourceLib_HM2.lib")
#pragma comment(lib, "../thirdparty/zhmtools/ResourceLib_HM3.lib")
#pragma comment(lib, "../thirdparty/quickentity-rs/quickentity_rs.dll.lib")

entity::entity() = default;

entity::entity(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version, std::string& output_path) {
    temp_rpkg_index = rpkgs_index;
    temp_hash_index = hash_index;

    int tblu_return_value = 0;
    std::vector<std::string> tblu_depends_file_name;
    uint64_t tblu_hash_value = 0;

    uint32_t temp_hash_reference_count =
            rpkgs.at(rpkgs_index).hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    if (temp_hash_reference_count > 0) {
        for (uint64_t i = 0; i < temp_hash_reference_count; i++) {
            std::vector<std::string> temp_tblu_depends_in_rpkgs;
            std::vector<uint32_t> temp_tblu_depends_rpkg_index;
            std::vector<uint32_t> temp_tblu_depends_hash_index;

            bool tblu_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++) {
                auto it = rpkgs.at(j).hash_map.find(
                        rpkgs.at(rpkgs_index).hash.at(hash_index).hash_reference_data.hash_reference.at(i));

                if (it == rpkgs.at(j).hash_map.end()) continue;

                if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TBLU") {
                    if (!tblu_found) {
                        tblu_depends_file_name.push_back(
                                util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                                rpkgs.at(j).hash.at(it->second).hash_resource_type);

                        tblu_return_value = TEMP_TBLU_FOUND;

                        tblu_hash_value = rpkgs.at(j).hash.at(it->second).hash_value;
                    } else {
                        if (util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                            rpkgs.at(j).hash.at(it->second).hash_resource_type != tblu_depends_file_name.back()) {
                            tblu_return_value = TEMP_TBLU_TOO_MANY;
                        }
                    }

                    tblu_found = true;
                }
            }
        }
    }

    if (tblu_return_value == TEMP_TBLU_FOUND) {
        uint32_t rpkg_index = rpkg_function::get_latest_hash(tblu_hash_value);

        if (rpkg_index != UINT32_MAX) {
            auto it6 = rpkgs.at(rpkg_index).hash_map.find(tblu_hash_value);

            if (it6 != rpkgs.at(rpkg_index).hash_map.end()) {
                tblu_rpkg_index = rpkg_index;
                tblu_hash_index = it6->second;

                uint64_t temp_hash_size;

                if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.lz4ed) {
                    temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.header.data_size;

                    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.xored) {
                        temp_hash_size &= 0x3FFFFFFF;
                    }
                } else {
                    temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.resource.size_final;
                }

                std::vector<char> temp_data;

                std::vector<char> temp_input_data = std::vector<char>(temp_hash_size, 0);

                std::ifstream file = std::ifstream(rpkgs.at(temp_rpkg_index).rpkg_file_path, std::ifstream::binary);

                if (!file.good()) {
                    LOG_AND_EXIT(
                            "Error: RPKG file " + rpkgs.at(temp_rpkg_index).rpkg_file_path + " could not be read.");
                }

                file.seekg(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.header.data_offset, std::ifstream::beg);
                file.read(temp_input_data.data(), temp_hash_size);
                file.close();

                if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.xored) {
                    crypto::xor_data(temp_input_data.data(), (uint32_t) temp_hash_size);
                }

                uint32_t temp_decompressed_size = rpkgs.at(temp_rpkg_index).hash.at(
                        temp_hash_index).data.resource.size_final;

                std::vector<char> temp_output_data = std::vector<char>(temp_decompressed_size, 0);

                if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).data.lz4ed) {
                    LZ4_decompress_safe(temp_input_data.data(), temp_output_data.data(), (int) temp_hash_size,
                                        temp_decompressed_size);

                    temp_data = temp_output_data;
                } else {
                    temp_data = temp_input_data;
                }

                std::vector<char>().swap(temp_output_data);
                std::vector<char>().swap(temp_input_data);

                ResourceConverter* resource_tool_converter_temp;

                if (temp_version == 2) {
                    resource_tool_converter_temp = HM2_GetConverterForResource("TEMP");
                } else {
                    resource_tool_converter_temp = HM3_GetConverterForResource("TEMP");
                }

                JsonString* temp_json_input = resource_tool_converter_temp->FromMemoryToJsonString(
                        (const void*) temp_data.data(), (size_t) temp_data.size());

                //std::string rt_temp_json = std::string(temp_json_input->JsonData, temp_json_input->StrSize);

                std::vector<char>().swap(temp_data);

                std::string tblu_hash_file_name =
                        util::uint64_t_to_hex_string(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_value) +
                        "." + rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_resource_type;

                uint64_t tblu_hash_size;

                if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.lz4ed) {
                    tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.header.data_size;

                    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.xored) {
                        tblu_hash_size &= 0x3FFFFFFF;
                    }
                } else {
                    tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.resource.size_final;
                }

                std::vector<char> tblu_data;

                std::vector<char> tblu_input_data = std::vector<char>(tblu_hash_size, 0);

                std::ifstream file2 = std::ifstream(rpkgs.at(tblu_rpkg_index).rpkg_file_path, std::ifstream::binary);

                if (!file2.good()) {
                    LOG_AND_EXIT(
                            "Error: RPKG file " + rpkgs.at(tblu_rpkg_index).rpkg_file_path + " could not be read.");
                }

                file2.seekg(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.header.data_offset, std::ifstream::beg);
                file2.read(tblu_input_data.data(), tblu_hash_size);
                file2.close();

                if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.xored) {
                    crypto::xor_data(tblu_input_data.data(), (uint32_t) tblu_hash_size);
                }

                uint32_t tblu_decompressed_size = rpkgs.at(tblu_rpkg_index).hash.at(
                        tblu_hash_index).data.resource.size_final;

                std::vector<char> tblu_output_data = std::vector<char>(tblu_decompressed_size, 0);

                if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).data.lz4ed) {
                    LZ4_decompress_safe(tblu_input_data.data(), tblu_output_data.data(), (int) tblu_hash_size,
                                        tblu_decompressed_size);

                    tblu_data = tblu_output_data;
                } else {
                    tblu_data = tblu_input_data;
                }

                std::vector<char>().swap(tblu_output_data);
                std::vector<char>().swap(tblu_input_data);

                ResourceConverter* resource_tool_converter_tblu;

                if (temp_version == 2) {
                    resource_tool_converter_tblu = HM2_GetConverterForResource("TBLU");
                } else {
                    resource_tool_converter_tblu = HM3_GetConverterForResource("TBLU");
                }

                JsonString* tblu_json_input = resource_tool_converter_tblu->FromMemoryToJsonString(
                        (const void*) tblu_data.data(), (size_t) tblu_data.size());

                //std::string rt_tblu_json = std::string(tblu_json_input->JsonData, tblu_json_input->StrSize);

                std::vector<char>().swap(tblu_data);

                std::string temp_meta_json = rpkg_function::generate_hash_meta_json(temp_rpkg_index, temp_hash_index);
                std::string tblu_meta_json = rpkg_function::generate_hash_meta_json(tblu_rpkg_index, tblu_hash_index);

                char* entity_json_string = convert_entity_to_qn(temp_json_input->JsonData, temp_meta_json.c_str(), tblu_json_input->JsonData, tblu_meta_json.c_str());

                //std::string entity_json = std::string(entity_json_string);

                if (output_path != "")
                    file::write_to_file(output_path, entity_json_string);

                entity_yyjson_doc = yyjson_read(entity_json_string, std::strlen(entity_json_string), 0);

                free_json_string(entity_json_string);
                resource_tool_converter_temp->FreeJsonString(temp_json_input);
                resource_tool_converter_tblu->FreeJsonString(tblu_json_input);
            }
        }
    }
}

uint32_t entity::search(std::vector<search_item>& search_items_input,
                        uint32_t results_count,
                        uint32_t max_results) {
    search_items = search_items_input;

    yyjson_val* root = yyjson_doc_get_root(entity_yyjson_doc);
    yyjson_val* entities = yyjson_obj_get(root, "entities");

    yyjson_val* key, * val;
    yyjson_obj_iter iter;

    yyjson_obj_iter_init(root, &iter);

    while ((key = yyjson_obj_iter_next(&iter))) {
        val = yyjson_obj_iter_get_val(key);

        std::string key_string(yyjson_get_str(key));

        if (key_string == "entities") {
            yyjson_val* key2, * val2;
            yyjson_obj_iter iter2;

            yyjson_obj_iter_init(entities, &iter2);

            std::string entities_size = std::to_string(yyjson_obj_size(entities));
            int entities_index = 0;

            timing_string = "Searching " + entities_size + " entities in " + util::hash_to_ioi_string(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, true) + "...";

            while ((key2 = yyjson_obj_iter_next(&iter2))) {
                const char* key_string2 = yyjson_get_str(key2);

                entities_index++;

                if (results_count < max_results) {
                    val2 = yyjson_obj_iter_get_val(key2);

                    bool found = false;

                    search_value(found, key_string2);

                    char* temp_json = yyjson_val_write(val2, YYJSON_WRITE_NOFLAG, NULL);
                    search_value(found, temp_json);
                    free(temp_json);

                    if (found) {
                        yyjson_val* name2 = yyjson_obj_get(val2, "name");

                        char* temp_json = yyjson_val_write(val2, YYJSON_WRITE_PRETTY, NULL);

                        entities_search_results += rpkgs.at(temp_rpkg_index).rpkg_file_path +
                            "||||" +
                            util::uint64_t_to_hex_string(
                                rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) +
                            "." +
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type +
                            " " +
                            util::hash_to_ioi_string(
                                rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, false) +
                            "||||" +
                            key_string2 +
                            ": " +
                            std::string(yyjson_get_str(name2)) +
                            "||||" +
                            std::string(temp_json) +
                            "||||" +
                            key_string +
                            "||||||";

                        free(temp_json);

                        results_count++;
                    }
                }
            }
        }
        else if (yyjson_is_arr(val)) {
            yyjson_val* val2;
            yyjson_arr_iter iter2;
            int arr_index = 0;

            yyjson_arr_iter_init(val, &iter2);

            while ((val2 = yyjson_arr_iter_next(&iter2))) {
                if (results_count < max_results) {
                    bool found = false;

                    //if (search_json(val2, search_string))
                    //    found = true;

                    char* temp_json = yyjson_val_write(val2, YYJSON_WRITE_NOFLAG, NULL);
                    search_value(found, temp_json);
                    free(temp_json);

                    if (found) {
                        char* temp_json = yyjson_val_write(val2, YYJSON_WRITE_PRETTY, NULL);

                        entities_search_results += rpkgs.at(temp_rpkg_index).rpkg_file_path +
                            "||||" +
                            util::uint64_t_to_hex_string(
                                rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) +
                            "." +
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type +
                            " " +
                            util::hash_to_ioi_string(
                                rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, false) +
                            "||||" +
                            key_string +
                            ": " +
                            std::to_string(arr_index) +
                            "||||" +
                            std::string(temp_json) +
                            "||||" +
                            key_string +
                            "||||||";

                        free(temp_json);

                        results_count++;
                    }
                }

                arr_index++;
            }
        }
        else if (yyjson_is_obj(val)) {
            yyjson_val* key2, * val2;
            yyjson_obj_iter iter2;

            yyjson_obj_iter_init(val, &iter);

            while ((key2 = yyjson_obj_iter_next(&iter2))) {
                if (results_count < max_results) {
                    val2 = yyjson_obj_iter_get_val(key2);

                    const char* key_string2 = yyjson_get_str(key2);

                    bool found = false;

                    search_value(found, key_string2);

                    char* temp_json = yyjson_val_write(val2, YYJSON_WRITE_NOFLAG, NULL);
                    search_value(found, temp_json);
                    free(temp_json);

                    if (found) {
                        char* temp_json = yyjson_val_write(val2, YYJSON_WRITE_PRETTY, NULL);

                        entities_search_results += rpkgs.at(temp_rpkg_index).rpkg_file_path +
                            "||||" +
                            util::uint64_t_to_hex_string(
                                rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) +
                            "." +
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type +
                            " " +
                            util::hash_to_ioi_string(
                                rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, false) +
                            "||||" +
                            key_string +
                            ": " +
                            key_string2 +
                            "||||" +
                            std::string(temp_json) +
                            "||||" +
                            key_string +
                            "||||||";

                        free(temp_json);

                        results_count++;
                    }
                }
            }
        }
        else if (yyjson_is_num(val)) {
            if (results_count < max_results) {
                std::string val_string = "";

                if (yyjson_is_uint(val))
                    val_string = std::to_string(yyjson_get_uint(val));
                if (yyjson_is_sint(val))
                    val_string = std::to_string(yyjson_get_sint(val));
                if (yyjson_is_int(val))
                    val_string = std::to_string(yyjson_get_int(val));
                if (yyjson_is_real(val))
                    val_string = std::to_string(yyjson_get_real(val));

                bool found = false;

                search_value(found, val_string.c_str());

                if (found) {
                    char* temp_json = yyjson_val_write(val, YYJSON_WRITE_PRETTY, NULL);

                    entities_search_results += rpkgs.at(temp_rpkg_index).rpkg_file_path +
                        "||||" +
                        util::uint64_t_to_hex_string(
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) +
                        "." +
                        rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type +
                        " " +
                        util::hash_to_ioi_string(
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, false) +
                        "||||" +
                        key_string +
                        ": " + 
                        val_string +
                        "||||" +
                        std::string(temp_json) +
                        "||||" +
                        key_string +
                        "||||||";

                    free(temp_json);

                    results_count++;
                }
            }
        }
        else if (yyjson_is_str(val)) {
            if (results_count < max_results) {
                const char* val_string = yyjson_get_str(val);

                bool found = false;

                search_value(found, val_string);

                if (found) {
                    char* temp_json = yyjson_val_write(val, YYJSON_WRITE_PRETTY, NULL);

                    entities_search_results += rpkgs.at(temp_rpkg_index).rpkg_file_path +
                        "||||" +
                        util::uint64_t_to_hex_string(
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) +
                        "." +
                        rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type +
                        " " +
                        util::hash_to_ioi_string(
                            rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, false) +
                        "||||" +
                        key_string +
                        ": " +
                        val_string +
                        "||||" +
                        std::string(temp_json) +
                        "||||" +
                        key_string +
                        "||||||";

                    free(temp_json);

                    results_count++;
                }
            }
        }
    }

    std::vector<search_item>().swap(search_items);

    return results_count;
}

bool entity::search_json(yyjson_val* json, std::string& search_string)
{
    if (yyjson_is_null(json))
        return false;

    if (yyjson_is_arr(json)) {
        yyjson_val* val;
        yyjson_arr_iter iter;
        yyjson_arr_iter_init(json, &iter);

        while ((val = yyjson_arr_iter_next(&iter))) {
            if (search_json(val, search_string))
                return true;
        }
    }
    else if (yyjson_is_obj(json)) {
        yyjson_val* key, * val;
        yyjson_obj_iter iter;
        yyjson_obj_iter_init(json, &iter);

        while ((key = yyjson_obj_iter_next(&iter))) {
            val = yyjson_obj_iter_get_val(key);

            std::string key_string = util::to_lower_case(yyjson_get_str(key));

            if (key_string.find(search_string) != std::string::npos)
                return true;

            if (search_json(val, search_string))
                return true;
        }
    }
    else if (yyjson_is_num(json)) {
        std::string val_string = "";

        if (yyjson_is_uint(json))
            val_string = std::to_string(yyjson_get_uint(json));
        if (yyjson_is_sint(json))
            val_string = std::to_string(yyjson_get_sint(json));
        if (yyjson_is_int(json))
            val_string = std::to_string(yyjson_get_int(json));
        if (yyjson_is_real(json))
            val_string = std::to_string(yyjson_get_real(json));        

        if (val_string.find(search_string) != std::string::npos)
            return true;
    }
    else if (yyjson_is_str(json)) {
        std::string val_string = util::to_lower_case(yyjson_get_str(json));

        if (val_string.find(search_string) != std::string::npos)
            return true;
    }

    return false;
}

void entity::free_yyjson_doc() {
    yyjson_doc_free(entity_yyjson_doc);
}

bool entity::find_ci(const char* s1, const char* s2) {
    // s1 is haystack, s2 is needle
    // s2 should already be all lower case
    size_t pos = 0;
    bool found = false;

    while (!found) {
        if (s1[pos] == '\0')
            return false;

        size_t pos1 = pos;
        size_t pos2 = 0;
        found = true;

        while (s2[pos2] != '\0') {
            if (s1[pos1] == '\0')
                return false;

            if (std::tolower(s1[pos1]) != s2[pos2]) {
                found = false;
                break;
            }

            pos1++;
            pos2++;
        }

        pos++;
    }

    return true;
}

void entity::search_value(bool& found, const char* value) {
    for (auto item : search_items) {
        bool temp_found = false;

        if (item.type == DEFAULT) {
            if (find_ci(value, item.search.c_str()))
                temp_found = true;
        }
        else if (item.type == REGEX) {
            if (std::regex_search(value, std::regex(item.search.c_str(), std::regex_constants::icase)))
                temp_found = true;
        }

        if (item.operation == NONE || item.operation == OR)
            found = found || temp_found;
        else if (item.operation == AND)
            found = found && temp_found;
    }
}