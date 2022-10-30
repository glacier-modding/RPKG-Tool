#include "rpkg_function.h"
#include "global.h"
#include "util.h"
#include "console.h"
#include "file.h"
#include "crypto.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

void rpkg_function::extract_from_rpkg(rpkg_extraction_vars& rpkg_vars)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    LOG("Extract from RPKG file: " << rpkg_vars.input_path);

    std::string message = "Extracting from RPKG: ";

    std::vector<std::string> filters = util::parse_input_filter(rpkg_vars.filter);

    if (filters.size() > 0)
    {
        LOG("Extract: All hash files with filter \"" << rpkg_vars.filter << "\"");
    }
    else
    {
        LOG("Extract: All hash files");
    }

    rpkg_function::import_rpkg(rpkg_vars.input_path, true);

    std::string rpkg_file_name = file::get_base_file_name(rpkg_vars.input_path);

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path != rpkg_vars.input_path)
            continue;
        
        std::string rpkg_file_base_name = file::get_root_file_name(rpkg_file_name);
        std::string hash_file_path = file::output_path_append(rpkg_file_base_name, rpkg_vars.output_path);
        std::string final_path;

        if (!rpkg_vars.search_mode && !rpkg_vars.ores_mode)
        {
            rpkg_function::extract_rpkg_meta(i, hash_file_path);
        }

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        std::vector<bool> extracted;

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            extracted.push_back(false);
        }

        for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((j * (uint64_t)100000) / rpkgs.at(i).hash.size()) % (uint64_t)100 == 0 && j > 0)
            {
                stringstream_length = console::update_console(message, rpkgs.at(i).hash.size(), j, start_time, stringstream_length);
            }

            std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(j).hash_value) + "." + rpkgs.at(i).hash.at(j).hash_resource_type;

            bool found = false;

            uint64_t input_filter_index = 0;

            for (uint64_t z = 0; z < filters.size(); z++)
            {
                std::size_t found_position = hash_file_name.find(filters.at(z));

                if (found_position != std::string::npos && filters.at(z) != "")
                {
                    found = true;

                    input_filter_index = z;

                    break;
                }
            }

            if (found || rpkg_vars.filter.empty())
            {
                if (!filters.empty())
                {
                    extracted.at(input_filter_index) = true;
                }

                uint64_t hash_size;

                if (rpkgs.at(i).hash.at(j).data.lz4ed)
                {
                    hash_size = rpkgs.at(i).hash.at(j).data.header.data_size;

                    if (rpkgs.at(i).hash.at(j).data.xored)
                    {
                        hash_size &= 0x3FFFFFFF;
                    }
                }
                else
                {
                    hash_size = rpkgs.at(i).hash.at(j).data.resource.size_final;
                }

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkg_vars.input_path, std::ifstream::binary);

                if (!file.good())
                {
                    LOG_AND_EXIT("Error: RPKG file " + rpkg_vars.input_path + " could not be read.");
                }

                file.seekg(rpkgs.at(i).hash.at(j).data.header.data_offset, file.beg);
                file.read(input_data.data(), hash_size);

                if (rpkgs.at(i).hash.at(j).data.xored)
                {
                    crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                }

                final_path = hash_file_path + "\\" + rpkgs.at(i).hash.at(j).hash_resource_type;

                if (!rpkg_vars.search_mode)
                {
                    file::create_directories(final_path);
                }

                std::vector<char>* output_data;
                uint64_t output_data_size;

                uint32_t decompressed_size = rpkgs.at(i).hash.at(j).data.resource.size_final;
                std::vector<char> lz4_output_data(decompressed_size, 0);

                if (rpkgs.at(i).hash.at(j).data.lz4ed)
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
                    rpkg_function::extract_hash_meta(i, j, final_path);
                }
            }
        }

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            if (!extracted.at(z))
            {
                LOG(std::endl << "Error: " << filters.at(z) << " is not in " << rpkg_vars.input_path);
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