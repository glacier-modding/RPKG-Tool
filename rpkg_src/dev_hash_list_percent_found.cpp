#include "rpkg_function.h"
#include "dev_function.h"
#include "global.h"
#include "generic_function.h"
#include <iostream>
#include <filesystem>

void dev_function::dev_hash_list_percent_found(std::string& input_path, std::string& filter, std::string& output_path)
{
    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    uint64_t matched = 0;

    for (uint64_t h = 0; h < hash_list_hash_value_strings.size(); h++)
    {
        std::string hash_string = generic_function::compute_ioi_hash(hash_list_hash_strings.at(h));

        //std::cout << hash_string_upper << "," << hash_list_hash_value_strings.at(h) << std::endl;

        if (hash_string == hash_list_hash_value_strings.at(h))
        {
            matched++;
            //std::cout << "MATCHED" << std::endl;
        }
    }

    std::cout << "The number of hashes in the hash list: " << hash_list_hash_value_strings.size() << std::endl;
    std::cout << "The number of hashes matched in the hash list: " << matched << std::endl;
    std::cout << "The percentage of hashes in the hash list that were matched: " << static_cast<double>(matched) / static_cast<double>(hash_list_hash_value_strings.size()) * 100.0 << "%" << std::endl;
}