#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <unordered_map>
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

    if (!file::path_exists(input_path))
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }

    if (!hash_list_loaded)
    {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(true);
        LOG("Loading Hash List: Done");
    }

    if (!input_path_is_rpkg_file)
    {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

    file::create_directories(file::output_path_append("WWES", output_path));

    std::vector<std::string> filters = util::parse_input_filter(filter);

    bool extract_single_hash = false;

    if (filters.size() == 1)
    {
        if (util::is_valid_hash(filters.at(0)))
        {
            extract_single_hash = true;
        }
    }

    uint64_t wwes_count = 0;
    uint64_t wwes_hash_size_total = 0;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (auto& rpkg : rpkgs)
    {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++)
        {
            if (rpkg.hash_resource_types.at(r) != "WWES")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++)
            {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                    rpkg.hash.at(hash_index).hash_resource_type;

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

                wwes_hash_size_total += rpkg.hash.at(hash_index).data.resource.size_final;

                wwes_count++;
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

    if (!filter.empty())
    {
        LOG("Extracting WWES to *.wem/*.ogg files with filter \"" << filter << "\"");
    }

    std::unordered_map<std::string, uint32_t> wwes_name_map;

    std::vector<std::string> found_in;
    std::vector<std::string> not_found_in;

    for (uint64_t z = 0; z < filters.size(); z++)
    {
        found_in.push_back("");

        not_found_in.push_back("");
    }

    for (auto& rpkg : rpkgs)
    {
        std::vector<bool> extracted;

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            extracted.push_back(false);
        }

        if (rpkg.rpkg_file_path == input_path || !input_path_is_rpkg_file)
        {
            for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++)
            {
                if (rpkg.hash_resource_types.at(r) != "WWES")
                    continue;

                for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                    if (gui_control == ABORT_CURRENT_TASK)
                    {
                        return;
                    }

                    std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                        rpkg.hash.at(hash_index).hash_resource_type;

                    if (((wwes_count_current * static_cast<uint64_t>(100000)) / wwes_count) % static_cast<uint64_t>(10) == 0 && wwes_count_current > 0)
                    {
                        stringstream_length = console::update_console(message, wwes_hash_size_total, wwes_hash_size_current, start_time, stringstream_length);
                    }

                    wwes_hash_size_current += rpkg.hash.at(hash_index).data.resource.size_final;

                    wwes_count_current++;

                    if (!extract_single_hash || (extract_single_hash && filter == util::uint64_t_to_hex_string(
                        rpkg.hash.at(hash_index).hash_value)))
                    {
                        std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                            rpkg.hash.at(hash_index).hash_resource_type;

                        std::string hash_list_string = "";
                        std::string wwes_ioi_path = "";
                        std::string wwes_ioi_directory = "";
                        std::string wwes_base_name = "";

                        bool full_wwes_ioi_path_unknown = false;
                        bool wwes_ioi_path_found = false;

                        std::unordered_map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkg.hash.at(hash_index).hash_value);

                        if (it != hash_list_hash_map.end())
                        {
                            hash_list_string = hash_list_hash_strings.at(it->second);
                            wwes_ioi_path = file::output_path_append("WWES\\" + rpkg.rpkg_file_name + "\\assembly", output_path);

                            if (hash_list_string.find("/unknown/") != std::string::npos)
                            {
                                full_wwes_ioi_path_unknown = true;
                            }

                            size_t pos1 = hash_list_string.find("[assembly:");
                            size_t pos2 = hash_list_string.find(".wav]");
                            size_t pos3 = hash_list_string.find(".pc_wes");

                            if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos && (pos2 - pos1) > 12)
                            {
                                wwes_ioi_path.append(hash_list_string.substr((pos1 + 10), pos2 - (pos1 + 10)));

                                std::replace(wwes_ioi_path.begin(), wwes_ioi_path.end(), '/', '\\');

                                size_t pos4 = wwes_ioi_path.find_last_of("\\");

                                wwes_base_name = wwes_ioi_path.substr(pos4 + 1);

                                wwes_ioi_directory = wwes_ioi_path.substr(0, pos4);

                                if (full_wwes_ioi_path_unknown)
                                {
                                    wwes_ioi_path += "." + util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value);
                                }

                                wwes_ioi_path_found = true;
                            }
                        }

                        if (wwes_ioi_path_found)
                        {
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

                            std::vector<char>* wwes_data;

                            if (rpkg.hash.at(hash_index).data.lz4ed)
                            {
                                LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size), decompressed_size);

                                wwes_data = &output_data;
                            }
                            else
                            {
                                wwes_data = &input_data;
                            }

                            std::string wem_file_name = wwes_ioi_path + ".wem";
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
                                    LOG("WWEV resource found: " << hash_file_name << " in RPKG file: " << rpkg.rpkg_file_name);
                                    LOG("Error parsing ogg file " << wem_file_name << " could not be created.");
                                    LOG("Error: " << pe);
                                }

                                output_file.close();

                                revorb(ogg_file);

                                std::remove(wem_file_name.c_str());
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
                    found_in.at(z).append(rpkg.rpkg_file_name);
                }
                else
                {
                    found_in.at(z).append(", " + rpkg.rpkg_file_name);
                }
            }
            else
            {
                if (not_found_in.at(z) == "")
                {
                    not_found_in.at(z).append(rpkg.rpkg_file_name);
                }
                else
                {
                    not_found_in.at(z).append(", " + rpkg.rpkg_file_name);
                }
            }
        }
    }

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    ss.str(std::string());

    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    percent_progress = static_cast<uint32_t>(100);

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
