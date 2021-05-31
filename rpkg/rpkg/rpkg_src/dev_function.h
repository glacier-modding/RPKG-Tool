#pragma once
#include "rpkg.h"
#include "hash.h"
#include <string>
#include <vector>
#include <stdint.h>

class dev_function
{
public:
	static void dev_diff_rpkg_supermetas(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_rpkg_supermetas(std::string& input_path, std::string& output_path);
	static void dev_extract_wwise_ids(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_all_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_wwem_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_wwes_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_wwev_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static void dev_extract_materials_textures_strings(std::string& input_path, std::string& filter, std::string& output_path);
	static std::string dev_compute_ioi_hash(std::string input_to_ioi_hash);
};