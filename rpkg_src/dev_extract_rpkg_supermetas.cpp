#include "dev_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "hash.h"
#include "console.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

void dev_function::dev_extract_rpkg_supermetas(std::string& input_path, std::string& output_path)
{
    /*input_path = file::parse_input_folder_path(input_path);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_path))
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

        if (std::filesystem::is_regular_file(entry.path().string()))
        {
            std::size_t pos = entry.path().string().find_last_of("\\/");

            std::string rpkg_file_name = "";
            std::string hash = "";
            std::string hash_resource_type = "";

            if (pos != std::string::npos)
            {
                rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
            }
            else
            {
                rpkg_file_name = entry.path().string();
            }

            if (file::is_rpkg_file(rpkg_file_name))
            {
                std::string rpkg_to_import = entry.path().string();

                std::cout << "Importing: " << rpkg_to_import << std::endl;

                std::ifstream file = std::ifstream(rpkg_to_import, std::ifstream::binary);
                char input[1024];
                char bytes1 = 0;
                uint32_t bytes4 = 0, file_count = 0, table_offset = 0, table_size = 0, patch_entry_count = 0;
                uint64_t bytes8 = 0, offset1 = 0, offset2 = 0, rpkg_file_size = 0;
                std::string message = "Importing RPKG file data: ";

                if (!file.good())
                {
                    LOG_AND_EXIT("Error: RPKG file " + rpkg_to_import + " could not be read.");
                }

                file.seekg(0, file.end);
                rpkg_file_size = file.tellg();
                file.seekg(0, file.beg);

                if (rpkg_file_size <= 0x4)
                {
                    LOG_AND_RETURN("Error: " + rpkg_to_import + " is not a valid RPKG file.");
                }

                rpkg temp_rpkg;

                temp_rpkg.rpkg_file_path = rpkg_to_import;
                temp_rpkg.rpkg_file_name = file::get_base_file_name(rpkg_to_import);

                file.read(input, 4);
                input[4] = 0;

                if (std::string(input) == "GKPR")
                {
                    temp_rpkg.rpkg_file_version = 1;

                    LOG("Valid RPKGv1 file magic signature found.");
                }
                else if (std::string(input) == "2KPR")
                {
                    temp_rpkg.rpkg_file_version = 2;

                    LOG("Valid RPKGv2 file magic signature found.");
                }
                else
                {
                    LOG_AND_RETURN("Error: " + rpkg_to_import + " is not a valid RPKG file.");
                }

                if ((temp_rpkg.rpkg_file_version == 1 && rpkg_file_size <= 0x14) || (temp_rpkg.rpkg_file_version == 2 && rpkg_file_size <= 0x1D))
                {
                    LOG_AND_RETURN("Error: " + rpkg_to_import + " is a empty RPKG file.");
                }

                if (temp_rpkg.rpkg_file_version == 2)
                {
                    file.read(input, sizeof(bytes4));

                    file.read(input, sizeof(bytes1));
                    std::memcpy(&bytes1, input, sizeof(bytes1));
                    temp_rpkg.rpkgv2_chunk_number = bytes1;

                    file.read(input, sizeof(bytes4));
                }

                file.read(input, sizeof(file_count));
                std::memcpy(&file_count, input, sizeof(file_count));
                temp_rpkg.rpkg_file_count = file_count;

                file.read(input, sizeof(table_offset));
                std::memcpy(&table_offset, input, sizeof(table_offset));
                temp_rpkg.rpkg_table_offset = table_offset;

                file.read(input, sizeof(table_size));
                std::memcpy(&table_size, input, sizeof(table_size));
                temp_rpkg.rpkg_table_size = table_size;

                uint64_t position = file.tellg();

                file.read(input, sizeof(patch_entry_count));
                std::memcpy(&patch_entry_count, input, sizeof(patch_entry_count));

                temp_rpkg.patch_entry_count = patch_entry_count;

                if (temp_rpkg.rpkg_file_version == 1 && ((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14 + (uint64_t)0x10) >= rpkg_file_size)
                {
                    temp_rpkg.is_patch_file = false;

                    LOG("RPKGv1 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
                }
                else if (temp_rpkg.rpkg_file_version == 2 && ((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D + (uint64_t)0x10) >= rpkg_file_size)
                {
                    temp_rpkg.is_patch_file = false;

                    LOG("RPKGv2 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
                }
                else
                {
                    char patch_zero_test = 0;
                    uint64_t patch_value = 0;

                    if (temp_rpkg.rpkg_file_version == 1)
                    {
                        file.seekg(((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14), file.beg);
                    }
                    else
                    {
                        file.seekg(((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D), file.beg);
                    }

                    file.read(input, 0x7);
                    file.read(input, sizeof(bytes1));
                    std::memcpy(&patch_zero_test, input, sizeof(bytes1));
                    file.read(input, sizeof(bytes8));
                    std::memcpy(&patch_value, input, sizeof(bytes8));

                    if (temp_rpkg.rpkg_file_version == 1 && patch_value == ((uint64_t)table_offset + (uint64_t)table_size + (uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14) && patch_zero_test == 0x0)
                    {
                        temp_rpkg.is_patch_file = true;

                        LOG("RPKGv1 file " << temp_rpkg.rpkg_file_name << " is a patch file.");
                    }
                    else if (temp_rpkg.rpkg_file_version == 2 && patch_value == ((uint64_t)table_offset + (uint64_t)table_size + (uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D) && patch_zero_test == 0x0)
                    {
                        temp_rpkg.is_patch_file = true;

                        LOG("RPKGv2 file " << temp_rpkg.rpkg_file_name << " is a patch file.");
                    }
                    else
                    {
                        temp_rpkg.is_patch_file = false;

                        if (temp_rpkg.rpkg_file_version == 1)
                        {
                            LOG("RPKGv1 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
                        }
                        else
                        {
                            LOG("RPKGv2 file " << temp_rpkg.rpkg_file_name << " is not a patch file.");
                        }
                    }
                }

                file.seekg(position, file.beg);

                uint64_t input_file_size = 0;

                if (temp_rpkg.is_patch_file)
                {
                    if (temp_rpkg.rpkg_file_version == 1)
                    {
                        LOG("Importing index from RPKGv1 file: " << temp_rpkg.rpkg_file_name);
                    }
                    else
                    {
                        LOG("Importing index from RPKGv2 file: " << temp_rpkg.rpkg_file_name);
                    }

                    file.read(input, sizeof(patch_entry_count));
                    std::memcpy(&patch_entry_count, input, sizeof(patch_entry_count));
                    temp_rpkg.patch_entry_count = patch_entry_count;

                    for (uint64_t i = 0; i < patch_entry_count; i++)
                    {
                        file.read(input, sizeof(bytes8));
                        std::memcpy(&bytes8, input, sizeof(bytes8));
                        temp_rpkg.patch_entry_list.push_back(bytes8);
                    }

                    if (temp_rpkg.rpkg_file_version == 1)
                    {
                        input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x14 + (uint64_t)patch_entry_count * (uint64_t)0x8;
                    }
                    else
                    {
                        input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x1D + (uint64_t)patch_entry_count * (uint64_t)0x8;
                    }
                }
                else
                {
                    if (temp_rpkg.rpkg_file_version == 1)
                    {
                        input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x10;

                        LOG("Importing index from RPKGv1 file: " << temp_rpkg.rpkg_file_name);
                    }
                    else
                    {
                        input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x19;

                        LOG("Importing index from RPKGv2 file: " << temp_rpkg.rpkg_file_name);
                    }
                }

                position = (uint64_t)file.tellg();

                offset2 = position + table_offset;

                std::vector<char> input_file_data(input_file_size, 0);

                file.seekg(0x0, file.beg);

                file.read(input_file_data.data(), input_file_size);

                file.close();

                std::string output_metas_path = file::output_path_append("RPKGSUPERMETAS", output_path);

                file::create_directories(output_metas_path);

                std::string final_path = output_metas_path + "\\" + file::get_root_file_name(rpkg_file_name) + ".supermeta";

                std::ofstream rpkg_super_meta_data_file = std::ofstream(final_path, std::ofstream::binary);

                if (!rpkg_super_meta_data_file.good())
                {
                    LOG_AND_EXIT("Error: Meta data file " + final_path + " could not be created.");
                }

                rpkg_super_meta_data_file.write(input_file_data.data(), input_file_size);
            }
        }
    }*/
}