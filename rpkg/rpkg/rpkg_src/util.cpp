#include "util.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <regex>

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
    ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << bytes1;
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

std::string util::uint8_t_to_string(uint8_t bytes1)
{
    std::stringstream ss;
    ss << bytes1;
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
