#include "rpkg_function.h"
#include "global.h"
#include "file.h"
#include "crypto.h"
#include "thirdparty/lz4/lz4.h"
#include <fstream>
#include <regex>

void rpkg_function::extract_from_rpkg_with_map(rpkg_extraction_vars& rpkg_vars)
{
    rpkg_function::import_rpkg(rpkg_vars.input_path, true);

    std::string rpkg_file_name = file::get_base_file_name(rpkg_vars.input_path);

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_vars.input_path)
        {
            std::string rpkg_file_base_name = file::get_root_file_name(rpkg_file_name);
            std::string hash_file_path = file::output_path_append(rpkg_file_base_name, rpkg_vars.output_path);
            std::string final_path;

            if (!rpkg_vars.search_mode && !rpkg_vars.ores_mode)
            {
                rpkg_function::extract_rpkg_meta(i, hash_file_path);
            }

            uint64_t hash_value = std::strtoull(rpkg_vars.filter.c_str(), nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

            if (it != rpkgs.at(i).hash_map.end())
            {
                std::string hash_file_name = rpkgs.at(i).hash.at(it->second).hash_file_name;

                uint64_t hash_size;

                if (rpkgs.at(i).hash.at(it->second).is_lz4ed == 1)
                {
                    hash_size = rpkgs.at(i).hash.at(it->second).hash_size;

                    if (rpkgs.at(i).hash.at(it->second).is_xored == 1)
                    {
                        hash_size &= 0x3FFFFFFF;
                    }
                }
                else
                {
                    hash_size = rpkgs.at(i).hash.at(it->second).hash_size_final;
                }

                //if (rpkg_vars.ores_mode)
                //{
                //    std::ofstream ores_meta_hash_file = std::ofstream(rpkg_vars.ores_path + "_" + hash_file_name, std::ofstream::binary);

                //    ores_meta_hash_file.close();
                //}

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkg_vars.input_path, std::ifstream::binary);

                if (!file.good())
                {
                    LOG_AND_EXIT("Error: RPKG file " + rpkg_vars.input_path + " could not be read.");
                }

                file.seekg(rpkgs.at(i).hash.at(it->second).hash_offset, file.beg);
                file.read(input_data.data(), hash_size);

                if (rpkgs.at(i).hash.at(it->second).is_xored == 1)
                {
                    crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                }

                final_path = hash_file_path + "\\" + rpkgs.at(i).hash.at(it->second).hash_resource_type;

                if (!rpkg_vars.search_mode && !rpkg_vars.ores_mode)
                {
                    file::create_directories(final_path);
                }

                std::vector<char>* output_data;
                uint64_t output_data_size;

                uint32_t decompressed_size = rpkgs.at(i).hash.at(it->second).hash_size_final;
                std::vector<char> lz4_output_data(decompressed_size, 0);

                if (rpkgs.at(i).hash.at(it->second).is_lz4ed)
                {
                    LZ4_decompress_safe(input_data.data(), lz4_output_data.data(), (int)hash_size, decompressed_size);

                    output_data = &lz4_output_data;
                    output_data_size = decompressed_size;
                }
                else
                {
                    output_data = &input_data;
                    output_data_size = hash_size;
                }

                final_path += "\\" + hash_file_name;

                if (!rpkg_vars.search_mode)
                {
                    if (rpkg_vars.ores_mode)
                    {
                        final_path = rpkg_vars.ores_path;
                    }

                    std::ofstream output_file = std::ofstream(final_path, std::ofstream::binary);

                    if (!output_file.good())
                    {
                        LOG_AND_EXIT("Error: Hash file " + final_path + " could not be created.");
                    }

                    output_file.write((*output_data).data(), output_data_size);

                    output_file.close();
                }

                if (rpkg_vars.search_mode)
                {
                    rpkg_function::search_hash_data(rpkg_vars.search_type, rpkg_vars.search, *output_data, hash_file_name);
                }

                if (!rpkg_vars.search_mode && !rpkg_vars.ores_mode)
                {
                    rpkg_function::extract_hash_meta(i, it->second, final_path);
                }
            }
        }
    }
}