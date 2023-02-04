#include "asva.h"
#include "rpkg_function.h"
#include "file_reader.hpp"
#include "stream_reader.hpp"
#include "stream_writer.hpp"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "file.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <set>
#include <filesystem>
#include <typeinfo>

asva::asva() = default;

asva::asva(uint64_t rpkgs_index, uint64_t hash_index)
{
    asva_rpkg_index = rpkgs_index;
    asva_hash_index = hash_index;

    asva_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." + rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    uint64_t asva_hash_size;

    if (rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.lz4ed)
    {
        asva_hash_size = rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.header.data_size;

        if (rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.xored)
        {
            asva_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        asva_hash_size = rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.resource.size_final;
    }

    asva_input_data = std::vector<char>(asva_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(asva_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(asva_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.header.data_offset, file.beg);
    file.read(asva_input_data.data(), asva_hash_size);
    file.close();

    if (rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.xored)
    {
        crypto::xor_data(asva_input_data.data(), (uint32_t)asva_hash_size);
    }

    const uint32_t asva_decompressed_size = rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.resource.size_final;

    asva_output_data = std::vector<char>(asva_decompressed_size, 0);

    if (rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).data.lz4ed)
    {
        LZ4_decompress_safe(asva_input_data.data(), asva_output_data.data(), (int)asva_hash_size, asva_decompressed_size);

        asva_data = asva_output_data;
    }
    else
    {
        asva_data = asva_input_data;
    }

    std::vector<char>().swap(asva_output_data);
    std::vector<char>().swap(asva_input_data);
    
    // Load ASVA data in StreamReader
    StreamReader asva_reader(asva_data.data(), asva_data.size());

    asva_reader.Read<asva::Header>(&header);
    header.data_size = util::uint32_t_byteswap(header.data_size);

    table.resize(header.entry_count);
    asva_reader.Read<asva::Table>(table.data(), header.entry_count);

    if (log_output)
    {
        std::cout << header.bin1 << std::endl;
        std::cout << util::uint32_t_to_hex_string(header.data_size) << std::endl;
        std::cout << util::uint8_t_to_hex_string(header.type1) << std::endl;
        std::cout << util::uint8_t_to_hex_string(header.type2) << std::endl;
        std::cout << util::uint32_t_to_hex_string(header.table_offset) << std::endl;
        std::cout << util::uint32_t_to_hex_string(header.strings_offset1) << std::endl;
        std::cout << util::uint32_t_to_hex_string(header.strings_offset2) << std::endl;
        std::cout << util::uint32_t_to_hex_string(header.entry_count) << std::endl;

        for (auto& t : table)
        {
            std::cout << util::uint32_t_to_hex_string(t.string_length) << std::endl;
            std::cout << util::uint32_t_to_hex_string(t.string_offset) << std::endl;
            std::cout << util::uint32_t_to_hex_string(0xFFFFFFFF - t.depends_index) << std::endl;
            std::cout << std::string(&asva_data[t.string_offset + 0x10]) << std::endl;
        }
    }
}

void asva::generate_json()
{
    json = nlohmann::ordered_json::object();

    json["ASVA"] = util::hash_to_ioi_string(rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).hash_value, true);

    json["Variations"] = nlohmann::ordered_json::array();

    for (auto& t : table)
    {
        nlohmann::ordered_json temp_json = nlohmann::ordered_json::object();

        uint32_t depends_index = 0xFFFFFFFF - t.depends_index;

        if (depends_index < rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).hash_reference_data.hash_reference.size())
        {
            temp_json[std::string(&asva_data[t.string_offset + 0x10])] = util::hash_to_ioi_string(rpkgs.at(asva_rpkg_index).hash.at(asva_hash_index).hash_reference_data.hash_reference.at(depends_index), true);
        }

        json["Variations"].push_back(temp_json);
    }
}

void asva::write_json_to_file(std::string output_path)
{
    std::ofstream json_file = std::ofstream(output_path, std::ofstream::binary);

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

asva::asva(std::string json_path, std::string output_path)
{
    std::ifstream input_json_file(json_path);

    try
    {
        input_json_file >> json;
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;
        ss << "Error: " << json_path << "\n"
            << "Error message: " << e.what() << '\n'
            << "Error exception id: " << e.id << '\n'
            << "Error byte position of error: " << e.byte;
        json_error = ss.str();
    }

    input_json_file.close();

    try
    {
        uint64_t asva_hash_value = 0;
        std::vector<uint64_t> depends;
        std::unordered_map<uint64_t, uint32_t> depends_map;
        std::unordered_map<std::string, uint32_t> variation_depends;

        std::string temp_string = json["ASVA"];

        if (temp_string.find("[") != std::string::npos)
            asva_hash_value = util::ioi_string_to_hash(temp_string);
        else
            asva_hash_value = std::strtoull(temp_string.c_str(), nullptr, 16);

        for (auto& variation : json["Variations"])
        {
            for (auto& v : variation.items())
            {
                temp_string = v.value();

                if (temp_string.length() > 0)
                {
                    uint64_t temp_hash = 0;

                    if (temp_string.find("[") != std::string::npos)
                        temp_hash = util::ioi_string_to_hash(temp_string);
                    else
                        temp_hash = std::strtoull(temp_string.c_str(), nullptr, 16);

                    if (temp_hash && depends_map.find(temp_hash) == depends_map.end())
                    {
                        depends_map[temp_hash] = depends_map.size();
                        depends.push_back(temp_hash);
                    }

                    variation_depends[std::string(v.key())] = depends_map[temp_hash];
                }
            }
        }

        if (!depends_map.size())
            return;

        std::vector<char> asva_meta_data;

        StreamWriter asva_meta_writer(&asva_meta_data);

        asva_meta_writer.Write<uint64_t>(asva_hash_value);
        asva_meta_writer.Write<uint64_t>(0);
        asva_meta_writer.Write<uint32_t>(0x80000001);
        std::string short_name = "ASVA";
        asva_meta_writer.Write(short_name);

        uint32_t depends_count = depends_map.size();
        uint32_t depends_count2 = depends_map.size();
        uint32_t hash_reference_table_size = depends_count2 * 0x8 + depends_count2 + 0x4;
        depends_count2 |= 0xC0000000;

        asva_meta_writer.Write<uint32_t>(hash_reference_table_size);
        asva_meta_writer.Write<uint32_t>(0);

        uint32_t asva_size_offset = asva_meta_data.size();

        asva_meta_writer.Write<uint32_t>(0);
        asva_meta_writer.Write<uint64_t>(0xFFFFFFFFFFFFFFFF);
        asva_meta_writer.Write<uint32_t>(depends_count2);
        asva_meta_writer.Write<uint8_t>(0x1F, depends_count);

        for (auto& depend : depends)
        {
            asva_meta_writer.Write<uint64_t>(depend);
        }

        StreamWriter asva_data_writer(&asva_data);
        short_name = "BIN1";
        asva_data_writer.Write(short_name);
        asva_data_writer.Write<uint32_t>(0x00020800);
        asva_data_writer.Write<uint64_t>(0);
        uint32_t variations_count = json["Variations"].size();
        asva_data_writer.Write<uint64_t>(0x20);
        uint64_t strings_offset = variations_count * 0x20 + 0x20;
        asva_data_writer.Write<uint64_t>(strings_offset);
        asva_data_writer.Write<uint64_t>(strings_offset);
        asva_data_writer.Write<uint32_t>(0);
        asva_data_writer.Write<uint32_t>(variations_count);

        asva_data_writer.Resize(variations_count * 0x20);

        uint32_t last_string_offset = 0;

        for (auto& variation : json["Variations"])
        {
            for (auto& v : variation.items())
            {
                std::string variation_string = v.key();
                asva_data_writer.Write<uint64_t>(variation_string.length() | 0x40000000);
                size_t data_offset = asva_data_writer.GetPosition();
                asva_data_writer.SeekToEnd();
                asva_data_writer.Write<uint32_t>(variation_string.length() + 1);
                asva_data_writer.Write(variation_string);
                asva_data_writer.Write<uint8_t>(0);
                strings_offset += 4;
                uint32_t string_offset_addition = variation_string.length() + 1;
                last_string_offset = asva_data_writer.GetPosition();
                while (asva_data_writer.GetPosition() % 4 != 0)
                {
                    asva_data_writer.Write<uint8_t>(0);
                    string_offset_addition++;
                }
                asva_data_writer.SeekTo(data_offset);
                asva_data_writer.Write<uint64_t>(strings_offset);
                strings_offset += string_offset_addition;
                asva_data_writer.Write<uint32_t>(0xFFFFFFFF);
                asva_data_writer.Write<uint32_t>(0xFFFFFFFF - variation_depends[std::string(v.key())]);
                asva_data_writer.Write<uint64_t>(0x0000000080000000);
            }
        }

        asva_data_writer.SeekToEnd();
        uint32_t data_section_size = last_string_offset - 0x10;
        data_section_size = util::uint32_t_byteswap(data_section_size);
        asva_data_writer.SeekTo(0x8);
        asva_data_writer.Write<uint32_t>(data_section_size);
        asva_data_writer.SeekTo(last_string_offset);

        asva_data_writer.Write<uint32_t>(0x12EBA5ED);
        asva_data_writer.Write<uint32_t>((variations_count + 3) * 4 + 4);
        asva_data_writer.Write<uint32_t>(variations_count + 3);
        asva_data_writer.Write<uint32_t>(0);
        asva_data_writer.Write<uint32_t>(8);
        asva_data_writer.Write<uint32_t>(0x10);

        for (uint32_t v = 0; v < variations_count; v++)
        {
            asva_data_writer.Write<uint32_t>(v * 0x20 + 0x28);
        }

        asva_data_writer.Write<uint32_t>(0x64603664);
        asva_data_writer.Write<uint32_t>(variations_count * 8 + 4);
        asva_data_writer.Write<uint32_t>(variations_count * 2);

        for (uint32_t v = 0; v < variations_count; v++)
        {
            asva_data_writer.Write<uint32_t>(v * 0x20 + 0x30);
            asva_data_writer.Write<uint32_t>(v * 0x20 + 0x38);
        }

        std::ofstream asva_file;
        std::ofstream asva_meta_file;

        file::create_directories(output_path);
        asva_file = std::ofstream(file::output_path_append(util::uint64_t_to_hex_string(asva_hash_value) + ".ASVA", output_path), std::ofstream::binary);
        asva_meta_file = std::ofstream(file::output_path_append(util::uint64_t_to_hex_string(asva_hash_value) + ".ASVA.meta", output_path), std::ofstream::binary);

        asva_file.write(asva_data.data(), asva_data.size());
        asva_file.close();
        uint32_t asva_data_size = asva_data.size();
        std::memcpy(&asva_meta_data.data()[asva_size_offset], &asva_data_size, 0x4);
        asva_meta_file.write(asva_meta_data.data(), asva_meta_data.size());
        asva_meta_file.close();
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;
        ss << "Error: " << json_path << "\n"
            << "Error message: " << e.what() << '\n'
            << "Error exception id: " << e.id << '\n'
            << "Error byte position of error: " << e.byte;
        json_error = ss.str();
    }
}