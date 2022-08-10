#include "rpkg_function.h"
#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
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
        std::string hash_string = generic_function::compute_ioi_hash_string(hash_list_hash_strings.at(h));
        hash_string = hash_string;
        std::string hash_string_upper = "";

        for (int i = 0; i < hash_string.length(); i++)
        {
            hash_string_upper += std::tolower(hash_string[i]);
        }

        //std::cout << hash_string_upper << "," << hash_list_hash_value_strings.at(h) << std::endl;

        if (hash_string_upper == hash_list_hash_value_strings.at(h))
        {
            matched++;
            //std::cout << "MATCHED" << std::endl;
        }
    }

    std::cout << "The number of hashes in the hash list: " << hash_list_hash_value_strings.size() << std::endl;
    std::cout << "The number of hashes matched in the hash list: " << matched << std::endl;
    std::cout << "The percentage of hashes in the hash list that were matched: " << (double)(((double)matched / (double)hash_list_hash_value_strings.size()) * (double)100.0) << "%" << std::endl;
}