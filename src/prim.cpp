#include "prim.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "borg.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <fstream>

prim::prim(uint64_t rpkgs_index, uint64_t hash_index) {
    prim_rpkg_index = rpkgs_index;
    prim_hash_index = hash_index;

    prim_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." +
                     rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    load_hash_depends();

    uint64_t prim_hash_size;

    if (rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.lz4ed) {
        prim_hash_size = rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.header.data_size;

        if (rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.xored) {
            prim_hash_size &= 0x3FFFFFFF;
        }
    } else {
        prim_hash_size = rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.resource.size_final;
    }

    prim_input_data = std::vector<char>(prim_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(prim_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(prim_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.header.data_offset, std::ifstream::beg);
    file.read(prim_input_data.data(), prim_hash_size);
    file.close();

    if (rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.xored) {
        crypto::xor_data(prim_input_data.data(), (uint32_t) prim_hash_size);
    }

    uint32_t prim_decompressed_size = rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.resource.size_final;

    prim_output_data = std::vector<char>(prim_decompressed_size, 0);

    if (rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).data.lz4ed) {
        LZ4_decompress_safe(prim_input_data.data(), prim_output_data.data(), (int) prim_hash_size,
                            prim_decompressed_size);

        prim_data = prim_output_data;
    } else {
        prim_data = prim_input_data;
    }

    std::vector<char>().swap(prim_output_data);
    std::vector<char>().swap(prim_input_data);

    prim_meta_data = prim_data;

    asset3ds_data.root_name = prim_file_name;

    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;

    uint32_t prim_position = 0;

    //LOG("PRIM file: " + prim_file_name);

    auto it2 = hash_list_hash_map.find(rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).hash_value);

    if (it2 != hash_list_hash_map.end()) {
        //LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
    }

    std::memcpy(&prim_primary_header_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary header offset: " + util::uint32_t_to_hex_string(prim_primary_header_offset));

    prim_position = prim_primary_header_offset;

    prim_object_header primary_prim_object_header(prim_data, prim_position);

    prim_object_headers.push_back(primary_prim_object_header);

    if (primary_prim_object_header.prim_header_instance.type == (uint16_t) prim_header::header_type::OBJECT) {
        //LOG("  - PRIM primary header type: Object");
    } else if (primary_prim_object_header.prim_header_instance.type == (uint16_t) prim_header::header_type::MESH) {
        //LOG("Error: Not supported: PRIM primary header type: Mesh");
        success = false;
        return;
    } else {
        //LOG("Error: Not supported: PRIM primary header type: Unknown");
        success = false;
        return;
    }

    prim_position = primary_prim_object_header.object_offset;

    for (uint32_t o = 0; o < primary_prim_object_header.object_count; o++) {
        std::memcpy(&bytes4, &prim_data[prim_position], BYTES4);
        prim_position += 0x4;

        prim_object_offsets.push_back(bytes4);

        //LOG("  - PRIM object (" + std::to_string(o) + ") offset:" + util::uint32_t_to_hex_string(prim_object_offsets.back()));
    }

    for (uint32_t o = 0; o < prim_object_offsets.size(); o++) {
        //LOG("  - PRIM object (" + std::to_string(o) + ") offset:" + util::uint32_t_to_hex_string(prim_object_offsets.at(o)));

        prim_position = prim_object_offsets.at(o);

        //prim_object temp_prim_object(prim_data, prim_position, o);

        uint16_t header_type_value = 0;
        uint16_t object_sub_type_value = 0;

        prim_position += 0x2;

        std::memcpy(&header_type_value, &prim_data[prim_position], BYTES2);

        prim_position += 0x2;

        //LOG("      - PRIM object (" + std::to_string(o) + ") header type value: " + util::uint16_t_to_hex_string(header_type_value));

//        if (header_type_value == 1) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header type: Object");
//        } else if (header_type_value == 2) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header type: Mesh");
//        } else {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header type: Unknown");
//        }

        std::memcpy(&object_sub_type_value, &prim_data[prim_position], BYTES1);

        //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type value: " + util::uint8_t_to_hex_string(object_sub_type_value));

//        if (object_sub_type_value == 0x0) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Standard");
//        } else if (object_sub_type_value == 0x1) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Linked");
//        } else if (object_sub_type_value == 0x2) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Weighted");
//        } else if (object_sub_type_value == 0x3) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV2");
//        } else if (object_sub_type_value == 0x4) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV3");
//        } else if (object_sub_type_value == 0x5) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV4");
//        } else if (object_sub_type_value == 0x6) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: SpeedTree");
//        }

        prim_position = prim_object_offsets.at(o);

        if (header_type_value == (uint16_t) prim_header::header_type::MESH) {
            if (object_sub_type_value == (uint16_t) prim_object::SUBTYPE::STANDARD) {
                prim_mesh temp_prim_mesh(prim_data, prim_position, o);
                prim_meshes.push_back(temp_prim_mesh);

                prim_position = temp_prim_mesh.sub_mesh_table_offset;

                prim_sub_mesh temp_prim_sub_mesh(prim_data, prim_position, o, temp_prim_mesh, prim_meta_data,
                                                 primary_prim_object_header.flag_high_resolution, success);
                prim_sub_meshes.push_back(temp_prim_sub_mesh);

                asset3ds_data.names.push_back(prim_file_name + "_" + std::to_string(o));
                asset3ds_data.indexes.push_back(temp_prim_sub_mesh.indexes);
                asset3ds_data.vertexes.push_back(temp_prim_sub_mesh.vertexes);
                asset3ds_data.normals.push_back(temp_prim_sub_mesh.vertexes_normals);
                asset3ds_data.tangents.push_back(temp_prim_sub_mesh.vertexes_tangents);
                asset3ds_data.uvs.push_back(temp_prim_sub_mesh.vertexes_uvs);
                asset3ds_data.colors.push_back(temp_prim_sub_mesh.vertexes_colors);
                asset3ds_data.vertexes_weighted_bone_ids_0.push_back(temp_prim_sub_mesh.vertexes_weighted_bone_ids_0);
                asset3ds_data.vertexes_weighted_bone_ids_1.push_back(temp_prim_sub_mesh.vertexes_weighted_bone_ids_1);
                asset3ds_data.vertexes_weighted_weights_0.push_back(temp_prim_sub_mesh.vertexes_weighted_weights_0);
                asset3ds_data.vertexes_weighted_weights_1.push_back(temp_prim_sub_mesh.vertexes_weighted_weights_1);
                asset3ds_data.bones_nodes.push_back(temp_prim_sub_mesh.bones_nodes_data);
                asset3ds_data.bones_infos.push_back(temp_prim_sub_mesh.bones_info_data);
                asset3ds_data.bones_indices.push_back(temp_prim_sub_mesh.bones_indices_data);
                asset3ds_data.material_ids.push_back(temp_prim_mesh.prim_object_instance.material_id);

                prim_weighted_mesh temp_prim_weighted_mesh;
                prim_weighted_meshes.push_back(temp_prim_weighted_mesh);

                prim_object_is_weighted.push_back(false);
            } else if (object_sub_type_value == (uint16_t) prim_object::SUBTYPE::WEIGHTED ||
                       object_sub_type_value == (uint16_t) prim_object::SUBTYPE::LINKED) {
                prim_weighted_mesh temp_prim_weighted_mesh(prim_data, prim_position, o);
                prim_weighted_meshes.push_back(temp_prim_weighted_mesh);

                prim_position = temp_prim_weighted_mesh.prim_mesh_instance.sub_mesh_table_offset;

                prim_weighted_sub_mesh temp_prim_weighted_sub_mesh(prim_data, prim_position, o, temp_prim_weighted_mesh,
                                                                   prim_meta_data,
                                                                   primary_prim_object_header.flag_high_resolution,
                                                                   success);
                prim_weighted_sub_meshes.push_back(temp_prim_weighted_sub_mesh);

                asset3ds_data.names.push_back(prim_file_name + "_" + std::to_string(o));
                asset3ds_data.indexes.push_back(temp_prim_weighted_sub_mesh.indexes);
                asset3ds_data.vertexes.push_back(temp_prim_weighted_sub_mesh.vertexes);
                asset3ds_data.normals.push_back(temp_prim_weighted_sub_mesh.vertexes_normals);
                asset3ds_data.tangents.push_back(temp_prim_weighted_sub_mesh.vertexes_tangents);
                asset3ds_data.uvs.push_back(temp_prim_weighted_sub_mesh.vertexes_uvs);
                asset3ds_data.colors.push_back(temp_prim_weighted_sub_mesh.vertexes_colors);
                asset3ds_data.vertexes_weighted_bone_ids_0.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_bone_ids_0);
                asset3ds_data.vertexes_weighted_bone_ids_1.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_bone_ids_1);
                asset3ds_data.vertexes_weighted_weights_0.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_weights_0);
                asset3ds_data.vertexes_weighted_weights_1.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_weights_1);
                asset3ds_data.bones_nodes.push_back(temp_prim_weighted_sub_mesh.bones_nodes_data);
                asset3ds_data.bones_infos.push_back(temp_prim_weighted_sub_mesh.bones_info_data);
                asset3ds_data.bones_indices.push_back(temp_prim_weighted_sub_mesh.bones_indices_data);
                asset3ds_data.material_ids.push_back(
                        temp_prim_weighted_mesh.prim_mesh_instance.prim_object_instance.material_id);

                prim_mesh temp_prim_mesh;
                prim_meshes.push_back(temp_prim_mesh);

                if (object_sub_type_value == (uint16_t) prim_object::SUBTYPE::WEIGHTED) {
                    prim_object_is_weighted.push_back(true);
                } else {
                    prim_object_is_weighted.push_back(false);
                }
            } else {
                //LOG("Error: Not supported: PRIM object type: " + util::uint32_t_to_hex_string(object_sub_type_value));
                success = false;
                return;
            }
        }
    }

    if (!borg_depends_file_name.empty()) {
        borg temp_borg(borg_depends_rpkg_index.at(0).at(borg_depends_rpkg_index_index.at(0)),
                       borg_depends_hash_index.at(0).at(borg_depends_hash_index_index.at(0)));

        asset3ds_data.bones_data = temp_borg.bones_data;
        asset3ds_data.bones_positions = temp_borg.bones_positions;
        asset3ds_data.bones_inverse_matrices = temp_borg.bones_inverse_bind_matrices;
    }

    asset3ds_data.weighted = prim_object_is_weighted;
}

prim::prim(const std::string& prim_file_path) {
    std::ifstream file = std::ifstream(prim_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: PRIM file " + prim_file_path + " could not be read.");
    }

    file.seekg(0, std::ifstream::beg);

    uint64_t rpkg_file_name_length = 0;
    uint32_t prim_decompressed_size = 0;
    uint32_t prim_hash_size = 0;
    uint32_t bone_name_count = 0;

    char input[1024];
    char char4[4];

    file.read(char4, 0x4);
    std::memcpy(&rpkg_file_name_length, &char4, 0x4);

    //LOG("  - TEXT meta RPKG file name length: " + util::uint64_t_to_hex_string(rpkg_file_name_length));

    file.read(input, rpkg_file_name_length);

    rpkg_output_file = std::string(input);

    //LOG("  - TEXT meta RPKG file name: " + rpkg_output_file);

    file.read(char4, 0x4);
    std::memcpy(&prim_decompressed_size, &char4, 0x4);

    file.read(char4, 0x4);
    std::memcpy(&prim_hash_size, &char4, 0x4);

    file.read(char4, 0x4);
    std::memcpy(&bone_name_count, &char4, 0x4);

    prim_input_data = std::vector<char>(prim_hash_size, 0);

    file.read(prim_input_data.data(), prim_hash_size);

    for (uint32_t b = 0; b < bone_name_count; b++) {
        uint32_t bone_name_length = 0;

        file.read(char4, 0x4);
        std::memcpy(&bone_name_length, &char4, 0x4);

        file.read(input, bone_name_length);
        borg_bone_name_map[std::string(input)] = b;
    }

    file.close();

    prim_output_data = std::vector<char>(prim_decompressed_size, 0);

    LZ4_decompress_safe(prim_input_data.data(), prim_output_data.data(), (int) prim_hash_size, prim_decompressed_size);

    prim_data = prim_output_data;

    std::vector<char>().swap(prim_output_data);
    std::vector<char>().swap(prim_input_data);

    prim_meta_data = prim_data;

    asset3ds_data.root_name = prim_file_name;

    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    uint32_t prim_position = 0;

    //LOG("PRIM file: " + prim_file_name);

    std::memcpy(&prim_primary_header_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary header offset: " + util::uint32_t_to_hex_string(prim_primary_header_offset));

    prim_position = prim_primary_header_offset;

    prim_object_header primary_prim_object_header(prim_data, prim_position);

    prim_object_headers.push_back(primary_prim_object_header);

    if (primary_prim_object_header.prim_header_instance.type == (uint16_t) prim_header::header_type::OBJECT) {
        //LOG("  - PRIM primary header type: Object");
    } else if (primary_prim_object_header.prim_header_instance.type == (uint16_t) prim_header::header_type::MESH) {
        //LOG("Error: Not supported: PRIM primary header type: Mesh");
        success = false;
        return;
    } else {
        //LOG("Error: Not supported: PRIM primary header type: Unknown");
        success = false;
        return;
    }

    prim_position = primary_prim_object_header.object_offset;

    for (uint32_t o = 0; o < primary_prim_object_header.object_count; o++) {
        std::memcpy(&bytes4, &prim_data[prim_position], BYTES4);
        prim_position += 0x4;

        prim_object_offsets.push_back(bytes4);

        //LOG("  - PRIM object (" + std::to_string(o) + ") offset:" + util::uint32_t_to_hex_string(prim_object_offsets.back()));
    }

    for (uint32_t o = 0; o < prim_object_offsets.size(); o++) {
        //LOG("  - PRIM object (" + std::to_string(o) + ") offset:" + util::uint32_t_to_hex_string(prim_object_offsets.at(o)));

        prim_position = prim_object_offsets.at(o);

        //prim_object temp_prim_object(prim_data, prim_position, o);

        uint16_t header_type_value = 0;
        uint16_t object_sub_type_value = 0;

        prim_position += 0x2;

        std::memcpy(&header_type_value, &prim_data[prim_position], BYTES2);

        prim_position += 0x2;

        //LOG("      - PRIM object (" + std::to_string(o) + ") header type value: " + util::uint16_t_to_hex_string(header_type_value));

//        if (header_type_value == 1) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header type: Object");
//        } else if (header_type_value == 2) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header type: Mesh");
//        } else {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header type: Unknown");
//        }

        std::memcpy(&object_sub_type_value, &prim_data[prim_position], BYTES1);

        //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type value: " + util::uint8_t_to_hex_string(object_sub_type_value));

//        if (object_sub_type_value == 0x0) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Standard");
//        } else if (object_sub_type_value == 0x1) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Linked");
//        } else if (object_sub_type_value == 0x2) {
//            LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: Weighted");
//        } else if (object_sub_type_value == 0x3) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV2");
//        } else if (object_sub_type_value == 0x4) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV3");
//        } else if (object_sub_type_value == 0x5) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: StandardUUV4");
//        } else if (object_sub_type_value == 0x6) {
            //LOG("      - PRIM object (" + std::to_string(o) + ") header sub type: SpeedTree");
//        }

        prim_position = prim_object_offsets.at(o);

        if (header_type_value == (uint16_t) prim_header::header_type::MESH) {
            if (object_sub_type_value == (uint16_t) prim_object::SUBTYPE::STANDARD) {
                prim_mesh temp_prim_mesh(prim_data, prim_position, o);
                prim_meshes.push_back(temp_prim_mesh);

                prim_position = temp_prim_mesh.sub_mesh_table_offset;

                prim_sub_mesh temp_prim_sub_mesh(prim_data, prim_position, o, temp_prim_mesh, prim_meta_data,
                                                 primary_prim_object_header.flag_high_resolution, success);
                prim_sub_meshes.push_back(temp_prim_sub_mesh);

                asset3ds_data.names.push_back(prim_file_name + "_" + std::to_string(o));
                asset3ds_data.indexes.push_back(temp_prim_sub_mesh.indexes);
                asset3ds_data.vertexes.push_back(temp_prim_sub_mesh.vertexes);
                asset3ds_data.normals.push_back(temp_prim_sub_mesh.vertexes_normals);
                asset3ds_data.tangents.push_back(temp_prim_sub_mesh.vertexes_tangents);
                asset3ds_data.uvs.push_back(temp_prim_sub_mesh.vertexes_uvs);
                asset3ds_data.colors.push_back(temp_prim_sub_mesh.vertexes_colors);
                asset3ds_data.vertexes_weighted_bone_ids_0.push_back(temp_prim_sub_mesh.vertexes_weighted_bone_ids_0);
                asset3ds_data.vertexes_weighted_bone_ids_1.push_back(temp_prim_sub_mesh.vertexes_weighted_bone_ids_1);
                asset3ds_data.vertexes_weighted_weights_0.push_back(temp_prim_sub_mesh.vertexes_weighted_weights_0);
                asset3ds_data.vertexes_weighted_weights_1.push_back(temp_prim_sub_mesh.vertexes_weighted_weights_1);
                asset3ds_data.bones_nodes.push_back(temp_prim_sub_mesh.bones_nodes_data);
                asset3ds_data.bones_infos.push_back(temp_prim_sub_mesh.bones_info_data);
                asset3ds_data.bones_indices.push_back(temp_prim_sub_mesh.bones_indices_data);

                prim_weighted_mesh temp_prim_weighted_mesh;
                prim_weighted_meshes.push_back(temp_prim_weighted_mesh);

                prim_object_is_weighted.push_back(false);
            } else if (object_sub_type_value == (uint16_t) prim_object::SUBTYPE::WEIGHTED ||
                       object_sub_type_value == (uint16_t) prim_object::SUBTYPE::LINKED) {
                prim_weighted_mesh temp_prim_weighted_mesh(prim_data, prim_position, o);
                prim_weighted_meshes.push_back(temp_prim_weighted_mesh);

                prim_position = temp_prim_weighted_mesh.prim_mesh_instance.sub_mesh_table_offset;

                prim_weighted_sub_mesh temp_prim_weighted_sub_mesh(prim_data, prim_position, o, temp_prim_weighted_mesh,
                                                                   prim_meta_data,
                                                                   primary_prim_object_header.flag_high_resolution,
                                                                   success);
                prim_weighted_sub_meshes.push_back(temp_prim_weighted_sub_mesh);

                asset3ds_data.names.push_back(prim_file_name + "_" + std::to_string(o));
                asset3ds_data.indexes.push_back(temp_prim_weighted_sub_mesh.indexes);
                asset3ds_data.vertexes.push_back(temp_prim_weighted_sub_mesh.vertexes);
                asset3ds_data.normals.push_back(temp_prim_weighted_sub_mesh.vertexes_normals);
                asset3ds_data.tangents.push_back(temp_prim_weighted_sub_mesh.vertexes_tangents);
                asset3ds_data.uvs.push_back(temp_prim_weighted_sub_mesh.vertexes_uvs);
                asset3ds_data.colors.push_back(temp_prim_weighted_sub_mesh.vertexes_colors);
                asset3ds_data.vertexes_weighted_bone_ids_0.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_bone_ids_0);
                asset3ds_data.vertexes_weighted_bone_ids_1.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_bone_ids_1);
                asset3ds_data.vertexes_weighted_weights_0.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_weights_0);
                asset3ds_data.vertexes_weighted_weights_1.push_back(
                        temp_prim_weighted_sub_mesh.vertexes_weighted_weights_1);
                asset3ds_data.bones_nodes.push_back(temp_prim_weighted_sub_mesh.bones_nodes_data);
                asset3ds_data.bones_infos.push_back(temp_prim_weighted_sub_mesh.bones_info_data);
                asset3ds_data.bones_indices.push_back(temp_prim_weighted_sub_mesh.bones_indices_data);

                prim_mesh temp_prim_mesh;
                prim_meshes.push_back(temp_prim_mesh);

                if (object_sub_type_value == (uint16_t) prim_object::SUBTYPE::WEIGHTED) {
                    prim_object_is_weighted.push_back(true);
                } else {
                    prim_object_is_weighted.push_back(false);
                }
            } else {
                //LOG("Error: Not supported: PRIM object type: " + util::uint32_t_to_hex_string(object_sub_type_value));
                success = false;
                return;
            }
        }
    }

//    if (!borg_depends_file_name.empty()) {
        //borg temp_borg(borg_depends_rpkg_index.at(0).at(borg_depends_rpkg_index_index.at(0)), borg_depends_hash_index.at(0).at(borg_depends_hash_index_index.at(0)));

        //asset3ds_data.bones_data = temp_borg.bones_data;
        //asset3ds_data.bones_positions = temp_borg.bones_positions;
        //asset3ds_data.bones_inverse_matrices = temp_borg.bones_inverse_bind_matrices;
//    }

    asset3ds_data.weighted = prim_object_is_weighted;
}

void prim::load_hash_depends() {
    const uint32_t prim_hash_reference_count =
            rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    if (prim_hash_reference_count > 0) {
        for (uint64_t i = 0; i < prim_hash_reference_count; i++) {
            std::vector<std::string> prim_mati_depends_in_rpkgs;
            std::vector<uint32_t> prim_mati_depends_rpkg_index;
            std::vector<uint32_t> prim_mati_depends_hash_index;

            std::vector<std::string> prim_borg_depends_in_rpkgs;
            std::vector<uint32_t> prim_borg_depends_rpkg_index;
            std::vector<uint32_t> prim_borg_depends_hash_index;

            bool mati_found = false;
            bool borg_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++) {
                auto it = rpkgs.at(j).hash_map.find(
                        rpkgs.at(prim_rpkg_index).hash.at(prim_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end()) {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "MATI") {
                        if (!mati_found) {
                            mati_depends_file_name.push_back(
                                    util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                                    rpkgs.at(j).hash.at(it->second).hash_resource_type);

                            mati_depends_index.push_back(i);

                            //LOG("Found MATI hash depend:");
                            //LOG("  - File Name: " + mati_depends_file_name.back());
                        }

                        mati_found = true;

                        prim_mati_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        prim_mati_depends_rpkg_index.push_back(j);
                        prim_mati_depends_hash_index.push_back(it->second);

                        //LOG("  - In RPKG: " + prim_mati_depends_in_rpkgs.back());
                        //LOG("  - RPKG Index: " + util::uint64_t_to_string(prim_mati_depends_rpkg_index.back()));
                        //LOG("  - Hash Index: " + util::uint64_t_to_string(prim_mati_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "BORG") {
                        if (!borg_found) {
                            borg_depends_file_name.push_back(
                                    util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                                    rpkgs.at(j).hash.at(it->second).hash_resource_type);

                            borg_depends_index.push_back(i);

                            //LOG("Found BORG hash depend:");
                            //LOG("  - File Name: " + borg_depends_file_name.back());
                        }

                        borg_found = true;

                        prim_borg_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        prim_borg_depends_rpkg_index.push_back(j);
                        prim_borg_depends_hash_index.push_back(it->second);

                        //LOG("  - In RPKG: " + prim_borg_depends_in_rpkgs.back());
                        //LOG("  - RPKG Index: " + util::uint64_t_to_string(prim_borg_depends_rpkg_index.back()));
                        //LOG("  - Hash Index: " + util::uint64_t_to_string(prim_borg_depends_hash_index.back()));
                    }
                }
            }

            uint32_t mati_value = 0;

            if (mati_found) {
                mati_depends_in_rpkgs.push_back(prim_mati_depends_in_rpkgs);
                mati_depends_rpkg_index.push_back(prim_mati_depends_rpkg_index);
                mati_depends_hash_index.push_back(prim_mati_depends_hash_index);
                mati_depends_in_rpkgs_index.push_back(mati_value);
                mati_depends_rpkg_index_index.push_back(mati_value);
                mati_depends_hash_index_index.push_back(mati_value);
            }

            if (borg_found) {
                borg_depends_in_rpkgs.push_back(prim_borg_depends_in_rpkgs);
                borg_depends_rpkg_index.push_back(prim_borg_depends_rpkg_index);
                borg_depends_hash_index.push_back(prim_borg_depends_hash_index);
                borg_depends_in_rpkgs_index.push_back(mati_value);
                borg_depends_rpkg_index_index.push_back(mati_value);
                borg_depends_hash_index_index.push_back(mati_value);
            }
        }
    }

    for (uint64_t k = 0; k < mati_depends_file_name.size(); k++) {
        //LOG("  - MATI File Name: " + mati_depends_file_name.at(k));

        bool mati_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < mati_depends_in_rpkgs.at(k).size(); d++) {
            const std::string mati_depends_in_rpkgs_upper_case = util::to_upper_case(mati_depends_in_rpkgs.at(k).at(d));

            const std::string_view mati_depends_in_rpkgs_string_view(mati_depends_in_rpkgs_upper_case.c_str(),
                                                               mati_depends_in_rpkgs_upper_case.length());

            const size_t pos1 = mati_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos) {
                const size_t pos2 = mati_depends_in_rpkgs_string_view.substr(pos1).find('.');

                if (pos2 != std::string::npos) {
                    mati_patch_name_found = true;

                    const long new_patch_level = std::strtol(
                            std::string(mati_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level) {
                        patch_level = new_patch_level;

                        mati_depends_in_rpkgs_index.at(k) = d;
                        mati_depends_rpkg_index_index.at(k) = d;
                        mati_depends_hash_index_index.at(k) = d;
                    }
                }
            } else {
                if (!mati_patch_name_found) {
                    mati_depends_in_rpkgs_index.at(k) = d;
                    mati_depends_rpkg_index_index.at(k) = d;
                    mati_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - MATI File Name In RPKG: " + mati_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - MATI File Name In RPKG Used: " + mati_depends_in_rpkgs.at(k).at(mati_depends_rpkg_index_index.at(k)));
    }

    for (uint64_t k = 0; k < borg_depends_file_name.size(); k++) {
        //LOG("  - BORG File Name: " + borg_depends_file_name.at(k));

        bool borg_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < borg_depends_in_rpkgs.at(k).size(); d++) {
            const std::string borg_depends_in_rpkgs_upper_case = util::to_upper_case(borg_depends_in_rpkgs.at(k).at(d));

            const std::string_view borg_depends_in_rpkgs_string_view(borg_depends_in_rpkgs_upper_case.c_str(),
                                                               borg_depends_in_rpkgs_upper_case.length());

            const size_t pos1 = borg_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos) {
                size_t pos2 = borg_depends_in_rpkgs_string_view.substr(pos1).find('.');

                if (pos2 != std::string::npos) {
                    borg_patch_name_found = true;

                    const long new_patch_level = std::strtol(
                            std::string(borg_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level) {
                        patch_level = new_patch_level;

                        borg_depends_in_rpkgs_index.at(k) = d;
                        borg_depends_rpkg_index_index.at(k) = d;
                        borg_depends_hash_index_index.at(k) = d;
                    }
                }
            } else {
                if (!borg_patch_name_found) {
                    borg_depends_in_rpkgs_index.at(k) = d;
                    borg_depends_rpkg_index_index.at(k) = d;
                    borg_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - BORG File Name In RPKG: " + borg_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - BORG File Name In RPKG Used: " + borg_depends_in_rpkgs.at(k).at(borg_depends_rpkg_index_index.at(k)));
    }
}

prim::prim_header::prim_header() = default;

prim::prim_header::prim_header(std::vector<char>& prim_data, uint32_t& prim_position) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    offset = prim_position;

    std::memcpy(&draw, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    //LOG("  - PRIM header draw value: " + util::uint8_t_to_hex_string(draw));

    std::memcpy(&pack, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    //LOG("  - PRIM header pack value: " + util::uint8_t_to_hex_string(draw));

    std::memcpy(&type, &prim_data[prim_position], BYTES2);
    prim_position += 0x2;

    //LOG("  - PRIM header type value: " + util::uint16_t_to_hex_string(type));
}

prim::prim_object_header::prim_object_header() = default;

prim::prim_object_header::prim_object_header(std::vector<char>& prim_data, uint32_t& prim_position) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    prim_header_instance = prim_header(prim_data, prim_position);

    std::memcpy(&property_flags, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary header property flags: " + util::uint32_t_to_hex_string(property_flags));

    if ((property_flags & 0x1) == 0x1) {
        flag_bones = true;

        //LOG("  - PRIM primary header flag set: Bones");
    }
    if ((property_flags & 0x2) == 0x2) {
        flag_frames = true;

        //LOG("  - PRIM primary header flag set: Frames");
    }
    if ((property_flags & 0x4) == 0x4) {
        flag_linked = true;

        //LOG("  - PRIM primary header flag set: Linked");
    }
    if ((property_flags & 0x8) == 0x8) {
        flag_weighted = true;

        //LOG("  - PRIM primary header flag set: Weighted");
    }
    if ((property_flags & 0x100) == 0x100) {
        flag_bounds = true;

        //LOG("  - PRIM primary header flag set: Bounds");
    }
    if ((property_flags & 0x200) == 0x200) {
        flag_high_resolution = true;

        //LOG("  - PRIM primary header flag set: High Resolution");
    }

    std::memcpy(&borg_index, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary BORG index: " + util::uint32_t_to_hex_string(borg_index));

    std::memcpy(&object_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary object count: " + util::uint32_t_to_hex_string(object_count));

    std::memcpy(&object_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary object offset: " + util::uint32_t_to_hex_string(object_offset));

    std::memcpy(&bounding_box_min.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_min.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_min.z, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    std::memcpy(&bounding_box_max.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_max.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_max.z, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("  - PRIM primary bounding box min:");
    //LOG("      - x: " + util::float_to_string(bounding_box_min.x));
    //LOG("      - y: " + util::float_to_string(bounding_box_min.y));
    //LOG("      - z: " + util::float_to_string(bounding_box_min.z));

    //LOG("  - PRIM primary bounding box max:");
    //LOG("      - x: " + util::float_to_string(bounding_box_max.x));
    //LOG("      - y: " + util::float_to_string(bounding_box_max.y));
    //LOG("      - z: " + util::float_to_string(bounding_box_max.z));
}

prim::prim_object::prim_object() = default;

prim::prim_object::prim_object(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    prim_header_instance = prim_header(prim_data, prim_position);

    object = prim_object_number;

    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    sub_type = bytes1;

    //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type value: " + util::uint8_t_to_hex_string(sub_type));

//    if (sub_type == 0x0) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: Standard");
//    } else if (sub_type == 0x1) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: Linked");
//    } else if (sub_type == 0x2) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: Weighted");
//    } else if (sub_type == 0x3) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: StandardUUV2");
//    } else if (sub_type == 0x4) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: StandardUUV3");
//    } else if (sub_type == 0x5) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: StandardUUV4");
//    } else if (sub_type == 0x6) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header sub type: SpeedTree");
//    }

    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    property_flags = bytes1;

    //LOG("      - PRIM object (" + std::to_string(object) + ") header property flags: " + util::uint8_t_to_hex_string(property_flags));

//    if (property_flags == 0x0) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: None");
//    }
//    if ((property_flags & 0x1) == 0x1) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: XAxisLocked");
//    }
//    if ((property_flags & 0x2) == 0x2) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: YAxisLocked");
//    }
//    if ((property_flags & 0x4) == 0x4) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: ZAxisLocked");
//    }
//    if ((property_flags & 0x8) == 0x8) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: 32BitVertexData");
//    }
//    if ((property_flags & 0x10) == 0x10) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: PS3");
//    }
//    if ((property_flags & 0x20) == 0x20) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: Color1");
//    }
//    if ((property_flags & 0x40) == 0x40) {
        //LOG("      - PRIM object (" + std::to_string(object) + ") header flag set: NoPhysics");
//    }

    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    lod = bytes1;

    //LOG("      - PRIM object (" + std::to_string(object) + ") LOD: " + util::uint8_t_to_hex_string(lod));

    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    variant = bytes1;

    //LOG("      - PRIM object (" + std::to_string(object) + ") variant: " + util::uint8_t_to_hex_string(variant));

    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    bias = bytes1;

    //LOG("      - PRIM object (" + std::to_string(object) + ") bias: " + util::uint8_t_to_hex_string(bias));

    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
    prim_position += 0x1;

    offset = bytes1;

    //LOG("      - PRIM object (" + std::to_string(object) + ") offset: " + util::uint8_t_to_hex_string(offset));

    std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);
    prim_position += 0x2;

    material_id = bytes2;

    //LOG("      - PRIM object (" + std::to_string(object) + ") material id: " + util::uint16_t_to_hex_string(material_id));

    std::memcpy(&bytes4, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    wire_color = bytes4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") object wire color: " + util::uint32_t_to_hex_string(wire_color));

    std::memcpy(&bytes4, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    color = bytes4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") object color: " + util::uint32_t_to_hex_string(color));

    std::memcpy(&bounding_box_min.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_min.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_min.z, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    std::memcpy(&bounding_box_max.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_max.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&bounding_box_max.z, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") bounding box min:");
    //LOG("          - x: " + util::float_to_string(bounding_box_min.x));
    //LOG("          - y: " + util::float_to_string(bounding_box_min.y));
    //LOG("          - z: " + util::float_to_string(bounding_box_min.z));

    //LOG("      - PRIM object (" + std::to_string(object) + ") bounding box max:");
    //LOG("          - x: " + util::float_to_string(bounding_box_max.x));
    //LOG("          - y: " + util::float_to_string(bounding_box_max.y));
    //LOG("          - z: " + util::float_to_string(bounding_box_max.z));
}

prim::prim_mesh::prim_mesh() = default;

prim::prim_mesh::prim_mesh(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    prim_object_instance = prim_object(prim_data, prim_position, prim_object_number);

    uint32_t object = prim_object_number;

    std::memcpy(&sub_mesh_table_offset_pointer, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh table offset pointer:" + util::uint32_t_to_hex_string(sub_mesh_table_offset_pointer));

    std::memcpy(&vertex_position_scale.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&vertex_position_scale.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&vertex_position_scale.z, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&vertex_position_scale.w, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") mesh vertex position scale:");
    //LOG("          - x: " + util::float_to_string(vertex_position_scale.x));
    //LOG("          - y: " + util::float_to_string(vertex_position_scale.y));
    //LOG("          - z: " + util::float_to_string(vertex_position_scale.z));
    //LOG("          - w: " + util::float_to_string(vertex_position_scale.w));

    std::memcpy(&vertex_position_bias.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&vertex_position_bias.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&vertex_position_bias.z, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&vertex_position_bias.w, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") mesh vertex position bias:");
    //LOG("          - x: " + util::float_to_string(vertex_position_bias.x));
    //LOG("          - y: " + util::float_to_string(vertex_position_bias.y));
    //LOG("          - z: " + util::float_to_string(vertex_position_bias.z));
    //LOG("          - w: " + util::float_to_string(vertex_position_bias.w));

    std::memcpy(&uv_position_scale.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&uv_position_scale.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") mesh uv position bias:");
    //LOG("          - x: " + util::float_to_string(uv_position_scale.x));
    //LOG("          - y: " + util::float_to_string(uv_position_scale.y));

    std::memcpy(&uv_position_bias.x, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;
    std::memcpy(&uv_position_bias.y, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") mesh uv position bias:");
    //LOG("          - x: " + util::float_to_string(uv_position_bias.x));
    //LOG("          - y: " + util::float_to_string(uv_position_bias.y));

    std::memcpy(&cloth_flags, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") mesh cloth flags:" + util::uint32_t_to_hex_string(cloth_flags));

    uint32_t prim_position_backup = prim_position;

    prim_position = sub_mesh_table_offset_pointer;

    std::memcpy(&sub_mesh_table_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh table offset:" + util::uint32_t_to_hex_string(sub_mesh_table_offset));

    prim_position = prim_position_backup;
}

prim::prim_weighted_mesh::prim_weighted_mesh() = default;

prim::prim_weighted_mesh::prim_weighted_mesh(std::vector<char>& prim_data, uint32_t& prim_position,
                                             uint32_t prim_object_number) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    prim_mesh_instance = prim_mesh(prim_data, prim_position, prim_object_number);

    uint32_t object = prim_object_number;

    std::memcpy(&bones_nodes_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") bones nodes count:" + util::uint32_t_to_hex_string(bones_nodes_count));

    std::memcpy(&bones_nodes_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") bones nodes offset:" + util::uint32_t_to_hex_string(bones_nodes_offset));

    std::memcpy(&bone_indices_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") bones indices offset:" + util::uint32_t_to_hex_string(bone_indices_offset));

    std::memcpy(&bone_info_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("      - PRIM object (" + std::to_string(object) + ") bones info offset:" + util::uint32_t_to_hex_string(bone_info_offset));

}

prim::prim_sub_mesh::prim_sub_mesh() = default;

prim::prim_sub_mesh::prim_sub_mesh(std::vector<char>& prim_data, uint32_t& prim_position, uint32_t prim_object_number,
                                   prim_mesh& temp_prim_mesh, std::vector<char>& prim_meta_data,
                                   bool flag_high_resolution, bool& success) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    uint8_t bytes1_zero = 0;
    uint16_t bytes2_zero = 0;
    uint32_t bytes4_zero = 0;
    uint64_t bytes8_zero = 0;

    prim_object_instance = prim_object(prim_data, prim_position, prim_object_number);

    uint32_t object = prim_object_number;

    std::memcpy(&vertex_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex count: " + util::uint32_t_to_hex_string(vertex_count));

    std::memcpy(&vertex_buffer_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex buffer offset: " + util::uint32_t_to_hex_string(vertex_buffer_offset));

    std::memcpy(&index_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh index count: " + util::uint32_t_to_hex_string(index_count));

    std::memcpy(&index_count_additional, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh index count additional: " + util::uint32_t_to_hex_string(index_count_additional));

    std::memcpy(&index_buffer_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh index buffer offset: " + util::uint32_t_to_hex_string(index_buffer_offset));

    std::memcpy(&collision_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh collision offset: " + util::uint32_t_to_hex_string(collision_offset));

    std::memcpy(&cloth_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh cloth offset: " + util::uint32_t_to_hex_string(cloth_offset));

    std::memcpy(&uv_channel_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh uv channel count: " + util::uint32_t_to_hex_string(uv_channel_count));

    std::memcpy(&unknown_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh unknown offset: " + util::uint32_t_to_hex_string(unknown_offset));

    if (uv_channel_count == 1) {
        prim_position = index_buffer_offset;

        uint32_t index_count_total = index_count + index_count_additional;

        //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh indexes at (" + util::uint32_t_to_hex_string(index_buffer_offset) + "): ");

        for (uint32_t x = 0; x < index_count_total; x++) {
            std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);
            std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
            prim_position += 0x2;

            indexes.push_back(bytes2);

            //LOG("              - PRIM object (" + std::to_string(object) + ") sub mesh index: " + util::uint16_t_to_hex_string(indexes.back()));
        }

        prim_position = vertex_buffer_offset;

        if (flag_high_resolution) {
            for (uint32_t v = 0; v < vertex_count; v++) {
                vector4 temp_vector4;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex (32bit float): ");

                std::memcpy(&temp_vector4.x, &prim_data[prim_position], BYTES4);
                std::memcpy(&prim_meta_data[prim_position], &bytes4_zero, BYTES4);
                prim_position += 0x4;

                //LOG("              - x: " + util::float_to_string(temp_vector4.x));

                std::memcpy(&temp_vector4.y, &prim_data[prim_position], BYTES4);
                std::memcpy(&prim_meta_data[prim_position], &bytes4_zero, BYTES4);
                prim_position += 0x4;

                //LOG("              - y: " + util::float_to_string(temp_vector4.y));

                std::memcpy(&temp_vector4.z, &prim_data[prim_position], BYTES4);
                std::memcpy(&prim_meta_data[prim_position], &bytes4_zero, BYTES4);
                prim_position += 0x4;

                //LOG("              - z: " + util::float_to_string(temp_vector4.z));

                vertexes.push_back(temp_vector4.x);
                vertexes.push_back(temp_vector4.y);
                vertexes.push_back(temp_vector4.z);
            }
        } else {
            for (uint32_t v = 0; v < vertex_count; v++) {
                short temp_short = 0;

                vector4 temp_vector4;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex compressed (short): ");

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - x: " + util::short_to_string(temp_short));

                temp_vector4.x = (static_cast<float>(temp_short) * temp_prim_mesh.vertex_position_scale.x) /
                                 std::numeric_limits<short>::max() + temp_prim_mesh.vertex_position_bias.x;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - y: " + util::short_to_string(temp_short));

                temp_vector4.y = (static_cast<float>(temp_short) * temp_prim_mesh.vertex_position_scale.y) /
                                 std::numeric_limits<short>::max() + temp_prim_mesh.vertex_position_bias.y;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - z: " + util::short_to_string(temp_short));

                temp_vector4.z = (static_cast<float>(temp_short) * temp_prim_mesh.vertex_position_scale.z) /
                                 std::numeric_limits<short>::max() + temp_prim_mesh.vertex_position_bias.z;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - w: " + util::short_to_string(temp_short));

                temp_vector4.w = (static_cast<float>(temp_short) * temp_prim_mesh.vertex_position_scale.w) /
                                 std::numeric_limits<short>::max() + temp_prim_mesh.vertex_position_bias.w;

                vertexes.push_back(temp_vector4.x);
                vertexes.push_back(temp_vector4.y);
                vertexes.push_back(temp_vector4.z);

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex decompressed (float):");
                //LOG("              - x: " + util::float_to_string(temp_vector4.x));
                //LOG("              - y: " + util::float_to_string(temp_vector4.y));
                //LOG("              - z: " + util::float_to_string(temp_vector4.z));
                //LOG("              - w: " + util::float_to_string(temp_vector4.w));

                //std::cout << "v " << util::float_to_string(temp_vector4.x) << " " << util::float_to_string(temp_vector4.y) << " " << util::float_to_string(temp_vector4.z) << " " << util::float_to_string(temp_vector4.w) << std::endl;
            }
        }

//        if (1)//if (prim_object_sub_type.at(o) != 0x2)
//        {
        for (uint32_t v = 0; v < vertex_count; v++) {
            vector4 temp_vector4;

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex normal compressed (uint8_t):");

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

            vertexes_normals.push_back(temp_vector4.x);
            vertexes_normals.push_back(temp_vector4.y);
            vertexes_normals.push_back(temp_vector4.z);

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex normal decompressed (float):");
            //LOG("              - x: " + util::float_to_string(temp_vector4.x));
            //LOG("              - y: " + util::float_to_string(temp_vector4.y));
            //LOG("              - z: " + util::float_to_string(temp_vector4.z));
            //LOG("              - w: " + util::float_to_string(temp_vector4.w));

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex tangents compressed (uint8_t):");

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

            vertexes_tangents.push_back(temp_vector4.x);
            vertexes_tangents.push_back(temp_vector4.y);
            vertexes_tangents.push_back(temp_vector4.z);
            vertexes_tangents.push_back(temp_vector4.w);

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex tangents decompressed (float):");
            //LOG("              - x: " + util::float_to_string(temp_vector4.x));
            //LOG("              - y: " + util::float_to_string(temp_vector4.y));
            //LOG("              - z: " + util::float_to_string(temp_vector4.z));
            //LOG("              - w: " + util::float_to_string(temp_vector4.w));

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex bitangents compressed (uint8_t):");

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

            std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
            std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
            prim_position += 0x1;

            temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

            //LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

            vertexes_bitangents.push_back(temp_vector4.x);
            vertexes_bitangents.push_back(temp_vector4.y);
            vertexes_bitangents.push_back(temp_vector4.z);

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex bitangents decompressed (float):");
            //LOG("              - x: " + util::float_to_string(temp_vector4.x));
            //LOG("              - y: " + util::float_to_string(temp_vector4.y));
            //LOG("              - z: " + util::float_to_string(temp_vector4.z));
            //LOG("              - w: " + util::float_to_string(temp_vector4.w));

            short temp_short = 0;

            vector2 temp_vector2;

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex uv compressed (short): ");

            std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
            std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
            prim_position += 0x2;

            //LOG("              - x: " + util::short_to_string(temp_short));

            temp_vector2.x = (static_cast<float>(temp_short) * temp_prim_mesh.uv_position_scale.x) /
                             std::numeric_limits<short>::max() + temp_prim_mesh.uv_position_bias.x;

            std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
            std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
            prim_position += 0x2;

            //LOG("              - y: " + util::short_to_string(temp_short));

            temp_vector2.y = (static_cast<float>(temp_short) * temp_prim_mesh.uv_position_scale.y) /
                             std::numeric_limits<short>::max() + temp_prim_mesh.uv_position_bias.y;

            vertexes_uvs.push_back(temp_vector2.x);
            vertexes_uvs.push_back(temp_vector2.y);

            //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex uv decompressed (float):");
            //LOG("              - x: " + util::float_to_string(temp_vector2.x));
            //LOG("              - y: " + util::float_to_string(temp_vector2.y));
        }

        if (temp_prim_mesh.prim_object_instance.sub_type == (uint32_t) prim_object::SUBTYPE::WEIGHTED ||
            ((prim_object_instance.property_flags & (uint32_t) prim_object::PROPERTY_FLAGS::COLOR1) == 0x0)) {
            for (uint32_t v = 0; v < vertex_count; v++) {
                rgba temp_rgba;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color r: " + util::uint8_t_to_hex_string(bytes1));

                temp_rgba.r = bytes1;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color g: " + util::uint8_t_to_hex_string(bytes1));

                temp_rgba.g = bytes1;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color b: " + util::uint8_t_to_hex_string(bytes1));

                temp_rgba.b = bytes1;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color a: " + util::uint8_t_to_hex_string(bytes1));

                temp_rgba.a = bytes1;

                vertexes_colors.push_back(temp_rgba.r);
                vertexes_colors.push_back(temp_rgba.g);
                vertexes_colors.push_back(temp_rgba.b);
                vertexes_colors.push_back(temp_rgba.a);
            }
        }

        if (cloth_offset) {
            prim_position = cloth_offset;

            if ((temp_prim_mesh.cloth_flags & (uint32_t) prim_mesh::CLOTH_FLAGS::SMALL) ==
                (uint32_t) prim_mesh::CLOTH_FLAGS::SMALL) {
                std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                uint32_t cloth_data_size = bytes2;

                cloth_data.resize(cloth_data_size);

                std::memcpy(cloth_data.data(), &prim_data[prim_position], cloth_data_size);
            } else {
                uint32_t cloth_data_size = vertex_count * 0x14;

                cloth_data.resize(cloth_data_size);

                std::memcpy(cloth_data.data(), &prim_data[prim_position], cloth_data_size);
            }
        }

        if (collision_offset) {
            prim_position = collision_offset;

            if (temp_prim_mesh.prim_object_instance.sub_type == (uint32_t) prim_object::SUBTYPE::STANDARD ||
                temp_prim_mesh.prim_object_instance.sub_type == (uint32_t) prim_object::SUBTYPE::WEIGHTED) {
                std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                uint32_t collision_data_size = bytes2;

                collision_data_size *= 6;
                collision_data_size += 4;

                collision_data.resize(collision_data_size);

                std::memcpy(collision_data.data(), &prim_data[prim_position], collision_data_size);
            } else {
                std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                uint32_t collision_data_size = bytes2;

                collision_data.resize(collision_data_size);

                std::memcpy(collision_data.data(), &prim_data[prim_position], collision_data_size);
            }
        }
//        } else {
//            //LOG("Error: PRIM_OBJECT_IS_NOT_A_MESH_TYPE");
//
//            task_multiple_status = PRIM_OBJECT_IS_NOT_A_MESH_TYPE;
//
//            success = false;
//
//            //return;
//        }
    } else {
        //LOG("Error: PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1");

        task_multiple_status = PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1;

        success = false;

        //return;
    }
}

prim::prim_weighted_sub_mesh::prim_weighted_sub_mesh() = default;

prim::prim_weighted_sub_mesh::prim_weighted_sub_mesh(std::vector<char>& prim_data, uint32_t& prim_position,
                                                     uint32_t prim_object_number,
                                                     prim_weighted_mesh& temp_weighted_prim_mesh,
                                                     std::vector<char>& prim_meta_data, bool flag_high_resolution,
                                                     bool& success) {
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    uint8_t bytes1_zero = 0;
    uint16_t bytes2_zero = 0;
    uint32_t bytes4_zero = 0;
    uint64_t bytes8_zero = 0;

    prim_object_instance = prim_object(prim_data, prim_position, prim_object_number);

    uint32_t object = prim_object_number;

    std::memcpy(&vertex_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex count: " + util::uint32_t_to_hex_string(vertex_count));

    std::memcpy(&vertex_buffer_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex buffer offset: " + util::uint32_t_to_hex_string(vertex_buffer_offset));

    std::memcpy(&index_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh index count: " + util::uint32_t_to_hex_string(index_count));

    std::memcpy(&index_count_additional, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh index count additional: " + util::uint32_t_to_hex_string(index_count_additional));

    std::memcpy(&index_buffer_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh index buffer offset: " + util::uint32_t_to_hex_string(index_buffer_offset));

    std::memcpy(&collision_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh collision offset: " + util::uint32_t_to_hex_string(collision_offset));

    std::memcpy(&cloth_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh cloth offset: " + util::uint32_t_to_hex_string(cloth_offset));

    std::memcpy(&uv_channel_count, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh uv channel count: " + util::uint32_t_to_hex_string(uv_channel_count));

    std::memcpy(&unknown_offset, &prim_data[prim_position], BYTES4);
    prim_position += 0x4;

    //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh unknown offset: " + util::uint32_t_to_hex_string(unknown_offset));

    if (uv_channel_count == 1) {
        prim_position = index_buffer_offset;

        uint32_t index_count_total = index_count + index_count_additional;

        //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh indexes at (" + util::uint32_t_to_hex_string(index_buffer_offset) + "): ");

        for (uint32_t x = 0; x < index_count_total; x++) {
            std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);
            std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
            prim_position += 0x2;

            indexes.push_back(bytes2);

            //LOG("              - PRIM object (" + std::to_string(object) + ") sub mesh index: " + util::uint16_t_to_hex_string(indexes.back()));
        }

        prim_position = vertex_buffer_offset;

        if (flag_high_resolution) {
            for (uint32_t v = 0; v < vertex_count; v++) {
                vector4 temp_vector4;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex (32bit float): ");

                std::memcpy(&temp_vector4.x, &prim_data[prim_position], BYTES4);
                std::memcpy(&prim_meta_data[prim_position], &bytes4_zero, BYTES4);
                prim_position += 0x4;

                //LOG("              - x: " + util::float_to_string(temp_vector4.x));

                std::memcpy(&temp_vector4.y, &prim_data[prim_position], BYTES4);
                std::memcpy(&prim_meta_data[prim_position], &bytes4_zero, BYTES4);
                prim_position += 0x4;

                //LOG("              - y: " + util::float_to_string(temp_vector4.y));

                std::memcpy(&temp_vector4.z, &prim_data[prim_position], BYTES4);
                std::memcpy(&prim_meta_data[prim_position], &bytes4_zero, BYTES4);
                prim_position += 0x4;

                //LOG("              - z: " + util::float_to_string(temp_vector4.z));

                vertexes.push_back(temp_vector4.x);
                vertexes.push_back(temp_vector4.y);
                vertexes.push_back(temp_vector4.z);
            }
        } else {
            for (uint32_t v = 0; v < vertex_count; v++) {
                short temp_short = 0;

                vector4 temp_vector4;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex compressed (short): ");

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - x: " + util::short_to_string(temp_short));

                temp_vector4.x = (static_cast<float>(temp_short) *
                                  temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_scale.x) /
                                 std::numeric_limits<short>::max() +
                                 temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_bias.x;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - y: " + util::short_to_string(temp_short));

                temp_vector4.y = (static_cast<float>(temp_short) *
                                  temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_scale.y) /
                                 std::numeric_limits<short>::max() +
                                 temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_bias.y;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - z: " + util::short_to_string(temp_short));

                temp_vector4.z = (static_cast<float>(temp_short) *
                                  temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_scale.z) /
                                 std::numeric_limits<short>::max() +
                                 temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_bias.z;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - w: " + util::short_to_string(temp_short));

                temp_vector4.w = (static_cast<float>(temp_short) *
                                  temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_scale.w) /
                                 std::numeric_limits<short>::max() +
                                 temp_weighted_prim_mesh.prim_mesh_instance.vertex_position_bias.w;

                vertexes.push_back(temp_vector4.x);
                vertexes.push_back(temp_vector4.y);
                vertexes.push_back(temp_vector4.z);

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex decompressed (float):");
                //LOG("              - x: " + util::float_to_string(temp_vector4.x));
                //LOG("              - y: " + util::float_to_string(temp_vector4.y));
                //LOG("              - z: " + util::float_to_string(temp_vector4.z));
                //LOG("              - w: " + util::float_to_string(temp_vector4.w));

                //std::cout << "v " << util::float_to_string(temp_vector4.x) << " " << util::float_to_string(temp_vector4.y) << " " << util::float_to_string(temp_vector4.z) << " " << util::float_to_string(temp_vector4.w) << std::endl;
            }
        }

        if (temp_weighted_prim_mesh.prim_mesh_instance.prim_object_instance.sub_type ==
            (uint16_t) prim_object::SUBTYPE::WEIGHTED) {
            for (uint32_t v = 0; v < vertex_count; v++) {
                vector6 temp_vector6;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector6.a = static_cast<float>(bytes1) / 255.0f;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector6.b = static_cast<float>(bytes1) / 255.0f;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector6.c = static_cast<float>(bytes1) / 255.0f;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector6.d = static_cast<float>(bytes1) / 255.0f;

                uint8_t_6 temp_uint8_t_6;

                std::memcpy(&temp_uint8_t_6.a, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                std::memcpy(&temp_uint8_t_6.b, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                std::memcpy(&temp_uint8_t_6.c, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                std::memcpy(&temp_uint8_t_6.d, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector6.e = static_cast<float>(bytes1) / 255.0f;

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector6.f = static_cast<float>(bytes1) / 255.0f;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh weighted vertex weights: ");
                //LOG("              - a: " + util::float_to_string(temp_vector6.a));
                //LOG("              - b: " + util::float_to_string(temp_vector6.b));
                //LOG("              - c: " + util::float_to_string(temp_vector6.c));
                //LOG("              - d: " + util::float_to_string(temp_vector6.d));
                //LOG("              - e: " + util::float_to_string(temp_vector6.e));
                //LOG("              - f: " + util::float_to_string(temp_vector6.f));

                vertexes_weighted_weights_0.push_back(temp_vector6.a);
                vertexes_weighted_weights_0.push_back(temp_vector6.b);
                vertexes_weighted_weights_0.push_back(temp_vector6.c);
                vertexes_weighted_weights_0.push_back(temp_vector6.d);
                vertexes_weighted_weights_1.push_back(temp_vector6.e);
                vertexes_weighted_weights_1.push_back(temp_vector6.f);
                vertexes_weighted_weights_1.push_back(0x0);
                vertexes_weighted_weights_1.push_back(0x0);

                std::memcpy(&temp_uint8_t_6.e, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                std::memcpy(&temp_uint8_t_6.f, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh weighted vertex bone_ids: ");
                //LOG("              - a: " + util::uint8_t_to_string(temp_uint8_t_6.a));
                //LOG("              - b: " + util::uint8_t_to_string(temp_uint8_t_6.b));
                //LOG("              - c: " + util::uint8_t_to_string(temp_uint8_t_6.c));
                //LOG("              - d: " + util::uint8_t_to_string(temp_uint8_t_6.d));
                //LOG("              - e: " + util::uint8_t_to_string(temp_uint8_t_6.e));
                //LOG("              - f: " + util::uint8_t_to_string(temp_uint8_t_6.f));

                vertexes_weighted_bone_ids_0.push_back(temp_uint8_t_6.a);
                vertexes_weighted_bone_ids_0.push_back(temp_uint8_t_6.b);
                vertexes_weighted_bone_ids_0.push_back(temp_uint8_t_6.c);
                vertexes_weighted_bone_ids_0.push_back(temp_uint8_t_6.d);
                vertexes_weighted_bone_ids_1.push_back(temp_uint8_t_6.e);
                vertexes_weighted_bone_ids_1.push_back(temp_uint8_t_6.f);
                vertexes_weighted_bone_ids_1.push_back(0x0);
                vertexes_weighted_bone_ids_1.push_back(0x0);
            }
        }

        if (1)//if (prim_object_sub_type.at(o) != 0x2)
        {
            for (uint32_t v = 0; v < vertex_count; v++) {
                vector4 temp_vector4;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex normal compressed (uint8_t):");

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

                vertexes_normals.push_back(temp_vector4.x);
                vertexes_normals.push_back(temp_vector4.y);
                vertexes_normals.push_back(temp_vector4.z);

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex normal decompressed (float):");
                //LOG("              - x: " + util::float_to_string(temp_vector4.x));
                //LOG("              - y: " + util::float_to_string(temp_vector4.y));
                //LOG("              - z: " + util::float_to_string(temp_vector4.z));
                //LOG("              - w: " + util::float_to_string(temp_vector4.w));

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex tangents compressed (uint8_t):");

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

                vertexes_tangents.push_back(temp_vector4.x);
                vertexes_tangents.push_back(temp_vector4.y);
                vertexes_tangents.push_back(temp_vector4.z);
                vertexes_tangents.push_back(temp_vector4.w);

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex tangents decompressed (float):");
                //LOG("              - x: " + util::float_to_string(temp_vector4.x));
                //LOG("              - y: " + util::float_to_string(temp_vector4.y));
                //LOG("              - z: " + util::float_to_string(temp_vector4.z));
                //LOG("              - w: " + util::float_to_string(temp_vector4.w));

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex bitangents compressed (uint8_t):");

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.x = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - x: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.y = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - y: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.z = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - z: " + util::uint8_t_to_hex_string(bytes1));

                std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                prim_position += 0x1;

                temp_vector4.w = ((2.0f * static_cast<float>(bytes1)) / 255.0f) - 1.0f;

                //LOG("              - w: " + util::uint8_t_to_hex_string(bytes1));

                vertexes_bitangents.push_back(temp_vector4.x);
                vertexes_bitangents.push_back(temp_vector4.y);
                vertexes_bitangents.push_back(temp_vector4.z);

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex bitangents decompressed (float):");
                //LOG("              - x: " + util::float_to_string(temp_vector4.x));
                //LOG("              - y: " + util::float_to_string(temp_vector4.y));
                //LOG("              - z: " + util::float_to_string(temp_vector4.z));
                //LOG("              - w: " + util::float_to_string(temp_vector4.w));

                short temp_short = 0;

                vector2 temp_vector2;

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex uv compressed (short): ");

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - x: " + util::short_to_string(temp_short));

                temp_vector2.x = (static_cast<float>(temp_short) *
                                  temp_weighted_prim_mesh.prim_mesh_instance.uv_position_scale.x) /
                                 std::numeric_limits<short>::max() +
                                 temp_weighted_prim_mesh.prim_mesh_instance.uv_position_bias.x;

                std::memcpy(&temp_short, &prim_data[prim_position], BYTES2);
                std::memcpy(&prim_meta_data[prim_position], &bytes2_zero, BYTES2);
                prim_position += 0x2;

                //LOG("              - y: " + util::short_to_string(temp_short));

                temp_vector2.y = (static_cast<float>(temp_short) *
                                  temp_weighted_prim_mesh.prim_mesh_instance.uv_position_scale.y) /
                                 std::numeric_limits<short>::max() +
                                 temp_weighted_prim_mesh.prim_mesh_instance.uv_position_bias.y;

                vertexes_uvs.push_back(temp_vector2.x);
                vertexes_uvs.push_back(temp_vector2.y);

                //LOG("          - PRIM object (" + std::to_string(object) + ") sub mesh vertex uv decompressed (float):");
                //LOG("              - x: " + util::float_to_string(temp_vector2.x));
                //LOG("              - y: " + util::float_to_string(temp_vector2.y));
            }

            if (temp_weighted_prim_mesh.prim_mesh_instance.prim_object_instance.sub_type ==
                (uint32_t) prim_object::SUBTYPE::WEIGHTED ||
                ((prim_object_instance.property_flags & (uint32_t) prim_object::PROPERTY_FLAGS::COLOR1) == 0x0)) {
                for (uint32_t v = 0; v < vertex_count; v++) {
                    rgba temp_rgba;

                    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                    std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                    prim_position += 0x1;

                    //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color r: " + util::uint8_t_to_hex_string(bytes1));

                    temp_rgba.r = bytes1;

                    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                    std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                    prim_position += 0x1;

                    //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color g: " + util::uint8_t_to_hex_string(bytes1));

                    temp_rgba.g = bytes1;

                    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                    std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                    prim_position += 0x1;

                    //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color b: " + util::uint8_t_to_hex_string(bytes1));

                    temp_rgba.b = bytes1;

                    std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                    std::memcpy(&prim_meta_data[prim_position], &bytes1_zero, BYTES1);
                    prim_position += 0x1;

                    //LOG("      - PRIM object (" + std::to_string(object) + ") sub mesh vertex color a: " + util::uint8_t_to_hex_string(bytes1));

                    temp_rgba.a = bytes1;

                    vertexes_colors.push_back(temp_rgba.r);
                    vertexes_colors.push_back(temp_rgba.g);
                    vertexes_colors.push_back(temp_rgba.b);
                    vertexes_colors.push_back(temp_rgba.a);
                }
            }

            if (cloth_offset) {
                prim_position = cloth_offset;

                if ((temp_weighted_prim_mesh.prim_mesh_instance.cloth_flags &
                     (uint32_t) prim_mesh::CLOTH_FLAGS::SMALL) == (uint32_t) prim_mesh::CLOTH_FLAGS::SMALL) {
                    std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                    uint32_t cloth_data_size = bytes2;

                    cloth_data.resize(cloth_data_size);

                    std::memcpy(cloth_data.data(), &prim_data[prim_position], cloth_data_size);
                } else {
                    uint32_t cloth_data_size = vertex_count * 0x14;

                    cloth_data.resize(cloth_data_size);

                    std::memcpy(cloth_data.data(), &prim_data[prim_position], cloth_data_size);
                }
            }

            if (collision_offset) {
                prim_position = collision_offset;

                if (temp_weighted_prim_mesh.prim_mesh_instance.prim_object_instance.sub_type ==
                    (uint32_t) prim_object::SUBTYPE::STANDARD ||
                    temp_weighted_prim_mesh.prim_mesh_instance.prim_object_instance.sub_type ==
                    (uint32_t) prim_object::SUBTYPE::WEIGHTED) {
                    std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                    uint32_t collision_data_size = bytes2;

                    collision_data_size *= 6;
                    collision_data_size += 4;

                    collision_data.resize(collision_data_size);

                    std::memcpy(collision_data.data(), &prim_data[prim_position], collision_data_size);
                } else {
                    std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                    uint32_t collision_data_size = bytes2;

                    collision_data.resize(collision_data_size);

                    std::memcpy(collision_data.data(), &prim_data[prim_position], collision_data_size);
                }
            }

            if (temp_weighted_prim_mesh.prim_mesh_instance.prim_object_instance.sub_type !=
                (uint32_t) prim_object::SUBTYPE::STANDARD) {
                if (temp_weighted_prim_mesh.bones_nodes_count) {
                    prim_position = temp_weighted_prim_mesh.bones_nodes_offset;

                    uint32_t bones_nodes_data_size = temp_weighted_prim_mesh.bones_nodes_count * 2;

                    for (uint32_t b = 0; b < bones_nodes_data_size; b++) {
                        std::memcpy(&bytes4, &prim_data[prim_position], BYTES4);
                        prim_position += 0x4;

                        bones_nodes_data.push_back(bytes4);
                    }
                }

                if (temp_weighted_prim_mesh.bone_info_offset) {
                    prim_position = temp_weighted_prim_mesh.bone_info_offset;

                    std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);

                    uint32_t bone_info_data_size = bytes2;

                    for (uint32_t b = 0; b < bone_info_data_size; b++) {
                        std::memcpy(&bytes1, &prim_data[prim_position], BYTES1);
                        prim_position += 0x1;

                        bones_info_data.push_back(bytes1);
                    }
                }

                if (temp_weighted_prim_mesh.bone_indices_offset) {
                    prim_position = temp_weighted_prim_mesh.bone_indices_offset;

                    std::memcpy(&bytes4, &prim_data[prim_position], BYTES4);
                    prim_position += 0x4;

                    uint32_t bone_indices_data_size = bytes4;
                    bone_indices_data_size -= 2;

                    for (uint32_t b = 0; b < bone_indices_data_size; b++) {
                        std::memcpy(&bytes2, &prim_data[prim_position], BYTES2);
                        prim_position += 0x2;

                        bones_indices_data.push_back(bytes2);
                    }
                }
            }
        } else {
            //LOG("Error: PRIM_OBJECT_IS_NOT_A_MESH_TYPE");

            task_multiple_status = PRIM_OBJECT_IS_NOT_A_MESH_TYPE;

            success = false;

            //return;
        }
    } else {
        //LOG("Error: PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1");

        task_multiple_status = PRIM_UV_CHANNEL_COUNT_GREATER_THAN_1;

        success = false;

        //return;
    }
}

void prim::extract_meta(const std::string& output_path) {
    char char4[4];

    std::string prim_meta_file_name = file::output_path_append(prim_file_name + ".glb.meta", output_path);

    std::ofstream output_file = std::ofstream(prim_meta_file_name, std::ofstream::binary);

    if (!output_file.good()) {
        LOG_AND_EXIT("Error: PRIM GLB meta file " + prim_meta_file_name + " could not be created.");
    }

    std::string rpkg_base_file_name = util::to_lower_case(rpkgs.at(prim_rpkg_index).rpkg_file_name);

    size_t pos = rpkg_base_file_name.find("patch");

    if (pos != std::string::npos) {
        rpkg_base_file_name = rpkg_base_file_name.substr(0, pos);
    } else {
        pos = rpkg_base_file_name.find('.');

        if (pos != std::string::npos) {
            rpkg_base_file_name = rpkg_base_file_name.substr(0, pos);
        }
    }

    uint32_t rpkg_file_name_length = rpkg_base_file_name.length();
    rpkg_file_name_length++;

    std::memcpy(&char4, &rpkg_file_name_length, 0x4);
    output_file.write(char4, 0x4);

    output_file.write(rpkg_base_file_name.c_str(), rpkg_file_name_length);

    uint32_t prim_meta_data_size = prim_meta_data.size();

    uint32_t compressed_size = LZ4_compressBound((int) prim_meta_data_size);

    std::vector<char> output_file_data(compressed_size, 0);

    uint64_t compressed_size_final = LZ4_compress_HC(prim_meta_data.data(), output_file_data.data(),
                                                     (int) prim_meta_data_size, compressed_size, LZ4HC_CLEVEL_MAX);

    std::memcpy(&char4, &prim_meta_data_size, 0x4);
    output_file.write(char4, 0x4);

    std::memcpy(&char4, &compressed_size_final, 0x4);
    output_file.write(char4, 0x4);

    if (!borg_depends_file_name.empty()) {
        uint32_t bone_count = asset3ds_data.bones_data.size();
        std::memcpy(&char4, &bone_count, 0x4);
        output_file.write(char4, 0x4);
    } else {
        output_file.write("\0", 0x4);
    }

    output_file.write(output_file_data.data(), compressed_size_final);

    if (!borg_depends_file_name.empty()) {
        for (auto& b : asset3ds_data.bones_data) {
            uint32_t bone_name_length = b.name.length();
            bone_name_length++;

            std::memcpy(&char4, &bone_name_length, 0x4);
            output_file.write(char4, 0x4);

            output_file.write(b.name.c_str(), b.name.length());
            output_file.write("\0", 0x1);
        }
    }

    output_file.close();
}
