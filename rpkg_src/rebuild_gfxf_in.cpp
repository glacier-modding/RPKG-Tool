#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void rpkg_function::rebuild_gfxf_in(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        std::vector<std::string> gfxf_folders;

        std::string input_folder = input_rpkg_folder_path;

        if (input_folder.substr(input_folder.length() - 1, 1) == "\\")
        {
            input_folder = input_folder.substr(0, input_folder.length() - 1);
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path))
        {
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

                ss << "Scanning folder" << std::string(period_count, '.');

                timing_string = ss.str();

                LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                period_count++;
            }

            if (std::filesystem::is_directory(entry.path().string()))
            {
                input_folder = entry.path().string();

                if (input_folder.substr(input_folder.length() - 1, 1) == "\\")
                {
                    input_folder = input_folder.substr(0, input_folder.length() - 1);
                }

                bool gfxf_folder_found = false;

                for (uint64_t i = 0; i < gfxf_folders.size(); i++)
                {
                    if (input_folder == gfxf_folders.at(i))
                    {
                        gfxf_folder_found = true;
                    }
                }

                if (!gfxf_folder_found)
                {
                    gfxf_folders.push_back(input_folder);
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        std::string message = "Rebuilding GFXF files from: ";

        timing_string = message;

        for (uint64_t i = 0; i < gfxf_folders.size(); i++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((i * (uint64_t)100000) / (uint64_t)gfxf_folders.size()) % (uint64_t)10 == 0 && i > 0)
            {
                stringstream_length = console::update_console(message, gfxf_folders.size(), i, start_time, stringstream_length);
            }

            std::vector<std::string> gfx_file_names;
            std::vector<std::string> gfx_file_paths;
            std::vector<std::string> dds_tga_file_names;
            std::vector<std::string> dds_tga_file_paths;
            std::vector<uint64_t> gfxf_hashes;
            std::vector<std::string> gfxf_hash_strings;
            std::vector<std::string> gfxf_file_names;

            for (const auto& entry : std::filesystem::directory_iterator(gfxf_folders.at(i)))
            {
                if (std::filesystem::is_regular_file(entry.path().string()))
                {
                    std::size_t pos = entry.path().string().find_last_of("\\/");

                    std::string file_name = "";
                    std::string hash_file_name = "";
                    std::string hash_string = "";
                    std::string resource_type = "";

                    if (pos != std::string::npos)
                    {
                        file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    }
                    else
                    {
                        file_name = entry.path().string();
                    }

                    if (file_name.length() > 4)
                    {
                        if (util::to_upper_case(file_name.substr((file_name.length() - 4), 4)) == ".GFX")
                        {
                            hash_file_name = util::to_upper_case(file_name.substr(0, (file_name.length() - 4)));

                            pos = hash_file_name.find_last_of(".");

                            if (pos != std::string::npos)
                            {
                                hash_string = hash_file_name.substr(0, pos);
                                resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                            }

                            bool is_gfx_hash_file = true;
                            bool gfx_hash_meta = true;

                            if (hash_string.length() != 16)
                            {
                                is_gfx_hash_file = false;
                            }

                            if (resource_type != "GFXF")
                            {
                                is_gfx_hash_file = false;
                            }

                            if (!file::path_exists(gfxf_folders.at(i) + "\\meta"))
                            {
                                gfx_hash_meta = false;

                                LOG("GFX file " + entry.path().string() + " found but meta file is missing, can't rebuild.");
                            }

                            if (is_gfx_hash_file && gfx_hash_meta)
                            {
                                gfx_file_paths.push_back(entry.path().string());
                                gfx_file_names.push_back(file_name);
                                gfxf_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                                gfxf_file_names.push_back(util::to_upper_case(hash_file_name));
                                gfxf_hash_strings.push_back(util::to_upper_case(hash_string));
                            }
                        }
                        else if (util::to_upper_case(file_name.substr((file_name.length() - 4), 4)) == ".DDS")
                        {
                            dds_tga_file_paths.push_back(entry.path().string());
                            dds_tga_file_names.push_back(file_name);
                        }
                        else if (util::to_upper_case(file_name.substr((file_name.length() - 4), 4)) == ".TGA")
                        {
                            dds_tga_file_paths.push_back(entry.path().string());
                            dds_tga_file_names.push_back(file_name);
                        }
                    }
                }
            }

            if (gfx_file_paths.size() == 1)
            {
                std::ifstream meta_file = std::ifstream(gfxf_folders.at(i) + "\\meta", std::ifstream::binary);

                if (!meta_file.good())
                {
                    LOG_AND_EXIT("Error: meta file " + gfxf_folders.at(i) + "\\meta" + " could not be opened.");
                }

                meta_file.seekg(0, meta_file.end);

                uint64_t meta_file_size = meta_file.tellg();

                meta_file.seekg(0, meta_file.beg);

                std::vector<char> meta_data(meta_file_size, 0);
                std::vector<char> meta_data_footer;
                std::vector<char> gfxf_file_header_data;
                std::vector<char> gfxf_file_data;
                std::vector<char> gfxf_dds_tga_data;

                uint64_t gfx_file_size = 0;
                uint32_t gfxf_dds_tga_file_count = 0;
                std::vector<std::string> meta_dds_tga_file_names;
                std::vector<uint32_t> meta_dds_tga_file_name_lengths;

                char input[1024];
                char char4[4] = "";
                uint8_t bytes1 = 0;
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;
                uint64_t position = 0;

                meta_file.read(meta_data.data(), meta_file_size);

                meta_file.close();

                std::memcpy(&gfxf_dds_tga_file_count, &meta_data.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                for (uint64_t j = 0; j < gfxf_dds_tga_file_count; j++)
                {
                    std::memcpy(&bytes4, &meta_data.data()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    meta_dds_tga_file_name_lengths.push_back(bytes4);

                    std::memcpy(&input, &meta_data.data()[position], bytes4);
                    input[bytes4] = 0;
                    position += bytes4;

                    meta_dds_tga_file_names.push_back(std::string(input));
                }

                //if ((meta_file_size - position) > 0)
                //{
                    //for (uint64_t k = 0; k < (meta_file_size - position); k++)
                    //{
                        //meta_data_footer.push_back(meta_data.data()[position + k]);
                    //}
                //}

                bool all_dds_tga_exist = true;

                for (uint64_t j = 0; j < meta_dds_tga_file_names.size(); j++)
                {
                    //std::cout << gfxf_folders.at(i) + "\\" + meta_dds_tga_file_names.at(j) << std::endl;

                    if (!file::path_exists(gfxf_folders.at(i) + "\\" + meta_dds_tga_file_names.at(j)))
                    {
                        all_dds_tga_exist = false;
                    }
                }

                if (!all_dds_tga_exist || (gfxf_dds_tga_file_count != meta_dds_tga_file_names.size()))
                {
                    LOG("GFXF file " + gfxf_file_names.back() + " has a dds/tga file mismatch.");
                }

                std::string gfxf_hash_file_name = gfxf_hash_strings.back() + ".GFXF";

                std::ifstream gfx_file = std::ifstream(gfx_file_paths.back(), std::ifstream::binary);

                if (!gfx_file.good())
                {
                    LOG_AND_EXIT("Error: GFX file " + gfx_file_paths.back() + " could not be created.");
                }

                gfx_file.seekg(0, gfx_file.end);
                gfx_file_size = (uint64_t)gfx_file.tellg();
                gfx_file.seekg(0, gfx_file.beg);

                std::vector<char> gfx_file_data(gfx_file_size, 0);

                gfx_file.read(gfx_file_data.data(), gfx_file_size);

                gfxf_file_header_data.push_back(0x42);
                gfxf_file_header_data.push_back(0x49);
                gfxf_file_header_data.push_back(0x4E);
                gfxf_file_header_data.push_back(0x31);
                gfxf_file_header_data.push_back(0x0);
                gfxf_file_header_data.push_back(0x8);
                gfxf_file_header_data.push_back(0x1);
                gfxf_file_header_data.push_back(0x0);

                if (dds_tga_file_paths.size() == 0)
                {
                    uint32_t header_value = gfx_file_size + 0x44;

                    std::memcpy(&char4, &header_value, sizeof(uint32_t));

                    gfxf_file_header_data.push_back(char4[3]);
                    gfxf_file_header_data.push_back(char4[2]);
                    gfxf_file_header_data.push_back(char4[1]);
                    gfxf_file_header_data.push_back(char4[0]);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x44);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);

                    std::memcpy(&input, &gfx_file_size, sizeof(uint32_t));

                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        gfxf_file_header_data.push_back(input[k]);
                    }

                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);
                    gfxf_file_header_data.push_back(0x0);

                    for (uint64_t k = 0; k < 0x30; k++)
                    {
                        gfxf_file_header_data.push_back((char)0xFF);
                    }

                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        gfxf_file_header_data.push_back(input[k]);
                    }

                    unsigned char footer_header[] = { 0xED, 0xA5, 0xEB, 0x12 };
                    unsigned char footer_serial_data[] = { 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00 };
                    uint32_t footer_serial_data_count = 7;

                    uint32_t footer_total_size = footer_serial_data_count * (uint32_t)0x4 + (uint32_t)0x4;

                    for (uint64_t k = 0; k < sizeof(footer_header); k++)
                    {
                        meta_data_footer.push_back(footer_header[k]);
                    }

                    std::memcpy(&input, &footer_total_size, sizeof(uint32_t));

                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data_footer.push_back(input[k]);
                    }

                    std::memcpy(&input, &footer_serial_data_count, sizeof(uint32_t));

                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data_footer.push_back(input[k]);
                    }

                    for (uint64_t k = 0; k < sizeof(footer_serial_data); k++)
                    {
                        meta_data_footer.push_back(footer_serial_data[k]);
                    }

                    std::string current_path = gfxf_folders.at(i) + "\\GFXF.rebuilt";

                    file::create_directories(current_path);

                    std::ofstream gfxf_file = std::ofstream(current_path + "\\" + gfxf_hash_file_name, std::ofstream::binary);

                    if (!gfxf_file.good())
                    {
                        LOG_AND_EXIT("Error: GFXF file " + current_path + "\\" + gfxf_hash_file_name + " could not be created.");
                    }

                    gfxf_file.write(gfxf_file_header_data.data(), gfxf_file_header_data.size());

                    gfxf_file.write(gfx_file_data.data(), gfx_file_data.size());

                    gfxf_file.write(meta_data_footer.data(), meta_data_footer.size());
                }
                else
                {
                    position = 0;
                    uint32_t header_value = 0;
                    uint32_t dds_tga_names_offset_start = 0;
                    uint32_t dds_tga_names_offset_end = 0;
                    uint32_t dds_tga_data_offset_start = 0;
                    uint32_t dds_tga_data_offset_end = 0;
                    std::vector<uint32_t> dds_tga_name_offsets;
                    std::vector<uint32_t> dds_tga_data_1_offsets;
                    std::vector<uint32_t> dds_tga_data_2_offsets;
                    std::vector<uint32_t> dds_tga_data_3_offsets;

                    if (all_dds_tga_exist && (gfxf_dds_tga_file_count == meta_dds_tga_file_names.size()))
                    {
                        position = gfx_file_size + 0x44;

                        while (((position & 0xF) != 0x4) && ((position & 0xF) != 0xC))
                        {
                            gfxf_file_data.push_back(0x0);
                            position++;
                        }

                        dds_tga_names_offset_start = position;

                        std::memcpy(&input, &gfxf_dds_tga_file_count, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_data.push_back(input[k]);
                        }

                        position += 0x4;

                        uint32_t offset = (uint64_t)dds_tga_names_offset_start + ((uint64_t)gfxf_dds_tga_file_count * (uint64_t)0x10) + (uint64_t)0x8;

                        for (uint64_t j = 0; j < meta_dds_tga_file_names.size(); j++)
                        {
                            uint32_t dds_tga_file_length1 = meta_dds_tga_file_names.at(j).length();
                            uint32_t dds_tga_file_length2 = meta_dds_tga_file_names.at(j).length();

                            dds_tga_file_length1 |= 0x40000000;

                            std::memcpy(&input, &dds_tga_file_length1, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            dds_tga_name_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            offset += 0x4;

                            offset += 0x1;

                            offset += dds_tga_file_length2;

                            while (offset % 4 != 0)
                            {
                                offset++;
                            }
                        }

                        dds_tga_names_offset_end = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        for (uint64_t j = 0; j < meta_dds_tga_file_names.size(); j++)
                        {
                            uint32_t dds_tga_file_length1 = meta_dds_tga_file_names.at(j).length();
                            dds_tga_file_length1++;

                            std::memcpy(&input, &dds_tga_file_length1, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            for (uint64_t k = 0; k < meta_dds_tga_file_names.at(j).length(); k++)
                            {
                                gfxf_file_data.push_back(meta_dds_tga_file_names.at(j)[k]);
                            }

                            position += (uint64_t)meta_dds_tga_file_names.at(j).length();

                            gfxf_file_data.push_back(0x0);

                            position++;

                            while (dds_tga_file_length1 % 4 != 0)
                            {
                                gfxf_file_data.push_back(0x0);
                                dds_tga_file_length1++;
                                position++;
                            }
                        }

                        std::vector<std::vector<char>> dds_tga_file_data;

                        for (uint64_t j = 0; j < meta_dds_tga_file_names.size(); j++)
                        {
                            uint32_t dds_tga_file_size = 0;

                            std::ifstream dds_tga_file = std::ifstream(gfxf_folders.at(i) + "\\" + meta_dds_tga_file_names.at(j), std::ifstream::binary);

                            if (!dds_tga_file.good())
                            {
                                LOG_AND_EXIT("Error: dds/tga file " + gfxf_folders.at(i) + "\\" + meta_dds_tga_file_names.at(j) + " could not be created.");
                            }

                            dds_tga_file.seekg(0, dds_tga_file.end);
                            dds_tga_file_size = (uint32_t)dds_tga_file.tellg();
                            dds_tga_file.seekg(0, dds_tga_file.beg);

                            std::vector<char> temp_dds_tga_file_data(dds_tga_file_size, 0);

                            dds_tga_file.read(temp_dds_tga_file_data.data(), dds_tga_file_size);

                            dds_tga_file.close();

                            dds_tga_file_data.push_back(temp_dds_tga_file_data);
                        }

                        std::memcpy(&input, &gfxf_dds_tga_file_count, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_data.push_back(input[k]);
                        }

                        position += 0x4;

                        dds_tga_data_offset_start = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        offset = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size() + ((uint64_t)gfxf_dds_tga_file_count * (uint64_t)0x18) + (uint64_t)0x4;

                        for (uint64_t j = 0; j < dds_tga_file_data.size(); j++)
                        {
                            uint32_t dds_tga_file_length = dds_tga_file_data.at(j).size();

                            dds_tga_data_1_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            offset += dds_tga_file_length;

                            dds_tga_data_2_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            dds_tga_data_3_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            offset += 0x4;
                        }

                        dds_tga_data_offset_end = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        for (uint64_t j = 0; j < dds_tga_file_data.size(); j++)
                        {
                            uint32_t dds_tga_file_length = dds_tga_file_data.at(j).size();

                            std::memcpy(&input, &dds_tga_file_length, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            for (uint64_t k = 0; k < dds_tga_file_length; k++)
                            {
                                gfxf_file_data.push_back(dds_tga_file_data.at(j)[k]);
                            }
                        }

                        uint32_t header_value = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        std::memcpy(&input, &header_value, sizeof(uint32_t));

                        gfxf_file_header_data.push_back(input[3]);
                        gfxf_file_header_data.push_back(input[2]);
                        gfxf_file_header_data.push_back(input[1]);
                        gfxf_file_header_data.push_back(input[0]);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x44);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &gfx_file_size, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        dds_tga_names_offset_start += 0x4;

                        std::memcpy(&input, &dds_tga_names_offset_start, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_names_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_names_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_data_offset_start, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_data_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_data_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &gfx_file_size, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        unsigned char footer_header[] = { 0xED, 0xA5, 0xEB, 0x12 };
                        unsigned char footer_serial_data[] = { 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00 };
                        uint32_t footer_serial_data_count = 7;

                        uint32_t footer_data_count = (uint32_t)meta_dds_tga_file_names.size() * (uint32_t)0x4 + footer_serial_data_count;

                        uint32_t footer_total_size = footer_data_count * (uint32_t)0x4 + (uint32_t)0x4;

                        for (uint64_t k = 0; k < sizeof(footer_header); k++)
                        {
                            meta_data_footer.push_back(footer_header[k]);
                        }

                        std::memcpy(&input, &footer_total_size, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data_footer.push_back(input[k]);
                        }

                        std::memcpy(&input, &footer_data_count, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data_footer.push_back(input[k]);
                        }

                        for (uint64_t k = 0; k < sizeof(footer_serial_data); k++)
                        {
                            meta_data_footer.push_back(footer_serial_data[k]);
                        }

                        for (uint64_t d = 0; d < dds_tga_name_offsets.size(); d++)
                        {
                            std::memcpy(&input, &dds_tga_name_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }
                        }

                        for (uint64_t d = 0; d < dds_tga_data_1_offsets.size(); d++)
                        {
                            std::memcpy(&input, &dds_tga_data_1_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }

                            std::memcpy(&input, &dds_tga_data_2_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }

                            std::memcpy(&input, &dds_tga_data_3_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }
                        }

                        std::string current_path = gfxf_folders.at(i) + "\\GFXF.rebuilt";

                        file::create_directories(current_path);

                        std::ofstream gfxf_file = std::ofstream(current_path + "\\" + gfxf_hash_file_name, std::ofstream::binary);

                        if (!gfxf_file.good())
                        {
                            LOG_AND_EXIT("Error: GFXF file " + current_path + "\\" + gfxf_hash_file_name + " could not be created.");
                        }

                        gfxf_file.write(gfxf_file_header_data.data(), gfxf_file_header_data.size());

                        gfxf_file.write(gfx_file_data.data(), gfx_file_data.size());

                        gfxf_file.write(gfxf_file_data.data(), gfxf_file_data.size());

                        gfxf_file.write(meta_data_footer.data(), meta_data_footer.size());
                    }
                    else
                    {
                        position = gfx_file_size + 0x44;

                        gfxf_dds_tga_file_count = dds_tga_file_names.size();

                        while (((position & 0xF) != 0x4) && ((position & 0xF) != 0xC))
                        {
                            gfxf_file_data.push_back(0x0);
                            position++;
                        }

                        dds_tga_names_offset_start = position;

                        std::memcpy(&input, &gfxf_dds_tga_file_count, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_data.push_back(input[k]);
                        }

                        position += 0x4;

                        uint32_t offset = (uint64_t)dds_tga_names_offset_start + ((uint64_t)gfxf_dds_tga_file_count * (uint64_t)0x10) + (uint64_t)0x8;

                        for (uint64_t j = 0; j < dds_tga_file_names.size(); j++)
                        {
                            uint32_t dds_tga_file_length1 = dds_tga_file_names.at(j).length();
                            uint32_t dds_tga_file_length2 = dds_tga_file_names.at(j).length();

                            dds_tga_file_length1 |= 0x40000000;

                            std::memcpy(&input, &dds_tga_file_length1, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;
                            
                            dds_tga_name_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            offset += 0x4;

                            offset += 0x1;

                            offset += dds_tga_file_length2;

                            while (offset % 4 != 0)
                            {
                                offset++;
                            }
                        }

                        dds_tga_names_offset_end = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        for (uint64_t j = 0; j < dds_tga_file_names.size(); j++)
                        {
                            uint32_t dds_tga_file_length1 = dds_tga_file_names.at(j).length();
                            dds_tga_file_length1++;

                            std::memcpy(&input, &dds_tga_file_length1, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            for (uint64_t k = 0; k < dds_tga_file_names.at(j).length(); k++)
                            {
                                gfxf_file_data.push_back(dds_tga_file_names.at(j)[k]);
                            }

                            position += (uint64_t)dds_tga_file_names.at(j).length();

                            gfxf_file_data.push_back(0x0);

                            position++;

                            while (dds_tga_file_length1 % 4 != 0)
                            {
                                gfxf_file_data.push_back(0x0);
                                dds_tga_file_length1++;
                                position++;
                            }
                        }

                        std::vector<std::vector<char>> dds_tga_file_data;

                        for (uint64_t j = 0; j < dds_tga_file_paths.size(); j++)
                        {
                            uint32_t dds_tga_file_size = 0;

                            std::ifstream dds_tga_file = std::ifstream(dds_tga_file_paths.at(j), std::ifstream::binary);

                            if (!dds_tga_file.good())
                            {
                                LOG_AND_EXIT("Error: dds/tga file " + dds_tga_file_paths.at(j) + " could not be created.");
                            }

                            dds_tga_file.seekg(0, dds_tga_file.end);
                            dds_tga_file_size = (uint32_t)dds_tga_file.tellg();
                            dds_tga_file.seekg(0, dds_tga_file.beg);

                            std::vector<char> temp_dds_tga_file_data(dds_tga_file_size, 0);

                            dds_tga_file.read(temp_dds_tga_file_data.data(), dds_tga_file_size);

                            dds_tga_file.close();

                            dds_tga_file_data.push_back(temp_dds_tga_file_data);
                        }

                        std::memcpy(&input, &gfxf_dds_tga_file_count, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_data.push_back(input[k]);
                        }

                        position += 0x4;

                        dds_tga_data_offset_start = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        offset = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size() + ((uint64_t)gfxf_dds_tga_file_count * (uint64_t)0x18) + (uint64_t)0x4;

                        for (uint64_t j = 0; j < dds_tga_file_data.size(); j++)
                        {
                            uint32_t dds_tga_file_length = dds_tga_file_data.at(j).size();

                            dds_tga_data_1_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            offset += dds_tga_file_length;

                            dds_tga_data_2_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            dds_tga_data_3_offsets.push_back((uint32_t)position);

                            std::memcpy(&input, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            position += 0x4;

                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);
                            gfxf_file_data.push_back(0x0);

                            position += 0x4;

                            offset += 0x4;
                        }

                        dds_tga_data_offset_end = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        for (uint64_t j = 0; j < dds_tga_file_data.size(); j++)
                        {
                            uint32_t dds_tga_file_length = dds_tga_file_data.at(j).size();

                            std::memcpy(&input, &dds_tga_file_length, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                gfxf_file_data.push_back(input[k]);
                            }

                            for (uint64_t k = 0; k < dds_tga_file_length; k++)
                            {
                                gfxf_file_data.push_back(dds_tga_file_data.at(j)[k]);
                            }
                        }

                        uint32_t header_value = (uint32_t)gfx_file_size + (uint32_t)0x44 + (uint32_t)gfxf_file_data.size();

                        std::memcpy(&input, &header_value, sizeof(uint32_t));

                        gfxf_file_header_data.push_back(input[3]);
                        gfxf_file_header_data.push_back(input[2]);
                        gfxf_file_header_data.push_back(input[1]);
                        gfxf_file_header_data.push_back(input[0]);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x44);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &gfx_file_size, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        dds_tga_names_offset_start += 0x4;

                        std::memcpy(&input, &dds_tga_names_offset_start, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_names_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_names_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_data_offset_start, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_data_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &dds_tga_data_offset_end, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);
                        gfxf_file_header_data.push_back(0x0);

                        std::memcpy(&input, &gfx_file_size, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            gfxf_file_header_data.push_back(input[k]);
                        }

                        unsigned char footer_header[] = { 0xED, 0xA5, 0xEB, 0x12 };
                        unsigned char footer_serial_data[] = { 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00 };
                        uint32_t footer_serial_data_count = 7;

                        uint32_t footer_data_count = (uint32_t)dds_tga_file_names.size() * (uint32_t)0x4 + footer_serial_data_count;

                        uint32_t footer_total_size = footer_data_count * (uint32_t)0x4 + (uint32_t)0x4;

                        for (uint64_t k = 0; k < sizeof(footer_header); k++)
                        {
                            meta_data_footer.push_back(footer_header[k]);
                        }

                        std::memcpy(&input, &footer_total_size, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data_footer.push_back(input[k]);
                        }

                        std::memcpy(&input, &footer_data_count, sizeof(uint32_t));

                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data_footer.push_back(input[k]);
                        }

                        for (uint64_t k = 0; k < sizeof(footer_serial_data); k++)
                        {
                            meta_data_footer.push_back(footer_serial_data[k]);
                        }

                        for (uint64_t d = 0; d < dds_tga_name_offsets.size(); d++)
                        {
                            std::memcpy(&input, &dds_tga_name_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }
                        }

                        for (uint64_t d = 0; d < dds_tga_data_1_offsets.size(); d++)
                        {
                            std::memcpy(&input, &dds_tga_data_1_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }

                            std::memcpy(&input, &dds_tga_data_2_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }

                            std::memcpy(&input, &dds_tga_data_3_offsets.at(d), sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data_footer.push_back(input[k]);
                            }
                        }

                        std::string current_path = gfxf_folders.at(i) + "\\GFXF.rebuilt";

                        file::create_directories(current_path);

                        std::ofstream gfxf_file = std::ofstream(current_path + "\\" + gfxf_hash_file_name, std::ofstream::binary);

                        if (!gfxf_file.good())
                        {
                            LOG_AND_EXIT("Error: GFXF file " + current_path + "\\" + gfxf_hash_file_name + " could not be created.");
                        }

                        gfxf_file.write(gfxf_file_header_data.data(), gfxf_file_header_data.size());

                        gfxf_file.write(gfx_file_data.data(), gfx_file_data.size());

                        gfxf_file.write(gfxf_file_data.data(), gfxf_file_data.size());

                        gfxf_file.write(meta_data_footer.data(), meta_data_footer.size());
                    }
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to rebuild the GFXFs does not exist.");
    }
}