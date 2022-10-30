#include "rpkg_function.h"
#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "generic_function.h"
#include <iostream>

void dev_function::dev_extract_all_strings(std::string &input_path, std::string &output_path)
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

    for (auto& rpkg : rpkgs)
    {
        for (auto& j : rpkg.hash)
        {
            auto it = hash_list_hash_map.find(j.hash_value);

            if (it != hash_list_hash_map.end())
            {
                std::cout << util::to_upper_case(hash_list_hash_file_names.at(it->second)) << "," << hash_list_hash_strings.at(it->second) << std::endl;
            }
            else
            {
                std::cout << util::uint64_t_to_hex_string(j.hash_value) + "." + j.hash_resource_type << "," << std::endl;
            }
        }
    }

	std::cout << "Extracting WWEM strings..." << std::endl;

	dev_function::dev_extract_wwem_strings(output_path);

	std::cout << "Extracting WWES strings..." << std::endl;

	dev_function::dev_extract_wwes_strings(output_path);

	std::cout << "Extracting WWEV strings..." << std::endl;

	dev_function::dev_extract_wwev_strings(output_path);
}