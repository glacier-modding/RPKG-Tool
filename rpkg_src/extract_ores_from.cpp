#include "rpkg_function.h"
#include "file.h"
#include "util.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <regex>

void rpkg_function::extract_ores_from(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path))
    {
        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        file::create_directories(file::output_path_append("ORES", output_path));

        std::vector<std::string> filters = util::parse_input_filter(filter);

        bool extract_single_hash = false;

        if (filters.size() == 1)
        {
            std::string legal_chars = "0123456789ABCDEF";

            bool is_a_hash = false;

            if (filters.at(0).length() == 16)
            {
                is_a_hash = true;

                for (int i = 0; i < 16; i++)
                {
                    bool is_legal = false;

                    for (int j = 0; j < 16; j++)
                    {
                        if (filters.at(0)[i] == legal_chars[j])
                        {
                            is_legal = true;
                        }
                    }

                    if (!is_legal)
                    {
                        is_a_hash = false;
                    }
                }
            }

            if (is_a_hash)
            {
                extract_single_hash = true;
            }
        }

        timing_string = "Extracting ORES to IOI Paths...";

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        log_output = false;

        uint64_t temp_hash_value = 0x00858D45F5F9E3CA;

        uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_hash_value);

        if (rpkg_index != UINT32_MAX)
        {
            std::map<uint64_t, uint64_t>::iterator it6 = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

            if (it6 != rpkgs.at(rpkg_index).hash_map.end())
            {
                uint64_t hash_index = it6->second;

                std::string hash_file_name = rpkgs.at(rpkg_index).hash.at(hash_index).hash_file_name;

                uint64_t hash_size;

                if (rpkgs.at(rpkg_index).hash.at(hash_index).is_lz4ed == 1)
                {
                    hash_size = rpkgs.at(rpkg_index).hash.at(hash_index).hash_size;

                    if (rpkgs.at(rpkg_index).hash.at(hash_index).is_xored == 1)
                    {
                        hash_size &= 0x3FFFFFFF;
                    }
                }
                else
                {
                    hash_size = rpkgs.at(rpkg_index).hash.at(hash_index).hash_size_final;
                }

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkgs.at(rpkg_index).rpkg_file_path, std::ifstream::binary);

                if (!file.good())
                {
                    LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(rpkg_index).rpkg_file_path + " could not be read.");
                }

                file.seekg(rpkgs.at(rpkg_index).hash.at(hash_index).hash_offset, file.beg);
                file.read(input_data.data(), hash_size);
                file.close();

                if (rpkgs.at(rpkg_index).hash.at(hash_index).is_xored == 1)
                {
                    crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                }

                uint32_t decompressed_size = rpkgs.at(rpkg_index).hash.at(hash_index).hash_size_final;

                std::vector<char> output_data(decompressed_size, 0);

                std::vector<char>* ores_data;

                if (rpkgs.at(rpkg_index).hash.at(hash_index).is_lz4ed)
                {
                    LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                    ores_data = &output_data;
                }
                else
                {
                    ores_data = &input_data;
                }

                std::vector<uint64_t> ores_hash_resource;
                std::vector<std::string> ores_hash_resource_file_path;
                std::vector<std::vector<std::string>> ores_hash_resource_file_path_elements;

                uint32_t ores_hash_resource_file_count = 0;

                uint32_t position = 0x10;

                uint8_t bytes1 = 0;
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;

                std::memcpy(&bytes4, (&ores_data->data()[0] + position), sizeof(bytes4));
                position = bytes4 + 0xC;

                std::memcpy(&ores_hash_resource_file_count, (&ores_data->data()[0] + position), sizeof(bytes4));
                position += 0x4;

                std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
                int stringstream_length = 80;

                for (uint64_t k = 0; k < ores_hash_resource_file_count; k++)
                {
                    if (gui_control == ABORT_CURRENT_TASK)
                    {
                        return;
                    }

                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                    if (time_in_seconds_from_start_time > console_update_rate)
                    {
                        start_time = end_time;

                        if (period_count > 3)
                        {
                            period_count = 0;
                        }

                        std::stringstream ss;

                        ss << "Extracting ORES to IOI Paths" << std::string(period_count, '.');

                        timing_string = ss.str() + std::string((80 - ss.str().length()), ' ');

                        LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                        period_count++;
                    }

                    uint32_t string_offset = 0;
                    uint8_t string_length = 0;
                    uint64_t hash_lsb = 0;
                    uint64_t hash_msb = 0;
                    uint64_t hash = 0;

                    std::memcpy(&string_length, (&ores_data->data()[0] + position), sizeof(bytes1));
                    position += 0x8;

                    std::memcpy(&string_offset, (&ores_data->data()[0] + position), sizeof(bytes4));
                    string_offset += 0x10;
                    position += 0x8;

                    std::memcpy(&hash_lsb, (&ores_data->data()[0] + position), sizeof(bytes4));
                    position += 0x4;

                    std::memcpy(&hash_msb, (&ores_data->data()[0] + position), sizeof(bytes4));
                    position += 0x4;

                    hash = (hash_lsb << 32) | hash_msb;

                    std::string hash_string = util::uint64_t_to_hex_string(hash);

                    if (!extract_single_hash || (extract_single_hash && filter == hash_string))
                    {
                        for (uint64_t i = 0; i < rpkgs.size(); i++)
                        {
                            uint64_t rpkg_index2 = i;

                            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash);

                            if (it != rpkgs.at(i).hash_map.end())
                            {
                                uint64_t hash_index2 = it->second;

                                std::string ores_ioi_path = "";
                                std::string ores_ioi_directory = "";
                                std::string ores_base_name = "";

                                ores_ioi_path = file::output_path_append("ORES\\" + rpkgs.at(rpkg_index2).rpkg_file_name, output_path);

                                ores_ioi_path += "/" + std::string(&ores_data->data()[0] + string_offset);

                                std::replace(ores_ioi_path.begin(), ores_ioi_path.end(), '/', '\\');

                                size_t pos3 = ores_ioi_path.find_last_of("\\");

                                ores_base_name = ores_ioi_path.substr(pos3 + 1);

                                ores_ioi_directory = ores_ioi_path.substr(0, pos3);

                                bool found = false;

                                for (uint64_t z = 0; z < filters.size(); z++)
                                {
                                    std::size_t found_position_hash = rpkgs.at(rpkg_index2).hash.at(hash_index2).hash_file_name.find(filters.at(z));

                                    std::size_t found_position_ores = util::to_upper_case(ores_ioi_path).find(filters.at(z));

                                    if ((found_position_hash != std::string::npos && filters.at(z) != "") || (found_position_ores != std::string::npos && filters.at(z) != ""))
                                    {
                                        found = true;

                                        break;
                                    }
                                }

                                if (found || filter == "")
                                {
                                    file::create_directories(ores_ioi_directory);

                                    rpkg_extraction_vars rpkg_vars;

                                    rpkg_vars.input_path = rpkgs.at(rpkg_index2).rpkg_file_path;
                                    rpkg_vars.filter = hash_string;
                                    rpkg_vars.ores_path = ores_ioi_path;
                                    rpkg_vars.ores_mode = true;

                                    extract_from_rpkg_with_map(rpkg_vars);
                                }
                            }
                        }
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting ORES linked files: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;

        log_output = true;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}