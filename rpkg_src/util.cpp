#include "util.h"
#include "global.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/directxtex/DirectXTex.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <regex>
#include <Windows.h>

std::string util::generate_guid()
{
    GUID guid;

    CoCreateGuid(&guid);

    std::string guidString = "";

    guidString += uint32_t_to_hex_string(guid.Data1);
    guidString += '-';
    guidString += uint16_t_to_hex_string(guid.Data2);
    guidString += '-';
    guidString += uint16_t_to_hex_string(guid.Data3);
    guidString += '-';
    guidString += uint8_t_to_hex_string(guid.Data4[0]);
    guidString += uint8_t_to_hex_string(guid.Data4[1]);
    guidString += '-';
    guidString += uint8_t_to_hex_string(guid.Data4[2]);
    guidString += uint8_t_to_hex_string(guid.Data4[3]);
    guidString += uint8_t_to_hex_string(guid.Data4[4]);
    guidString += uint8_t_to_hex_string(guid.Data4[5]);
    guidString += uint8_t_to_hex_string(guid.Data4[6]);
    guidString += uint8_t_to_hex_string(guid.Data4[7]);

    return util::to_lower_case(guidString);
}

bool util::is_valid_hash(std::string hash)
{
    std::string valid_chars = "0123456789ABCDEF";

    hash = util::to_upper_case(hash);

    if (hash.length() != 16)
    {
        return false;
    }

    for (int i = 0; i < 16; i++)
    {
        if (valid_chars.find(hash[i]) == std::string::npos)
        {
            return false;
        }
    }

    return true;
}

std::string util::hash_type(uint64_t hash_value)
{
    if (hash_list_loaded)
    {
        std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(hash_value);

        if (it2 != hash_list_hash_map.end())
        {
            return hash_list_hash_file_names.at(it2->second).substr(17);
        }
    }

    return "";
}

uint64_t util::ioi_string_to_hash(std::string ioi_string)
{
    return std::strtoull(generic_function::compute_ioi_hash_string(ioi_string).c_str(), nullptr, 16);
}

std::string util::hash_to_ioi_string(uint64_t hash_value, bool return_hash_if_not_found)
{
    if (hash_list_loaded)
    {
        std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(hash_value);

        if (it2 != hash_list_hash_map.end())
        {
            if (hash_list_hash_strings.at(it2->second) != "")
            {
                if (hash_value == std::strtoull(generic_function::compute_ioi_hash_string(hash_list_hash_strings.at(it2->second)).c_str(), nullptr, 16))
                {
                    return hash_list_hash_strings.at(it2->second);
                }       
            }
        }
    }

    if (return_hash_if_not_found)
    {
        return util::uint64_t_to_hex_string(hash_value);
    }
    else
    {
        return "";
    }
}

std::string util::hash_to_hash_list_string(uint64_t hash_value)
{
    if (hash_list_loaded)
    {
        std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(hash_value);

        if (it2 != hash_list_hash_map.end())
        {
            if (hash_list_hash_strings.at(it2->second) != "")
            {
                return hash_list_hash_strings.at(it2->second);
            }
        }
    }

    return util::uint64_t_to_hex_string(hash_value);
}

bool util::floats_equal(float value1, float value2)
{
    if (value1 == 0.0f)
    {
        if (value2 < 0.00000001f)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (value2 == 0.0f)
    {
        if (value1 < 0.00000001f)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (std::fabs(value1 - value2) <= 0.00000001f)
    {
        return true;
    }
    else
    {
        return (std::fabs(value1 - value2) <= 1E-5 * std::fmax(std::fabs(value1), std::fabs(value2)));
    }
}

std::string util::find_string_between_strings(std::string_view& input_string, std::string string_before, std::string string_after)
{
    std::string output_string = "";

    size_t pos1 = input_string.find(string_before);

    if (pos1 != std::string::npos)
    {
        size_t position_after_string_before = pos1 + string_before.length();

        size_t pos2 = input_string.substr(position_after_string_before).find(string_after);

        if (pos2 != std::string::npos)
        {
            output_string = input_string.substr(position_after_string_before, pos2);
        }
    }

    return output_string;
}

void util::split_string_view(std::string_view& temp_string_view, std::string split_string, std::vector<uint64_t>& split_positions)
{
    bool done = false;

    uint64_t position = 0;

    while (!done)
    {
        size_t pos1 = temp_string_view.substr(position).find(split_string);

        if (pos1 != std::string::npos)
        {
            split_positions.push_back(position + pos1);
        }
        else
        {
            done = true;
        }

        position += pos1 + 1;
    }
}

void util::split_string(std::string& temp_string, std::string split_string, std::vector<uint64_t>& split_positions)
{
    bool done = false;

    uint64_t position = 0;

    while (!done)
    {
        size_t pos1 = temp_string.substr(position).find(split_string);

        if (pos1 != std::string::npos)
        {
            split_positions.push_back(position + pos1);
        }
        else
        {
            done = true;
        }

        position += pos1 + 1;
    }
}

std::string util::string_to_hex_string(std::string input_string)
{
    std::string output_string;

    for (uint64_t k = 0; k < input_string.size(); k++)
    {
        char value[5];
        sprintf_s(value, "\\x%02X", (int)(unsigned char)input_string.data()[k]);
        output_string += value;
    }

    return output_string;
}

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

std::string util::uint64_t_to_hex_string_for_qn(uint64_t bytes8)
{
    std::stringstream ss;
    ss << std::hex << bytes8;
    return ss.str();
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

std::string util::bool_to_string(bool value)
{
    if (value)
    {
        return "True";
    }
    else
    {
        return "False";
    }
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