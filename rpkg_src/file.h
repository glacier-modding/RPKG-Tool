#pragma once
#include <string>
#include <vector>
#include <filesystem>

class file
{
public:
    static uint64_t get_hash_value_from_path(const std::filesystem::path& path, const std::string& extension);
    static std::vector<std::filesystem::path> get_recursive_file_list(std::string path);
    static bool write_to_file(const std::string& file_name, const std::string& data);
    static bool path_exists(const std::string& path);
    static void create_directories(const std::string& path);
    static std::string output_path_append(const std::string& file_name, std::string output_path);
    static std::string get_base_file_name(const std::string& input);
    static std::string get_root_file_name(const std::string& input);
    static std::string parse_input_folder_path(std::string input);
    static void parse_input_file_name(const std::string& input, const std::string& ends_with);
    static bool is_json_file(const std::string& input);
    static bool is_rpkg_file(const std::string& input);
    static bool is_supermeta_file(const std::string& input);
};