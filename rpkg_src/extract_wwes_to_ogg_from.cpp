#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void rpkg_function::extract_wwes_to_ogg_from(std::string& input_path, std::string& filter, std::string& output_path)
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
        LOG("Loading Hash List...");

        generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        file::create_directories(file::output_path_append("WWES", output_path));

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

        uint64_t wwes_count = 0;
        uint64_t wwes_hash_size_total = 0;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
            {
                if (rpkgs.at(i).hash_resource_types.at(r) == "WWES")
                {
                    for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                    {
                        uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                        if (gui_control == ABORT_CURRENT_TASK)
                        {
                            return;
                        }

                        std::string hash_file_name = rpkgs.at(i).hash.at(hash_index).hash_file_name;

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

                            ss << "Scanning RPKGs for WWES files" << std::string(period_count, '.');

                            timing_string = ss.str();

                            LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                            period_count++;
                        }

                        wwes_hash_size_total += rpkgs.at(i).hash.at(hash_index).hash_size_final;

                        wwes_count++;
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Scanning RPKGs for WWES files: Done";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        uint64_t wwes_count_current = 0;
        uint64_t wwes_hash_size_current = 0;

        std::string message = "Extracting WWES to IOI Paths: ";

        if (filter != "")
        {
            LOG("Extracting WWES to *.wem/*.ogg files with filter \"" << filter << "\"");
        }

        std::map<std::string, uint32_t> wwes_name_map;

        std::vector<std::string> found_in;
        std::vector<std::string> not_found_in;

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            found_in.push_back("");

            not_found_in.push_back("");
        }

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            std::vector<bool> extracted;

            for (uint64_t z = 0; z < filters.size(); z++)
            {
                extracted.push_back(false);
            }

            if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file)
            {
                for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
                {
                    if (rpkgs.at(i).hash_resource_types.at(r) == "WWES")
                    {
                        for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                        {
                            uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                            if (gui_control == ABORT_CURRENT_TASK)
                            {
                                return;
                            }

                            std::string hash_file_name = rpkgs.at(i).hash.at(hash_index).hash_file_name;

                            if (((wwes_count_current * (uint64_t)100000) / (uint64_t)wwes_count) % (uint64_t)10 == 0 && wwes_count_current > 0)
                            {
                                stringstream_length = console::update_console(message, wwes_hash_size_total, wwes_hash_size_current, start_time, stringstream_length);
                            }

                            wwes_hash_size_current += rpkgs.at(i).hash.at(hash_index).hash_size_final;

                            wwes_count_current++;

                            if (!extract_single_hash || (extract_single_hash && filter == rpkgs.at(i).hash.at(hash_index).hash_string))
                            {
                                std::string hash_file_name = rpkgs.at(i).hash.at(hash_index).hash_file_name;

                                std::string hash_list_string = "";
                                std::string wwes_ioi_path = "";
                                std::string wwes_ioi_directory = "";
                                std::string wwes_base_name = "";

                                bool full_wwes_ioi_path_unknown = false;
                                bool wwes_ioi_path_found = false;

                                std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(i).hash.at(hash_index).hash_value);

                                if (it != hash_list_hash_map.end())
                                {
                                    hash_list_string = hash_list_hash_strings.at(it->second);
                                    wwes_ioi_path = file::output_path_append("WWES\\" + rpkgs.at(i).rpkg_file_name + "\\assembly", output_path);

                                    if (hash_list_string.find("/unknown/") != std::string::npos)
                                    {
                                        full_wwes_ioi_path_unknown = true;
                                    }

                                    size_t pos1 = hash_list_string.find("[assembly:");
                                    size_t pos2 = hash_list_string.find(".pc_wem");

                                    if (pos1 != std::string::npos && pos2 != std::string::npos && (pos2 - pos1) > 12)
                                    {
                                        wwes_ioi_path.append(hash_list_string.substr((pos1 + 10), hash_list_string.length()));

                                        std::replace(wwes_ioi_path.begin(), wwes_ioi_path.end(), '/', '\\');

                                        size_t pos3 = wwes_ioi_path.find_last_of("\\");

                                        wwes_base_name = wwes_ioi_path.substr(pos3 + 1);

                                        wwes_ioi_directory = wwes_ioi_path.substr(0, pos3);

                                        if (full_wwes_ioi_path_unknown)
                                        {
                                            wwes_ioi_path.push_back('\\');
                                            wwes_ioi_path.append(wwes_base_name);
                                            wwes_ioi_directory.push_back('\\');
                                            wwes_ioi_directory.append(wwes_base_name);
                                        }

                                        rpkg_function::get_unique_name(wwes_name_map, wwes_ioi_path);

                                        wwes_ioi_path_found = true;
                                    }
                                }

                                if (wwes_ioi_path_found)
                                {
                                    uint64_t hash_size;

                                    if (rpkgs.at(i).hash.at(hash_index).is_lz4ed == 1)
                                    {
                                        hash_size = rpkgs.at(i).hash.at(hash_index).hash_size;

                                        if (rpkgs.at(i).hash.at(hash_index).is_xored == 1)
                                        {
                                            hash_size &= 0x3FFFFFFF;
                                        }
                                    }
                                    else
                                    {
                                        hash_size = rpkgs.at(i).hash.at(hash_index).hash_size_final;
                                    }

                                    std::vector<char> input_data(hash_size, 0);

                                    std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                                    if (!file.good())
                                    {
                                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                                    }

                                    file.seekg(rpkgs.at(i).hash.at(hash_index).hash_offset, file.beg);
                                    file.read(input_data.data(), hash_size);
                                    file.close();

                                    if (rpkgs.at(i).hash.at(hash_index).is_xored == 1)
                                    {
                                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                                    }

                                    uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).hash_size_final;

                                    std::vector<char> output_data(decompressed_size, 0);

                                    std::vector<char>* wwes_data;

                                    if (rpkgs.at(i).hash.at(hash_index).is_lz4ed)
                                    {
                                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                                        wwes_data = &output_data;
                                    }
                                    else
                                    {
                                        wwes_data = &input_data;
                                    }

                                    std::string wem_file_name = wwes_ioi_path + ".wem";
                                    std::string wwes_meta_data_file_name = wwes_ioi_path + "_" + hash_file_name;
                                    std::string ogg_file = wwes_ioi_path + ".ogg";

                                    bool found = false;

                                    uint64_t input_filter_index = 0;

                                    for (uint64_t z = 0; z < filters.size(); z++)
                                    {
                                        std::size_t found_position_hash = hash_file_name.find(filters.at(z));

                                        std::size_t found_position_wwem = util::to_upper_case(hash_list_string).find(filters.at(z));

                                        if ((found_position_hash != std::string::npos && filters.at(z) != "") || (found_position_wwem != std::string::npos && filters.at(z) != ""))
                                        {
                                            found = true;

                                            input_filter_index = z;

                                            break;
                                        }
                                    }

                                    if (found || filter == "")
                                    {
                                        file::create_directories(wwes_ioi_directory);

                                        if (filters.size() > 0)
                                        {
                                            extracted.at(input_filter_index) = true;
                                        }

                                        std::ofstream wem_file = std::ofstream(wem_file_name, std::ofstream::binary);

                                        if (!wem_file.good())
                                        {
                                            LOG_AND_EXIT("Error: wem file " + wem_file_name + " could not be created.");
                                        }

                                        wem_file.write(wwes_data->data(), decompressed_size);

                                        wem_file.close();

                                        std::ofstream wwes_meta_data_file = std::ofstream(wwes_meta_data_file_name, std::ofstream::binary);

                                        if (!wwes_meta_data_file.good())
                                        {
                                            LOG_AND_EXIT("Error: meta file " + wwes_meta_data_file_name + " could not be created.");
                                        }

                                        wwes_meta_data_file.close();

                                        if (!file::path_exists("packed_codebooks_aoTuV_603.bin"))
                                        {
                                            LOG("Error: Missing packed_codebooks_aoTuV_603.bin file.");
                                            LOG("       Attempting to create the packed_codebooks_aoTuV_603.bin file.");

                                            std::ofstream output_file = std::ofstream("packed_codebooks_aoTuV_603.bin", std::ofstream::binary);

                                            if (!output_file.good())
                                            {
                                                LOG_AND_EXIT("Error: packed_codebooks_aoTuV_603.bin file packed_codebooks_aoTuV_603.bin could not be created.");
                                            }

                                            output_file.write((const char*)codebook, sizeof(codebook));
                                        }

                                        std::ofstream output_file = std::ofstream(ogg_file, std::ofstream::binary);

                                        if (!output_file.good())
                                        {
                                            LOG_AND_EXIT("Error: ogg file " + ogg_file + " could not be created.");
                                        }

                                        try
                                        {
                                            Wwise_RIFF_Vorbis ww(wem_file_name, "packed_codebooks_aoTuV_603.bin", false, false, kNoForcePacketFormat);

                                            ww.generate_ogg(output_file);
                                        }
                                        catch (const Parse_error& pe)
                                        {
                                            LOG("WWEV resource found: " << hash_file_name << " in RPKG file: " << rpkgs.at(i).rpkg_file_name);
                                            LOG("Error parsing ogg file " << wem_file_name << " could not be created.");
                                            LOG("Error: " << pe);
                                        }

                                        output_file.close();

                                        revorb(ogg_file);

                                        std::string metas_directory = wwes_ioi_directory + "\\metas";

                                        file::create_directories(metas_directory);

                                        std::string final_path = metas_directory + "\\" + rpkgs.at(i).hash.at(hash_index).hash_file_name;

                                        rpkg_function::extract_hash_meta(i, hash_index, final_path);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (uint64_t z = 0; z < filters.size(); z++)
            {
                if (extracted.at(z))
                {
                    if (found_in.at(z) == "")
                    {
                        found_in.at(z).append(rpkgs.at(i).rpkg_file_name);
                    }
                    else
                    {
                        found_in.at(z).append(", " + rpkgs.at(i).rpkg_file_name);
                    }
                }
                else
                {
                    if (not_found_in.at(z) == "")
                    {
                        not_found_in.at(z).append(rpkgs.at(i).rpkg_file_name);
                    }
                    else
                    {
                        not_found_in.at(z).append(", " + rpkgs.at(i).rpkg_file_name);
                    }
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        if (filter != "")
        {
            for (uint64_t z = 0; z < filters.size(); z++)
            {
                LOG(std::endl << "\"" << filters.at(z) << "\" was found in and extracted from: " << found_in.at(z));

                LOG(std::endl << "\"" << filters.at(z) << "\" was not found in RPKG file(s): " << not_found_in.at(z));
            }
        }

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}