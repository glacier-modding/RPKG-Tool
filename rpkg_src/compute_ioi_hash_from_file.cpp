#include "generic_function.h"
#include "global.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "thirdparty/md5/md5.h"

extern "C" void MD5Init(MD5Context* ctx);
extern "C" void MD5Update(MD5Context* ctx, unsigned char* buf, unsigned len);
extern "C" void MD5Final(unsigned char digest[16], MD5Context* ctx);
extern "C" void MD5Transform(uint32 buf[4], uint32 in[16]);

void generic_function::compute_ioi_hash_from_file(std::string& input_path)
{
    std::ifstream file = std::ifstream(input_path, std::ifstream::binary);
    
    if (!file.good())
    {
        std::cout << "Error: " + input_path + " could not be read." << std::endl;
    }

    std::map<std::string, uint64_t> ioi_string_map;
    std::vector<std::string> ioi_strings;

    std::string line = "";

    while (std::getline(file, line))
    {
        if (line.length() > 4)
        {
            std::string ioi_string = line.substr(0, line.length() - 33);

            std::string substring = ioi_string;

            //std::cout << "String: " << ioi_string << std::endl;

            if (auto it = ioi_string_map.find(substring); it == ioi_string_map.end())
            {
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
            lowercase += std::tolower(i);
        }

        //LOG(main_data->console_prefix << "Input: " << main_data->input_to_ioi_hash << ", " << lowercase);

        MD5Init(&md5c);
        MD5Update(&md5c, (unsigned char*)lowercase.c_str(), static_cast<unsigned>(lowercase.length()));
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

        std::cout << ss.str() << "," << ioi_string << std::endl;
    }
}
