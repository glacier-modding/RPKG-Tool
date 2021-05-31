#pragma once
#include <string>

class file
{
public:
    static bool path_exists(const std::string& s);
    static void create_directories(std::string s);
    static std::string output_path_append(std::string file_name, std::string output_path);
    static std::string get_base_file_name(std::string input);
    static std::string get_root_file_name(std::string input);
    static std::string parse_input_folder_path(std::string input);
    static void parse_input_file_name(std::string input, std::string ends_with);
    static bool is_json_file(std::string input);
    static bool is_rpkg_file(std::string input);
    static bool is_supermeta_file(std::string input);
};