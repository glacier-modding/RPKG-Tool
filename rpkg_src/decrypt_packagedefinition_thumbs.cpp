#include "generic_function.h"
#include "crypto.h"
#include "file.h"
#include "global.h"
#include "crc32.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

void generic_function::decrypt_packagedefinition_thumbs(std::string& input_path, std::string& output_path)
{
    std::ifstream file = std::ifstream(input_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: packagedefinitions.txt / thumbs.dat file " + input_path + " could not be read.");
    }

    file.seekg(0, file.end);

    uint64_t packagedefinitions_thumbs_file_size = file.tellg();

    int packagedefinitions_thumbs_header_size = 16;

	packagedefinitions_thumbs_file_size -= packagedefinitions_thumbs_header_size;
	packagedefinitions_thumbs_file_size -= 4; // -4 to skip the checksum

    file.seekg(0, file.beg);

    std::vector<char> packagedefinitions_thumbs_header(packagedefinitions_thumbs_header_size, 0);

    file.read(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

	std::vector<char> checksum_raw(4, 0);

	file.read(checksum_raw.data(), 4);

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
        std::memcpy(&data[0], &input_data[(uint64_t)i * (uint64_t)8], sizeof(uint32_t));
        std::memcpy(&data[1], &input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

        crypto::xtea_decrypt_packagedefinition_thumbs(data);

        std::memcpy(&input_data[(uint64_t)i * (uint64_t)8], &data[0], sizeof(uint32_t));
        std::memcpy(&input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], &data[1], sizeof(uint32_t));
    }

    uint64_t last_zero_position = input_data.size();

    if (input_data.size() > 0)
    {
        for (uint64_t i = (input_data.size() - 1); i > 0; i--)
        {
            if (input_data.at(i) != 0)
            {
                break;
            }
            else
            {
                last_zero_position--;
            }
        }
    }

	uint32_t table[256];
	crc32::generate_table(table);
	uint32_t crc = crc32::update(table, 0, input_data.data(), last_zero_position);

	uint32_t old_crc = 0;
	memcpy(&old_crc, &checksum_raw.data()[0], sizeof(old_crc));

	std::string output_file_base_name = file::output_path_append(file::get_base_file_name(input_path), output_path);

	if (crc != old_crc) {
		LOG("Could not decrypt " + output_file_base_name + "!\nReason: Checksum mismatch! Exiting...");
	}

    std::ofstream output_file = std::ofstream(output_file_base_name + ".decrypted", std::ofstream::binary);

    if (!output_file.good())
    {
        LOG_AND_EXIT("Error: Output (packagedefinitions.txt / thumbs.dat).decrypted file " + output_file_base_name + ".decrypted" + " could not be created.");
    }

    output_file.write(input_data.data(), last_zero_position);

    LOG("Successfully decrypted " << output_file_base_name << " and saved to " << output_file_base_name + ".decrypted" << std::endl);
}