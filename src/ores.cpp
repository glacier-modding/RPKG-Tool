#include "ores.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include <unordered_map>
#include <fstream>
#include <iomanip>

ores::ores() = default;

ores::ores(uint64_t rpkgs_index, uint64_t hash_index)
{
    ores_rpkg_index = rpkgs_index;
    ores_hash_index = hash_index;

    uint64_t ores_hash_size;

    if (rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.lz4ed)
    {
        ores_hash_size = rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.header.data_size;

        if (rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.xored)
        {
            ores_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        ores_hash_size = rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.resource.size_final;
    }

    std::vector<char> ores_input_data = std::vector<char>(ores_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(ores_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(ores_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.header.data_offset, file.beg);
    file.read(ores_input_data.data(), ores_hash_size);
    file.close();

    if (rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.xored)
    {
        crypto::xor_data(ores_input_data.data(), (uint32_t)ores_hash_size);
    }

    uint32_t ores_decompressed_size = rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.resource.size_final;

    std::vector<char> ores_output_data = std::vector<char>(ores_decompressed_size, 0);

    if (rpkgs.at(ores_rpkg_index).hash.at(ores_hash_index).data.lz4ed)
    {
        LZ4_decompress_safe(ores_input_data.data(), ores_output_data.data(), (int)ores_hash_size, ores_decompressed_size);

        ores_data = ores_output_data;
    }
    else
    {
        ores_data = ores_input_data;
    }

    std::vector<char>().swap(ores_output_data);
    std::vector<char>().swap(ores_input_data);

    std::vector<uint64_t> ores_hash_resource;
    std::vector<std::string> ores_hash_resource_file_path;
    std::vector<std::vector<std::string>> ores_hash_resource_file_path_elements;

    uint32_t ores_hash_resource_file_count = 0;

    uint32_t position = 0x10;

    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    std::memcpy(&bytes4, (&ores_data.data()[0] + position), sizeof(bytes4));
    position = bytes4 + 0xC;

    std::memcpy(&ores_hash_resource_file_count, (&ores_data.data()[0] + position), sizeof(bytes4));
    position += 0x4;

    for (uint64_t k = 0; k < ores_hash_resource_file_count; k++)
    {
        uint32_t string_offset = 0;
        uint8_t string_length = 0;
        uint64_t hash_lsb = 0;
        uint64_t hash_msb = 0;
        uint64_t hash = 0;

        std::memcpy(&string_length, (&ores_data.data()[0] + position), sizeof(bytes1));
        position += 0x8;

        std::memcpy(&string_offset, (&ores_data.data()[0] + position), sizeof(bytes4));
        string_offset += 0x10;
        position += 0x8;

        std::memcpy(&hash_lsb, (&ores_data.data()[0] + position), sizeof(bytes4));
        position += 0x4;

        std::memcpy(&hash_msb, (&ores_data.data()[0] + position), sizeof(bytes4));
        position += 0x4;

        hash = (hash_lsb << 32) | hash_msb;

        std::string hash_string = util::uint64_t_to_hex_string(hash);

        ores_entries[std::string(&ores_data.data()[0] + string_offset)] = hash;
    }
}