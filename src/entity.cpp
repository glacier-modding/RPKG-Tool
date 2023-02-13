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

#pragma comment(lib, "../thirdparty/zhmtools/ResourceLib_HM2.lib")
#pragma comment(lib, "../thirdparty/zhmtools/ResourceLib_HM3.lib")
#pragma comment(lib, "../thirdparty/quickentity-rs/quickentity_rs.dll.lib")

entity::entity() = default;

entity::entity(uint64_t rpkgs_index, uint64_t hash_index, uint32_t temp_version) {
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

                temp_yyjson_doc = yyjson_read(temp_json_input->JsonData, temp_json_input->StrSize, 0);

                rt_temp_json = std::string(temp_json_input->JsonData, temp_json_input->StrSize);

                resource_tool_converter_temp->FreeJsonString(temp_json_input);

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

                tblu_yyjson_doc = yyjson_read(tblu_json_input->JsonData, tblu_json_input->StrSize, 0);

                rt_tblu_json = std::string(tblu_json_input->JsonData, tblu_json_input->StrSize);

                resource_tool_converter_tblu->FreeJsonString(tblu_json_input);

                std::vector<char>().swap(tblu_data);
            }
        }
    }
}

void entity::to_qn_json(std::string& output_path)
{
    std::string temp_meta_json = rpkg_function::generate_hash_meta_json(temp_rpkg_index, temp_hash_index);
    std::string tblu_meta_json = rpkg_function::generate_hash_meta_json(tblu_rpkg_index, tblu_hash_index);

    char* entity_json_string = convert_entity_to_qn(rt_temp_json.c_str(), temp_meta_json.c_str(), rt_tblu_json.c_str(), tblu_meta_json.c_str());

    std::string entity_json = std::string(entity_json_string);

    free_json_string(entity_json_string);

    file::write_to_file(output_path, entity_json);
}

uint32_t
entity::search(std::string search_string, bool search_entity_ids, bool search_entity_names, bool search_property_names,
               bool search_property_values, uint32_t results_count, uint32_t max_results) {
    yyjson_val* temp_root = yyjson_doc_get_root(temp_yyjson_doc);
    yyjson_val* tblu_root = yyjson_doc_get_root(tblu_yyjson_doc);

    yyjson_val* temp_subEntities = yyjson_obj_get(temp_root, "subEntities");
    yyjson_val* tblu_subEntities = yyjson_obj_get(tblu_root, "subEntities");

    if (yyjson_arr_size(temp_subEntities) != yyjson_arr_size(tblu_subEntities)) {
        return results_count;
    }

    size_t tblu_subEntities_index;
    size_t tblu_subEntities_max;
    yyjson_val* tblu_subEntities_entry;

    yyjson_arr_foreach(tblu_subEntities, tblu_subEntities_index, tblu_subEntities_max, tblu_subEntities_entry) {
        if (results_count < max_results) {
            yyjson_val* tblu_entityId = yyjson_obj_get(tblu_subEntities_entry, "entityId");
            yyjson_val* tblu_entityName = yyjson_obj_get(tblu_subEntities_entry, "entityName");

            std::string entityIdString = util::uint64_t_to_hex_string_for_qn(yyjson_get_uint(tblu_entityId));
            std::string entityNameString = util::to_lower_case(yyjson_get_str(tblu_entityName));

            if (entityIdString.find(search_string) != std::string::npos ||
                entityNameString.find(search_string) != std::string::npos) {
                entities_search_results += rpkgs.at(temp_rpkg_index).rpkg_file_path + "||||" +
                                           util::uint64_t_to_hex_string(
                                                   rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value) +
                                           "." + rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_resource_type +
                                           " " + util::hash_to_ioi_string(
                        rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_value, false) + "||||" +
                                           entityIdString + ": " + std::string(yyjson_get_str(tblu_entityName)) +
                                           "||||||";

                results_count++;
            }
        }
    }

    return results_count;
}
