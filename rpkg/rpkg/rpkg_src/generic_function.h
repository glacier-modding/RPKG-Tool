#pragma once
#include <string>

class generic_function
{
public:
	static void load_hash_list();
	static void command_json(std::string& json_file_path);
	static void compute_ioi_hash(std::string& input_to_ioi_hash);
	static void compute_ioi_hash_from_file(std::string& input_path);
	static void decrypt_packagedefinition_thumbs(std::string& input_path, std::string& output_path);
	static void encrypt_packagedefinition_thumbs(std::string& input_path, std::string& output_path);
};