#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "gpudevice.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/directxtex/DirectXTex.h"
#include "thirdparty/directxtex/DDS.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

bool rpkg_function::rebuild_text(std::string& text_folder, std::string& tga_file_path, std::string& text_file_name, std::string& meta_file_path, std::string& rpkg_output_file, std::string& rpkgs_path, bool generate_rpkgs)
{
    gpudevice gpu;

    std::ifstream meta_file = std::ifstream(meta_file_path, std::ifstream::binary);

    if (!meta_file.good())
    {
        LOG("Error: TEXT PNG Meta file " + meta_file_path + " could not be opened.");

        return false;
    }

    meta_file.seekg(0, meta_file.end);

    uint64_t meta_file_size = meta_file.tellg();

    meta_file.seekg(0, meta_file.beg);

    std::vector<char> meta_data(meta_file_size, 0);

    meta_file.read(meta_data.data(), meta_file_size);

    meta_file.close();

    char input[1024];
    char char1;
    char char2[2];
    char char4[4];
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    uint32_t position = 0;
    uint32_t meta_position = 0;

    uint64_t rpkg_file_name_length = 0;
    uint64_t texd_hash_value = 0;
    uint32_t texd_data_size = 0;
    uint32_t text_data_size = 0;
    uint16_t text_header_magic = 0;
    uint16_t text_type = 0;
    uint32_t text_file_size = 0;
    uint32_t text_flags = 0;
    uint16_t text_width = 0;
    uint16_t text_height = 0;
    uint16_t texd_width = 0;
    uint16_t texd_height = 0;
    uint16_t text_directx_format = 0;
    uint8_t text_mips_count = 0;
    uint8_t text_mips_default = 0;
    uint8_t text_mips_unknown_1 = 0;
    uint8_t text_mips_unknown_2 = 0;
    uint16_t text_mips_mode = 0;
    std::vector<uint32_t> text_mips_data_1;
    std::vector<uint32_t> text_mips_data_2;
    uint32_t text_atlas_size = 0;
    uint32_t text_atlas_offset = 0;
    uint8_t text_scaling_data_1 = 0;
    uint8_t text_scaling_width = 0;
    uint8_t text_scaling_height = 0;
    uint8_t text_scaling_data_2 = 0;
    uint32_t text_texd_scale = 0;
    uint32_t text_texd_scale_width = 0;
    uint32_t text_texd_scale_height = 0;
    uint32_t text_texture_data_offset = 0;
    uint32_t text_texture_data_size = 0;
    bool texd_found = false;
    bool text_is_lz4ed = false;

    LOG("TEXT meta file: " + text_file_name);

    std::memcpy(&rpkg_file_name_length, &meta_data.data()[position], sizeof(bytes4));
    position += 0x4;

    LOG("  - TEXT meta RPKG file name length: " + util::uint64_t_to_hex_string(rpkg_file_name_length));

    std::memcpy(&input, &meta_data.data()[position], rpkg_file_name_length);
    position += rpkg_file_name_length;

    rpkg_output_file = std::string(input);

    LOG("  - TEXT meta RPKG file name: " + rpkg_output_file);

    std::string current_path = text_folder + "\\REBUILT";

    file::create_directories(current_path);

    std::string rpkg_output_dir = rpkgs_path + "\\" + rpkg_output_file;

    std::string rpkg_output_file_name = rpkg_output_file;

    if (generate_rpkgs)
    {
        file::create_directories(rpkg_output_dir);
    }

    rpkg_output_file = rpkg_output_dir;

    std::memcpy(&texd_hash_value, &meta_data.data()[position], sizeof(bytes8));
    position += 0x8;

    meta_position = position;

    LOG("  - TEXT meta TEXD hash value: " + util::uint64_t_to_hex_string(texd_hash_value));

    if (texd_hash_value == 0x0)
    {
        texd_found = false;

        LOG("  - TEXT meta TEXD found: False");
    }
    else
    {
        texd_found = true;

        LOG("  - TEXT meta TEXD found: True");
    }

    std::memcpy(&text_header_magic, &meta_data.data()[position], sizeof(bytes2));
    position += 0x2;

    LOG("  - TEXT meta header magic: " + util::uint16_t_to_hex_string(text_header_magic));

    std::memcpy(&text_type, &meta_data.data()[position], sizeof(bytes2));
    position += 0x2;

    LOG("  - TEXT meta texture type: " + util::uint16_t_to_hex_string(text_type));

    if (text_type == 0x0)
    {
        LOG("  - TEXT meta texture type: Color");
    }
    else if (text_type == 0x1)
    {
        LOG("  - TEXT meta texture type: Normal");
    }
    else if (text_type == 0x2)
    {
        LOG("  - TEXT meta texture type: Height");
    }
    else if (text_type == 0x3)
    {
        LOG("  - TEXT meta texture type: Compound Normal");
    }
    else if (text_type == 0x4)
    {
        LOG("  - TEXT meta texture type: Billboard");
    }
    else
    {
        LOG("  - TEXT meta texture type: Unknown");
    }

    std::memcpy(&text_file_size, &meta_data.data()[position], sizeof(bytes4));
    position += 0x4;

    LOG("  - TEXT meta file size: " + util::uint32_t_to_hex_string(text_file_size));

    std::memcpy(&text_flags, &meta_data.data()[position], sizeof(bytes4));
    position += 0x4;

    LOG("  - TEXT meta flags: " + util::uint32_t_to_hex_string(text_flags));

    std::memcpy(&texd_width, &meta_data.data()[position], sizeof(bytes2));
    position += 0x2;

    LOG("  - TEXD width: " + util::uint32_t_to_hex_string(texd_width));

    std::memcpy(&texd_height, &meta_data.data()[position], sizeof(bytes2));
    position += 0x2;

    LOG("  - TEXD height: " + util::uint32_t_to_hex_string(texd_height));

    std::memcpy(&text_directx_format, &meta_data.data()[position], sizeof(bytes2));
    position += 0x2;

    LOG("  - TEXT meta DirectX format: " + util::uint32_t_to_hex_string(text_directx_format));

    if (text_directx_format < 123)
    {
        LOG("  - TEXT meta DirectX format: " + DirectXFormatsStrings[text_directx_format]);
    }
    else
    {
        LOG("  - TEXT meta DirectX format: Unknown");
    }

    std::memcpy(&text_mips_count, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta mips count: " + util::uint32_t_to_hex_string(text_mips_count));

    std::memcpy(&text_mips_default, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta mips default: " + util::uint32_t_to_hex_string(text_mips_default));

    std::memcpy(&text_mips_unknown_1, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta mips unknown 1: " + util::uint32_t_to_hex_string(text_mips_unknown_1));

    std::memcpy(&text_mips_unknown_2, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta mips unknown 2: " + util::uint32_t_to_hex_string(text_mips_unknown_2));

    std::memcpy(&text_mips_mode, &meta_data.data()[position], sizeof(bytes2));
    position += 0x2;

    LOG("  - TEXT meta mips mode: " + util::uint32_t_to_hex_string(text_mips_mode));

    for (uint32_t m = 0; m < 0xE; m++)
    {
        std::memcpy(&bytes4, &meta_data.data()[position], sizeof(bytes4));
        position += 0x4;

        if (m < text_mips_count)
        {
            LOG("  - TEXT meta mips data 1 (" + std::to_string(m) + "): " + util::uint32_t_to_hex_string(bytes4));

            text_mips_data_1.push_back(bytes4);
        }
    }

    for (uint32_t m = 0; m < 0xE; m++)
    {
        std::memcpy(&bytes4, &meta_data.data()[position], sizeof(bytes4));
        position += 0x4;

        if (m < text_mips_count)
        {
            LOG("  - TEXT meta mips data 2 (" + std::to_string(m) + "): " + util::uint32_t_to_hex_string(bytes4));

            text_mips_data_2.push_back(bytes4);
        }
    }

    std::memcpy(&text_atlas_size, &meta_data.data()[position], sizeof(bytes4));
    position += 0x4;

    LOG("  - TEXT meta atlas size: " + util::uint32_t_to_hex_string(text_atlas_size));

    std::memcpy(&text_atlas_offset, &meta_data.data()[position], sizeof(bytes4));
    position += 0x4;

    LOG("  - TEXT meta atlas offset: " + util::uint32_t_to_hex_string(text_atlas_offset));

    std::memcpy(&text_scaling_data_1, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta scaling data 1: " + util::uint32_t_to_hex_string(text_scaling_data_1));

    std::memcpy(&text_scaling_width, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta scaling width (2^): " + util::uint32_t_to_string(text_scaling_width));

    std::memcpy(&text_scaling_height, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta scaling height (2^): " + util::uint32_t_to_string(text_scaling_height));

    if (text_scaling_width == 0 && text_scaling_height == 0)
    {
        text_texd_scale = 1;
        text_width = texd_width;
        text_height = texd_height;
    }
    else
    {
        text_texd_scale_width = (2 << (text_scaling_width - 1));
        text_texd_scale_height = (2 << (text_scaling_height - 1));
        text_texd_scale = text_texd_scale_width * text_texd_scale_height;
        text_width = texd_width / text_texd_scale_width;
        text_height = texd_height / text_texd_scale_height;
    }

    LOG("  - TEXT meta TEXD scaling factor: " + util::uint32_t_to_string(text_texd_scale));

    LOG("  - TEXT meta TEXD scaled width: " + util::uint32_t_to_string(text_texd_scale_width));

    LOG("  - TEXT meta TEXD scaled height: " + util::uint32_t_to_string(text_texd_scale_height));

    std::memcpy(&text_scaling_data_2, &meta_data.data()[position], sizeof(bytes1));
    position += 0x1;

    LOG("  - TEXT meta scaling data 2: " + util::uint32_t_to_hex_string(text_scaling_data_2));

    text_texture_data_offset = text_atlas_offset + text_atlas_size;

    //text_texture_data_size = text_data.size();
    //text_texture_data_size -= text_texture_data_offset;

    if (text_mips_data_1.at(0) != text_mips_data_2.at(0))
    {
        text_is_lz4ed = true;

        LOG("  - TEXT data is LZ4ed: True");
    }
    else
    {
        LOG("  - TEXT data is LZ4ed: False");
    }

    std::vector<std::vector<char>> text_images;
    std::vector<uint32_t> text_images_sizes;
    std::vector<std::vector<char>> texd_images;
    std::vector<uint32_t> texd_images_sizes;
    std::vector<uint32_t> mipmaps_table_1;
    std::vector<uint32_t> mipmaps_table_2;

    DirectX::TexMetadata texture_meta_data;
    DirectX::ScratchImage texd_tga;
    DirectX::ScratchImage text_tga;
    DirectX::ScratchImage text_tga_compressed;
    DirectX::ScratchImage texd_mipmaps;
    DirectX::ScratchImage texd_mipmaps_compressed;
    DirectX::ScratchImage text_mipmaps;
    DirectX::ScratchImage text_mipmaps_compressed;

    std::filesystem::path texture_file_path = tga_file_path;

    HRESULT hresult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    if (FAILED(hresult))
    {
        LOG("Failed to initilize COM.");

        return false;
    }

    uint16_t header_texd_width = 0;
    uint16_t header_texd_height = 0;
    uint8_t header_mipmaps_count = 0;

    if (texd_found)
    {
        //hresult = DirectX::LoadFromWICFile(texture_file_path.generic_wstring().c_str(), DirectX::WIC_FLAGS_NONE, &texture_meta_data, texd_tga);
        hresult = DirectX::LoadFromTGAFile(texture_file_path.generic_wstring().c_str(), DirectX::TGA_FLAGS_NONE, &texture_meta_data, texd_tga);

        if (FAILED(hresult))
        {
            LOG("Error: TGA file " + texture_file_path.generic_string() + " importing failed.");

            return false;
        }

        //if (texd_tga.GetImage(0, 0, 0)->width != texd_width || texd_tga.GetImage(0, 0, 0)->height != texd_height)
        //{
            //LOG("Error: Dimensions of imported TGA file " + texture_file_path.generic_string() + " do not match original TEXD's dimentions.");
            //LOG("Warning: Dimensions of imported TGA file " + texture_file_path.generic_string() + " do not match original TEXD's dimentions.");

            LOG("TEXD width original: " + util::uint16_t_to_string(texd_width));
            LOG("TEXD height original: " + util::uint16_t_to_string(texd_height));
            LOG("TEXD width imported: " + util::uint16_t_to_string(texd_tga.GetImage(0, 0, 0)->width));
            LOG("TEXD width imported: " + util::uint16_t_to_string(texd_tga.GetImage(0, 0, 0)->height));

            header_texd_width = texd_tga.GetImage(0, 0, 0)->width;
            header_texd_height = texd_tga.GetImage(0, 0, 0)->height;

            //std::exit(0);

            //LOG("Attempting to resize the imported image to " + util::uint16_t_to_string(texd_width) + "x" + util::uint16_t_to_string(texd_height));

            //hresult = DirectX::Resize(*texd_tga.GetImage(0, 0, 0), texd_width, texd_height, DirectX::TEX_FILTER_DEFAULT, texd_tga);

            //if (FAILED(hresult))
            //{
                //LOG_AND_EXIT("Error: TGA resizing of " + texture_file_path.generic_string() + " for TEXD failed.");
            //}
        //}

        hresult = DirectX::Resize(*texd_tga.GetImage(0, 0, 0), text_width, text_height, DirectX::TEX_FILTER_DEFAULT, text_tga);

        if (FAILED(hresult))
        {
            LOG("Error: TGA resizing of " + texture_file_path.generic_string() + " for TEXT failed.");

            return false;
        }
    }
    else
    {
        //hresult = DirectX::LoadFromWICFile(texture_file_path.generic_wstring().c_str(), DirectX::WIC_FLAGS_NONE, &texture_meta_data, text_tga);
        hresult = DirectX::LoadFromTGAFile(texture_file_path.generic_wstring().c_str(), DirectX::TGA_FLAGS_NONE, &texture_meta_data, text_tga);

        if (FAILED(hresult))
        {
            LOG("Error: TGA file " + texture_file_path.generic_string() + " importing failed.");

            return false;
        }

        if (text_tga.GetImage(0, 0, 0)->width != text_width || text_tga.GetImage(0, 0, 0)->height != text_height)
        {
            LOG("Error: Dimensions of imported TGA file " + texture_file_path.generic_string() + " do not match original TEXT's dimentions.");

            LOG("TEXT width original: " + util::uint16_t_to_string(text_width));
            LOG("TEXT height original: " + util::uint16_t_to_string(text_height));
            LOG("TEXT width imported: " + util::uint16_t_to_string(text_tga.GetImage(0, 0, 0)->width));
            LOG("TEXT width imported: " + util::uint16_t_to_string(text_tga.GetImage(0, 0, 0)->height));

            return false;

            //LOG("Attempting to resize the imported image to " + util::uint16_t_to_string(texd_width) + "x" + util::uint16_t_to_string(texd_height));

            //hresult = DirectX::Resize(*texd_tga.GetImage(0, 0, 0), texd_width, texd_height, DirectX::TEX_FILTER_DEFAULT, texd_tga);

            //if (FAILED(hresult))
            //{
                //LOG_AND_EXIT("Error: TGA resizing of " + texture_file_path.generic_string() + " for TEXD failed.");
            //}
        }
    }

    if (texd_found)
    {
        uint32_t mips_count = 1;
        uint32_t temp_text_width = header_texd_width;
        uint32_t temp_text_height = header_texd_height;

        while (temp_text_width > 1 || temp_text_height > 1)
        {
            if (temp_text_width > 1)
            {
                temp_text_width >>= 1;
            }

            if (temp_text_height > 1)
            {
                temp_text_height >>= 1;
            }

            mips_count++;
        }

        header_mipmaps_count = mips_count;

        //hresult = DirectX::GenerateMipMaps(*texd_tga.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, text_mips_count, texd_mipmaps);
        hresult = DirectX::GenerateMipMaps(*texd_tga.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, mips_count, texd_mipmaps);

        if (FAILED(hresult))
        {
            LOG("Error: Couldn't generate mipmaps for TGA file " + texture_file_path.generic_string() + ".");

            return false;
        }
    }
    else
    {
        if (text_mips_count > 1)
        {
            uint32_t mips_count = 1;
            uint32_t temp_text_width = text_width;
            uint32_t temp_text_height = text_height;

            while (temp_text_width > 1 || temp_text_height > 1)
            {
                if (temp_text_width > 1)
                {
                    temp_text_width >>= 1;
                }

                if (temp_text_height > 1)
                {
                    temp_text_height >>= 1;
                }

                mips_count++;
            }

            if (text_mips_count != mips_count)
            {
                LOG("Error: TEXD header mipmap count and calculated mipmap count mismatch.");

                return false;
            }

            hresult = DirectX::GenerateMipMaps(*text_tga.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, mips_count, text_mipmaps);

            if (FAILED(hresult))
            {
                LOG("Error: Couldn't generate resized mipmaps for TGA file " + texture_file_path.generic_string() + ".");

                return false;
            }
        }
    }

    //std::cout << "TEXD mipmaps in texd_mipmaps ScratchImage: " << texd_mipmaps.GetImageCount() << std::endl;

    //std::cout << "TEXT mipmaps in text_mipmaps ScratchImage: " << text_mipmaps.GetImageCount() << std::endl;

    /*switch (format) {
    case TextureFormat::R8G8B8A8: //0x1C
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::A8: //0x42
        return DXGI_FORMAT_A8_UNORM;
    case TextureFormat::R8G8: //0x34
        return DXGI_FORMAT_R8G8_UNORM;
    case TextureFormat::DXT1: //0x49
        return DXGI_FORMAT_BC1_UNORM;
    case TextureFormat::DXT5: //0x4F
        return DXGI_FORMAT_BC3_UNORM;
    case TextureFormat::BC7: //0x5A
        return DXGI_FORMAT_BC7_UNORM;
    case TextureFormat::BC4: //0x52
        return DXGI_FORMAT_BC4_UNORM;
    case TextureFormat::BC5: //0x55
        return DXGI_FORMAT_BC5_UNORM;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }*/

    DXGI_FORMAT directx_compression_format;

    if (text_directx_format == 0x34) //R8G8
    {
        directx_compression_format = DXGI_FORMAT_R8G8_UNORM;
    }
    else if (text_directx_format == 0x42) //A8
    {

    }
    else if (text_directx_format == 0x49) //DXT1
    {
        directx_compression_format = DXGI_FORMAT_BC1_UNORM;
    }
    else if (text_directx_format == 0x4F) //DXT5
    {
        directx_compression_format = DXGI_FORMAT_BC3_UNORM;
    }
    else if (text_directx_format == 0x52) //BC4
    {
        directx_compression_format = DXGI_FORMAT_BC4_UNORM;
    }
    else if (text_directx_format == 0x55) //BC5
    {
        directx_compression_format = DXGI_FORMAT_BC5_UNORM;

        if (texd_found)
        {
            for (uint32_t d = 2; d < texd_mipmaps.GetPixelsSize(); d += 4)
            {
                texd_mipmaps.GetPixels()[d] = 0xFF;
            }
        }

        if (text_mips_count > 1)
        {
            for (uint32_t d = 2; d < text_mipmaps.GetPixelsSize(); d += 4)
            {
                text_mipmaps.GetPixels()[d] = 0xFF;
            }
        }
        else
        {
            for (uint32_t d = 2; d < text_tga.GetPixelsSize(); d += 4)
            {
                text_tga.GetPixels()[d] = 0xFF;
            }
        }
    }
    else if (text_directx_format == 0x5A) //BC7
    {
        directx_compression_format = DXGI_FORMAT_BC7_UNORM_SRGB;
    }
    else
    {
        LOG("Error: DirectX format (" + util::uint16_t_to_hex_string(text_directx_format) + ") unsupported.");

        return false;
    }

    uint32_t mipmap_count_1 = 0;
    uint32_t mipmap_count_2 = 0;

    if (texd_found)
    {
        for (uint32_t m = 0; m < texd_mipmaps.GetImageCount(); m++)
        {
            hresult = DirectX::Compress(gpu.get_device_d3d11(), *texd_mipmaps.GetImage(m, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);
            //hresult = DirectX::Compress(texd_mipmaps.GetImages(), texd_mipmaps.GetImageCount(), texd_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);

            if (FAILED(hresult))
            {
                hresult = DirectX::Compress(*texd_mipmaps.GetImage(m, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);
                //hresult = DirectX::Compress(texd_mipmaps.GetImages(), texd_mipmaps.GetImageCount(), texd_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);

                if (FAILED(hresult))
                {
                    LOG("Error: DDS conversion failed.");

                    return false;
                }
            }

            std::cout << "TEXD compressed mipmaps in texd_mipmaps_compressed ScratchImage: " << texd_mipmaps_compressed.GetImageCount() << std::endl;

            std::cout << "TEXD pixels size of compressed mipmaps in texd_mipmaps_compressed ScratchImage: " << texd_mipmaps_compressed.GetPixelsSize() << std::endl;

            std::cout << "  - TEXD compressed mipmaps (" << m << ") width: " << texd_mipmaps_compressed.GetImage(0, 0, 0)->width << std::endl;

            std::cout << "  - TEXD compressed mipmaps (" << m << ") height: " << texd_mipmaps_compressed.GetImage(0, 0, 0)->height << std::endl;

            mipmap_count_1 += (uint32_t)texd_mipmaps_compressed.GetPixelsSize();

            mipmaps_table_1.push_back(mipmap_count_1);

            texd_images.emplace_back();

            if (text_is_lz4ed)
            {
                int texd_image_size = 0;

                bool result = util::lz4_compress_hc((const char*)texd_mipmaps_compressed.GetImage(0, 0, 0)->pixels, texd_images.back(), texd_mipmaps_compressed.GetPixelsSize(), texd_image_size);

                if (result != 0)
                {
                    std::cout << "TEXD LZ4 compressed bound: " << texd_images.back().size() << std::endl;

                    std::cout << "TEXD LZ4 compressed size: " << texd_image_size << std::endl;
                }
                else
                {
                    LOG("TEXD LZ4 compressed failed.");

                    return false;
                }

                texd_images_sizes.push_back((uint32_t)texd_image_size);

                mipmap_count_2 += (uint32_t)texd_image_size;

                mipmaps_table_2.push_back(mipmap_count_2);
            }
            else
            {
                texd_images.back() = std::vector<char>(texd_mipmaps_compressed.GetPixelsSize(), 0);

                std::memcpy(texd_images.back().data(), texd_mipmaps_compressed.GetImage(0, 0, 0)->pixels, texd_mipmaps_compressed.GetPixelsSize());

                texd_images_sizes.push_back((uint32_t)texd_mipmaps_compressed.GetPixelsSize());

                mipmap_count_2 += (uint32_t)texd_mipmaps_compressed.GetPixelsSize();

                mipmaps_table_2.push_back(mipmap_count_2);
            }
        }

        hresult = DirectX::Compress(gpu.get_device_d3d11(), *text_tga.GetImage(0, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, text_tga_compressed);
        //hresult = DirectX::Compress(text_mipmaps.GetImages(), text_mipmaps.GetImageCount(), text_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, text_mipmaps_compressed);

        if (FAILED(hresult))
        {
            hresult = DirectX::Compress(*text_tga.GetImage(0, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, text_tga_compressed);
            //hresult = DirectX::Compress(text_mipmaps.GetImages(), text_mipmaps.GetImageCount(), text_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, text_mipmaps_compressed);

            if (FAILED(hresult))
            {
                LOG("Error: DDS compression failed.");

                return false;
            }
        }

        std::cout << "TEXT compressed mipmaps in text_mipmaps_compressed ScratchImage: " << text_tga_compressed.GetImageCount() << std::endl;

        std::cout << "TEXT pixels size of compressed mipmaps in text_mipmaps_compressed ScratchImage: " << text_tga_compressed.GetPixelsSize() << std::endl;

        text_images.emplace_back();

        if (text_is_lz4ed)
        {
            int text_image_size = 0;

            bool result = util::lz4_compress_hc((const char*)text_tga_compressed.GetImage(0, 0, 0)->pixels, text_images.back(), text_tga_compressed.GetPixelsSize(), text_image_size);

            if (result != 0)
            {
                std::cout << "TEXT LZ4 compressed bound: " << text_images.back().size() << std::endl;

                std::cout << "TEXT LZ4 compressed size: " << text_image_size << std::endl;
            }
            else
            {
                LOG("TEXT LZ4 compressed failed.");

                return false;
            }

            text_images_sizes.push_back((uint32_t)text_image_size);
        }
        else
        {
            text_images.back() = std::vector<char>(text_tga_compressed.GetPixelsSize(), 0);

            std::memcpy(text_images.back().data(), text_tga_compressed.GetImage(0, 0, 0)->pixels, text_tga_compressed.GetPixelsSize());

            text_images_sizes.push_back((uint32_t)text_tga_compressed.GetPixelsSize());
        }
    }
    else
    {
        if (text_mips_count > 1)
        {
            for (uint32_t m = 0; m < text_mipmaps.GetImageCount(); m++)
            {
                hresult = DirectX::Compress(gpu.get_device_d3d11(), *text_mipmaps.GetImage(m, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, text_mipmaps_compressed);
                //hresult = DirectX::Compress(texd_mipmaps.GetImages(), texd_mipmaps.GetImageCount(), texd_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);

                if (FAILED(hresult))
                {
                    hresult = DirectX::Compress(*text_mipmaps.GetImage(m, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, text_mipmaps_compressed);
                    //hresult = DirectX::Compress(texd_mipmaps.GetImages(), texd_mipmaps.GetImageCount(), texd_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);

                    if (FAILED(hresult))
                    {
                        LOG("Error: DDS conversion failed.");

                        return false;
                    }
                }

                std::cout << "TEXT compressed mipmaps in texd_mipmaps_compressed ScratchImage: " << text_mipmaps_compressed.GetImageCount() << std::endl;

                std::cout << "TEXT pixels size of compressed mipmaps in texd_mipmaps_compressed ScratchImage: " << text_mipmaps_compressed.GetPixelsSize() << std::endl;

                std::cout << "  - TEXT compressed mipmaps (" << m << ") width: " << text_mipmaps_compressed.GetImage(0, 0, 0)->width << std::endl;

                std::cout << "  - TEXT compressed mipmaps (" << m << ") height: " << text_mipmaps_compressed.GetImage(0, 0, 0)->height << std::endl;

                mipmap_count_1 += (uint32_t)text_mipmaps_compressed.GetPixelsSize();

                mipmaps_table_1.push_back(mipmap_count_1);

                text_images.emplace_back();

                if (text_is_lz4ed)
                {
                    int text_image_size = 0;

                    bool result = util::lz4_compress_hc((const char*)text_mipmaps_compressed.GetImage(0, 0, 0)->pixels, text_images.back(), text_mipmaps_compressed.GetPixelsSize(), text_image_size);

                    if (result != 0)
                    {
                        std::cout << "TEXT LZ4 compressed bound: " << text_images.back().size() << std::endl;

                        std::cout << "TEXT LZ4 compressed size: " << text_image_size << std::endl;
                    }
                    else
                    {
                        LOG("TEXT LZ4 compressed failed.");

                        return false;
                    }

                    mipmap_count_2 += (uint32_t)text_image_size;

                    mipmaps_table_2.push_back(mipmap_count_2);

                    text_images_sizes.push_back((uint32_t)text_image_size);
                }
                else
                {
                    text_images.back() = std::vector<char>(text_mipmaps_compressed.GetPixelsSize(), 0);

                    std::memcpy(text_images.back().data(), text_mipmaps_compressed.GetImage(0, 0, 0)->pixels, text_mipmaps_compressed.GetPixelsSize());

                    mipmap_count_2 += (uint32_t)text_mipmaps_compressed.GetPixelsSize();

                    mipmaps_table_2.push_back(mipmap_count_2);

                    text_images_sizes.push_back((uint32_t)text_mipmaps_compressed.GetPixelsSize());
                }
            }
        }
        else
        {
            hresult = DirectX::Compress(gpu.get_device_d3d11(), *text_tga.GetImage(0, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, text_tga_compressed);
            //hresult = DirectX::Compress(texd_mipmaps.GetImages(), texd_mipmaps.GetImageCount(), texd_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);

            if (FAILED(hresult))
            {
                hresult = DirectX::Compress(*text_tga.GetImage(0, 0, 0), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, text_tga_compressed);
                //hresult = DirectX::Compress(texd_mipmaps.GetImages(), texd_mipmaps.GetImageCount(), texd_mipmaps.GetMetadata(), directx_compression_format, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, texd_mipmaps_compressed);

                if (FAILED(hresult))
                {
                    LOG("Error: DDS conversion failed.");

                    return false;
                }
            }

            std::cout << "TEXT compressed mipmaps in texd_mipmaps_compressed ScratchImage: " << text_tga_compressed.GetImageCount() << std::endl;

            std::cout << "TEXT pixels size of compressed mipmaps in texd_mipmaps_compressed ScratchImage: " << text_tga_compressed.GetPixelsSize() << std::endl;

            std::cout << "  - TEXT compressed mipmaps (0) width: " << text_tga_compressed.GetImage(0, 0, 0)->width << std::endl;

            std::cout << "  - TEXT compressed mipmaps (0) height: " << text_tga_compressed.GetImage(0, 0, 0)->height << std::endl;

            mipmap_count_1 += (uint32_t)text_tga_compressed.GetPixelsSize();

            mipmaps_table_1.push_back(mipmap_count_1);

            text_images.emplace_back();

            if (text_is_lz4ed)
            {
                int text_image_size = 0;

                bool result = util::lz4_compress_hc((const char*)text_tga_compressed.GetImage(0, 0, 0)->pixels, text_images.back(), text_tga_compressed.GetPixelsSize(), text_image_size);

                if (result != 0)
                {
                    std::cout << "TEXT LZ4 compressed bound: " << text_images.back().size() << std::endl;

                    std::cout << "TEXT LZ4 compressed size: " << text_image_size << std::endl;
                }
                else
                {
                    LOG("TEXT LZ4 compressed failed.");

                    return false;
                }

                mipmap_count_2 += (uint32_t)text_image_size;

                mipmaps_table_2.push_back(mipmap_count_2);

                text_images_sizes.push_back((uint32_t)text_image_size);
            }
            else
            {
                text_images.back() = std::vector<char>(text_tga_compressed.GetPixelsSize(), 0);

                std::memcpy(text_images.back().data(), text_tga_compressed.GetImage(0, 0, 0)->pixels, text_tga_compressed.GetPixelsSize());

                mipmap_count_2 += (uint32_t)text_tga_compressed.GetPixelsSize();

                mipmaps_table_2.push_back(mipmap_count_2);

                text_images_sizes.push_back((uint32_t)text_tga_compressed.GetPixelsSize());
            }
        }        
    }

    std::vector<char> text_file_data;
    std::vector<char> texd_file_data;

    //for (uint64_t m = 0; m < 0x18; m++)
    //{
        //text_file_data.push_back(meta_data.data()[meta_position + m]);
    //}

    if (texd_found)
    {
        for (uint64_t m = 0; m < 0xC; m++)
        {
            text_file_data.push_back(meta_data.data()[meta_position + m]);
        }

        std::memcpy(&char2, &header_texd_width, 0x2);

        for (uint64_t n = 0; n < 0x2; n++)
        {
            text_file_data.push_back(char2[n]);
        }

        std::memcpy(&char2, &header_texd_height, 0x2);

        for (uint64_t n = 0; n < 0x2; n++)
        {
            text_file_data.push_back(char2[n]);
        }

        for (uint64_t m = 0; m < 0x2; m++)
        {
            text_file_data.push_back(meta_data.data()[meta_position + 0x10 + m]);
        }

        std::memcpy(&char1, &header_mipmaps_count, 0x1);

        text_file_data.push_back(char1);

        for (uint64_t m = 0; m < 0x5; m++)
        {
            text_file_data.push_back(meta_data.data()[meta_position + 0x13 + m]);
        }

    }
    else
    {
        for (uint64_t m = 0; m < 0x18; m++)
        {
            text_file_data.push_back(meta_data.data()[meta_position + m]);
        }
    }

    for (uint32_t m = 0; m < 0xE; m++)
    {
        if (m < mipmaps_table_1.size())
        {
            std::memcpy(&char4, &mipmaps_table_1.at(m), 0x4);

            for (uint64_t n = 0; n < 0x4; n++)
            {
                text_file_data.push_back(char4[n]);
            }
        }
        else
        {
            for (uint64_t n = 0; n < 0x4; n++)
            {
                text_file_data.push_back(0x0);
            }
        }
    }

    for (uint32_t m = 0; m < 0xE; m++)
    {
        if (m < mipmaps_table_2.size())
        {
            std::memcpy(&char4, &mipmaps_table_2.at(m), 0x4);

            for (uint64_t n = 0; n < 0x4; n++)
            {
                text_file_data.push_back(char4[n]);
            }
        }
        else
        {
            for (uint64_t n = 0; n < 0x4; n++)
            {
                text_file_data.push_back(0x0);
            }
        }
    }

    uint32_t meta_offset = meta_position + 0x88;

    for (uint64_t m = 0; m < (meta_data.size() - meta_offset); m++)
    {
        text_file_data.push_back(meta_data.data()[meta_offset + m]);
    }

    for (uint64_t m = 0; m < text_images.size(); m++)
    {
        for (uint64_t n = 0; n < text_images_sizes.at(m); n++)
        {
            text_file_data.push_back(text_images.at(m).at(n));
        }
    }

    std::ofstream text_file = std::ofstream(current_path + "\\" + text_file_name, std::ofstream::binary);

    if (!text_file.good())
    {
        LOG("Error: TEXT file " + current_path + "\\" + text_file_name + " could not be created.");

        return false;
    }

    text_file.write(text_file_data.data(), text_file_data.size());

    text_file.close();

    if (generate_rpkgs)
    {
        std::filesystem::copy(current_path + "\\" + text_file_name, rpkg_output_dir + "\\" + text_file_name, std::filesystem::copy_options::overwrite_existing);
    }

    std::filesystem::copy(text_folder + "\\metas\\" + text_file_name + ".meta", current_path + "\\" + text_file_name + ".meta", std::filesystem::copy_options::overwrite_existing);

    //std::filesystem::copy(text_folder + "\\metas\\" + text_file_name + ".tga.meta", rpkg_output_dir + "\\" + text_file_name + ".tga.meta", std::filesystem::copy_options::overwrite_existing);

    if (generate_rpkgs)
    {
        std::filesystem::copy(text_folder + "\\metas\\" + text_file_name + ".meta", rpkg_output_dir + "\\" + text_file_name + ".meta", std::filesystem::copy_options::overwrite_existing);
    }

    if (texd_found)
    {
        for (uint64_t m = 0; m < texd_images.size(); m++)
        {
            for (uint64_t n = 0; n < texd_images_sizes.at(m); n++)
            {
                texd_file_data.push_back(texd_images.at(m).at(n));
            }
        }

        std::string texd_file_name = util::uint64_t_to_hex_string(texd_hash_value) + ".TEXD";

        std::ofstream texd_file = std::ofstream(current_path + "\\" + texd_file_name, std::ofstream::binary);

        if (!texd_file.good())
        {
            LOG("Error: TEXT file " + current_path + "\\" + texd_file_name + " could not be created.");

            return false;
        }

        texd_file.write(texd_file_data.data(), texd_file_data.size());

        texd_file.close();

        if (generate_rpkgs)
        {
            std::filesystem::copy(current_path + "\\" + texd_file_name, rpkg_output_dir + "\\" + texd_file_name, std::filesystem::copy_options::overwrite_existing);
        }

        std::filesystem::copy(text_folder + "\\metas\\" + texd_file_name + ".meta", current_path + "\\" + texd_file_name + ".meta", std::filesystem::copy_options::overwrite_existing);

        if (generate_rpkgs)
        {
            std::filesystem::copy(text_folder + "\\metas\\" + texd_file_name + ".meta", rpkg_output_dir + "\\" + texd_file_name + ".meta", std::filesystem::copy_options::overwrite_existing);
        }
    }

    std::ofstream rpkg_file_name_file = std::ofstream(current_path + "\\rpkgfilename.txt", std::ofstream::binary);

    if (!rpkg_file_name_file.good())
    {
        LOG("Error: RPKG file name file " + current_path + "\\rpkgfilename.txt" + " could not be created.");

        return false;
    }

    rpkg_file_name_file.write(rpkg_output_file_name.c_str(), rpkg_output_file_name.length());

    rpkg_file_name_file.close();

    return true;
}
