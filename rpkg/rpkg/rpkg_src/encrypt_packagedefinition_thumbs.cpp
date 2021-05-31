#include "generic_function.h"
#include "crypto.h"
#include "file.h"
#include "global.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

void generic_function::encrypt_packagedefinition_thumbs(std::string& input_path, std::string& output_path)
{
    if (file::path_exists(input_path + ".meta"))
    {
        std::ifstream meta_file = std::ifstream(input_path + ".meta", std::ifstream::binary);

        if (!meta_file.good())
        {
            LOG_AND_EXIT("Error: packagedefinitions.txt / thumbs.dat meta file " + input_path + ".meta" + " could not be read.");
        }

        meta_file.seekg(0, meta_file.end);

        uint64_t packagedefinitions_thumbs_meta_file_size = meta_file.tellg();

        meta_file.seekg(0, meta_file.beg);

        if (packagedefinitions_thumbs_meta_file_size != 20)
        {
            LOG_AND_EXIT("Error: packagedefinitions.txt / thumbs.dat meta file " + input_path + ".meta" + " is corrupt.");
        }

        int packagedefinitions_thumbs_header_size = 20;

        std::vector<char> packagedefinitions_thumbs_header(packagedefinitions_thumbs_header_size, 0);

        meta_file.read(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

        meta_file.close();

        std::ifstream file = std::ifstream(input_path, std::ifstream::binary);

        if (!file.good())
        {
            LOG_AND_EXIT("Error: packagedefinitions.txt / thumbs.dat file " + input_path + " could not be read.");
        }

        file.seekg(0, file.end);

        uint64_t packagedefinitions_thumbs_file_size = file.tellg();

        file.seekg(0, file.beg);

        std::vector<char> input_data(packagedefinitions_thumbs_file_size, 0);

        file.read(input_data.data(), packagedefinitions_thumbs_file_size);

        file.close();

        while (input_data.size() % 8 != 0)
        {
            input_data.push_back(0x0);
        }

        uint32_t zero_pad_length = (uint32_t)(packagedefinitions_thumbs_file_size - input_data.size());

        for (uint64_t i = 0; i < input_data.size() / 8; i++)
        {
            uint32_t data[2];
            std::memcpy(data, &input_data[(uint64_t)i * (uint64_t)8], sizeof(uint32_t));
            std::memcpy(data + 1, &input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

            crypto::xtea_encrypt_packagedefinition_thumbs(data);

            std::memcpy(&input_data[(uint64_t)i * (uint64_t)8], data, sizeof(uint32_t));
            std::memcpy(&input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
        }

        std::string output_file_base_name = file::output_path_append(file::get_base_file_name(input_path), output_path);

        std::ofstream output_file = std::ofstream(output_file_base_name + ".encrypted", std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: Output (packagedefinitions.txt / thumbs.dat).encrypted file " + output_file_base_name + ".encrypted" + " could not be created.");
        }

        output_file.write(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

        output_file.write(input_data.data(), input_data.size());

        LOG("Successfully encrypted " << output_file_base_name << " and saved to " << output_file_base_name + ".encrypted");
    }
    else
    {
        LOG_AND_EXIT("Error: packagedefinitions.txt / thumbs.dat meta file " + input_path + ".meta" + " could not be found.");
    }
}