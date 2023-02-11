#include "rpkg_function.h"
#include "global.h"
#include <iostream>
#include <iomanip>
#include <regex>

void rpkg_function::search_hash_data(std::string& search_type, std::string& search, std::vector<char>& search_data,
                                     std::string& hash_file_name) {
    if (search_type == "text") {
        uint64_t position = 0;

        while (true) {
            if ((position + search.length()) > search_data.size()) {
                break;
            }

            for (uint64_t k = 0; k < search.length(); k++) {
                if (std::tolower(search_data[position + k]) != std::tolower(search[k])) {
                    break;
                }

                if (k == (search.length() - 1)) {
                    LOG("Found text string \"" << search << "\" in hash file/resouce " << hash_file_name
                                               << " at offset 0x" << std::hex << std::uppercase << position);

                    uint64_t position_start = position;

                    while (true) {
                        if (position_start == 0) {
                            break;
                        }

                        if (search_data[position_start] < 0x20 || search_data[position_start] > 0x7E) {
                            break;
                        }

                        position_start--;
                    }

                    LOG("Text string \"" << search << "\" is contained in string: "
                                         << std::string(search_data.data() + position_start + 1));

                    break;
                }
            }

            position++;
        }
    } else if (search_type == "hex") {
        uint64_t position = 0;

        std::vector<char> hex_search;

        if (search.length() % 2 != 0) {
            search = "0" + search;
        }

        for (uint64_t i = 0; i < (search.length() / 2); i++) {
            hex_search.push_back((char) strtoul(search.substr(i * 2, 2).c_str(), nullptr, 16));
        }

        while (true) {
            if ((position + hex_search.size()) > search_data.size()) {
                break;
            }

            for (uint64_t k = 0; k < hex_search.size(); k++) {
                if (search_data[position + k] != hex_search.at(k)) {
                    break;
                }

                if (k == (hex_search.size() - 1)) {
                    LOG_NO_ENDL("Found hex string \"");

                    for (char val : hex_search) {
                        LOG_NO_ENDL(std::hex << std::setw(2) << std::setfill('0') << std::uppercase
                                             << (int) (unsigned char) val);
                    }

                    LOG("\" in hash file/resouce " << hash_file_name << " at offset 0x" << std::hex
                                                   << std::uppercase << position);

                    break;
                }
            }

            position++;
        }
    } else if (search_type == "regex") {
        std::smatch m;
        std::regex re(search);
        std::string data_string;
        data_string.reserve((uint64_t) search_data.size() * (uint64_t) 6);

        for (char k : search_data) {
            if (k >= 0x20 && k <= 0x7E) {
                data_string += k;
            } else {
                char value[5];
                sprintf_s(value, "\\x%02X", (int) (unsigned char) k);
                data_string += value;
            }
        }

        uint64_t position = 0;

        while (std::regex_search(data_string, m, re)) {
            position += m.position();

            LOG("Regex search with regex \"" << search << "\" returned result in hash file/resouce " << hash_file_name);

            for (size_t k = 0; k < m.size(); k++) {
                LOG("Match[" << k << "]: " << m[k].str());
            }

            data_string = m.suffix().str();
        }
    }
}