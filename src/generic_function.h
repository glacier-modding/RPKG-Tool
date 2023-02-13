#pragma once

#include <string>
#include "global.h"

class generic_function {
public:
    static void load_hash_list(bool exit_if_no_hash_list, const std::string& path = "");

    static std::string compute_ioi_hash(const std::string& input_to_ioi_hash);

    static void compute_ioi_hash_from_file(std::string& input_path);

    static void decrypt_packagedefinition_thumbs(std::string& input_path, std::string& output_path);

    static void encrypt_packagedefinition_thumbs(std::string& input_path, std::string& output_path);
};

inline void force_load_hash_list() {
    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(true);
        LOG("Loading Hash List: Done");
    }
}
