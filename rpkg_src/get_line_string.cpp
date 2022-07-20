#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include "rpkg.h"
#include "hash.h"
#include "crypto.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>

void rpkg_function::get_line_string(std::string& input_path, std::string& filter, std::string& output_path)
{
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        std::stringstream ss;

        ss << "Scanning folder: Done";

        //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        std::vector<std::string> filters = util::parse_input_filter(filter);

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (uint64_t z = 0; z < filters.size(); z++)
        {
            uint64_t hash_value = std::strtoull(filters.at(z).c_str(), nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(hash_value);

            if (it2 != hash_list_hash_map.end())
            {
                LOG("Scanning RPKG files for: " + filters.at(z));
                LOG(filters.at(z) + "'s IOI string is: " + hash_list_hash_strings.at(it2->second));
            }
            else
            {
                LOG("Scanning RPKG files for: " + filters.at(z));
            }

            uint32_t rpkg_index = rpkg_function::get_latest_hash(hash_value);

            if (rpkg_index != UINT32_MAX)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    uint64_t hash_size;

                    if (rpkgs.at(rpkg_index).hash.at(it->second).is_lz4ed == 1)
                    {
                        hash_size = rpkgs.at(rpkg_index).hash.at(it->second).hash_size;

                        if (rpkgs.at(rpkg_index).hash.at(it->second).is_xored == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkgs.at(rpkg_index).hash.at(it->second).hash_size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(rpkg_index).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(rpkg_index).rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkgs.at(rpkg_index).hash.at(it->second).hash_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkgs.at(rpkg_index).hash.at(it->second).is_xored == 1)
                    {
                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                    }

                    uint32_t decompressed_size = rpkgs.at(rpkg_index).hash.at(it->second).hash_size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* line_data;

                    if (rpkgs.at(rpkg_index).hash.at(it->second).is_lz4ed)
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                        line_data = &output_data;
                    }
                    else
                    {
                        line_data = &input_data;
                    }

                    uint32_t line_crc32 = 0;

                    std::memcpy(&line_crc32, &line_data->data()[0], sizeof(uint32_t));

                    for (uint32_t i = 0; i < rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.size(); i++)
                    {
                        if (util::hash_type(rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i)) == "locr")
                        {
                            uint32_t rpkg_index2 = rpkg_function::get_latest_hash(rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i));

                            if (rpkg_index2 != UINT32_MAX)
                            {
                                std::map<uint64_t, uint64_t>::iterator it6 = rpkgs.at(rpkg_index2).hash_map.find(rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i));

                                std::string hash_string = util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i));

                                if (it6 != rpkgs.at(rpkg_index2).hash_map.end())
                                {
                                    rpkg_function::extract_locr_to_json_from(rpkgs.at(rpkg_index2).rpkg_file_path, hash_string, output_path, true);

                                    for (const auto& it : localization_json.items())
                                    {
                                        bool language_found = false;

                                        std::string language = "";

                                        for (const auto& it2 : it.value().items())
                                        {
                                            if (it2.value().contains("Language"))
                                            {
                                                language_found = true;

                                                language = it2.value()["Language"];
                                            }
                                        }

                                        if (language_found)
                                        {
                                            for (const auto& it2 : it.value().items())
                                            {
                                                if (it2.value().contains("StringHash"))
                                                {
                                                    if ((uint32_t)it2.value()["StringHash"] == line_crc32)
                                                    {
                                                        //LOG("FOUND CRC32: " + util::uint32_t_to_hex_string(line_crc32) + " in " << hash_string);

                                                        if (it2.value().contains("String"))
                                                        {
                                                            if (localization_line_string == "")
                                                            {
                                                                localization_line_string = "\nLINE CRC32: " + util::uint32_t_to_string(line_crc32) + "\nLOCR Strings:\n";

                                                                localization_line_string += "  - " + language + ": " + std::string(it2.value()["String"]) + "\n";
                                                            }
                                                            else
                                                            {
                                                                localization_line_string += "  - " + language + ": " + std::string(it2.value()["String"]) + "\n";
                                                            }

                                                            //LOG("FOUND STRING: " + std::string(it2.value()["String"]) + " in " << hash_string);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to search for RPKG files for latest hash mode does not exist.");
    }

    LOG(localization_line_string);
}