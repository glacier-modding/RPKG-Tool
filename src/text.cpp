#include "text.h"
#include "rpkg_function.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "texd.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/directxtex/DirectXTex.h"
#include "thirdparty/directxtex/DDS.h"
#include <unordered_map>
#include <fstream>
#include <locale>
#include <filesystem>

text::text() {

}

text::text(uint64_t rpkgs_index, uint64_t hash_index) {
    text_rpkg_index = rpkgs_index;
    text_hash_index = hash_index;

    text_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." +
                     rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    load_hash_depends();

    uint64_t text_hash_size;

    if (rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.lz4ed) {
        text_hash_size = rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.header.data_size;

        if (rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.xored) {
            text_hash_size &= 0x3FFFFFFF;
        }
    } else {
        text_hash_size = rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.resource.size_final;
    }

    text_input_data = std::vector<char>(text_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(text_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(text_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.header.data_offset, file.beg);
    file.read(text_input_data.data(), text_hash_size);
    file.close();

    if (rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.xored) {
        crypto::xor_data(text_input_data.data(), static_cast<uint32_t>(text_hash_size));
    }

    uint32_t text_decompressed_size = rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.resource.size_final;

    text_output_data = std::vector<char>(text_decompressed_size, 0);

    if (rpkgs.at(text_rpkg_index).hash.at(text_hash_index).data.lz4ed) {
        LZ4_decompress_safe(text_input_data.data(), text_output_data.data(), static_cast<int>(text_hash_size),
                            text_decompressed_size);

        text_data = text_output_data;
    } else {
        text_data = text_input_data;
    }

    std::vector<char>().swap(text_output_data);
    std::vector<char>().swap(text_input_data);

    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;

    uint32_t text_position = 0;
    uint32_t texd_position = 0;

    LOG("TEXT file: " + text_file_name);

    std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(
            rpkgs.at(text_rpkg_index).hash.at(text_hash_index).hash_value);

    if (it2 != hash_list_hash_map.end()) {
        LOG("  - IOI String: " + hash_list_hash_strings.at(it2->second));
    } else {
        LOG("  - IOI String: ");
    }

    if (texd_depends_file_name.size() > 0) {
        temp_texd.load(texd_depends_rpkg_index.at(0).at(texd_depends_rpkg_index_index.at(0)),
                       texd_depends_hash_index.at(0).at(texd_depends_hash_index_index.at(0)));

        texd_found = true;
    } else {
        LOG("  - Depends on TEXD file: None");
    }

    std::memcpy(&text_header_magic, &text_data.data()[text_position], sizeof(bytes2));
    text_position += 0x2;

    LOG("  - TEXT header magic: " + util::uint16_t_to_hex_string(text_header_magic));

    std::memcpy(&text_type, &text_data.data()[text_position], sizeof(bytes2));
    text_position += 0x2;

    LOG("  - TEXT texture type: " + util::uint16_t_to_hex_string(text_type));

    if (text_type == 0x0) {
        LOG("  - TEXT texture type: Color");
    } else if (text_type == 0x1) {
        LOG("  - TEXT texture type: Normal");
    } else if (text_type == 0x2) {
        LOG("  - TEXT texture type: Height");
    } else if (text_type == 0x3) {
        LOG("  - TEXT texture type: Compound Normal");
    } else if (text_type == 0x4) {
        LOG("  - TEXT texture type: Billboard");
    } else {
        LOG("  - TEXT texture type: Unknown");
    }

    std::memcpy(&text_file_size, &text_data.data()[text_position], sizeof(bytes4));
    text_position += 0x4;

    LOG("  - TEXT file size: " + util::uint32_t_to_hex_string(text_file_size));

    std::memcpy(&text_flags, &text_data.data()[text_position], sizeof(bytes4));
    text_position += 0x4;

    LOG("  - TEXT flags: " + util::uint32_t_to_hex_string(text_flags));

    std::memcpy(&texd_width, &text_data.data()[text_position], sizeof(bytes2));
    text_position += 0x2;

    LOG("  - TEXD width: " + util::uint32_t_to_hex_string(texd_width));

    std::memcpy(&texd_height, &text_data.data()[text_position], sizeof(bytes2));
    text_position += 0x2;

    LOG("  - TEXD height: " + util::uint32_t_to_hex_string(texd_height));

    std::memcpy(&text_directx_format, &text_data.data()[text_position], sizeof(bytes2));
    text_position += 0x2;

    LOG("  - TEXT DirectX format: " + util::uint32_t_to_hex_string(text_directx_format));

    if (text_directx_format < 123) {
        LOG("  - TEXT DirectX format: " + DirectXFormatsStrings[text_directx_format]);
    } else {
        LOG("  - TEXT DirectX format: Unknown");
    }

    std::memcpy(&text_mips_count, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT mips count: " + util::uint32_t_to_hex_string(text_mips_count));

    std::memcpy(&text_mips_default, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT mips default: " + util::uint32_t_to_hex_string(text_mips_default));

    std::memcpy(&text_mips_unknown_1, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT mips unknown 1: " + util::uint32_t_to_hex_string(text_mips_unknown_1));

    std::memcpy(&text_mips_unknown_2, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT mips unknown 2: " + util::uint32_t_to_hex_string(text_mips_unknown_2));

    std::memcpy(&text_mips_mode, &text_data.data()[text_position], sizeof(bytes2));
    text_position += 0x2;

    LOG("  - TEXT mips mode: " + util::uint32_t_to_hex_string(text_mips_mode));

    for (uint32_t m = 0; m < 0xE; m++) {
        std::memcpy(&bytes4, &text_data.data()[text_position], sizeof(bytes4));
        text_position += 0x4;

        if (m < text_mips_count) {
            LOG("  - TEXT mips data 1 (" + std::to_string(m) + "): " + util::uint32_t_to_hex_string(bytes4));

            text_mips_data_1.push_back(bytes4);
        }
    }

    for (uint32_t m = 0; m < 0xE; m++) {
        std::memcpy(&bytes4, &text_data.data()[text_position], sizeof(bytes4));
        text_position += 0x4;

        if (m < text_mips_count) {
            LOG("  - TEXT mips data 2 (" + std::to_string(m) + "): " + util::uint32_t_to_hex_string(bytes4));

            text_mips_data_2.push_back(bytes4);
        }
    }

    std::memcpy(&text_atlas_size, &text_data.data()[text_position], sizeof(bytes4));
    text_position += 0x4;

    LOG("  - TEXT atlas size: " + util::uint32_t_to_hex_string(text_atlas_size));

    std::memcpy(&text_atlas_offset, &text_data.data()[text_position], sizeof(bytes4));
    text_position += 0x4;

    LOG("  - TEXT atlas offset: " + util::uint32_t_to_hex_string(text_atlas_offset));

    std::memcpy(&text_scaling_data_1, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT scaling data 1: " + util::uint32_t_to_hex_string(text_scaling_data_1));

    std::memcpy(&text_scaling_width, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT scaling width (2^): " + util::uint32_t_to_string(text_scaling_width));

    std::memcpy(&text_scaling_height, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT scaling height (2^): " + util::uint32_t_to_string(text_scaling_height));

    if (text_scaling_width == 0 && text_scaling_height == 0) {
        text_texd_scale = 1;
        text_width = texd_width;
        text_height = texd_height;
    } else {
        text_texd_scale_width = (2 << (text_scaling_width - 1));
        text_texd_scale_height = (2 << (text_scaling_height - 1));
        text_texd_scale = text_texd_scale_width * text_texd_scale_height;
        text_width = texd_width / text_texd_scale_width;
        text_height = texd_height / text_texd_scale_height;
    }

    LOG("  - TEXT TEXD scaling factor: " + util::uint32_t_to_string(text_texd_scale));

    LOG("  - TEXT TEXD scaled width: " + util::uint32_t_to_string(text_texd_scale_width));

    LOG("  - TEXT TEXD scaled height: " + util::uint32_t_to_string(text_texd_scale_height));

    std::memcpy(&text_scaling_data_2, &text_data.data()[text_position], sizeof(bytes1));
    text_position += 0x1;

    LOG("  - TEXT scaling data 2: " + util::uint32_t_to_hex_string(text_scaling_data_2));

    text_texture_data_offset = text_atlas_offset + text_atlas_size;

    text_texture_data_size = text_data.size();
    text_texture_data_size -= text_texture_data_offset;

    std::string rpkg_base_file_name = util::to_lower_case(rpkgs.at(text_rpkg_index).rpkg_file_name);

    size_t pos = rpkg_base_file_name.find("patch");

    if (pos != std::string::npos) {
        rpkg_base_file_name = rpkg_base_file_name.substr(0, pos);
    } else {
        pos = rpkg_base_file_name.find(".");

        if (pos != std::string::npos) {
            rpkg_base_file_name = rpkg_base_file_name.substr(0, pos);
        }
    }

    uint32_t meta_position = 0;

    uint32_t rpkg_file_name_length = rpkg_base_file_name.length();
    rpkg_file_name_length++;

    text_meta_data = std::vector<char>(static_cast<uint64_t>(text_texture_data_offset) + static_cast<uint64_t>(0xC) +
                                       (uint32_t) rpkg_file_name_length, 0);

    std::memcpy(&text_meta_data.data()[meta_position], &rpkg_file_name_length, 0x4);
    meta_position += 0x4;

    std::memcpy(&text_meta_data.data()[meta_position], &rpkg_base_file_name[0], rpkg_file_name_length);
    meta_position += rpkg_file_name_length;

    if (texd_found) {
        uint64_t temp_uint64_t = std::strtoull(texd_depends_file_name.at(0).c_str(), nullptr, 16);

        std::memcpy(&text_meta_data.data()[meta_position], &temp_uint64_t, 0x8);
        meta_position += 0x8;

        LOG("  - TEXT TEXD hash value: " + util::uint64_t_to_hex_string(temp_uint64_t));
    } else {
        uint64_t temp_uint64_t = 0;

        std::memcpy(&text_meta_data.data()[meta_position], &temp_uint64_t, 0x8);
        meta_position += 0x8;
    }

    std::memcpy(&text_meta_data.data()[meta_position], text_data.data(), text_texture_data_offset);

    if (text_mips_data_1.at(0) !=
        text_mips_data_2.at(0))//(!texd_found && text_mips_data_1.at(0) != text_mips_data_2.at(0))
    {
        text_is_lz4ed = true;

        LOG("  - TEXT data is LZ4ed: True");
    } else {
        LOG("  - TEXT data is LZ4ed: False");
    }

    if (text_is_lz4ed) {
        output_data_lz4ed = std::vector<char>(text_mips_data_1.at(0) / text_texd_scale, 0);

        LZ4_decompress_safe(&text_data.data()[text_texture_data_offset], output_data_lz4ed.data(),
                            static_cast<int>(text_texture_data_size),
                            static_cast<int>(text_mips_data_1.at(0) / text_texd_scale));

        text_data = output_data_lz4ed;

        text_position = 0;

        text_data_size = text_data.size();
    } else {
        text_data_size = text_data.size() - text_texture_data_offset;

        output_data_lz4ed = std::vector<char>(text_data_size, 0);

        text_position = text_texture_data_offset;

        std::memcpy(output_data_lz4ed.data(), &text_data.data()[text_position], text_data_size);

        text_data = output_data_lz4ed;

        text_position = 0;

        text_data_size = text_data.size();
    }

    if (texd_found) {
        if (text_mips_data_2.at(0) > 0 && text_mips_data_1.at(0) != text_mips_data_2.at(0)) {
            texd_lz4_output_data = std::vector<char>(text_mips_data_1.back(), 0);

            LZ4_decompress_safe(temp_texd.texd_data.data(), texd_lz4_output_data.data(),
                                static_cast<int>(temp_texd.texd_data.size()), static_cast<int>(text_mips_data_1.at(0)));

            texd_data = texd_lz4_output_data;
        } else {
            texd_data = temp_texd.texd_data;
        }

        texd_position = 0;

        texd_data_size = texd_data.size();
    }
}

void text::load_hash_depends() {
    const uint32_t text_hash_reference_count =
            rpkgs.at(text_rpkg_index).hash.at(text_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    if (text_hash_reference_count > 0) {
        for (uint64_t i = 0; i < text_hash_reference_count; i++) {
            std::vector<std::string> text_texd_depends_in_rpkgs;
            std::vector<uint32_t> text_texd_depends_rpkg_index;
            std::vector<uint32_t> text_texd_depends_hash_index;

            bool texd_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++) {
                std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(
                        rpkgs.at(text_rpkg_index).hash.at(text_hash_index).hash_reference_data.hash_reference.at(i));

                if (it == rpkgs.at(j).hash_map.end() || rpkgs.at(j).hash.at(it->second).hash_resource_type != "TEXD")
                    continue;

                if (!texd_found) {
                    texd_depends_file_name.push_back(
                            util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                            rpkgs.at(j).hash.at(it->second).hash_resource_type);

                    texd_depends_index.push_back(i);

                    LOG("Found TEXD hash depend:");
                    LOG("  - File Name: " + texd_depends_file_name.back());
                }

                texd_found = true;

                text_texd_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                text_texd_depends_rpkg_index.push_back(j);
                text_texd_depends_hash_index.push_back(it->second);

                LOG("  - In RPKG: " + text_texd_depends_in_rpkgs.back());
                LOG("  - RPKG Index: " + util::uint64_t_to_string(text_texd_depends_rpkg_index.back()));
                LOG("  - Hash Index: " + util::uint64_t_to_string(text_texd_depends_hash_index.back()));
            }

            uint32_t texd_value = 0;

            if (texd_found) {
                texd_depends_in_rpkgs.push_back(text_texd_depends_in_rpkgs);
                texd_depends_rpkg_index.push_back(text_texd_depends_rpkg_index);
                texd_depends_hash_index.push_back(text_texd_depends_hash_index);
                texd_depends_in_rpkgs_index.push_back(texd_value);
                texd_depends_rpkg_index_index.push_back(texd_value);
                texd_depends_hash_index_index.push_back(texd_value);
            }
        }
    }

    for (uint64_t k = 0; k < texd_depends_file_name.size(); k++) {
        LOG("  - TEXD File Name: " + texd_depends_file_name.at(k));

        bool texd_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < texd_depends_in_rpkgs.at(k).size(); d++) {
            std::string texd_depends_in_rpkgs_upper_case = util::to_upper_case(texd_depends_in_rpkgs.at(k).at(d));

            std::string_view texd_depends_in_rpkgs_string_view(texd_depends_in_rpkgs_upper_case.c_str(),
                                                               texd_depends_in_rpkgs_upper_case.length());

            const size_t pos1 = texd_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos) {
                const size_t pos2 = texd_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos) {
                    texd_patch_name_found = true;

                    const long new_patch_level = std::strtol(
                            std::string(texd_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level) {
                        patch_level = new_patch_level;

                        texd_depends_in_rpkgs_index.at(k) = d;
                        texd_depends_rpkg_index_index.at(k) = d;
                        texd_depends_hash_index_index.at(k) = d;
                    }
                }
            } else {
                if (!texd_patch_name_found) {
                    texd_depends_in_rpkgs_index.at(k) = d;
                    texd_depends_rpkg_index_index.at(k) = d;
                    texd_depends_hash_index_index.at(k) = d;
                }
            }

            LOG("  - TEXD File Name In RPKG: " + texd_depends_in_rpkgs.at(k).at(d));
        }

        LOG("  - TEXD File Name In RPKG Used: " + texd_depends_in_rpkgs.at(k).at(texd_depends_rpkg_index_index.at(k)));
    }
}

bool text::save_text_to_jpg(const std::string& png_path) {
    dds_header_data.size = 124;
    dds_header_data.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
    dds_header_data.width = text_width;
    dds_header_data.height = text_height;
    dds_header_data.pitchOrLinearSize = 0;
    dds_header_data.depth = 0;
    dds_header_data.mipMapCount = 1;// calculate_mips_count(dds_header_data.width, dds_header_data.height);
    std::fill(dds_header_data.reserved1, &dds_header_data.reserved1[11], 0);
    dds_header_data.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
    dds_header_data.caps2 = NULL;
    dds_header_data.caps3 = NULL;
    dds_header_data.caps4 = NULL;
    dds_header_data.reserved2 = NULL;

    if (text_directx_format == 0x1C) {
        dds_header_data.ddspf = DirectX::DDSPF_A8B8G8R8;
    }
    if (text_directx_format == 0x34) {
        dds_header_data.ddspf = DirectX::DDSPF_R8G8_B8G8;
    }
    if (text_directx_format == 0x42) {
        //assert((header.type == TextureType::Color));
        dds_header_data.ddspf = DirectX::DDSPF_A8;
    }
    if (text_directx_format == 0x49) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT1;
    }
    if (text_directx_format == 0x4F) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT5;
    }
    if (text_directx_format == 0x52) {
        dds_header_data.ddspf = DirectX::DDSPF_BC4_UNORM;
    }
    if (text_directx_format == 0x55) {
        dds_header_data.ddspf = DirectX::DDSPF_BC5_UNORM;
    }
    if (text_directx_format == 0x5A) {
        dds_header_data.ddspf = DirectX::DDSPF_DX10;
        //dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
        dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
        dds_header_data_dxt10.resourceDimension = DirectX::DDS_DIMENSION_TEXTURE2D;
        dds_header_data_dxt10.miscFlag = 0;
        dds_header_data_dxt10.arraySize = 1;
        dds_header_data_dxt10.miscFlags2 = 0;
    }

    dds_file_buffer_size += sizeof(DirectX::DDS_MAGIC);
    dds_file_buffer_size += sizeof(DirectX::DDS_HEADER);

    if (text_directx_format == 0x5A) {
        dds_file_buffer_size += sizeof(DirectX::DDS_HEADER_DXT10);
    }

    dds_file_buffer_size += text_data_size;

    directx_blob.Initialize(dds_file_buffer_size);

    char* dds_data = static_cast<char*>(directx_blob.GetBufferPointer());

    uint32_t dds_position = 0;

    std::memcpy(&dds_data[dds_position], &DirectX::DDS_MAGIC, sizeof(DirectX::DDS_MAGIC));
    dds_position += sizeof(DirectX::DDS_MAGIC);

    std::memcpy(&dds_data[dds_position], &dds_header_data, sizeof(dds_header_data));
    dds_position += sizeof(dds_header_data);

    if (text_directx_format == 0x5A) {
        std::memcpy(&dds_data[dds_position], &dds_header_data_dxt10, sizeof(dds_header_data_dxt10));
        dds_position += sizeof(dds_header_data_dxt10);
    }

    std::memcpy(&dds_data[dds_position], text_data.data(), text_data_size);

    DirectX::LoadFromDDSMemory(directx_blob.GetBufferPointer(), directx_blob.GetBufferSize(),
                               static_cast<DirectX::DDS_FLAGS>(0), &texture_meta_data, scratch_image_original);

    if (text_directx_format == 0x1C) //
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x34) //R8G8
    {
        const HRESULT hresult = DirectX::Convert(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM,
                                                 static_cast<DirectX::TEX_FILTER_FLAGS>(0), 0, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x42) //A8
    {
        scratch_image_to_export = &scratch_image_original;
    } else if (text_directx_format == 0x49) //DXT1
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x4F) //DXT5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x52) //BC4
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_A8_UNORM,
                                                    scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x55) //BC5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x5A) //BC7
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG("Error: DDS conversion failed.");

            return false;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else {
        LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

        return false;
    }

    std::filesystem::path texture_file_path = png_path;

    //HRESULT hresult = DirectX::SaveToTGAFile(*scratch_image_to_export->GetImage(0, 0, 0), texture_file_path.generic_wstring().c_str(), &texture_meta_data);

    HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    if (FAILED(hresult)) {
        LOG("Failed to initilize COM.");

        return false;
    }

    DirectX::Blob directx_blob;

    //hresult = DirectX::SaveToWICFile(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), texture_file_path.generic_wstring().c_str());
    hresult = DirectX::SaveToWICMemory(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE,
                                       DirectX::GetWICCodec(DirectX::WIC_CODEC_JPEG), directx_blob);

    text_data_jpg_size = directx_blob.GetBufferSize();

    text_data_jpg = std::vector<char>(text_data_jpg_size, 0);

    std::memcpy(text_data_jpg.data(), directx_blob.GetBufferPointer(), text_data_jpg_size);

    if (FAILED(hresult)) {
        LOG("Error: JPG file " + texture_file_path.generic_string() + " exporting failed.");

        return false;
    }

    return true;
}

bool text::save_texd_to_jpg(const std::string& png_path) {
    if (texd_found) {
        dds_header_data.size = 124;
        dds_header_data.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
        dds_header_data.width = texd_width;
        dds_header_data.height = texd_height;
        dds_header_data.pitchOrLinearSize = 0;
        dds_header_data.depth = 0;
        dds_header_data.mipMapCount = 1;// text_mips_count;
        std::fill(dds_header_data.reserved1, &dds_header_data.reserved1[11], 0);
        dds_header_data.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
        dds_header_data.caps2 = NULL;
        dds_header_data.caps3 = NULL;
        dds_header_data.caps4 = NULL;
        dds_header_data.reserved2 = NULL;

        if (text_directx_format == 0x1C) {
            dds_header_data.ddspf = DirectX::DDSPF_A8B8G8R8;
        }
        if (text_directx_format == 0x34) {
            dds_header_data.ddspf = DirectX::DDSPF_R8G8_B8G8;
        }
        if (text_directx_format == 0x42) {
            //assert((header.type == TextureType::Color));
            dds_header_data.ddspf = DirectX::DDSPF_A8;
        }
        if (text_directx_format == 0x49) {
            dds_header_data.ddspf = DirectX::DDSPF_DXT1;
        }
        if (text_directx_format == 0x4F) {
            dds_header_data.ddspf = DirectX::DDSPF_DXT5;
        }
        if (text_directx_format == 0x52) {
            dds_header_data.ddspf = DirectX::DDSPF_BC4_UNORM;
        }
        if (text_directx_format == 0x55) {
            dds_header_data.ddspf = DirectX::DDSPF_BC5_UNORM;
        }
        if (text_directx_format == 0x5A) {
            dds_header_data.ddspf = DirectX::DDSPF_DX10;
            //dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
            dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
            dds_header_data_dxt10.resourceDimension = DirectX::DDS_DIMENSION_TEXTURE2D;
            dds_header_data_dxt10.miscFlag = 0;
            dds_header_data_dxt10.arraySize = 1;
            dds_header_data_dxt10.miscFlags2 = 0;
        }

        dds_file_buffer_size += sizeof(DirectX::DDS_MAGIC);
        dds_file_buffer_size += sizeof(DirectX::DDS_HEADER);

        if (text_directx_format == 0x5A) {
            dds_file_buffer_size += sizeof(DirectX::DDS_HEADER_DXT10);
        }

        dds_file_buffer_size += texd_data_size;

        directx_blob.Initialize(dds_file_buffer_size);

        char* dds_data = static_cast<char*>(directx_blob.GetBufferPointer());

        uint32_t dds_position = 0;

        std::memcpy(&dds_data[dds_position], &DirectX::DDS_MAGIC, sizeof(DirectX::DDS_MAGIC));
        dds_position += sizeof(DirectX::DDS_MAGIC);

        std::memcpy(&dds_data[dds_position], &dds_header_data, sizeof(dds_header_data));
        dds_position += sizeof(dds_header_data);

        if (text_directx_format == 0x5A) {
            std::memcpy(&dds_data[dds_position], &dds_header_data_dxt10, sizeof(dds_header_data_dxt10));
            dds_position += sizeof(dds_header_data_dxt10);
        }

        std::memcpy(&dds_data[dds_position], texd_data.data(), texd_data_size);

        DirectX::LoadFromDDSMemory(directx_blob.GetBufferPointer(), directx_blob.GetBufferSize(),
                                   static_cast<DirectX::DDS_FLAGS>(0), &texture_meta_data, scratch_image_original);

        if (text_directx_format == 0x1C) //
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x34) //R8G8
        {
            const HRESULT hresult = DirectX::Convert(*scratch_image_original.GetImage(0, 0, 0),
                                                     DXGI_FORMAT_R8G8B8A8_UNORM,
                                                     static_cast<DirectX::TEX_FILTER_FLAGS>(0), 0,
                                                     scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
                scratch_image_converted.GetPixels()[d] = 0xFF;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x42) //A8
        {
            scratch_image_to_export = &scratch_image_original;
        } else if (text_directx_format == 0x49) //DXT1
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x4F) //DXT5
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x52) //BC4
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_A8_UNORM,
                                                        scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x55) //BC5
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
                scratch_image_converted.GetPixels()[d] = 0xFF;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x5A) //BC7
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG("Error: DDS conversion failed.");

                return false;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else {
            LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

            return false;
        }

        std::filesystem::path texture_file_path = png_path;

        //HRESULT hresult = DirectX::SaveToTGAFile(*scratch_image_to_export->GetImage(0, 0, 0), texture_file_path.generic_wstring().c_str(), &texture_meta_data);

        HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

        if (FAILED(hresult)) {
            LOG("Failed to initilize COM.");

            return false;
        }

        DirectX::Blob directx_blob;

        //hresult = DirectX::SaveToWICFile(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), texture_file_path.generic_wstring().c_str());
        hresult = DirectX::SaveToWICMemory(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE,
                                           DirectX::GetWICCodec(DirectX::WIC_CODEC_JPEG), directx_blob);

        texd_data_jpg_size = directx_blob.GetBufferSize();

        texd_data_jpg = std::vector<char>(texd_data_jpg_size, 0);

        std::memcpy(texd_data_jpg.data(), directx_blob.GetBufferPointer(), texd_data_jpg_size);

        if (FAILED(hresult)) {
            LOG("Error: JPG file " + texture_file_path.generic_string() + " exporting failed.");

            return false;
        }
    }

    return true;
}

void text::save_text_to_png(const std::string& png_path) {
    dds_header_data.size = 124;
    dds_header_data.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
    dds_header_data.width = text_width;
    dds_header_data.height = text_height;
    dds_header_data.pitchOrLinearSize = 0;
    dds_header_data.depth = 0;
    dds_header_data.mipMapCount = 1;// calculate_mips_count(dds_header_data.width, dds_header_data.height);
    std::fill(dds_header_data.reserved1, &dds_header_data.reserved1[11], 0);
    dds_header_data.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
    dds_header_data.caps2 = NULL;
    dds_header_data.caps3 = NULL;
    dds_header_data.caps4 = NULL;
    dds_header_data.reserved2 = NULL;

    if (text_directx_format == 0x1C) {
        dds_header_data.ddspf = DirectX::DDSPF_A8B8G8R8;
    }
    if (text_directx_format == 0x34) {
        dds_header_data.ddspf = DirectX::DDSPF_R8G8_B8G8;
    }
    if (text_directx_format == 0x42) {
        //assert((header.type == TextureType::Color));
        dds_header_data.ddspf = DirectX::DDSPF_A8;
    }
    if (text_directx_format == 0x49) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT1;
    }
    if (text_directx_format == 0x4F) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT5;
    }
    if (text_directx_format == 0x52) {
        dds_header_data.ddspf = DirectX::DDSPF_BC4_UNORM;
    }
    if (text_directx_format == 0x55) {
        dds_header_data.ddspf = DirectX::DDSPF_BC5_UNORM;
    }
    if (text_directx_format == 0x5A) {
        dds_header_data.ddspf = DirectX::DDSPF_DX10;
        //dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
        dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
        dds_header_data_dxt10.resourceDimension = DirectX::DDS_DIMENSION_TEXTURE2D;
        dds_header_data_dxt10.miscFlag = 0;
        dds_header_data_dxt10.arraySize = 1;
        dds_header_data_dxt10.miscFlags2 = 0;
    }

    dds_file_buffer_size += sizeof(DirectX::DDS_MAGIC);
    dds_file_buffer_size += sizeof(DirectX::DDS_HEADER);

    if (text_directx_format == 0x5A) {
        dds_file_buffer_size += sizeof(DirectX::DDS_HEADER_DXT10);
    }

    dds_file_buffer_size += text_data_size;

    directx_blob.Initialize(dds_file_buffer_size);

    char* dds_data = static_cast<char*>(directx_blob.GetBufferPointer());

    uint32_t dds_position = 0;

    std::memcpy(&dds_data[dds_position], &DirectX::DDS_MAGIC, sizeof(DirectX::DDS_MAGIC));
    dds_position += sizeof(DirectX::DDS_MAGIC);

    std::memcpy(&dds_data[dds_position], &dds_header_data, sizeof(dds_header_data));
    dds_position += sizeof(dds_header_data);

    if (text_directx_format == 0x5A) {
        std::memcpy(&dds_data[dds_position], &dds_header_data_dxt10, sizeof(dds_header_data_dxt10));
        dds_position += sizeof(dds_header_data_dxt10);
    }

    std::memcpy(&dds_data[dds_position], text_data.data(), text_data_size);

    DirectX::LoadFromDDSMemory(directx_blob.GetBufferPointer(), directx_blob.GetBufferSize(),
                               static_cast<DirectX::DDS_FLAGS>(0), &texture_meta_data, scratch_image_original);

    if (text_directx_format == 0x1C) //
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x34) //R8G8
    {
        const HRESULT hresult = DirectX::Convert(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM,
                                                 static_cast<DirectX::TEX_FILTER_FLAGS>(0), 0, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x42) //A8
    {
        scratch_image_to_export = &scratch_image_original;
    } else if (text_directx_format == 0x49) //DXT1
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x4F) //DXT5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x52) //BC4
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_A8_UNORM,
                                                    scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x55) //BC5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x5A) //BC7
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else {
        LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

        return;
    }

    const std::filesystem::path texture_file_path = png_path;

    //HRESULT hresult = DirectX::SaveToTGAFile(*scratch_image_to_export->GetImage(0, 0, 0), texture_file_path.generic_wstring().c_str(), &texture_meta_data);

    HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    if (FAILED(hresult)) {
        LOG_AND_EXIT("Failed to initilize COM.");
    }

    hresult = DirectX::SaveToWICFile(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE,
                                     DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
                                     texture_file_path.generic_wstring().c_str());

    if (FAILED(hresult)) {
        LOG_AND_EXIT("Error: PNG file " + texture_file_path.generic_string() + " exporting failed.");
    }
}

void text::save_texd_to_png(const std::string& png_path) {
    if (!texd_found)
        return;

    dds_header_data.size = 124;
    dds_header_data.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
    dds_header_data.width = texd_width;
    dds_header_data.height = texd_height;
    dds_header_data.pitchOrLinearSize = 0;
    dds_header_data.depth = 0;
    dds_header_data.mipMapCount = 1;// text_mips_count;
    std::fill(dds_header_data.reserved1, &dds_header_data.reserved1[11], 0);
    dds_header_data.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
    dds_header_data.caps2 = NULL;
    dds_header_data.caps3 = NULL;
    dds_header_data.caps4 = NULL;
    dds_header_data.reserved2 = NULL;

    if (text_directx_format == 0x1C) {
        dds_header_data.ddspf = DirectX::DDSPF_A8B8G8R8;
    }
    if (text_directx_format == 0x34) {
        dds_header_data.ddspf = DirectX::DDSPF_R8G8_B8G8;
    }
    if (text_directx_format == 0x42) {
        //assert((header.type == TextureType::Color));
        dds_header_data.ddspf = DirectX::DDSPF_A8;
    }
    if (text_directx_format == 0x49) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT1;
    }
    if (text_directx_format == 0x4F) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT5;
    }
    if (text_directx_format == 0x52) {
        dds_header_data.ddspf = DirectX::DDSPF_BC4_UNORM;
    }
    if (text_directx_format == 0x55) {
        dds_header_data.ddspf = DirectX::DDSPF_BC5_UNORM;
    }
    if (text_directx_format == 0x5A) {
        dds_header_data.ddspf = DirectX::DDSPF_DX10;
        //dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
        dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
        dds_header_data_dxt10.resourceDimension = DirectX::DDS_DIMENSION_TEXTURE2D;
        dds_header_data_dxt10.miscFlag = 0;
        dds_header_data_dxt10.arraySize = 1;
        dds_header_data_dxt10.miscFlags2 = 0;
    }

    dds_file_buffer_size += sizeof(DirectX::DDS_MAGIC);
    dds_file_buffer_size += sizeof(DirectX::DDS_HEADER);

    if (text_directx_format == 0x5A) {
        dds_file_buffer_size += sizeof(DirectX::DDS_HEADER_DXT10);
    }

    dds_file_buffer_size += texd_data_size;

    directx_blob.Initialize(dds_file_buffer_size);

    char* dds_data = static_cast<char*>(directx_blob.GetBufferPointer());

    uint32_t dds_position = 0;

    std::memcpy(&dds_data[dds_position], &DirectX::DDS_MAGIC, sizeof(DirectX::DDS_MAGIC));
    dds_position += sizeof(DirectX::DDS_MAGIC);

    std::memcpy(&dds_data[dds_position], &dds_header_data, sizeof(dds_header_data));
    dds_position += sizeof(dds_header_data);

    if (text_directx_format == 0x5A) {
        std::memcpy(&dds_data[dds_position], &dds_header_data_dxt10, sizeof(dds_header_data_dxt10));
        dds_position += sizeof(dds_header_data_dxt10);
    }

    std::memcpy(&dds_data[dds_position], texd_data.data(), texd_data_size);

    DirectX::LoadFromDDSMemory(directx_blob.GetBufferPointer(), directx_blob.GetBufferSize(),
                               static_cast<DirectX::DDS_FLAGS>(0), &texture_meta_data, scratch_image_original);

    if (text_directx_format == 0x1C) //
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x34) //R8G8
    {
        const HRESULT hresult = DirectX::Convert(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM,
                                                 static_cast<DirectX::TEX_FILTER_FLAGS>(0), 0, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x42) //A8
    {
        scratch_image_to_export = &scratch_image_original;
    } else if (text_directx_format == 0x49) //DXT1
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x4F) //DXT5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x52) //BC4
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_A8_UNORM,
                                                    scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x55) //BC5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x5A) //BC7
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else {
        LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

        return;
    }

    const std::filesystem::path texture_file_path = png_path;

    //HRESULT hresult = DirectX::SaveToTGAFile(*scratch_image_to_export->GetImage(0, 0, 0), texture_file_path.generic_wstring().c_str(), &texture_meta_data);

    HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    if (FAILED(hresult)) {
        LOG_AND_EXIT("Failed to initilize COM.");
    }

    hresult = DirectX::SaveToWICFile(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE,
                                     DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
                                     texture_file_path.generic_wstring().c_str());

    if (FAILED(hresult)) {
        LOG_AND_EXIT("Error: PNG file " + texture_file_path.generic_string() + " exporting failed.");
    }
}

uint32_t text::calculate_mips_count(uint32_t width, uint32_t height) {
    uint32_t mips_count = 1;

    while (width > 1 || height > 1) {
        if (width > 1) {
            width >>= 1;
        }

        if (height > 1) {
            height >>= 1;
        }

        mips_count++;
    }

    return mips_count;
}

void text::save_text_to_tga(const std::string& tga_path) {
    dds_header_data.size = 124;
    dds_header_data.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
    dds_header_data.width = text_width;
    dds_header_data.height = text_height;
    dds_header_data.pitchOrLinearSize = 0;
    dds_header_data.depth = 0;
    dds_header_data.mipMapCount = 1;// calculate_mips_count(dds_header_data.width, dds_header_data.height);
    std::fill(dds_header_data.reserved1, &dds_header_data.reserved1[11], 0);
    dds_header_data.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
    dds_header_data.caps2 = NULL;
    dds_header_data.caps3 = NULL;
    dds_header_data.caps4 = NULL;
    dds_header_data.reserved2 = NULL;

    if (text_directx_format == 0x1C) {
        dds_header_data.ddspf = DirectX::DDSPF_A8B8G8R8;
    }
    if (text_directx_format == 0x34) {
        dds_header_data.ddspf = DirectX::DDSPF_R8G8_B8G8;
    }
    if (text_directx_format == 0x42) {
        //assert((header.type == TextureType::Color));
        dds_header_data.ddspf = DirectX::DDSPF_A8;
    }
    if (text_directx_format == 0x49) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT1;
    }
    if (text_directx_format == 0x4F) {
        dds_header_data.ddspf = DirectX::DDSPF_DXT5;
    }
    if (text_directx_format == 0x52) {
        dds_header_data.ddspf = DirectX::DDSPF_BC4_UNORM;
    }
    if (text_directx_format == 0x55) {
        dds_header_data.ddspf = DirectX::DDSPF_BC5_UNORM;
    }
    if (text_directx_format == 0x5A) {
        dds_header_data.ddspf = DirectX::DDSPF_DX10;
        //dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
        dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
        dds_header_data_dxt10.resourceDimension = DirectX::DDS_DIMENSION_TEXTURE2D;
        dds_header_data_dxt10.miscFlag = 0;
        dds_header_data_dxt10.arraySize = 1;
        dds_header_data_dxt10.miscFlags2 = 0;
    }

    dds_file_buffer_size += sizeof(DirectX::DDS_MAGIC);
    dds_file_buffer_size += sizeof(DirectX::DDS_HEADER);

    if (text_directx_format == 0x5A) {
        dds_file_buffer_size += sizeof(DirectX::DDS_HEADER_DXT10);
    }

    dds_file_buffer_size += text_data_size;

    directx_blob.Initialize(dds_file_buffer_size);

    char* dds_data = static_cast<char*>(directx_blob.GetBufferPointer());

    uint32_t dds_position = 0;

    std::memcpy(&dds_data[dds_position], &DirectX::DDS_MAGIC, sizeof(DirectX::DDS_MAGIC));
    dds_position += sizeof(DirectX::DDS_MAGIC);

    std::memcpy(&dds_data[dds_position], &dds_header_data, sizeof(dds_header_data));
    dds_position += sizeof(dds_header_data);

    if (text_directx_format == 0x5A) {
        std::memcpy(&dds_data[dds_position], &dds_header_data_dxt10, sizeof(dds_header_data_dxt10));
        dds_position += sizeof(dds_header_data_dxt10);
    }

    std::memcpy(&dds_data[dds_position], text_data.data(), text_data_size);

    DirectX::LoadFromDDSMemory(directx_blob.GetBufferPointer(), directx_blob.GetBufferSize(),
                               static_cast<DirectX::DDS_FLAGS>(0), &texture_meta_data, scratch_image_original);

    if (text_directx_format == 0x1C) //
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x34) //R8G8
    {
        const HRESULT hresult = DirectX::Convert(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM,
                                                 static_cast<DirectX::TEX_FILTER_FLAGS>(0), 0, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x42) //A8
    {
        scratch_image_to_export = &scratch_image_original;
    } else if (text_directx_format == 0x49) //DXT1
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x4F) //DXT5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x52) //BC4
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_A8_UNORM,
                                                    scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x55) //BC5
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
            scratch_image_converted.GetPixels()[d] = 0xFF;
        }

        scratch_image_to_export = &scratch_image_converted;
    } else if (text_directx_format == 0x5A) //BC7
    {
        const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                    DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: DDS conversion failed.");
        }

        scratch_image_to_export = &scratch_image_converted;
    } else {
        LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

        return;
    }

    const std::filesystem::path texture_file_path = tga_path;

    const HRESULT hresult = DirectX::SaveToTGAFile(*scratch_image_to_export->GetImage(0, 0, 0),
                                                   texture_file_path.generic_wstring().c_str(), &texture_meta_data);

    //HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    //if (FAILED(hresult))
    //{
    //LOG_AND_EXIT("Failed to initilize COM.");
    //}

    //REFGUID tga_format = DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG);

    //hresult = DirectX::SaveToWICFile(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), texture_file_path.generic_wstring().c_str());

    if (FAILED(hresult)) {
        LOG_AND_EXIT("Error: TGA file " + texture_file_path.generic_string() + " exporting failed.");
    }
}

void text::save_texd_to_tga(const std::string& tga_path) {
    if (texd_found) {
        dds_header_data.size = 124;
        dds_header_data.flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
        dds_header_data.width = texd_width;
        dds_header_data.height = texd_height;
        dds_header_data.pitchOrLinearSize = 0;
        dds_header_data.depth = 0;
        dds_header_data.mipMapCount = 1;// text_mips_count;
        std::fill(dds_header_data.reserved1, &dds_header_data.reserved1[11], 0);
        dds_header_data.caps = DDS_SURFACE_FLAGS_TEXTURE | DDS_SURFACE_FLAGS_MIPMAP;
        dds_header_data.caps2 = NULL;
        dds_header_data.caps3 = NULL;
        dds_header_data.caps4 = NULL;
        dds_header_data.reserved2 = NULL;

        if (text_directx_format == 0x1C) {
            dds_header_data.ddspf = DirectX::DDSPF_A8B8G8R8;
        }
        if (text_directx_format == 0x34) {
            dds_header_data.ddspf = DirectX::DDSPF_R8G8_B8G8;
        }
        if (text_directx_format == 0x42) {
            //assert((header.type == TextureType::Color));
            dds_header_data.ddspf = DirectX::DDSPF_A8;
        }
        if (text_directx_format == 0x49) {
            dds_header_data.ddspf = DirectX::DDSPF_DXT1;
        }
        if (text_directx_format == 0x4F) {
            dds_header_data.ddspf = DirectX::DDSPF_DXT5;
        }
        if (text_directx_format == 0x52) {
            dds_header_data.ddspf = DirectX::DDSPF_BC4_UNORM;
        }
        if (text_directx_format == 0x55) {
            dds_header_data.ddspf = DirectX::DDSPF_BC5_UNORM;
        }
        if (text_directx_format == 0x5A) {
            dds_header_data.ddspf = DirectX::DDSPF_DX10;
            //dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
            dds_header_data_dxt10.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
            dds_header_data_dxt10.resourceDimension = DirectX::DDS_DIMENSION_TEXTURE2D;
            dds_header_data_dxt10.miscFlag = 0;
            dds_header_data_dxt10.arraySize = 1;
            dds_header_data_dxt10.miscFlags2 = 0;
        }

        dds_file_buffer_size += sizeof(DirectX::DDS_MAGIC);
        dds_file_buffer_size += sizeof(DirectX::DDS_HEADER);

        if (text_directx_format == 0x5A) {
            dds_file_buffer_size += sizeof(DirectX::DDS_HEADER_DXT10);
        }

        dds_file_buffer_size += texd_data_size;

        directx_blob.Initialize(dds_file_buffer_size);

        char* dds_data = static_cast<char*>(directx_blob.GetBufferPointer());

        uint32_t dds_position = 0;

        std::memcpy(&dds_data[dds_position], &DirectX::DDS_MAGIC, sizeof(DirectX::DDS_MAGIC));
        dds_position += sizeof(DirectX::DDS_MAGIC);

        std::memcpy(&dds_data[dds_position], &dds_header_data, sizeof(dds_header_data));
        dds_position += sizeof(dds_header_data);

        if (text_directx_format == 0x5A) {
            std::memcpy(&dds_data[dds_position], &dds_header_data_dxt10, sizeof(dds_header_data_dxt10));
            dds_position += sizeof(dds_header_data_dxt10);
        }

        std::memcpy(&dds_data[dds_position], texd_data.data(), texd_data_size);

        DirectX::LoadFromDDSMemory(directx_blob.GetBufferPointer(), directx_blob.GetBufferSize(),
                                   static_cast<DirectX::DDS_FLAGS>(0), &texture_meta_data, scratch_image_original);

        if (text_directx_format == 0x1C) //
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x34) //R8G8
        {
            const HRESULT hresult = DirectX::Convert(*scratch_image_original.GetImage(0, 0, 0),
                                                     DXGI_FORMAT_R8G8B8A8_UNORM,
                                                     static_cast<DirectX::TEX_FILTER_FLAGS>(0), 0,
                                                     scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
                scratch_image_converted.GetPixels()[d] = 0xFF;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x42) //A8
        {
            scratch_image_to_export = &scratch_image_original;
        } else if (text_directx_format == 0x49) //DXT1
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x4F) //DXT5
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x52) //BC4
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0), DXGI_FORMAT_A8_UNORM,
                                                        scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x55) //BC5
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            for (uint32_t d = 2; d < scratch_image_converted.GetPixelsSize(); d += 4) {
                scratch_image_converted.GetPixels()[d] = 0xFF;
            }

            scratch_image_to_export = &scratch_image_converted;
        } else if (text_directx_format == 0x5A) //BC7
        {
            const HRESULT hresult = DirectX::Decompress(*scratch_image_original.GetImage(0, 0, 0),
                                                        DXGI_FORMAT_R8G8B8A8_UNORM, scratch_image_converted);

            if (FAILED(hresult)) {
                LOG_AND_EXIT("Error: DDS conversion failed.");
            }

            scratch_image_to_export = &scratch_image_converted;
        } else {
            LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

            return;
        }

        const std::filesystem::path texture_file_path = tga_path;

        const HRESULT hresult = DirectX::SaveToTGAFile(*scratch_image_to_export->GetImage(0, 0, 0),
                                                       texture_file_path.generic_wstring().c_str(), &texture_meta_data);

        //HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

        //if (FAILED(hresult))
        //{
        //LOG_AND_EXIT("Failed to initilize COM.");
        //}

        //REFGUID tga_format = DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG);

        //hresult = DirectX::SaveToWICFile(*scratch_image_to_export->GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), texture_file_path.generic_wstring().c_str());

        if (FAILED(hresult)) {
            LOG_AND_EXIT("Error: TGA file " + texture_file_path.generic_string() + " exporting failed.");
        }
    }
}

void text::generate_tga_meta_files(const std::string& meta_path) const {
    std::ofstream output_file = std::ofstream(meta_path + "\\" + text_file_name + ".tga.meta", std::ofstream::binary);

    if (!output_file.good()) {
        LOG_AND_EXIT("Error: TEXT TGA meta file " + meta_path + "\\" + text_file_name + ".tga.meta" +
                     " could not be created.");
    }

    output_file.write(text_meta_data.data(), text_meta_data.size());

    output_file.close();

    std::string meta_file_path = meta_path + "\\" + text_file_name;

    rpkg_function::extract_hash_meta(text_rpkg_index, text_hash_index, meta_file_path);

    if (texd_found) {
        meta_file_path = meta_path + "\\" + texd_depends_file_name.at(0);

        rpkg_function::extract_hash_meta(texd_depends_rpkg_index.at(0).at(texd_depends_rpkg_index_index.at(0)),
                                         texd_depends_hash_index.at(0).at(texd_depends_hash_index_index.at(0)),
                                         meta_file_path);
    }
}
