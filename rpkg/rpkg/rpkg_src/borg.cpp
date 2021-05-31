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
}