#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/directxtex/DirectXTex.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <regex>
#include <Windows.h>

std::vector<std::string> util::parse_input_filter(std::string input_string)
{
    std::vector<std::string> filters;

    std::smatch m;
    std::regex re("^([^, ]+)[, ]+");
    std::regex_search(input_string, m, re);

    if (m.size() > 0)
    {
        filters.push_back(util::to_upper_case(m[1].str()));

        std::smatch m2;
        re.assign("[, ]+([^, ]+)");

        while (std::regex_search(input_string, m2, re))
        {
            filters.push_back(util::to_upper_case(m2[1].str()));

            input_string = m2.suffix().str();
        }
    }
    else
    {
        filters.push_back(util::to_upper_case(input_string));
    }

    return filters;
}

std::string util::to_upper_case(std::string s)
{
    for (uint64_t i = 0; i < s.length(); i++)
    {
        s[i] = std::toupper(s[i]);
    }

    return s;
}

std::string util::to_lower_case(std::string s)
{
    for (uint64_t i = 0; i < s.length(); i++)
    {
        s[i] = std::tolower(s[i]);
    }

    return s;
}

std::string util::remove_all_string_from_string(std::string input_string, std::string to_remove)
{
    bool done = false;

    while (!done)
    {
        size_t pos = input_string.find(to_remove);

        if (pos != std::string::npos)
        {
            input_string.erase(pos, to_remove.length());
        }
        else
        {
            done = true;
        }
    }

    return input_string;
}

std::string util::uint64_t_to_hex_string(uint64_t bytes8)
{
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << std::uppercase << bytes8;
    return ss.str();
}

std::string util::uint32_t_to_hex_string(uint32_t bytes4)
{
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << std::uppercase << bytes4;
    return ss.str();
}

std::string util::uint16_t_to_hex_string(uint16_t bytes2)
{
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << bytes2;
    return ss.str();
}

std::string util::uint8_t_to_hex_string(uint8_t bytes1)
{
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)(unsigned char)bytes1;
    return ss.str();
}

std::string util::uint64_t_to_string(uint64_t bytes8)
{
    std::stringstream ss;
    ss << bytes8;
    return ss.str();
}

std::string util::uint32_t_to_string(uint32_t bytes4)
{
    std::stringstream ss;
    ss << bytes4;
    return ss.str();
}

std::string util::uint16_t_to_string(uint16_t bytes2)
{
    std::stringstream ss;
    ss << bytes2;
    return ss.str();
}

std::string util::short_to_string(short bytes2)
{
    std::stringstream ss;
    ss << bytes2;
    return ss.str();
}

std::string util::uint8_t_to_string(uint8_t bytes1)
{
    std::stringstream ss;
    ss << (int)(unsigned char)bytes1;
    return ss.str();
}

std::string util::int32_t_to_string(int32_t bytes4)
{
    std::stringstream ss;
    ss << bytes4;
    return ss.str();
}

std::string util::float_to_string(float bytes4)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(10) << bytes4;
    return ss.str();
}

void util::replace_all_string_in_string(std::string& input, const std::string& to_replace, const std::string& replace_with)
{
    std::string new_string;
    new_string.reserve(input.length());

    std::string::size_type last_pos = 0;
    std::string::size_type find_pos;

    while (std::string::npos != (find_pos = input.find(to_replace, last_pos)))
    {
        new_string.append(input, last_pos, find_pos - last_pos);
        new_string += replace_with;
        last_pos = find_pos + to_replace.length();
    }

    new_string += input.substr(last_pos);

    input.swap(new_string);
}

bool util::float_equal(float float_existing, float float_new, float tolerance)
{
    float float_new_abs = std::abs(float_new);
    float float_existing_abs = std::abs(float_existing);
    float difference = 0;

    if (float_new_abs > float_existing_abs)
    {
        difference = float_new_abs * tolerance;
    }
    else
    {
        difference = float_existing_abs * tolerance;
    }

    if (std::abs(float_new - float_existing) > difference)
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::wstring util::string_to_wstring(const std::string& input_string)
{
    if (input_string.empty())
    {
        return std::wstring();
    }
    int num_chars = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, input_string.c_str(), input_string.length(), NULL, 0);
    std::wstring wstrTo;
    if (num_chars)
    {
        wstrTo.resize(num_chars);
        if (MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, input_string.c_str(), input_string.length(), &wstrTo[0], num_chars))
        {
            return wstrTo;
        }
    }
    return std::wstring();
}

bool util::lz4_compress_hc(const char* source, std::vector<char>& destination, int source_size, int& compressed_size)
{
    int compressed_bound = LZ4_compressBound(source_size);

    destination = std::vector<char>(compressed_bound, 0);

    compressed_size = LZ4_compress_HC(source, destination.data(), source_size, compressed_bound, LZ4HC_CLEVEL_MAX);

    if (compressed_size == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}