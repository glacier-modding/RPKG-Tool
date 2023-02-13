#include "rpkg_function.h"
#include "global.h"
#include "util.h"

std::string rpkg_function::generate_hash_meta_json(const uint64_t rpkg_index, const uint64_t hash_index)
{
    // Output hash's meta data to JSON format
    nlohmann::ordered_json json;
    json["hash_value"] = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(hash_index).hash_value);
    json["hash_offset"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.header.data_offset;
    json["hash_size"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.header.data_size;
    json["hash_resource_type"] = std::string(rpkgs.at(rpkg_index).hash.at(hash_index).hash_resource_type.data(), 4);
    json["hash_reference_table_size"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.resource.reference_table_size;
    json["hash_reference_table_dummy"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.resource.reference_table_dummy;
    json["hash_size_final"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.resource.size_final;
    json["hash_size_in_memory"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.resource.size_in_memory;
    json["hash_size_in_video_memory"] = rpkgs.at(rpkg_index).hash.at(hash_index).data.resource.size_in_video_memory;
    json["hash_reference_data"] = nlohmann::ordered_json::array();

    uint32_t hash_reference_count = rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    for (size_t i = 0; i < rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference.size(); i++)
    {
        nlohmann::ordered_json depend;
        depend["hash"] = util::hash_to_ioi_string(rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference[i], true);
        depend["flag"] = util::uint8_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference_type[i]);
        json["hash_reference_data"].push_back(depend);
    }

    return json.dump();
}