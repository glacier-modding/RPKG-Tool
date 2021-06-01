#include "borg.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <fstream>
#include <set>

borg::borg()
{

}

borg::borg(uint64_t rpkgs_index, uint64_t hash_index)
{
    borg_rpkg_index = rpkgs_index;
    borg_hash_index = hash_index;

    borg_file_name = rpkgs.at(rpkgs_index).hash.at(hash_index).hash_file_name;

    uint64_t borg_hash_size;

    if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).is_lz4ed == 1)
    {
        borg_hash_size = rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).hash_size;

        if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).is_xored == 1)
        {
            borg_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        borg_hash_size = rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).hash_size_final;
    }

    borg_input_data = std::vector<char>(borg_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(borg_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(borg_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).hash_offset, file.beg);
    file.read(borg_input_data.data(), borg_hash_size);
    file.close();

    if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).is_xored == 1)
    {
        crypto::xor_data(borg_input_data.data(), (uint32_t)borg_hash_size);
    }

    uint32_t borg_decompressed_size = rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).hash_size_final;

    borg_output_data = std::vector<char>(borg_decompressed_size, 0);

    if (rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).is_lz4ed)
    {
        LZ4_decompress_safe(borg_input_data.data(), borg_output_data.data(), (int)borg_hash_size, borg_decompressed_size);

        borg_data = borg_output_data;
    }
    else
    {
        borg_data = borg_input_data;
    }

    std::vector<char>().swap(borg_output_data);
    std::vector<char>().swap(borg_input_data);

    char input[1024];
    char char2[2];
    char char4[4];
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    LOG("BORG file: " + borg_file_name);

    std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(borg_rpkg_index).hash.at(borg_hash_index).hash_value);

    if (it2 != hash_list_hash_map.end())
    {
        LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
    }
    else
    {
        LOG("  - IOI String: ");
    }

    std::memcpy(&borg_primary_header_offset, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG primary header offset: " + util::uint32_t_to_hex_string(borg_primary_header_offset));

    borg_position = borg_primary_header_offset;

    std::memcpy(&bones_count, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG bones count: " + util::uint32_t_to_hex_string(bones_count));

    std::memcpy(&bones_count_animated, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG bones count animated: " + util::uint32_t_to_hex_string(bones_count_animated));

    std::memcpy(&bones_offset, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG bones offset: " + util::uint32_t_to_hex_string(bones_offset));

    std::memcpy(&poses_offset, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG poses offset: " + util::uint32_t_to_hex_string(poses_offset));

    std::memcpy(&poses_inverse_matrices, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG poses inverse matrices: " + util::uint32_t_to_hex_string(poses_inverse_matrices));

    std::memcpy(&bones_constraints, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG bones constraints: " + util::uint32_t_to_hex_string(bones_constraints));

    std::memcpy(&poses_header, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG poses header: " + util::uint32_t_to_hex_string(poses_header));

    std::memcpy(&bones_invert, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG bones invert: " + util::uint32_t_to_hex_string(bones_invert));

    std::memcpy(&bones_map, &borg_data.data()[borg_position], sizeof(bytes4));
    borg_position += 0x4;

    LOG("  - BORG bones map: " + util::uint32_t_to_hex_string(bones_map));

    borg_position = bones_offset;

    for (uint32_t b = 0; b < bones_count; b++)
    {
        bone_data temp_bone_data;

        LOG("Bone " + std::to_string(b));

        std::memcpy(&temp_bone_data.position.x, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - temp_bone_data.position.x: " + util::float_to_string(temp_bone_data.position.x));

        std::memcpy(&temp_bone_data.position.y, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - temp_bone_data.position.y: " + util::float_to_string(temp_bone_data.position.y));

        std::memcpy(&temp_bone_data.position.z, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - temp_bone_data.position.z: " + util::float_to_string(temp_bone_data.position.z));

        std::memcpy(&temp_bone_data.parent_id, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - parent_id: " + util::uint32_t_to_hex_string(temp_bone_data.parent_id));

        std::memcpy(&temp_bone_data.size.x, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - temp_bone_data.size.x: " + util::float_to_string(temp_bone_data.size.x));

        std::memcpy(&temp_bone_data.size.y, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - temp_bone_data.size.y: " + util::float_to_string(temp_bone_data.size.y));

        std::memcpy(&temp_bone_data.size.z, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - temp_bone_data.size.z: " + util::float_to_string(temp_bone_data.size.z));

        std::memcpy(&input, &borg_data.data()[borg_position], 0x22);
        borg_position += 0x22;

        temp_bone_data.name = std::string(input);

        LOG("    - name: " + temp_bone_data.name);

        LOG("    - name length: " + util::uint32_t_to_string(temp_bone_data.name.length()));

        std::memcpy(&temp_bone_data.part, &borg_data.data()[borg_position], sizeof(bytes2));
        borg_position += 0x2;

        LOG("    - part: " + util::uint16_t_to_hex_string(temp_bone_data.part));

        bones_data.push_back(temp_bone_data);
    }

    borg_position = poses_offset;

    for (uint32_t b = 0; b < bones_count; b++)
    {
        LOG("Bone " + std::to_string(b));

        float temp_float_a = 0;
        float temp_float_b = 0;
        float temp_float_c = 0;
        float temp_float_d = 0;

        std::memcpy(&temp_float_a, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_float_b, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_float_c, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&temp_float_d, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        DirectX::XMVECTOR temp_bone_quaternion;
        temp_bone_quaternion.m128_f32[0] = temp_float_a;
        temp_bone_quaternion.m128_f32[1] = temp_float_b;
        temp_bone_quaternion.m128_f32[2] = temp_float_c;
        temp_bone_quaternion.m128_f32[3] = temp_float_d;

        LOG("    - temp_bone_quaternion.a: " + util::float_to_string(temp_bone_quaternion.m128_f32[0]));
        LOG("    - temp_bone_quaternion.b: " + util::float_to_string(temp_bone_quaternion.m128_f32[1]));
        LOG("    - temp_bone_quaternion.c: " + util::float_to_string(temp_bone_quaternion.m128_f32[2]));
        LOG("    - temp_bone_quaternion.d: " + util::float_to_string(temp_bone_quaternion.m128_f32[3]));

        vector4 temp_vector4;

        std::memcpy(&temp_vector4.x, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - bones_position.x: " + util::float_to_string(temp_vector4.x));

        std::memcpy(&temp_vector4.y, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - bones_position.y: " + util::float_to_string(temp_vector4.y));

        std::memcpy(&temp_vector4.z, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - bones_position.z: " + util::float_to_string(temp_vector4.z));

        std::memcpy(&temp_vector4.w, &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - bones_position.w: " + util::float_to_string(temp_vector4.w));

        bones_position.push_back(temp_vector4);

        DirectX::XMVECTOR temp_bone_quaternion_normalized = DirectX::XMQuaternionNormalize(temp_bone_quaternion);

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
        LOG("Bone " + std::to_string(b));

        DirectX::XMMATRIX inverse_bind_matrix = DirectX::XMMatrixIdentity();

        std::memcpy(&inverse_bind_matrix.r[0].m128_f32[0], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[0].m128_f32[1], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[0].m128_f32[2], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[1].m128_f32[0], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[1].m128_f32[1], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[1].m128_f32[2], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[2].m128_f32[0], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[2].m128_f32[1], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[2].m128_f32[2], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[3].m128_f32[0], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[3].m128_f32[1], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        std::memcpy(&inverse_bind_matrix.r[3].m128_f32[2], &borg_data.data()[borg_position], sizeof(bytes4));
        borg_position += 0x4;

        LOG("    - inverse_bind_matrix.r[0].m128_f32[0]: " + util::float_to_string(inverse_bind_matrix.r[0].m128_f32[0]));
        LOG("    - inverse_bind_matrix.r[0].m128_f32[1]: " + util::float_to_string(inverse_bind_matrix.r[0].m128_f32[1]));
        LOG("    - inverse_bind_matrix.r[0].m128_f32[2]: " + util::float_to_string(inverse_bind_matrix.r[0].m128_f32[2]));
        LOG("    - inverse_bind_matrix.r[0].m128_f32[3]: " + util::float_to_string(inverse_bind_matrix.r[0].m128_f32[3]));
        LOG("    - inverse_bind_matrix.r[1].m128_f32[0]: " + util::float_to_string(inverse_bind_matrix.r[1].m128_f32[0]));
        LOG("    - inverse_bind_matrix.r[1].m128_f32[1]: " + util::float_to_string(inverse_bind_matrix.r[1].m128_f32[1]));
        LOG("    - inverse_bind_matrix.r[1].m128_f32[2]: " + util::float_to_string(inverse_bind_matrix.r[1].m128_f32[2]));
        LOG("    - inverse_bind_matrix.r[1].m128_f32[3]: " + util::float_to_string(inverse_bind_matrix.r[1].m128_f32[3]));
        LOG("    - inverse_bind_matrix.r[2].m128_f32[0]: " + util::float_to_string(inverse_bind_matrix.r[2].m128_f32[0]));
        LOG("    - inverse_bind_matrix.r[2].m128_f32[1]: " + util::float_to_string(inverse_bind_matrix.r[2].m128_f32[1]));
        LOG("    - inverse_bind_matrix.r[2].m128_f32[2]: " + util::float_to_string(inverse_bind_matrix.r[2].m128_f32[2]));
        LOG("    - inverse_bind_matrix.r[2].m128_f32[3]: " + util::float_to_string(inverse_bind_matrix.r[2].m128_f32[3]));
        LOG("    - inverse_bind_matrix.r[3].m128_f32[0]: " + util::float_to_string(inverse_bind_matrix.r[3].m128_f32[0]));
        LOG("    - inverse_bind_matrix.r[3].m128_f32[1]: " + util::float_to_string(inverse_bind_matrix.r[3].m128_f32[1]));
        LOG("    - inverse_bind_matrix.r[3].m128_f32[2]: " + util::float_to_string(inverse_bind_matrix.r[3].m128_f32[2]));
        LOG("    - inverse_bind_matrix.r[3].m128_f32[3]: " + util::float_to_string(inverse_bind_matrix.r[3].m128_f32[3]));

        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[0].m128_f32[0]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[0].m128_f32[1]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[0].m128_f32[2]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[0].m128_f32[3]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[1].m128_f32[0]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[1].m128_f32[1]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[1].m128_f32[2]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[1].m128_f32[3]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[2].m128_f32[0]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[2].m128_f32[1]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[2].m128_f32[2]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[2].m128_f32[3]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[3].m128_f32[0]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[3].m128_f32[1]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[3].m128_f32[2]);
        bones_inverse_bind_matrices.push_back(inverse_bind_matrix.r[3].m128_f32[3]);

        /*DirectX::XMMATRIX inverse_inverse_bind_matrix = DirectX::XMMatrixInverse(nullptr, inverse_bind_matrix);

        DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(inverse_inverse_bind_matrix);

        bones_positions.push_back(quaternion.m128_f32[0]);
        bones_positions.push_back(quaternion.m128_f32[1]);
        bones_positions.push_back(quaternion.m128_f32[2]);
        bones_positions.push_back(quaternion.m128_f32[3]);
        bones_positions.push_back(inverse_inverse_bind_matrix.r[3].m128_f32[0]);
        bones_positions.push_back(inverse_inverse_bind_matrix.r[3].m128_f32[1]);
        bones_positions.push_back(inverse_inverse_bind_matrix.r[3].m128_f32[2]);
        bones_positions.push_back(inverse_inverse_bind_matrix.r[3].m128_f32[3]);*/
    }
}