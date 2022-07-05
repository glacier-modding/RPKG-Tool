#pragma once
#include "texd.h"
#include "thirdparty/directxtex/DirectXTex.h"
#include "thirdparty/directxtex/DDS.h"
#include <string>
#include <vector>
#include <stdint.h>

class text
{
public:
	text();
	text(uint64_t rpkgs_index, uint64_t hash_index);

	void load_hash_depends();
	bool save_text_to_jpg(std::string png_path);
	bool save_texd_to_jpg(std::string png_path);
	void save_text_to_png(std::string png_path);
	void save_texd_to_png(std::string png_path);
	void save_text_to_tga(std::string tga_path);
	void save_texd_to_tga(std::string tga_path);
	uint32_t calculate_mips_count(uint32_t width, uint32_t height);
	void generate_tga_meta_files(std::string meta_path);

	std::string text_file_name = "";
	uint32_t text_rpkg_index = 0;
	uint32_t text_hash_index = 0;
	std::vector<char> text_input_data;
	std::vector<char> text_output_data;
	std::vector<char> text_meta_data;
	std::vector<char> text_data;
	std::vector<char> texd_data;
	std::vector<char> text_data_jpg;
	std::vector<char> texd_data_jpg;
	uint64_t text_data_jpg_size;
	uint64_t texd_data_jpg_size;
	std::vector<std::string> texd_depends_file_name;
	std::vector<std::string> texd_depends_hash_string;
	std::vector<uint64_t> texd_depends_hash_value;
	std::vector<uint32_t> texd_depends_index;
	std::vector<std::vector<std::string>> texd_depends_in_rpkgs;
	std::vector<uint32_t> texd_depends_in_rpkgs_index;
	std::vector<std::vector<uint32_t>> texd_depends_rpkg_index;
	std::vector<uint32_t> texd_depends_rpkg_index_index;
	std::vector<std::vector<uint32_t>> texd_depends_hash_index;
	std::vector<uint32_t> texd_depends_hash_index_index;
	uint32_t texd_lz4_decompressed_size = 0;
	std::vector<char> texd_lz4_output_data;
	std::vector<char> output_data_lz4ed;
	bool texd_found = false;
	texd temp_texd;
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
	bool text_is_lz4ed = false;
	std::vector<char> texture_file_data;
	DirectX::Blob directx_blob;
	DirectX::TexMetadata texture_meta_data;
	DirectX::ScratchImage scratch_image_original;
	DirectX::ScratchImage scratch_image_converted;
	DirectX::ScratchImage scratch_image_mips;
	DirectX::ScratchImage scratch_image_mips_compressed;
	DirectX::ScratchImage scratch_image_output;
	DirectX::ScratchImage* scratch_image_to_export = nullptr;
	DirectX::DDS_HEADER dds_header_data{};
	DirectX::DDS_HEADER_DXT10 dds_header_data_dxt10{};
	size_t dds_file_buffer_size = 0;
};