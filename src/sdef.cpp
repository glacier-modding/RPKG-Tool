#include "sdef.h"
#include "rpkg_function.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "file.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <filesystem>
#include <typeinfo>

sdef::sdef() = default;

sdef::sdef(uint64_t rpkgs_index, uint64_t hash_index) {
    sdef_rpkg_index = rpkgs_index;
    sdef_hash_index = hash_index;

    sdef_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." +
                     rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    uint64_t sdef_hash_size;

    if (rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.lz4ed) {
        sdef_hash_size = rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.header.data_size;

        if (rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.xored) {
            sdef_hash_size &= 0x3FFFFFFF;
        }
    } else {
        sdef_hash_size = rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.resource.size_final;
    }

    sdef_input_data = std::vector<char>(sdef_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(sdef_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(sdef_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.header.data_offset, file.beg);
    file.read(sdef_input_data.data(), sdef_hash_size);
    file.close();

    if (rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.xored) {
        crypto::xor_data(sdef_input_data.data(), (uint32_t) sdef_hash_size);
    }

    const uint32_t sdef_decompressed_size = rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.resource.size_final;

    sdef_output_data = std::vector<char>(sdef_decompressed_size, 0);

    if (rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).data.lz4ed) {
        LZ4_decompress_safe(sdef_input_data.data(), sdef_output_data.data(), (int) sdef_hash_size,
                            sdef_decompressed_size);

        sdef_data = sdef_output_data;
    } else {
        sdef_data = sdef_input_data;
    }

    std::vector<char>().swap(sdef_output_data);
    std::vector<char>().swap(sdef_input_data);
}

void sdef::generate_json(std::string output_path) {
    uint32_t position = 0;
    uint32_t sdef_index = 0;
    uint32_t dlge_bytes = 0;

    std::memcpy(&sdef_entry_count, &sdef_data.data()[position], 0x4);
    position += 0x4;

    json["$schema"] = "https://glaciermodding.org/schemas/sdef.schema.json";

    for (uint32_t i = 0; i < sdef_entry_count; i++) {
        std::memcpy(&sdef_index, &sdef_data.data()[position], 0x4);
        position += 0x4;

        std::memcpy(&dlge_bytes, &sdef_data.data()[position], 0x4);
        position += 0x4;

        nlohmann::ordered_json temp_json;

        if (dlge_bytes == 0xFFFFFFFF) {
            //temp_json["Index"] = sdef_index;
            temp_json["DLGE"] = "";
            temp_json["_Path_Hint"] = "";
            temp_json["_Slot_Hint"] = slots[sdef_index];
        } else {
            //temp_json["Index"] = sdef_index;
            //temp_json["DLGE"] = util::hash_to_ioi_string(rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_reference_data.hash_reference.at(dlge_bytes), true);
            //temp_json["Path"] = util::hash_to_hash_list_string(rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_reference_data.hash_reference.at(dlge_bytes));
            //temp_json["Hint"] = slots[sdef_index];

            //temp_json["Index"] = sdef_index;

            std::string dlge_hash_value = util::uint64_t_to_hex_string(
                    rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_reference_data.hash_reference.at(
                            dlge_bytes));
            std::string dlge_path = util::hash_to_ioi_string(
                    rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_reference_data.hash_reference.at(
                            dlge_bytes), true);

            temp_json["DLGE"] = dlge_path;

            if (dlge_hash_value == dlge_path) {
                temp_json["_Path_Hint"] = util::hash_to_hash_list_string(
                        rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_reference_data.hash_reference.at(
                                dlge_bytes));
            }

            temp_json["_Slot_Hint"] = slots[sdef_index];
        }

        json["SDEF"].push_back(temp_json);
    }

    file::create_directories(output_path);

    std::ofstream json_file = std::ofstream(file::output_path_append(
                                                    util::uint64_t_to_hex_string(rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_value) + "." +
                                                    rpkgs.at(sdef_rpkg_index).hash.at(sdef_hash_index).hash_resource_type + ".JSON", output_path),
                                            std::ofstream::binary);

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

sdef::sdef(std::string sdef_path, std::string sdef_meta_path, uint64_t hash_value, std::string output_path,
           bool output_path_is_file) {
    std::ifstream sdef_file(sdef_path, std::ifstream::binary);

    sdef_file.seekg(0, sdef_file.end);
    uint64_t sdef_file_size = sdef_file.tellg();
    sdef_file.seekg(0, sdef_file.beg);

    sdef_data = std::vector<char>(sdef_file_size, 0);

    sdef_file.read(sdef_data.data(), sdef_file_size);

    sdef_file.close();

    std::ifstream sdef_meta_file(sdef_meta_path, std::ifstream::binary);

    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    char input[1024];

    std::string json_string = "{";

    sdef_meta_file.seekg(0, sdef_meta_file.beg);

    sdef_meta_file.read(input, sizeof(bytes8));
    std::memcpy(&bytes8, input, sizeof(bytes8));
    meta_data.hash_value = bytes8;

    sdef_meta_file.read(input, sizeof(bytes8));
    std::memcpy(&bytes8, input, sizeof(bytes8));
    meta_data.data.header.data_offset = bytes8;

    sdef_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.header.data_size = bytes4;

    if ((meta_data.data.header.data_size & 0x3FFFFFFF) != 0) {
        meta_data.data.lz4ed = true;
    } else {
        meta_data.data.lz4ed = false;
    }

    if ((meta_data.data.header.data_size & 0x80000000) == 0x80000000) {
        meta_data.data.xored = true;
    } else {
        meta_data.data.xored = false;
    }

    sdef_meta_file.read(input, sizeof(bytes4));
    input[4] = 0x0;
    meta_data.hash_resource_type = std::string(input);

    sdef_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.reference_table_size = bytes4;

    sdef_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.reference_table_dummy = bytes4;

    sdef_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_final = bytes4;

    sdef_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_in_memory = bytes4;

    sdef_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_in_video_memory = bytes4;

    hash_reference_variables temp_hash_reference_data;

    if (meta_data.data.resource.reference_table_size != 0x0) {
        temp_hash_reference_data.hash_value = meta_data.hash_value;

        sdef_meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        temp_hash_reference_data.hash_reference_count = bytes4;

        uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
            sdef_meta_file.read(input, sizeof(bytes1));
            std::memcpy(&bytes1, input, sizeof(bytes1));
            temp_hash_reference_data.hash_reference_type.push_back(bytes1);
        }

        for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
            sdef_meta_file.read(input, sizeof(bytes8));
            std::memcpy(&bytes8, input, sizeof(bytes8));
            temp_hash_reference_data.hash_reference.push_back(bytes8);
        }
    } else {
        temp_hash_reference_data.hash_reference_count = 0x0;
    }

    meta_data.hash_reference_data = temp_hash_reference_data;

    sdef_meta_file.close();

    uint32_t position = 0;
    uint32_t sdef_index = 0;
    uint32_t dlge_bytes = 0;

    std::memcpy(&sdef_entry_count, &sdef_data.data()[position], 0x4);
    position += 0x4;

    json["$schema"] = "https://glaciermodding.org/schemas/sdef.schema.json";

    for (uint32_t i = 0; i < sdef_entry_count; i++) {
        std::memcpy(&sdef_index, &sdef_data.data()[position], 0x4);
        position += 0x4;

        std::memcpy(&dlge_bytes, &sdef_data.data()[position], 0x4);
        position += 0x4;

        nlohmann::ordered_json temp_json;

        if (dlge_bytes == 0xFFFFFFFF) {
            //temp_json["Index"] = sdef_index;
            temp_json["DLGE"] = "";
            temp_json["_Path_Hint"] = "";
            temp_json["_Slot_Hint"] = slots[sdef_index];
        } else {
            //temp_json["Index"] = sdef_index;
            //temp_json["DLGE"] = util::hash_to_ioi_string(meta_data.hash_reference_data.hash_reference.at(dlge_bytes), true);
            //temp_json["Path"] = util::hash_to_hash_list_string(meta_data.hash_reference_data.hash_reference.at(dlge_bytes));
            //temp_json["Hint"] = slots[sdef_index];

            //temp_json["Index"] = sdef_index;

            std::string dlge_hash_value = util::uint64_t_to_hex_string(
                    meta_data.hash_reference_data.hash_reference.at(dlge_bytes));
            std::string dlge_path = util::hash_to_ioi_string(
                    meta_data.hash_reference_data.hash_reference.at(dlge_bytes), true);

            temp_json["DLGE"] = dlge_path;

            if (dlge_hash_value == dlge_path) {
                temp_json["_Path_Hint"] = util::hash_to_hash_list_string(
                        meta_data.hash_reference_data.hash_reference.at(dlge_bytes));
            }

            temp_json["_Slot_Hint"] = slots[sdef_index];
        }

        json["SDEF"].push_back(temp_json);
    }

    std::ofstream json_file;

    if (output_path_is_file) {
        json_file = std::ofstream(output_path, std::ofstream::binary);
    } else {
        file::create_directories(output_path);
        json_file = std::ofstream(
                file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".SDEF.JSON", output_path),
                std::ofstream::binary);
    }

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

sdef::sdef(std::string json_path, uint64_t hash_value, std::string output_path, bool output_path_is_file) {
    std::ifstream input_json_file(json_path);

    try {
        input_json_file >> json;
    }
    catch (json::parse_error& e) {
        std::stringstream ss;
        ss << "Error: " << json_path << "\n"
           << "Error message: " << e.what() << '\n'
           << "Error exception id: " << e.id << '\n'
           << "Error byte position of error: " << e.byte;
        json_error = ss.str();
    }

    input_json_file.close();

    try {
        std::vector<uint64_t> dlge_hash_depends;

        dlge_hash_depends.resize(json["SDEF"].size());

        uint32_t sdef_size = (uint32_t) json["SDEF"].size() * 0x8 + 0x4;

        sdef_data = std::vector<char>(sdef_size, 0);

        uint32_t position = 0;

        uint32_t sdef_entry_count = json["SDEF"].size();

        std::memcpy(&sdef_data[position], &sdef_entry_count, 0x4);
        position += 0x4;

        uint32_t dlge_hash_depends_count = 0;

        for (uint32_t i = 0; i < json["SDEF"].size(); i++) {
            //uint32_t sdef_index = json["SDEF"][i]["Index"];
            uint32_t sdef_index = i;

            position = sdef_index * 0x8 + 0x4;

            std::memcpy(&sdef_data[position], &sdef_index, 0x4);
            position += 0x4;

            std::string dlge_hash = json["SDEF"][i]["DLGE"];

            uint64_t dlge_hash_value = 0;

            if (dlge_hash == "") {
                dlge_hash_value = 0xFFFFFFFFFFFFFFFF;

                uint32_t dlge_missing = 0xFFFFFFFF;

                std::memcpy(&sdef_data[position], &dlge_missing, 0x4);
            } else {
                dlge_hash_value = util::ioi_string_to_hash(dlge_hash);

                std::memcpy(&sdef_data[position], &dlge_hash_depends_count, 0x4);

                dlge_hash_depends_count++;
            }

            dlge_hash_depends[sdef_index] = dlge_hash_value;
        }

        std::vector<char> meta_data;

        for (uint32_t k = 0; k < sizeof(hash_value); k++) {
            meta_data.push_back(*((char*) &hash_value + k));
        }

        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x1);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x80);
        meta_data.push_back('S');
        meta_data.push_back('D');
        meta_data.push_back('E');
        meta_data.push_back('F');

        uint32_t hash_reference_table_size = dlge_hash_depends_count * 0x8 + dlge_hash_depends_count + 0x4;

        dlge_hash_depends_count = dlge_hash_depends_count | 0xC0000000;

        for (uint32_t k = 0; k < sizeof(hash_reference_table_size); k++) {
            meta_data.push_back(*((char*) &hash_reference_table_size + k));
        }

        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);

        uint32_t sdef_size_offset = meta_data.size();
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);

        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);
        meta_data.push_back(0xFF);

        for (uint32_t k = 0; k < sizeof(dlge_hash_depends_count); k++) {
            meta_data.push_back(*((char*) &dlge_hash_depends_count + k));
        }

        for (uint32_t k = 0; k < dlge_hash_depends.size(); k++) {
            if (dlge_hash_depends[k] != 0xFFFFFFFFFFFFFFFF) {
                meta_data.push_back(0x5F);
            }
        }

        for (uint32_t i = 0; i < dlge_hash_depends.size(); i++) {
            if (dlge_hash_depends[i] != 0xFFFFFFFFFFFFFFFF) {
                for (uint32_t k = 0; k < sizeof(dlge_hash_depends.at(i)); k++) {
                    meta_data.push_back(*((char*) &dlge_hash_depends.at(i) + k));
                }
            }
        }

        std::ofstream sdef_file;
        std::ofstream meta_file;

        if (output_path_is_file) {
            sdef_file = std::ofstream(output_path, std::ofstream::binary);
            meta_file = std::ofstream(output_path + ".meta", std::ofstream::binary);
        } else {
            file::create_directories(output_path);
            sdef_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".SDEF", output_path),
                    std::ofstream::binary);
            meta_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".SDEF.meta", output_path),
                    std::ofstream::binary);
        }

        sdef_file.write(sdef_data.data(), sdef_data.size());

        sdef_file.close();

        uint32_t sdef_data_size = sdef_data.size();

        std::memcpy(&meta_data.data()[sdef_size_offset], &sdef_data_size, 0x4);

        meta_file.write(meta_data.data(), meta_data.size());

        meta_file.close();
    }
    catch (json::parse_error& e) {
        std::stringstream ss;
        ss << "Error: " << json_path << "\n"
           << "Error message: " << e.what() << '\n'
           << "Error exception id: " << e.id << '\n'
           << "Error byte position of error: " << e.byte;
        json_error = ss.str();
    }
}