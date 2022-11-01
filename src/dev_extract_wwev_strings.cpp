#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <fstream>

void dev_function::dev_extract_wwev_strings(std::string& output_path)
{
    for (auto& rpkg : rpkgs)
    {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++)
        {
            if (rpkg.hash_resource_types.at(r) != "WWEV")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++)
            {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                    rpkg.hash.at(hash_index).hash_resource_type;

                std::string current_path = file::output_path_append("WWEV\\" + rpkg.rpkg_file_name, output_path);

                uint64_t hash_size;

                if (rpkg.hash.at(hash_index).data.lz4ed)
                {
                    hash_size = rpkg.hash.at(hash_index).data.header.data_size;

                    if (rpkg.hash.at(hash_index).data.xored)
                    {
                        hash_size &= 0x3FFFFFFF;
                    }
                }
                else
                {
                    hash_size = rpkg.hash.at(hash_index).data.resource.size_final;
                }

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkg.rpkg_file_path, std::ifstream::binary);

                if (!file.good())
                {
                    LOG_AND_EXIT("Error: RPKG file " + rpkg.rpkg_file_path + " could not be read.");
                }

                file.seekg(rpkg.hash.at(hash_index).data.header.data_offset, file.beg);
                file.read(input_data.data(), hash_size);
                file.close();

                if (rpkg.hash.at(hash_index).data.xored)
                {
                    crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                }

                uint32_t decompressed_size = rpkg.hash.at(hash_index).data.resource.size_final;

                std::vector<char> output_data(decompressed_size, 0);

                std::vector<char>* wwev_data;

                if (rpkg.hash.at(hash_index).data.lz4ed)
                {
                    LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size), decompressed_size);

                    wwev_data = &output_data;
                }
                else
                {
                    wwev_data = &input_data;
                }

                uint32_t wwev_file_name_length = 0;
                uint32_t wwev_file_count = 0;
                uint32_t wwev_file_count_test = 0;

                uint32_t position = 0;

                char input[1024];
                uint32_t bytes4 = 0;

                std::memcpy(&wwev_file_name_length, &wwev_data->data()[position], sizeof(bytes4));

                std::vector<char> wwev_meta_data;

                char hash[8];

                std::memcpy(&hash, &rpkg.hash.at(hash_index).hash_value, 0x8);

                for (char& k : hash)
                {
                    wwev_meta_data.push_back(k);
                }

                std::memcpy(&input, &wwev_data->data()[position], (wwev_file_name_length + static_cast<uint64_t>(0xC)));
                for (uint64_t k = 0; k < (wwev_file_name_length + static_cast<uint64_t>(0xC)); k++)
                {
                    wwev_meta_data.push_back(input[k]);
                }

                position += 0x4;

                std::vector<char> wwev_file_name(static_cast<uint64_t>(wwev_file_name_length) + static_cast<uint64_t>(1), 0);
                wwev_file_name[wwev_file_name_length] = 0;

                std::memcpy(wwev_file_name.data(), &wwev_data->data()[position], wwev_file_name_length);
                position += wwev_file_name_length;
                position += 0x4;

                std::memcpy(&wwev_file_count, &wwev_data->data()[position], sizeof(bytes4));
                position += 0x4;

                std::memcpy(&wwev_file_count_test, &wwev_data->data()[position], sizeof(bytes4));

                current_path.append("\\");
                current_path.append(std::string(wwev_file_name.data()));

                std::string wem_path = current_path + "\\wem";

                std::string ogg_path = current_path + "\\ogg";

                std::string final_path = current_path + "\\" + util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                    rpkg.hash.at(hash_index).hash_resource_type;

                std::cout << hash_file_name << "," << "[assembly:/sound/wwise/exportedwwisedata/events/unknown/" << wwev_file_name.data() << ".wwiseevent].pc_wwisebank" << std::endl;
            }
        }
    }
}