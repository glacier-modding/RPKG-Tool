#include "texd.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "../thirdparty/lz4/lz4.h"
#include <iostream>
#include <fstream>

texd::texd() {

}

void texd::load(uint64_t rpkgs_index, uint64_t hash_index) {
    texd_rpkg_index = rpkgs_index;
    texd_hash_index = hash_index;

    texd_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." +
                     rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    uint64_t texd_hash_size;

    if (rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.lz4ed) {
        texd_hash_size = rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.header.data_size;

        if (rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.xored) {
            texd_hash_size &= 0x3FFFFFFF;
        }
    } else {
        texd_hash_size = rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.resource.size_final;
    }

    texd_input_data = std::vector<char>(texd_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(texd_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(texd_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.header.data_offset, file.beg);
    file.read(texd_input_data.data(), texd_hash_size);
    file.close();

    if (rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.xored) {
        crypto::xor_data(texd_input_data.data(), (uint32_t) texd_hash_size);
    }

    uint32_t texd_decompressed_size = rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.resource.size_final;

    texd_output_data = std::vector<char>(texd_decompressed_size, 0);

    if (rpkgs.at(texd_rpkg_index).hash.at(texd_hash_index).data.lz4ed) {
        LZ4_decompress_safe(texd_input_data.data(), texd_output_data.data(), (int) texd_hash_size,
                            texd_decompressed_size);

        texd_data = texd_output_data;
    } else {
        texd_data = texd_input_data;
    }

    std::vector<char>().swap(texd_output_data);
    std::vector<char>().swap(texd_input_data);
}