#include "rpkg_function.h"
#include "global.h"
#include <iostream>
#include <iomanip>
#include <regex>

void rpkg_function::search_hash_data(std::string& search_type, std::string& search, std::vector<char>& search_data, std::string& hash_file_name)
{
    if (search_type == "text")
    {
        uint64_t position = 0;

        bool done_searching = false;

        while (!done_searching)
        {
            if ((position + search.length()) > search_data.size())
            {
                done_searching = true;
                break;
            }

            for (uint64_t k = 0; k < search.length(); k++)
            {
                if (std::tolower(search_data.data()[position + k]) == std::tolower(search[k]))
                {
                    if (k == (search.length() - 1))
                    {
                        LOG("Found text string \"" << search << "\" in hash file/resouce " << hash_file_name << " at offset 0x" << std::hex << std::uppercase << position);

                        bool done_searching_start = false;

                        uint64_t position_start = position;

                        while (!done_searching_start)
                        {
                            if (position_start == 0)
                            {
                                done_searching_start = true;
                                break;
                            }

                            if (search_data.data()[position_start] < 0x20 || search_data.data()[position_start] > 0x7E)
                            {
                                done_searching_start = true;
                                break;
                            }

                            position_start--;
                        }

                        LOG("Text string \"" << search << "\" is contained in string: " << std::string(search_data.data() + position_start + 1));

                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            position++;
        }
    }
    else if (search_type == "hex")
    {
        uint64_t position = 0;

        bool done_searching = false;

        std::vector<char> hex_search;

        if (search.length() % 2 != 0)
        {
            search = "0" + search;
        }

        for (uint64_t i = 0; i < (search.length() / 2); i++)
        {
            hex_search.push_back((char)strtoul(search.substr(i * 2, 2).c_str(), nullptr, 16));
        }

        while (!done_searching)
        {
            if ((position + hex_search.size()) > search_data.size())
            {
                done_searching = true;
                break;
            }

            for (uint64_t k = 0; k < hex_search.size(); k++)
            {
                if (search_data.data()[position + k] == hex_search.at(k))
                {
                    if (k == (hex_search.size() - 1))
                    {
                        LOG_NO_ENDL("Found hex string \"");

                        for (uint64_t k = 0; k < hex_search.size(); k++)
                        {
                            LOG_NO_ENDL(std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)(unsigned char)hex_search.at(k));
                        }

                        LOG("\" in hash file/resouce " << hash_file_name << " at offset 0x" << std::hex << std::uppercase << position);

                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            position++;
        }
    }
    else if (search_type == "regex")
    {
        std::smatch m;
        std::regex re(search);
        std::string data_string;
        data_string.reserve((uint64_t)search_data.size() * (uint64_t)6);

        for (uint64_t k = 0; k < search_data.size(); k++)
        {
            if (search_data.data()[k] >= 0x20 && search_data.data()[k] <= 0x7E)
            {
                data_string += search_data.data()[k];
            }
            else
            {
                char value[5];
                sprintf_s(value, "\\x%02X", (int)(unsigned char)search_data.data()[k]);
                data_string += value;
            }
        }

        uint64_t position = 0;

        while (std::regex_search(data_string, m, re))
        {
            position += m.position();

            LOG("Regex search with regex \"" << search << "\" returned result in hash file/resouce " << hash_file_name);

            for (size_t k = 0; k < m.size(); k++)
            {
                LOG("Match[" << k << "]: " << m[k].str());
            }

            data_string = m.suffix().str();
        }
    }
}