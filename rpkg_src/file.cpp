#include "file.h"
#include "util.h"
#include "global.h"
#include <string>
#include <fstream>
#include <iostream>

uint64_t file::get_hash_value_from_path(std::filesystem::path path, std::string extension)
{
    std::string file_name = path.filename().string();

    uint64_t hash_value = 0;

    if (file_name.length() <= extension.length())
    {
        return hash_value;
    }

    if (util::to_upper_case(file_name.substr((file_name.length() - extension.length()), extension.length())) != util::to_upper_case(extension))
    {
        return hash_value;
    }

    std::string hash_string = util::to_upper_case(file_name.substr(0, (file_name.length() - extension.length())));

    if (!util::is_valid_hash(hash_string))
    {
        return hash_value;
    }

    return std::strtoull(hash_string.c_str(), nullptr, 16);
}

std::vector<std::filesystem::path> file::get_recursive_file_list(std::string path)
{
    std::vector<std::filesystem::path> files;

    path = file::parse_input_folder_path(path);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
    {
        if (std::filesystem::is_regular_file(entry.path().string()))
        {
            files.push_back(entry.path());
        }
    }

    return files;
}

bool file::write_to_file(std::string file_path, std::string& data)
{
    std::ofstream output_file = std::ofstream(file_path, std::ofstream::binary);

    if (!output_file.good())
    {
        return false;
    }

    output_file.write(data.c_str(), data.length());

    output_file.close();

    return true;
}

bool file::path_exists(const std::string& path)
{
    return std::filesystem::exists(path);
}

void file::create_directories(std::string path)
{
    if (!file::path_exists(path))
    {
        if (!std::filesystem::create_directories(path))
        {
            LOG_AND_EXIT("Error: Could not create directory " + path);
        }
    }
}

std::string file::output_path_append(std::string file_name, std::string output_path)
{
    std::string path = "";
    std::string base_file_name = file_name;

    if (output_path == "")
    {
        path = file_name;
    }
    else
    {
        if (output_path.substr(output_path.length() - 1, 1) == "\\")
        {
            output_path = output_path.substr(0, output_path.length() - 1);
        }

        if (output_path.substr(output_path.length() - 1, 1) == "/")
        {
            output_path = output_path.substr(0, output_path.length() - 1);
        }

        path = output_path + "\\" + file_name;
    }

    return path;
}

std::string file::parse_input_folder_path(std::string input)
{
    input = util::remove_all_string_from_string(input, "\"");
    input = util::remove_all_string_from_string(input, "\'");

    if (input.substr(input.length() - 1, 1) == "\\")
    {
        input = input.substr(0, input.length() - 1);
    }

    if (input.substr(input.length() - 1, 1) == "/")
    {
        input = input.substr(0, input.length() - 1);
    }

    input.append("\\");

    return input;
}

std::string file::get_base_file_name(std::string input)
{
    std::string input_file_name = "";

    size_t pos = input.find_last_of("\\/");

    if (pos != std::string::npos)
    {
        input_file_name = input.substr(pos + 1, input.length() - (pos + 1));
    }
    else
    {
        input_file_name = input;
    }

    return input_file_name;
}

std::string file::get_root_file_name(std::string input)
{
    std::string root_file_name = "";

    size_t pos = input.find_last_of(".");

    if (pos != std::string::npos)
    {
        root_file_name = input.substr(0, pos);
    }

    return root_file_name;
}

void file::parse_input_file_name(std::string input, std::string ends_with)
{
    std::string input_file_name = "";

    input_file_name = file::get_base_file_name(input);

    int ends_with_length = (int)ends_with.length();

    if (util::to_upper_case(input_file_name.substr((input_file_name.length() - ends_with_length), ends_with_length)) != util::to_upper_case(ends_with))
    {
        LOG_AND_EXIT("Error: Invalid input file path.");
    }
}

bool file::is_json_file(std::string input)
{
    if (input.length() > 6)
    {
        if (util::to_upper_case(input.substr((input.length() - 5), 5)) == ".JSON")
        {
            return true;
        }
    }

    return false;
}

bool file::is_rpkg_file(std::string input)
{
    if (input.length() > 6)
    {
        if (util::to_upper_case(input.substr((input.length() - 5), 5)) == ".RPKG")
        {
            return true;
        }
    }

    return false;
}

bool file::is_supermeta_file(std::string input)
{
    if (input.length() > 11)
    {
        if (util::to_upper_case(input.substr((input.length() - 10), 10)) == ".SUPERMETA")
        {
            return true;
        }
    }

    return false;
}