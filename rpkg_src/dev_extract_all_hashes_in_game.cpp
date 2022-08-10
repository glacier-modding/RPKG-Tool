#include "rpkg_function.h"
#include "global.h"
#include "util.h"
#include "dev_function.h"
#include "console.h"
#include "file.h"
#include "crypto.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

void dev_function::dev_extract_all_hashes_in_game(rpkg_extraction_vars& rpkg_vars)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    LOG("Extract from RPKG files in path: " << rpkg_vars.input_path);

    std::string message = "Extracting from RPKG: ";

    std::string input_rpkg_folder_path = file::parse_input_folder_path(rpkg_vars.input_path);

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    uint64_t hash_total = 0;
    uint64_t hash_count = 0;

    std::unordered_map<uint64_t, uint64_t> hashes_extracted;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        hash_total += rpkgs.at(i).hash.size();
    }

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::string hash_file_path = file::output_path_append("", rpkg_vars.output_path);
        std::string final_path;

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        LOG("Extracting hashes via the latest hash function fed by the hashes in RPKG: " << rpkgs.at(i).rpkg_file_path);

        for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
        {
            std::unordered_map<uint64_t, uint64_t>::iterator it = hashes_extracted.find(rpkgs.at(i).hash.at(j).hash_value);

            if (it == hashes_extracted.end())
            {
                hashes_extracted[rpkgs.at(i).hash.at(j).hash_value] = hashes_extracted.size();

                uint32_t rpkg_index = rpkg_function::get_latest_hash(rpkgs.at(i).hash.at(j).hash_value);

                if (rpkg_index == UINT32_MAX)
                {
                    LOG_AND_RETURN("Error: The input entity (TEMP) hash " + util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(j).hash_value) + "." + rpkgs.at(i).hash.at(j).hash_resource_type + " could not be found in any RPKGs.");
                }

                std::unordered_map<uint64_t, uint64_t>::iterator ith = rpkgs.at(rpkg_index).hash_map.find(rpkgs.at(i).hash.at(j).hash_value);

                if (ith != rpkgs.at(rpkg_index).hash_map.end())
                {
                    if (((hash_count * (uint64_t)100000) / hash_total) % (uint64_t)100 == 0 && hash_count > 0)
                    {
                        stringstream_length = console::update_console(message, hash_total, hash_count, start_time, stringstream_length);
                    }

                    hash_count++;

                    std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(ith->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(ith->second).hash_resource_type;

                    uint64_t hash_size;

                    if (rpkgs.at(rpkg_index).hash.at(ith->second).data.lz4ed)
                    {
                        hash_size = rpkgs.at(rpkg_index).hash.at(ith->second).data.header.data_size;

                        if (rpkgs.at(rpkg_index).hash.at(ith->second).data.xored)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkgs.at(rpkg_index).hash.at(ith->second).data.resource.size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(rpkg_index).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(rpkg_index).rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkgs.at(rpkg_index).hash.at(ith->second).data.header.data_offset, file.beg);
                    file.read(input_data.data(), hash_size);

                    if (rpkgs.at(rpkg_index).hash.at(ith->second).data.xored)
                    {
                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                    }

                    final_path = hash_file_path + "\\" + rpkgs.at(rpkg_index).hash.at(ith->second).hash_resource_type;

                    if (!rpkg_vars.search_mode)
                    {
                        file::create_directories(final_path);
                    }

                    std::vector<char>* output_data;
                    uint64_t output_data_size;

                    uint32_t decompressed_size = rpkgs.at(rpkg_index).hash.at(ith->second).data.resource.size_final;
                    std::vector<char> lz4_output_data(decompressed_size, 0);

                    if (rpkgs.at(rpkg_index).hash.at(ith->second).data.lz4ed)
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
                        rpkg_function::extract_hash_meta(rpkg_index, ith->second, final_path);
                    }
                }
            }
        }
    }

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    std::stringstream ss;

    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = (uint32_t)100;

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;
}