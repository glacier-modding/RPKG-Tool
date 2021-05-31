#include "file.h"
#include "util.h"
#include "global.h"
#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>

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