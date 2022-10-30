#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "thirdparty/md5/md5.h"

extern "C" void MD5Init(struct MD5Context* ctx);
extern "C" void MD5Update(struct MD5Context* ctx, unsigned char* buf, unsigned len);
extern "C" void MD5Final(unsigned char digest[16], struct MD5Context* ctx);
extern "C" void MD5Transform(uint32 buf[4], uint32 in[16]);


void rpkg_function::hash_probe_from_file(std::string& input_path, std::string& filter)
{
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path))
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for hash probe mode does not exist.");
    }

    std::string H1 = "Z:\\HITMAN1\\";
    std::string H2 = R"(Z:\HITMAN2\Runtime\)";
    std::string H3 = R"(C:\Program Files\Epic Games\HITMAN3\Runtime\)";

    rpkg_function::import_rpkg_files_in_folder(H1);
    rpkg_function::import_rpkg_files_in_folder(H2);
    rpkg_function::import_rpkg_files_in_folder(H3);

    std::stringstream ss;

    ss << "Scanning folder: Done";

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    std::ifstream file = std::ifstream(filter, std::ifstream::binary);

    if (!file.good())
    {
        std::cout << "Error: " + filter + " could not be read." << std::endl;
    }

    std::unordered_map<std::string, uint64_t> ioi_string_map;
    std::vector<std::string> ioi_strings;

    std::string line = "";

    while (std::getline(file, line))
    {
        if (line.length() > 4)
        {
            std::string ioi_string = line;

            size_t pos = ioi_string.find_last_of('\r');

            if (pos != std::string::npos)
            {
                ioi_string.erase(pos, 1);
            }

            std::string substring = ioi_string;

            //std::cout << "String: " << ioi_string << std::endl;

            auto it = ioi_string_map.find(substring);

            if (!(it != ioi_string_map.end())) {
                ioi_strings.push_back(ioi_string);
                ioi_string_map[ioi_string] = ioi_strings.size() - 1;
            }
        }
    }

    for (auto& ioi_string : ioi_strings)
    {
        unsigned char signature[16];
        struct MD5Context md5c;

        std::string lowercase;

        for (char i : ioi_string)
        {
            lowercase.push_back(std::tolower(i));
        }

        //LOG(main_data->console_prefix << "Input: " << main_data->input_to_ioi_hash << ", " << lowercase);

        MD5Init(&md5c);
        MD5Update(&md5c, (unsigned char*)lowercase.c_str(), static_cast<unsigned int>(lowercase.length()));
        MD5Final(signature, &md5c);

        std::stringstream ss;

        for (unsigned char m : signature)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(m);
        }

        ss.str(std::string());

        ss << "00";

        for (uint64_t m = 1; m < 8; m++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(signature[m]);
        }

        //std::cout << ss.str() << "," << ioi_strings.at(j) << std::endl;

        int found_count = 0;

        uint64_t hash = std::strtoull(ss.str().c_str(), nullptr, 16);

        if (hash == 0)
            continue;

        bool found = false;

        for (auto& rpkg : rpkgs)
        {
            auto it2 = rpkg.hash_map.find(hash);

            if (it2 != rpkg.hash_map.end())
            {
                found = true;

                found_count++;

                std::cout << util::uint64_t_to_hex_string(rpkg.hash.at(it2->second).hash_value) + "." + rpkg.hash.at(it2->second).hash_resource_type << "," <<
                    ioi_string << "," <<
                    rpkg.rpkg_file_path << std::endl;
            }
        }

        if (found)
        {
            LOG("Hash \"" << ss.str() << "\" was found in " << found_count << " RPKG files.");
        }
        else
        {
            LOG("Hash \"" << ss.str() << "," << ioi_string << "\" was not found in any RPKG files.");
        }
    }
}
