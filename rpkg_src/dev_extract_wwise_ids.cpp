#include "dev_function.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>

void dev_function::dev_extract_wwise_ids(std::string& input_path, std::string& filter, std::string& output_path)
{
    input_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_path);

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "WWEV")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." + rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    std::string current_path = file::output_path_append("WWEV\\" + rpkgs.at(i).rpkg_file_name, output_path);

                    uint64_t hash_size;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).data.header.data_size;

                        if (rpkgs.at(i).hash.at(hash_index).data.xored)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkgs.at(i).hash.at(hash_index).data.header.data_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkgs.at(i).hash.at(hash_index).data.xored)
                    {
                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                    }

                    uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* wwev_data;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

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
                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    std::memcpy(&wwev_file_name_length, &wwev_data->data()[position], sizeof(bytes4));

                    std::vector<char> wwev_meta_data;

                    char hash[8];

                    std::memcpy(&hash, &rpkgs.at(i).hash.at(hash_index).hash_value, 0x8);

                    std::memcpy(&input, &wwev_data->data()[position], (wwev_file_name_length + (uint64_t)0xC));

                    position += 0x4;

                    std::vector<char> wwev_file_name((uint64_t)wwev_file_name_length + (uint64_t)1, 0);
                    wwev_file_name[wwev_file_name_length] = 0;

                    std::memcpy(wwev_file_name.data(), &wwev_data->data()[position], wwev_file_name_length);
                    position += wwev_file_name_length;
                    position += 0x4;

                    std::memcpy(&wwev_file_count, &wwev_data->data()[position], sizeof(bytes4));
                    position += 0x4;

                    std::memcpy(&wwev_file_count_test, &wwev_data->data()[position], sizeof(bytes4));

                    bool found = false;

                    uint64_t input_filter_index = 0;

                    current_path.append("\\");
                    current_path.append(std::string(wwev_file_name.data()));

                    std::string wem_path = current_path + "\\wem";

                    std::string ogg_path = current_path + "\\ogg";

                    std::string final_path = current_path + "\\" + util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." + rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    std::cout << hash_file_name << "," << "[assembly:/sound/wwise/exportedwwisedata/events/unknown/" << wwev_file_name.data() << ".wwiseevent].pc_wwisebank" << std::endl;

                    if (wwev_file_count > 0)
                    {
                        for (uint64_t k = 0; k < wwev_file_count; k++)
                        {
                            std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                            std::cout << "WWEV file " << std::to_string(k) << ": " << std::endl;
                            std::cout << "  - Wwise ID: " << util::uint32_t_to_hex_string(bytes4) << std::endl;

                            position += 0x4;

                            uint32_t wem_size;

                            std::memcpy(&wem_size, &wwev_data->data()[position], sizeof(bytes4));
                            position += 0x4;

                            std::cout << "  - Length: " << util::uint32_t_to_hex_string(wem_size) << std::endl;

                            std::vector<char> wwev_file_data(wem_size, 0);

                            std::memcpy(wwev_file_data.data(), &wwev_data->data()[position], wem_size);
                            position += wem_size;

                            std::string wem_file = wem_path + "\\" + std::to_string(k) + ".wem";
                        }
                    }
                    else
                    {
                        std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                        if (bytes4 == 0)
                        {

                        }
                        else
                        {
                            std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                            uint32_t length = bytes4;

                            position += 0x4;

                            for (uint64_t k = 0; k < length; k++)
                            {
                                position += 0x4;

                                std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                                std::cout << "WWEV Link Type(0) " << std::to_string(k) << ": " << std::endl;
                                std::cout << "  - Wwise ID: " << util::uint32_t_to_hex_string(bytes4) << std::endl;

                                position += 0x4;

                                uint32_t wem_size;

                                std::memcpy(&wem_size, &wwev_data->data()[position], sizeof(bytes4));

                                position += 0x4;

                                std::cout << "  - Length: " << util::uint32_t_to_hex_string(wem_size) << std::endl;

                                position += wem_size;
                            }
                        }
                    }

                    if ((position + 0x4) <= decompressed_size)
                    {
                        std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                        if (bytes4 == 0)
                        {

                        }
                        else
                        {
                            std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                            uint32_t length = bytes4;

                            position += 0x4;

                            for (uint64_t k = 0; k < length; k++)
                            {
                                position += 0x4;

                                std::memcpy(&bytes4, &wwev_data->data()[position], 0x4);

                                std::cout << "WWEV Link Type(1) " << std::to_string(k) << ": " << std::endl;
                                std::cout << "  - Wwise ID: " << util::uint32_t_to_hex_string(bytes4) << std::endl;

                                position += 0x4;

                                uint32_t wem_size;

                                std::memcpy(&wem_size, &wwev_data->data()[position], sizeof(bytes4));

                                position += 0x4;

                                std::cout << "  - Length: " << util::uint32_t_to_hex_string(wem_size) << std::endl;

                                position += wem_size;
                            }
                        }
                    }
                }
            }
        }
    }
}