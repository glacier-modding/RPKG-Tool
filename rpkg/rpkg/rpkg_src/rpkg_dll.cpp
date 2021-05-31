#include "rpkg_dll.h"
#include "global.h"
#include "task.h"
#include "rpkg_function.h"
#include "crypto.h"
#include "console.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <sstream>
#include <fstream>
#include <iomanip>

int load_hash_list()
{
    generic_function::load_hash_list();

    return hash_list_version;
}

char* get_hash_list_string(char* hash_string)
{
    uint64_t hash = std::strtoull(hash_string, nullptr, 16);

    std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(hash);

    if (it != hash_list_hash_map.end())
    {
        hash_list_string = hash_list_hash_strings.at(it->second);
    }
    else
    {
        hash_list_string = "";
    }

    return &hash_list_string[0];
}

int task_execute(char* csharp_command, char* csharp_input_path, char* csharp_filter, char* csharp_search, char* csharp_search_type, char* csharp_output_path)
{
    gui_control = READY;
    task_single_status = READY;
    task_multiple_status = READY;

    std::string command = std::string(csharp_command);
    std::string input_path = std::string(csharp_input_path);
    std::string filter = std::string(csharp_filter);
    std::string search = std::string(csharp_search);
    std::string search_type = std::string(csharp_search_type);
    std::string output_path = std::string(csharp_output_path);

    task::execute(command, input_path, filter, search, search_type, output_path);

    return 0;
}

int get_current_percent()
{
    return percent_progress;
}

int reset_task_status()
{
    task_single_status = READY;
    task_multiple_status = READY;
    gui_control = READY;

    return 0;
}

int get_task_single_status()
{
    return task_single_status;
}

int reset_task_single_status()
{
    task_single_status = 0;

    return task_single_status;
}

int get_task_multiple_status()
{
    return task_multiple_status;
}

int reset_task_multiple_status()
{
    task_multiple_status = 0;

    return task_multiple_status;
}

int set_gui_control(int gui_control_value)
{
    gui_control = gui_control_value;

    return gui_control;
}

int get_gui_control()
{
    return gui_control;
}

char* get_timing_string()
{
    return &timing_string[0];
}

char* get_task_status_string()
{
    return &task_status_string[0];
}

uint32_t get_resource_types_count(char* rpkg_file)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            return (uint32_t)rpkgs.at(i).hash_resource_types.size();
        }
    }
}

char* get_resource_types_at(char* rpkg_file, uint32_t at_index)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            return &rpkgs.at(i).hash_resource_types.at(at_index)[0];
        }
    }
}

uint64_t get_resource_types_data_size(char* rpkg_file, char* resource_type)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash_resource_types.size(); j++)
            {
                if (rpkgs.at(i).hash_resource_types.at(j) == resource_type)
                {
                    return rpkgs.at(i).hash_resource_types_data_size.at(j);
                }
            }
        }
    }
}

uint32_t get_hash_based_on_resource_type_count(char* rpkg_file, char* resource_type)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash_resource_types.size(); j++)
            {
                if (rpkgs.at(i).hash_resource_types.at(j) == resource_type)
                {
                    return (uint32_t)rpkgs.at(i).hashes_based_on_resource_types.at(j).size();
                }
            }
        }
    }
}

char* get_hash_based_on_resource_type_at(char* rpkg_file, char* resource_type, uint32_t at_index)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash_resource_types.size(); j++)
            {
                if (rpkgs.at(i).hash_resource_types.at(j) == resource_type)
                {
                    return &rpkgs.at(i).hashes_based_on_resource_types.at(j).at(at_index)[0];
                }
            }
        }
    }
}

uint32_t get_all_hashes_in_rpkg_count(char* rpkg_file)
{
    uint32_t all_hashes_in_rpkg_count = 0;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash_resource_types.size(); j++)
            {
                all_hashes_in_rpkg_count += (uint32_t)rpkgs.at(i).hashes_based_on_resource_types.at(j).size();
            }
        }
    }

    return all_hashes_in_rpkg_count;
}

char* get_all_hashes_in_rpkg_at(char* rpkg_file, uint32_t at_index)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash_resource_types.size(); j++)
            {
                return &rpkgs.at(i).hash.at(at_index).hash_string[0];
            }
        }
    }
}

uint64_t get_all_hashes_in_rpkg_data_size(char* rpkg_file)
{
    uint64_t temp_total_data_size = 0;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash_resource_types.size(); j++)
            {
                temp_total_data_size += rpkgs.at(i).hash_resource_types_data_size.at(j);
            }
        }
    }

    return temp_total_data_size;
}

char* get_hash_details(char* rpkg_file, char* hash_string)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            uint64_t hash_value = std::strtoull(hash_string, nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

            if (it != rpkgs.at(i).hash_map.end())
            {
                response_string = "";

                std::stringstream ss;

                ss << std::hex;// << hash_string << " in RPKG file " << rpkgs.at(i).rpkg_file_path << ":" << std::endl;
                ss << "  - Data offset: 0x" << std::uppercase << rpkgs.at(i).hash.at(it->second).hash_offset << std::nouppercase << std::endl;
                ss << "  - Data size: 0x" << std::uppercase << (rpkgs.at(i).hash.at(it->second).hash_size & 0x3FFFFFFF) << std::nouppercase << std::endl;

                if (rpkgs.at(i).hash.at(it->second).is_lz4ed)
                {
                    ss << "  - LZ4: True" << std::endl;
                }
                else
                {
                    ss << "  - LZ4: False" << std::endl;
                }

                if (rpkgs.at(i).hash.at(it->second).is_xored)
                {
                    ss << "  - XOR: True" << std::endl;
                }
                else
                {
                    ss << "  - XOR: False" << std::endl;
                }

                ss << "  - Resource type: " << rpkgs.at(i).hash.at(it->second).hash_resource_type << std::endl;
                ss << "  - Hash reference table size: 0x" << std::uppercase << rpkgs.at(i).hash.at(it->second).hash_reference_table_size << std::nouppercase << std::endl;
                ss << "  - Forward hash depends: " << (rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF) << std::endl;
                ss << "  - Final size: 0x" << std::uppercase << rpkgs.at(i).hash.at(it->second).hash_size_final << std::nouppercase << std::endl;
                ss << "  - Size in memory: 0x" << std::uppercase << rpkgs.at(i).hash.at(it->second).hash_size_in_memory << std::nouppercase << std::endl;
                ss << "  - Size in video memory: 0x" << std::uppercase << rpkgs.at(i).hash.at(it->second).hash_size_in_video_memory << std::nouppercase << std::endl;

                ss << std::endl;

                uint32_t hash_reference_count = rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                ss << std::dec << "Depends on " << hash_reference_count << " other hash files/resources:" << std::endl;

                for (uint32_t j = 0; j < hash_reference_count; j++)
                {
                    std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j));

                    if (it2 != hash_list_hash_map.end())
                    {
                        ss << "  - " << hash_list_hash_file_names.at(it2->second) << " " << hash_list_hash_strings.at(it2->second) << std::endl;
                    }
                    else
                    {
                        ss << "  - " << rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_string.at(j) << std::endl;
                    }
                }

                response_string = ss.str();

                return &response_string[0];
            }
        }
    }
}

uint32_t get_hash_in_rpkg_size(char* rpkg_file, char* hash_string)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            uint64_t hash_value = std::strtoull(hash_string, nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

            if (it != rpkgs.at(i).hash_map.end())
            {
                uint32_t hash_size;

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

                uint32_t decompressed_size = rpkgs.at(i).hash.at(it->second).hash_size_final;

                if (rpkgs.at(i).hash.at(it->second).is_lz4ed)
                {

                    return decompressed_size;
                }
                else
                {
                    return hash_size;
                }
            }
        }
    }
}

char* get_hash_in_rpkg_data(char* rpkg_file, char* hash_string)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            uint64_t hash_value = std::strtoull(hash_string, nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

            if (it != rpkgs.at(i).hash_map.end())
            {
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

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                if (!file.good())
                {
                    response_string = "failed";
                    return &response_string[0];
                }

                file.seekg(rpkgs.at(i).hash.at(it->second).hash_offset, file.beg);
                file.read(input_data.data(), hash_size);

                if (rpkgs.at(i).hash.at(it->second).is_xored == 1)
                {
                    crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                }

                uint32_t decompressed_size = rpkgs.at(i).hash.at(it->second).hash_size_final;
                std::vector<char> lz4_output_data(decompressed_size, 0);

                if (rpkgs.at(i).hash.at(it->second).is_lz4ed)
                {
                    LZ4_decompress_safe(input_data.data(), lz4_output_data.data(), (int)hash_size, decompressed_size);

                    response_data = lz4_output_data;
                }
                else
                {
                    response_data = input_data;
                }

                return &response_data[0];
            }
        }
    }
}

int clear_hash_data_vector()
{
    std::vector<char>().swap(response_data);

    return (int)response_data.size();
}

char* get_hash_in_rpkg_data_in_hex_view(char* rpkg_file, char* hash_string)
{
    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            uint64_t hash_value = std::strtoull(hash_string, nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

            if (it != rpkgs.at(i).hash_map.end())
            {
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

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                if (!file.good())
                {
                    response_string = "failed";
                    return &response_string[0];
                }

                file.seekg(rpkgs.at(i).hash.at(it->second).hash_offset, file.beg);
                file.read(input_data.data(), hash_size);

                if (rpkgs.at(i).hash.at(it->second).is_xored == 1)
                {
                    crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                }

                uint32_t decompressed_size = rpkgs.at(i).hash.at(it->second).hash_size_final;
                std::vector<char> lz4_output_data(decompressed_size, 0);

                uint32_t data_size = 0;

                if (rpkgs.at(i).hash.at(it->second).is_lz4ed)
                {
                    LZ4_decompress_safe(input_data.data(), lz4_output_data.data(), (int)hash_size, decompressed_size);

                    data_size = decompressed_size;
                    response_data = lz4_output_data;
                }
                else
                {
                    data_size = hash_size;
                    response_data = input_data;
                }

                response_string = "";

                std::stringstream ss;

                ss << "Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F Decoded text\n\n";

                uint32_t count1 = 0;
                uint32_t index = 0;

                while (count1 < data_size)
                {
                    uint32_t count2 = 0;
                    uint32_t cursor = 0;

                    ss << std::hex << std::setw(8) << std::setfill('0') << std::uppercase << count1 << " ";

                    cursor += 9;

                    while (count2 < 16 && count1 < data_size)
                    {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)((unsigned char)response_data[count1]) << " ";

                        count1++;
                        count2++;
                        cursor += 3;
                    }

                    for (uint32_t j = cursor; j < 57; j++)
                    {
                        ss << " ";
                    }

                    for (uint32_t j = 0; j < count2; j++)
                    {
                        if ((int)((unsigned char)response_data[index]) > 0x20 && ((int)((unsigned char)response_data[index]) <= 0x80) || (int)((unsigned char)response_data[index]) >= 0xA0)
                        {
                            ss << response_data[index];
                        }
                        else
                        {
                            ss << ".";
                        }

                        index++;
                    }

                    ss << "\n";
                }

                response_string = ss.str();

                return &response_string[0];
            }
        }
    }
}

uint32_t generate_localization_string(char* rpkg_file, char* hash_string, char* resource_type)
{
    gui_control = READY;
    task_single_status = READY;
    task_multiple_status = READY;

    std::string input_path = std::string(rpkg_file);
    std::string filter = std::string(hash_string);
    std::string output_path = "";
    std::string resource = std::string(resource_type);

    if (resource == "DLGE")
    {
        rpkg_function::extract_dlge_to_json_from(input_path, filter, output_path, true);
    }
    else if (resource == "LOCR")
    {
        rpkg_function::extract_locr_to_json_from(input_path, filter, output_path, true);
    }
    else if (resource == "RTLV")
    {
        rpkg_function::extract_rtlv_to_json_from(input_path, filter, output_path, true);
    }

    return (uint32_t)localization_string.length();
}

char* get_localization_string()
{
    return &localization_string[0];
}

int get_hashes_with_no_reverse_depends(char* rpkg_file)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    hashes_with_no_reverse_depends = "";

    uint64_t total_hash_count = 0;
    uint64_t hash_count = 0;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        total_hash_count += rpkgs.at(i).hash.size();
    }

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    /*std::map<uint64_t, uint64_t> hashes_depends_map;

    std::string message = "Building " + std::string(rpkg_file) + "\'s hash dependecy map...";

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
        {
            if (((hash_count * (uint64_t)100000) / (uint64_t)total_hash_count) % (uint64_t)100 == 0 && hash_count > 0)
            {
                stringstream_length = console::update_console(message, total_hash_count, hash_count, start_time, stringstream_length);
            }

            hash_count++;

            uint32_t temp_hash_reference_count = rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint64_t k = 0; k < temp_hash_reference_count; k++)
            {
                std::map<uint64_t, uint64_t>::iterator it = hashes_depends_map.find(rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference.at(k));

                if (it == hashes_depends_map.end())
                {
                    hashes_depends_map[rpkgs.at(i).hash.at(j).hash_reference_data.hash_reference.at(k)] = hashes_depends_map.size();
                }
            }
        }
    }*/

    start_time = std::chrono::high_resolution_clock::now();

    std::string message = "Scanning hash dependecy map (of all imported RPKGs) for top level hash files/resources...";

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            for (uint64_t j = 0; j < rpkgs.at(i).hash.size(); j++)
            {
                if (((hash_count * (uint64_t)100000) / (uint64_t)total_hash_count) % (uint64_t)100 == 0 && hash_count > 0)
                {
                    stringstream_length = console::update_console(message, total_hash_count, hash_count, start_time, stringstream_length);
                }

                hash_count++;

                for (uint64_t k = 0; k < hashes_depends_map.size(); k++)
                {
                    if (hashes_depends_map_rpkg_file_paths.at(k) == rpkg_file)
                    {
                        std::map<uint64_t, uint64_t>::iterator it = hashes_depends_map.at(k).find(rpkgs.at(i).hash.at(j).hash_value);

                        if (it == hashes_depends_map.at(k).end())
                        {
                            hashes_with_no_reverse_depends.append(rpkgs.at(i).hash.at(j).hash_file_name);
                            hashes_with_no_reverse_depends.push_back(',');
                        }
                    }
                }
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;

    return 0;
}

char* get_hashes_with_no_reverse_depends_string()
{
    return &hashes_with_no_reverse_depends[0];
}

int get_direct_hash_depends(char* rpkg_file, char* hash_string)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    hash_direct_depends = "";

    std::string message = "Locating " + std::string(hash_string) + "\'s direct hash depends...";
    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        if (rpkgs.at(i).rpkg_file_path == rpkg_file)
        {
            uint64_t hash_value = std::strtoull(hash_string, nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(hash_value);

            if (it != rpkgs.at(i).hash_map.end())
            {
                uint32_t temp_hash_reference_count = rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint64_t j = 0; j < temp_hash_reference_count; j++)
                {
                    if (((j * (uint64_t)100000) / (uint64_t)temp_hash_reference_count) % (uint64_t)100 == 0 && j > 0)
                    {
                        stringstream_length = console::update_console(message, temp_hash_reference_count, j, start_time, stringstream_length);
                    }

                    for (uint64_t k = 0; k < rpkgs.size(); k++)
                    {
                        std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(k).hash_map.find(rpkgs.at(i).hash.at(it->second).hash_reference_data.hash_reference.at(j));

                        if (it2 != rpkgs.at(k).hash_map.end())
                        {
                            hash_direct_depends.append(rpkgs.at(k).hash.at(it2->second).hash_file_name);
                            hash_direct_depends.push_back('|');
                            hash_direct_depends.append(rpkgs.at(k).rpkg_file_name);
                            hash_direct_depends.push_back(',');
                        }
                    }
                }
            }
        }
    }

    task_single_status = TASK_SUCCESSFUL;
    task_multiple_status = TASK_SUCCESSFUL;

    return 0;
}

char* get_direct_hash_depends_string()
{
    return &hash_direct_depends[0];
}