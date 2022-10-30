#include "borg.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "../thirdparty/lz4/lz4.h"
#include <iostream>
#include <unordered_map>
#include <fstream>

borg::borg() = default;

borg::borg(uint64_t rpkgs_index, uint64_t hash_index)
{
    borg_rpkg_index = rpkgs_index;
    borg_hash_index = hash_index;

    borg_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." + rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    uint64_t borg_hash_size;

    if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.lz4ed)
    {
        borg_hash_size = rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.header.data_size;

        if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.xored)
        {
            borg_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        borg_hash_size = rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.resource.size_final;
    }

    borg_input_data = std::vector<char>(borg_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(borg_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(borg_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.header.data_offset, file.beg);
    file.read(borg_input_data.data(), borg_hash_size);
    file.close();

    if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.xored)
    {
        crypto::xor_data(borg_input_data.data(), (uint32_t)borg_hash_size);
    }

    uint32_t borg_decompressed_size = rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.resource.size_final;

    borg_output_data = std::vector<char>(borg_decompressed_size, 0);

    if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).data.lz4ed)
    {
        LZ4_decompress_safe(borg_input_data.data(), borg_output_data.data(), (int)borg_hash_size, borg_decompressed_size);
    }
    
    borg_data = borg_output_data;

    std::vector<char>().swap(borg_output_data);
    std::vector<char>().swap(borg_input_data);

    char input[1024];
    const uint16_t bytes2 = 0;
    const uint32_t bytes4 = 0;

    std::memcpy(&borg_primary_header_offset, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    borg_position = borg_primary_header_offset;

    std::memcpy(&bones_count, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&bones_count_animated, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&bones_offset, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&poses_offset, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&poses_inverse_matrices, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&bones_constraints, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&poses_header, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&bones_invert, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    std::memcpy(&bones_map, &borg_data[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    borg_position = bones_offset;

    if (log_output)
    {
        LOG("BORG file: " + borg_file_name);

        auto it2 = hash_list_hash_map.find(rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).hash_value);

        if (it2 != hash_list_hash_map.end())
        {
            LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
        }
        else
        {
            LOG("  - IOI String: ");
        }

        LOG("  - BORG primary header offset: " + util::uint32_t_to_hex_string(borg_primary_header_offset));
        LOG("  - BORG bones count: " + util::uint32_t_to_hex_string(bones_count));
        LOG("  - BORG bones count animated: " + util::uint32_t_to_hex_string(bones_count_animated));
        LOG("  - BORG bones offset: " + util::uint32_t_to_hex_string(bones_offset));
        LOG("  - BORG poses offset: " + util::uint32_t_to_hex_string(poses_offset));
        LOG("  - BORG poses inverse matrices: " + util::uint32_t_to_hex_string(poses_inverse_matrices));
        LOG("  - BORG bones constraints: " + util::uint32_t_to_hex_string(bones_constraints));
        LOG("  - BORG poses header: " + util::uint32_t_to_hex_string(poses_header));
        LOG("  - BORG bones invert: " + util::uint32_t_to_hex_string(bones_invert));
        LOG("  - BORG bones map: " + util::uint32_t_to_hex_string(bones_map));
    }

    for (uint32_t b = 0; b < bones_count; b++)
    {
        bone_data temp_bone_data;

        std::memcpy(&temp_bone_data.position.x, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_bone_data.position.y, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_bone_data.position.z, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_bone_data.parent_id, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_bone_data.size.x, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_bone_data.size.y, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_bone_data.size.z, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&input, &borg_data[borg_position], 0x22);
        borg_position += 0x22;

        temp_bone_data.name = std::string(input);

        std::memcpy(&temp_bone_data.part, &borg_data[borg_position], sizeof(bytes2));
        borg_position += 0x2;

        if (log_output)
        {
            LOG("Bone " + std::to_string(b));
            LOG("    - temp_bone_data.position.x: " + util::float_to_string(temp_bone_data.position.x));
            LOG("    - temp_bone_data.position.y: " + util::float_to_string(temp_bone_data.position.y));
            LOG("    - temp_bone_data.position.z: " + util::float_to_string(temp_bone_data.position.z));
            LOG("    - parent_id: " + util::uint32_t_to_hex_string(temp_bone_data.parent_id));
            LOG("    - temp_bone_data.size.x: " + util::float_to_string(temp_bone_data.size.x));
            LOG("    - temp_bone_data.size.y: " + util::float_to_string(temp_bone_data.size.y));
            LOG("    - temp_bone_data.size.z: " + util::float_to_string(temp_bone_data.size.z));
            LOG("    - name: " + temp_bone_data.name);
            LOG("    - name length: " + util::uint32_t_to_string(temp_bone_data.name.length()));
            LOG("    - part: " + util::uint16_t_to_hex_string(temp_bone_data.part));
        }

        bones_data.push_back(temp_bone_data);
    }

    borg_position = poses_offset;

    bs_compiler_hack(bytes4);

}

void borg::bs_compiler_hack(const uint32_t bytes4) {
    for (uint32_t b = 0; b < bones_count; b++)
    {
        float temp_float_a = 0;
        float temp_float_b = 0;
        float temp_float_c = 0;
        float temp_float_d = 0;

        memcpy(&temp_float_a, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        memcpy(&temp_float_b, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        memcpy(&temp_float_c, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        memcpy(&temp_float_d, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        DirectX::XMVECTOR temp_bone_quaternion;
        temp_bone_quaternion.m128_f32[0] = temp_float_a;
        temp_bone_quaternion.m128_f32[1] = temp_float_b;
        temp_bone_quaternion.m128_f32[2] = temp_float_c;
        temp_bone_quaternion.m128_f32[3] = temp_float_d;

        vector4 temp_vector4;

        memcpy(&temp_vector4.x, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        memcpy(&temp_vector4.y, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        memcpy(&temp_vector4.z, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        memcpy(&temp_vector4.w, &borg_data[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        if (log_output)
        {
            LOG("Bone " + std::to_string(b));
            LOG("    - temp_bone_quaternion.a: " + util::float_to_string(temp_bone_quaternion.m128_f32[0]));
            LOG("    - temp_bone_quaternion.b: " + util::float_to_string(temp_bone_quaternion.m128_f32[1]));
            LOG("    - temp_bone_quaternion.c: " + util::float_to_string(temp_bone_quaternion.m128_f32[2]));
            LOG("    - temp_bone_quaternion.d: " + util::float_to_string(temp_bone_quaternion.m128_f32[3]));
            LOG("    - bones_position.x: " + util::float_to_string(temp_vector4.x));
            LOG("    - bones_position.y: " + util::float_to_string(temp_vector4.y));
            LOG("    - bones_position.z: " + util::float_to_string(temp_vector4.z));
            LOG("    - bones_position.w: " + util::float_to_string(temp_vector4.w));
        }

        bones_position.push_back(temp_vector4);

        DirectX::XMQuaternionNormalize(temp_bone_quaternion);

        bones_positions.push_back(-temp_bone_quaternion.m128_f32[0]);
        bones_positions.push_back(-temp_bone_quaternion.m128_f32[1]);
        bones_positions.push_back(-temp_bone_quaternion.m128_f32[2]);
        bones_positions.push_back(temp_bone_quaternion.m128_f32[3]);
        bones_positions.push_back(temp_vector4.x);
        bones_positions.push_back(temp_vector4.y);
        bones_positions.push_back(temp_vector4.z);
        bones_positions.push_back(temp_vector4.w);
    }

    borg_position = poses_inverse_matrices;

    for (uint32_t b = 0; b < bones_count; b++)
    {
        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;

        bones_inverse_bind_matrices.push_back(borg_data[borg_position]);
        borg_position += 0x4;
    }
}
