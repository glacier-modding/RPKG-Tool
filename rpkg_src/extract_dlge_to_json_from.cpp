#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "rpkg.h"
#include "hash.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/json/json.hpp"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

using json = nlohmann::ordered_json;

void rpkg_function::extract_dlge_to_json_from(std::string &input_path, std::string &filter, std::string &output_path, bool output_to_string)
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

    if (!input_path_is_rpkg_file)
    {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    timing_string = ss.str();

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    if (output_to_string)
    {
        localization_string = "";
    }
    else
    {
        file::create_directories(file::output_path_append("DLGE", output_path));
    }

    std::vector<std::string> filters = util::parse_input_filter(filter);

    uint64_t dlge_count = 0;
    uint64_t dlge_current_count = 0;

    for (auto& rpkg : rpkgs)
    {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++)
        {
            if (rpkg.hash_resource_types.at(r) != "DLGE")
                continue;

            dlge_count += rpkg.hashes_indexes_based_on_resource_types.at(r).size();
        }
    }

    timing_string = "Extracting DLGE as JSON...";

    if (log_output)
        LOG("Extracting DLGE as JSON...");

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path != input_path && input_path_is_rpkg_file)
            continue;

        bool archive_folder_created = false;

        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) != "DLGE")
                continue;

            for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
            {
                uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." + rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                bool found = false;

                for (uint64_t z = 0; z < filters.size(); z++)
                {
                    std::size_t found_position = hash_file_name.find(filters.at(z));

                    if (found_position != std::string::npos && filters.at(z) != "")
                    {
                        found = true;

                        break;
                    }
                }

                if (!found && filter != "")
                    continue;

                std::string message = "Extracting DLGE as JSON: ";

                if (((dlge_current_count * static_cast<uint64_t>(100000)) / dlge_count) % static_cast<uint64_t>(100) == 0 && dlge_current_count > 0 && !output_to_string)
                {
                    stringstream_length = console::update_console(message, dlge_count, dlge_current_count, start_time, stringstream_length);
                }

                dlge_current_count++;

                std::string current_path = file::output_path_append("DLGE\\" + rpkgs.at(i).rpkg_file_name, output_path);

                if (!archive_folder_created && !output_to_string)
                {
                    file::create_directories(current_path);

                    archive_folder_created = true;
                }

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
                    crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                }

                uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                std::vector<char> output_data(decompressed_size, 0);

                std::vector<char> *dlge_data;

                if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                {
                    LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size), decompressed_size);

                    dlge_data = &output_data;
                }
                else
                {
                    dlge_data = &input_data;
                }

                uint32_t dlge_data_size = 0;
                uint32_t number_of_dlge_categories = 0;
                std::vector<uint32_t> dlge_categories;
                std::vector<uint32_t> dlge_identifiers;
                std::vector<uint64_t> dlge_section_metas;
                std::vector<char> dlge_footer;
                uint32_t number_of_languages = 0;
                std::vector<std::vector<std::string>> language_string;
                std::vector<std::vector<uint32_t>> language_string_sizes;
                std::vector<std::vector<uint64_t>> language_string_metas;
                std::vector<std::string> languages;
                languages.push_back("en");
                languages.push_back("fr");
                languages.push_back("it");
                languages.push_back("de");
                languages.push_back("es");
                languages.push_back("ru");
                languages.push_back("mx");
                languages.push_back("br");
                languages.push_back("pl");
                languages.push_back("cn");
                languages.push_back("jp");
                languages.push_back("tc");

                std::vector<char> json_meta_data;

                json json_object;

                uint32_t position = 0;

                uint8_t bytes1 = 0;
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;

                std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                position += sizeof(bytes4);

                if (bytes4 != 0)
                {
                    LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                    break;
                }

                std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                position += sizeof(bytes4);

                if (bytes4 != 1)
                {
                    LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                    break;
                }

                uint8_t text_available = 0;

                std::memcpy(&bytes1, &dlge_data->data()[position], sizeof(bytes1));

                position += sizeof(bytes1);

                if (bytes1 == 1)
                {
                    text_available = 1;
                }

                uint32_t text_count = 0;

                bool dlge_has_text = false;

                bool do_not_break = true;

                while (text_available)
                {
                    number_of_dlge_categories++;

                    std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                    for (uint64_t k = 0; k < sizeof(bytes4); k++)
                    {
                        json_meta_data.push_back(dlge_data->data()[position + k]);
                    }

                    position += sizeof(bytes4);

                    dlge_categories.push_back(bytes4);

                    std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                    for (uint64_t k = 0; k < sizeof(bytes4); k++)
                    {
                        json_meta_data.push_back(dlge_data->data()[position + k]);
                    }

                    position += sizeof(bytes4);

                    dlge_identifiers.push_back(bytes4);

                    std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                    position += sizeof(bytes4);

                    if (bytes4 != 0)
                    {
                        LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                        do_not_break = false;
                        break;
                    }

                    std::memcpy(&bytes8, &dlge_data->data()[position], sizeof(bytes8));

                    position += sizeof(bytes8);

                    if (bytes8 != 0xFFFFFFFFFFFFFFFF)
                    {
                        LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                        do_not_break = false;
                        break;
                    }

                    std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                    position += sizeof(bytes4);

                    if (bytes4 != 0)
                    {
                        LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                        do_not_break = false;
                        break;
                    }

                    std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                    if (bytes4 == 0)
                    {
                        for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        {
                            json_meta_data.push_back(dlge_data->data()[position + k]);
                        }

                        position += sizeof(bytes4);
                    }

                    uint32_t check_variable_1 = 0;
                    uint32_t check_variable_2 = 0;

                    std::memcpy(&check_variable_1, &dlge_data->data()[position], sizeof(bytes4));

                    position += sizeof(bytes4);

                    std::memcpy(&check_variable_2, &dlge_data->data()[position], sizeof(bytes4));

                    position += sizeof(bytes4);

                    position -= sizeof(bytes8);

                    if ((check_variable_1 == 0xFFFFFFFF && check_variable_2 == 0xFFFFFFFF) || ((check_variable_1 + 1) == check_variable_2) || check_variable_2 == 0xFFFFFFFF)
                    {
                        std::memcpy(&bytes8, &dlge_data->data()[position], sizeof(bytes8));

                        for (uint64_t k = 0; k < sizeof(bytes8); k++)
                        {
                            json_meta_data.push_back(dlge_data->data()[position + k]);
                        }

                        position += sizeof(bytes8);

                        dlge_section_metas.push_back(bytes8);
                    }
                    else
                    {
                        LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                        do_not_break = false;
                        break;
                    }

                    bool done_scanning_languages = false;

                    std::vector<uint32_t> temp_language_string_sizes;

                    std::vector<uint64_t> temp_language_string_metas;

                    number_of_languages = 0;

                    while (!done_scanning_languages)
                    {
                        json temp_language_json_object;

                        temp_language_json_object["Language"] = languages.at(number_of_languages);

                        std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));
                        position += sizeof(bytes4);

                        temp_language_string_sizes.push_back(bytes4);

                        std::vector<char> temp_string;

                        for (uint64_t l = 0; l < temp_language_string_sizes.back(); l++)
                        {
                            temp_string.push_back(dlge_data->data()[position]);
                            position += 1;
                        }

                        if (temp_language_string_sizes.back() % 8 != 0)
                        {
                            LOG("Error: DLGE file " << hash_file_name << " in " << rpkgs.at(i).rpkg_file_name << " is malformed.");
                            do_not_break = false;
                            break;
                        }

                        for (uint32_t m = 0; m < temp_language_string_sizes.back() / 8; m++)
                        {
                            uint32_t data[2];
                            std::memcpy(data, &temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8)], sizeof(uint32_t));
                            std::memcpy(data + 1, &temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8) + static_cast<uint64_t>(4)], sizeof(uint32_t));

                            crypto::xtea_decrypt_localization(data);

                            std::memcpy(&temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8)], data, sizeof(uint32_t));
                            std::memcpy(&temp_string[static_cast<uint64_t>(m) * static_cast<uint64_t>(8) + static_cast<uint64_t>(4)], data + 1, sizeof(uint32_t));

                            dlge_has_text = true;
                        }

                        uint32_t last_zero_position = static_cast<uint32_t>(temp_string.size());

                        if (temp_string.size() > 0)
                        {
                            uint32_t m = static_cast<uint32_t>(temp_string.size() - 1);

                            while (m >= 0)
                            {
                                if (temp_string.at(m) != 0)
                                {
                                    break;
                                }
                                                
                                last_zero_position--;
                                m--;
                            }
                        }

                        if (temp_string.size() > 0)
                        {
                            std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(), temp_string.end()).substr(0, last_zero_position);

                            temp_language_json_object["String"] = temp_string_with_zero_pad_removed;
                        }
                        else
                        {
                            temp_language_json_object["String"] = "";
                        }

                        json_object.push_back(temp_language_json_object);

                        if ((position + 0x8) <= decompressed_size)
                        {
                            std::memcpy(&bytes4, &dlge_data->data()[position], sizeof(bytes4));

                            if (bytes4 == 0)
                            {
                                for (uint64_t k = 0; k < sizeof(bytes4); k++)
                                {
                                    json_meta_data.push_back(dlge_data->data()[position + k]);
                                }

                                position += sizeof(bytes4);
                            }

                            std::memcpy(&check_variable_1, &dlge_data->data()[position], sizeof(bytes4));
                            position += sizeof(bytes4);

                            std::memcpy(&check_variable_2, &dlge_data->data()[position], sizeof(bytes4));
                            position += sizeof(bytes4);

                            position -= sizeof(bytes8);

                            if ((check_variable_1 == 0xFFFFFFFF && check_variable_2 == 0xFFFFFFFF) || ((check_variable_1 + 1) == check_variable_2) || check_variable_2 == 0xFFFFFFFF)
                            {
                                std::memcpy(&bytes8, &dlge_data->data()[position], sizeof(bytes8));

                                for (uint64_t k = 0; k < sizeof(bytes8); k++)
                                {
                                    json_meta_data.push_back(dlge_data->data()[position + k]);
                                }

                                position += sizeof(bytes8);

                                temp_language_string_metas.push_back(bytes8);
                            }
                            else
                            {
                                done_scanning_languages = true;
                            }
                        }
                        else
                        {
                            done_scanning_languages = true;
                        }

                        number_of_languages++;
                    }

                    if ((position + 0x1) <= decompressed_size)
                    {
                        std::memcpy(&bytes1, &dlge_data->data()[position], sizeof(bytes1));

                        if (bytes1 == 1)
                        {
                            text_available = 1;

                            position += sizeof(bytes1);
                        }
                        else
                        {
                            text_available = 0;
                        }
                    }
                    else
                    {
                        text_available = 0;
                    }

                    text_count++;
                }

                if ((decompressed_size - position) > 0)
                {
                    for (uint64_t k = 0; k < (decompressed_size - position); k++)
                    {
                        json_meta_data.push_back(dlge_data->data()[position + k]);
                    }
                }

                if (!dlge_has_text || !do_not_break)
                    continue;

                if (output_to_string)
                {
                    std::stringstream ss;

                    ss << std::setw(4) << json_object << std::endl;

                    localization_string = ss.str();

                    localization_json = json_object;
                }
                else
                {
                    std::string json_path = current_path + "\\" + hash_file_name + ".JSON";

                    std::ofstream json_file = std::ofstream(json_path, std::ofstream::binary);

                    if (!json_file.good())
                    {
                        LOG_AND_EXIT("Error: JSON file " + json_path + " could not be created.");
                    }

                    json_file << std::setw(4) << json_object << std::endl;

                    json_file.close();

                    std::string json_meta_path = current_path + "\\" + hash_file_name + ".JSON.meta";

                    std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ofstream::binary);

                    if (!json_meta_file.good())
                    {
                        LOG_AND_EXIT("Error: JSON meta file " + json_meta_path + " could not be created.");
                    }

                    char char4[4];

                    std::memcpy(&char4, &number_of_dlge_categories, sizeof(bytes4));
                    json_meta_file.write(char4, sizeof(bytes4));

                    std::memcpy(&char4, &number_of_languages, sizeof(bytes4));
                    json_meta_file.write(char4, sizeof(bytes4));

                    json_meta_file.write(json_meta_data.data(), json_meta_data.size());

                    json_meta_file.close();

                    std::vector<hash_depends_variables> hash_depends_data;

                    uint64_t hash_value = rpkgs.at(i).hash.at(hash_index).hash_value;

                    for (uint64_t a = 0; a < rpkgs.size(); a++)
                    {
                        std::unordered_map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(a).hash_map.find(hash_value);

                        hash_depends_variables temp_hash_depends_data;

                        temp_hash_depends_data.rpkg_file_name = rpkgs.at(a).rpkg_file_name;

                        if (it2 == rpkgs.at(a).hash_map.end())
                            continue;

                        temp_hash_depends_data.hash_value = hash_value;

                        uint32_t temp_hash_reference_count = rpkgs.at(a).hash.at(it2->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                        if (temp_hash_reference_count > 0)
                        {
                            for (uint64_t b = 0; b < temp_hash_reference_count; b++)
                            {
                                std::vector<std::string> dependency_in_rpkg_file;

                                bool found = false;

                                for (uint64_t c = 0; c < rpkgs.size(); c++)
                                {
                                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(c).hash_map.find(rpkgs.at(a).hash.at(it2->second).hash_reference_data.hash_reference.at(b));

                                    if (it3 == rpkgs.at(c).hash_map.end())
                                        continue;
                                    
                                    if (!found)
                                    {
                                        temp_hash_depends_data.hash_dependency_file_name.push_back(util::uint64_t_to_hex_string(rpkgs.at(c).hash.at(it3->second).hash_value) + "." + rpkgs.at(c).hash.at(it3->second).hash_resource_type);

                                        std::string hash_link_file_name = current_path + "\\" + hash_file_name + "_" + util::uint64_t_to_hex_string(rpkgs.at(c).hash.at(it3->second).hash_value) + "." + rpkgs.at(c).hash.at(it3->second).hash_resource_type;

                                        if (rpkgs.at(c).hash.at(it3->second).hash_resource_type == "WWES")
                                        {
                                            if (!file::path_exists(hash_link_file_name))
                                            {
                                                std::ofstream json_meta_hash_link_file = std::ofstream(hash_link_file_name, std::ofstream::binary);

                                                if (!json_meta_hash_link_file.good())
                                                {
                                                    LOG_AND_EXIT("Error: JSON meta file " + hash_link_file_name + " could not be created.");
                                                }

                                                json_meta_hash_link_file.close();
                                            }
                                        }
                                    }

                                    found = true;

                                    dependency_in_rpkg_file.push_back(rpkgs.at(c).rpkg_file_name);
                                }

                                if (!found)
                                {
                                    temp_hash_depends_data.hash_dependency_file_name.push_back(util::uint64_t_to_hex_string(rpkgs.at(a).hash.at(it2->second).hash_reference_data.hash_reference.at(b)));
                                }

                                temp_hash_depends_data.hash_dependency_map[hash_value] = temp_hash_depends_data.hash_dependency_map.size();

                                temp_hash_depends_data.hash_dependency.push_back(util::uint64_t_to_hex_string(hash_value));

                                temp_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
                            }
                        }

                        hash_depends_data.push_back(temp_hash_depends_data);
                    }
                }
            }
        }
    }

    ss.str(std::string());

    ss << "Extracting DLGE as JSON: Done";
        
    if (log_output)
        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    if (!output_to_string)
    {
        percent_progress = static_cast<uint32_t>(100);

        task_single_status = TASK_SUCCESSFUL;
        task_multiple_status = TASK_SUCCESSFUL;
    }
}
