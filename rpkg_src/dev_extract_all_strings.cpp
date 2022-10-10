#include "rpkg_function.h"
#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "generic_function.h"
#include <iostream>
#include <unordered_map>
#include <regex>
#include <filesystem>

void dev_function::dev_extract_all_strings(std::string& input_path, std::string& filter, std::string& output_path)
{
	input_path = file::parse_input_folder_path(input_path);

	rpkg_function::import_rpkg_files_in_folder(input_path);

    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

	std::cout << "Extracting all known hashes from loaded RPKGs vs the Hash List..." << std::endl;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
        {
            std::unordered_map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(i).hash.at(j).hash_value);

            if (it != hash_list_hash_map.end())
            {
                std::cout << util::to_upper_case(hash_list_hash_file_names.at(it->second)) << "," << hash_list_hash_strings.at(it->second) << std::endl;
            }
            else
            {
                std::cout << util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(j).hash_value) + "." + rpkgs.at(i).hash.at(j).hash_resource_type << "," << std::endl;
            }
        }
    }

	std::cout << "Extracting ORES strings..." << std::endl;

	dev_function::dev_extract_ores_strings(input_path, filter, output_path);

	std::cout << "Extracting WWEM strings..." << std::endl;

	dev_function::dev_extract_wwem_strings(input_path, filter, output_path);

	std::cout << "Extracting WWES strings..." << std::endl;

	dev_function::dev_extract_wwes_strings(input_path, filter, output_path);

	std::cout << "Extracting WWEV strings..." << std::endl;

	dev_function::dev_extract_wwev_strings(input_path, filter, output_path);

	std::cout << "Extracting Materials strings..." << std::endl;

	dev_function::dev_extract_materials_textures_strings(input_path, filter, output_path);
}