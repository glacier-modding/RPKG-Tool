#include "material.h"
#include "rpkg_function.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "file.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <typeinfo>

material::material() = default;

material::material(uint64_t rpkgs_index, uint64_t hash_index) {
    mati_rpkg_index = rpkgs_index;
    mati_hash_index = hash_index;

    std::string mati_hash_file_name =
            util::uint64_t_to_hex_string(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_value) + "." +
            rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_resource_type;

    uint64_t mati_hash_size;

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.lz4ed) {
        mati_hash_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.header.data_size;

        if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.xored) {
            mati_hash_size &= 0x3FFFFFFF;
        }
    } else {
        mati_hash_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.resource.size_final;
    }

    std::vector<char> mati_input_data = std::vector<char>(mati_hash_size, 0);

    std::ifstream file3 = std::ifstream(rpkgs.at(mati_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file3.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(mati_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file3.seekg(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.header.data_offset, std::ifstream::beg);
    file3.read(mati_input_data.data(), mati_hash_size);
    file3.close();

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.xored == 1) {
        crypto::xor_data(mati_input_data.data(), (uint32_t) mati_hash_size);
    }

    uint32_t mati_decompressed_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.resource.size_final;

    std::vector<char> mati_output_data = std::vector<char>(mati_decompressed_size, 0);

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.lz4ed) {
        LZ4_decompress_safe(mati_input_data.data(), mati_output_data.data(), (int) mati_hash_size,
                            mati_decompressed_size);

        mati_data = mati_output_data;
    } else {
        mati_data = mati_input_data;
    }

    std::vector<char>().swap(mati_output_data);
    std::vector<char>().swap(mati_input_data);

    matt_hash_value = 0;
    matb_hash_value = 0;

    for (uint64_t a = 0; a < rpkgs.size(); a++) {
        if (matt_hash_value && matb_hash_value) continue;

        for (uint64_t b = 0; b < rpkgs.at(a).hash.size(); b++) {
            if (matt_hash_value && matb_hash_value) continue;

            const uint32_t temp_hash_reference_count =
                    rpkgs.at(a).hash.at(b).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint64_t c = 0; c < temp_hash_reference_count; c++) {
                if (rpkgs.at(a).hash.at(b).hash_resource_type != "MATT") continue;

                if (rpkgs.at(a).hash.at(b).hash_reference_data.hash_reference.at(c) !=
                    rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value)
                    continue;

                matt_hash_value = rpkgs.at(a).hash.at(b).hash_value;

                const uint32_t matt_hash_reference_count =
                        rpkgs.at(a).hash.at(b).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                if (matt_hash_reference_count <= 0) continue;

                for (uint64_t i = 0; i < matt_hash_reference_count; i++) {
                    for (uint64_t j = 0; j < rpkgs.size(); j++) {
                        if (matt_hash_value && matb_hash_value) continue;

                        auto it = rpkgs.at(j).hash_map.find(
                                rpkgs.at(a).hash.at(b).hash_reference_data.hash_reference.at(
                                        i));

                        if (it != rpkgs.at(j).hash_map.end()) {
                            if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "MATB") {
                                matb_hash_value = rpkgs.at(j).hash.at(it->second).hash_value;
                            }
                        }
                    }
                }
            }
        }
    }

    if (!matt_hash_value || !matb_hash_value) {
        return;
    }

    uint32_t rpkg_index = rpkg_function::get_latest_hash(matt_hash_value);

    if (rpkg_index != UINT32_MAX) {
        auto it6 = rpkgs.at(rpkg_index).hash_map.find(matt_hash_value);

        if (it6 != rpkgs.at(rpkg_index).hash_map.end()) {
            matt_rpkg_index = rpkg_index;
            matt_hash_index = it6->second;

            uint32_t rpkg_index2 = rpkg_function::get_latest_hash(matb_hash_value);

            if (rpkg_index2 != UINT32_MAX) {
                auto it4 = rpkgs.at(rpkg_index2).hash_map.find(
                        matb_hash_value);

                if (it4 != rpkgs.at(rpkg_index2).hash_map.end()) {
                    matb_rpkg_index = rpkg_index2;
                    matb_hash_index = it4->second;

                    uint64_t matt_hash_size;

                    if (rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).data.lz4ed) {
                        matt_hash_size = rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).data.header.data_size;

                        if (rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).data.xored) {
                            matt_hash_size &= 0x3FFFFFFF;
                        }
                    } else {
                        matt_hash_size = rpkgs.at(matt_rpkg_index).hash.at(
                                matt_hash_index).data.resource.size_final;
                    }

                    std::vector<char> matt_input_data = std::vector<char>(matt_hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(matt_rpkg_index).rpkg_file_path,
                                                       std::ifstream::binary);

                    if (!file.good()) {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(matt_rpkg_index).rpkg_file_path +
                                     " could not be read.");
                    }

                    file.seekg(rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).data.header.data_offset,
                               std::ifstream::beg);
                    file.read(matt_input_data.data(), matt_hash_size);
                    file.close();

                    if (rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).data.xored == 1) {
                        crypto::xor_data(matt_input_data.data(), (uint32_t) matt_hash_size);
                    }

                    uint32_t matt_decompressed_size = rpkgs.at(matt_rpkg_index).hash.at(
                            matt_hash_index).data.resource.size_final;

                    std::vector<char> matt_output_data = std::vector<char>(matt_decompressed_size, 0);

                    if (rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).data.lz4ed) {
                        LZ4_decompress_safe(matt_input_data.data(), matt_output_data.data(), (int) matt_hash_size,
                                            matt_decompressed_size);

                        matt_data = matt_output_data;
                    } else {
                        matt_data = matt_input_data;
                    }

                    std::vector<char>().swap(matt_output_data);
                    std::vector<char>().swap(matt_input_data);

                    std::string matb_hash_file_name = util::uint64_t_to_hex_string(
                            rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).hash_value) + "." +
                                                      rpkgs.at(matb_rpkg_index).hash.at(
                                                              matb_hash_index).hash_resource_type;

                    uint64_t matb_hash_size;

                    if (rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).data.lz4ed) {
                        matb_hash_size = rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).data.header.data_size;

                        if (rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).data.xored) {
                            matb_hash_size &= 0x3FFFFFFF;
                        }
                    } else {
                        matb_hash_size = rpkgs.at(matb_rpkg_index).hash.at(
                                matb_hash_index).data.resource.size_final;
                    }

                    std::vector<char> matb_input_data = std::vector<char>(matb_hash_size, 0);

                    std::ifstream file2 = std::ifstream(rpkgs.at(matb_rpkg_index).rpkg_file_path,
                                                        std::ifstream::binary);

                    if (!file2.good()) {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(matb_rpkg_index).rpkg_file_path +
                                     " could not be read.");
                    }

                    file2.seekg(rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).data.header.data_offset,
                                std::ifstream::beg);
                    file2.read(matb_input_data.data(), matb_hash_size);
                    file2.close();

                    if (rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).data.xored == 1) {
                        crypto::xor_data(matb_input_data.data(), (uint32_t) matb_hash_size);
                    }

                    uint32_t matb_decompressed_size = rpkgs.at(matb_rpkg_index).hash.at(
                            matb_hash_index).data.resource.size_final;

                    std::vector<char> matb_output_data = std::vector<char>(matb_decompressed_size, 0);

                    if (rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).data.lz4ed) {
                        LZ4_decompress_safe(matb_input_data.data(), matb_output_data.data(), (int) matb_hash_size,
                                            matb_decompressed_size);

                        matb_data = matb_output_data;
                    } else {
                        matb_data = matb_input_data;
                    }

                    std::vector<char>().swap(matb_output_data);
                    std::vector<char>().swap(matb_input_data);
                }
            }
        }
    }
}

void material::generate_json() {
    uint32_t position = 0;
    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    uint32_t header_offset = 0;
    uint32_t type_offset = 0;
    uint32_t texture_count = 0;
    uint32_t class_flags = 0;
    uint32_t instance_flags = 0;
    int32_t eres_index = -1;
    int32_t mate_index = -1;
    uint32_t instance_offset = 0;

    std::memcpy(&header_offset, &mati_data[position], BYTES4);
    position = header_offset;

    std::memcpy(&type_offset, &mati_data[position], BYTES4);
    position += 4;

    type = std::string(&mati_data[type_offset]);

    std::memcpy(&texture_count, &mati_data[position], BYTES4);
    position += 0x4;

    std::memcpy(&class_flags, &mati_data[position], BYTES4);
    position += 0x4;

    std::memcpy(&instance_flags, &mati_data[position], BYTES4);
    position += 0x4;

    std::memcpy(&eres_index, &mati_data[position], BYTES4);
    position += 0xC;

    std::memcpy(&instance_offset, &mati_data[position], BYTES4);
    position = instance_offset;

    if (log_output) {
        std::cout << "MATI header_offset: " << util::uint32_t_to_hex_string(header_offset) << std::endl;
        std::cout << "MATI type_offset: " << util::uint32_t_to_hex_string(type_offset) << std::endl;
        std::cout << "MATI type: " << type << std::endl;
        std::cout << "MATI texture_count: " << texture_count << std::endl;
        std::cout << "MATI class_flags: " << class_flags << std::endl;
        std::cout << "MATI instance_flags: " << instance_flags << std::endl;
        std::cout << "MATI eres_index: " << eres_index << std::endl;
        std::cout << "MATI instance_offset: " << util::uint32_t_to_hex_string(instance_offset) << std::endl;
    }

    json["$schema"] = "https://wiki.glaciermodding.org/schemas/material.schema.json";

    json["MATI"] = util::hash_to_ioi_string(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_value, true);

    if (matt_hash_value && matb_hash_value) {
        json["MATT"] = util::hash_to_ioi_string(rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).hash_value, true);
        json["MATB"] = util::hash_to_ioi_string(rpkgs.at(matb_rpkg_index).hash.at(matb_hash_index).hash_value, true);
    } else {
        json["MATT"] = "";
        json["MATB"] = "";
    }

    for (uint32_t i = 0;
         i < rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.size(); i++) {
        if (util::hash_type(
                rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(i)) ==
            "MATE") {
            mate_index = i;
        }
    }

    if (mate_index == -1) {
        json["MATE"] = "";
    } else {
        json["MATE"] = util::hash_to_ioi_string(
                rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(mate_index),
                true);
    }

    if (eres_index == -1) {
        json["ERES"] = "";
    } else {
        json["ERES"] = util::hash_to_ioi_string(
                rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(eres_index),
                true);
    }

    json["TYPE"] = type;

    mati_json = read_mati_properties_json(mati_json, position, -1, false);

    json["Material"] = mati_json;

    json["Overrides"] = nlohmann::ordered_json::object();

    //std::cout << rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).hash_file_name << std::endl;

    if (matt_hash_value && matb_hash_value) {
        position = 0;

        while (position < matb_data.size()) {
            uint8_t matb_type = 0;
            std::memcpy(&matb_type, &matb_data[position], sizeof(matb_type));
            position += sizeof(matb_type);

            uint32_t matb_string_length = 0;
            std::memcpy(&matb_string_length, &matb_data[position], sizeof(matb_string_length));
            position += sizeof(matb_string_length);

            std::string matb_string = "";

            for (uint32_t i = 0; i < (matb_string_length - 1); i++) {
                matb_string.push_back(matb_data[position]);
                position++;
            }

            position++;

            matb_entries.push_back(matb_string);

            //std::cout << "Type: " << util::uint8_t_to_hex_string(matb_type) << ", String: " << matb_string << std::endl;
        }

        position = 0;

        while (position < matt_data.size()) {
            uint8_t matt_type = 0;
            std::memcpy(&matt_type, &matt_data[position], sizeof(matt_type));
            position += sizeof(matt_type);

            if (matt_type == 1) {
                int32_t texture_hash_depends_index = 0;
                std::memcpy(&texture_hash_depends_index, &matt_data[position],
                            sizeof(texture_hash_depends_index));
                position += sizeof(texture_hash_depends_index);

                matt_entries.emplace_back(texture_hash_depends_index);
                matt_types.push_back(matt_type);

                //std::cout << "Texture: " << util::uint32_t_to_hex_string(texture_hash_depends_index) << std::endl;
            } else if (matt_type == 2) {
                vector3 vector3_value;

                std::memcpy(&vector3_value.x, &matt_data[position], sizeof(vector3_value.x));
                position += sizeof(vector3_value.x);
                std::memcpy(&vector3_value.y, &matt_data[position], sizeof(vector3_value.y));
                position += sizeof(vector3_value.y);
                std::memcpy(&vector3_value.z, &matt_data[position], sizeof(vector3_value.z));
                position += sizeof(vector3_value.z);

                matt_entries.emplace_back(vector3_value);
                matt_types.push_back(matt_type);

                //std::cout << "Float: " << util::float_to_string(vector3_value.x) << ", " << util::float_to_string(vector3_value.y) << ", " << util::float_to_string(vector3_value.z) << std::endl;
            } else if (matt_type == 3) {
                vector4 vector4_value;

                std::memcpy(&vector4_value.x, &matt_data[position], sizeof(vector4_value.x));
                position += sizeof(vector4_value.x);
                std::memcpy(&vector4_value.y, &matt_data[position], sizeof(vector4_value.y));
                position += sizeof(vector4_value.y);
                std::memcpy(&vector4_value.z, &matt_data[position], sizeof(vector4_value.z));
                position += sizeof(vector4_value.z);
                std::memcpy(&vector4_value.w, &matt_data[position], sizeof(vector4_value.w));
                position += sizeof(vector4_value.w);

                matt_entries.emplace_back(vector4_value);
                matt_types.push_back(matt_type);

                //std::cout << "Float: " << util::float_to_string(vector3_value.x) << ", " << util::float_to_string(vector3_value.y) << ", " << util::float_to_string(vector3_value.z) << std::endl;
            } else if (matt_type == 4) {
                float float_value = 0;
                std::memcpy(&float_value, &matt_data[position], sizeof(float_value));
                position += sizeof(float_value);

                matt_entries.emplace_back(float_value);
                matt_types.push_back(matt_type);

                //std::cout << "Float: " << util::float_to_string(float_value) << std::endl;
            } else if (matt_type == 5) {
                vector2 vector2_value;

                std::memcpy(&vector2_value.x, &matt_data[position], sizeof(vector2_value.x));
                position += sizeof(vector2_value.x);
                std::memcpy(&vector2_value.y, &matt_data[position], sizeof(vector2_value.y));
                position += sizeof(vector2_value.y);

                matt_entries.emplace_back(vector2_value);
                matt_types.push_back(matt_type);

                //std::cout << "Float: " << util::float_to_string(vector3_value.x) << ", " << util::float_to_string(vector3_value.y) << ", " << util::float_to_string(vector3_value.z) << std::endl;
            } else if (matt_type == 6) {
                vector3 vector3_value;

                std::memcpy(&vector3_value.x, &matt_data[position], sizeof(vector3_value.x));
                position += sizeof(vector3_value.x);
                std::memcpy(&vector3_value.y, &matt_data[position], sizeof(vector3_value.y));
                position += sizeof(vector3_value.y);
                std::memcpy(&vector3_value.z, &matt_data[position], sizeof(vector3_value.z));
                position += sizeof(vector3_value.z);

                matt_entries.emplace_back(vector3_value);
                matt_types.push_back(matt_type);

                //std::cout << "Float: " << util::float_to_string(vector3_value.x) << ", " << util::float_to_string(vector3_value.y) << ", " << util::float_to_string(vector3_value.z) << std::endl;
            } else if (matt_type == 7) {
                vector4 vector4_value;

                std::memcpy(&vector4_value.x, &matt_data[position], sizeof(vector4_value.x));
                position += sizeof(vector4_value.x);
                std::memcpy(&vector4_value.y, &matt_data[position], sizeof(vector4_value.y));
                position += sizeof(vector4_value.y);
                std::memcpy(&vector4_value.z, &matt_data[position], sizeof(vector4_value.z));
                position += sizeof(vector4_value.z);
                std::memcpy(&vector4_value.w, &matt_data[position], sizeof(vector4_value.w));
                position += sizeof(vector4_value.w);

                matt_entries.emplace_back(vector4_value);
                matt_types.push_back(matt_type);

                //std::cout << "Float: " << util::float_to_string(vector3_value.x) << ", " << util::float_to_string(vector3_value.y) << ", " << util::float_to_string(vector3_value.z) << std::endl;
            } else {
                LOG_AND_EXIT("Error: Unknown MATT Type");
            }
        }

        if (matt_entries.size() != matb_entries.size()) {
            LOG_AND_EXIT("Error: MATT and MATB Entries Don't Match");
        }

        for (uint32_t i = 0; i < matt_entries.size(); i++) {
            if (matt_entries[i].index() == 0) {
                if (std::get<int32_t>(matt_entries[i]) != -1) {
                    json["Overrides"]["Texture"][matb_entries[i]] = util::hash_to_ioi_string(
                            rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).hash_reference_data.hash_reference.at(
                                    std::get<int32_t>(matt_entries[i])), true);

                    //std::cout << matb_entries[i] << ": " << util::hash_to_ioi_string(rpkgs.at(matt_rpkg_index).hash.at(matt_hash_index).hash_reference_data.hash_reference.at(std::get<int32_t>(matt_entries[i])), true) << std::endl;
                } else {
                    json["Overrides"]["Texture"][matb_entries[i]] = "";
                }
            } else if (matt_entries[i].index() == 1) {
                json["Overrides"][matb_entries[i]] = std::get<float>(matt_entries[i]);

                //std::cout << matb_entries[i] << ": " << util::float_to_string(std::get<float>(matt_entries[i])) << std::endl;
            } else if (matt_entries[i].index() == 2) {
                vector2 vector2_value = std::get<vector2>(matt_entries[i]);
                json["Overrides"][matb_entries[i]] = nlohmann::ordered_json::array();
                json["Overrides"][matb_entries[i]].push_back(vector2_value.x);
                json["Overrides"][matb_entries[i]].push_back(vector2_value.y);

                //std::cout << matb_entries[i] << ": " << util::float_to_string(vector2_value.x) << ", " << util::float_to_string(vector2_value.y) << std::endl;
            } else if (matt_entries[i].index() == 3) {
                vector3 vector3_value = std::get<vector3>(matt_entries[i]);

                if (matt_types[i] == 2) {
                    json["Overrides"]["Color"][matb_entries[i]] = nlohmann::ordered_json::array();
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector3_value.x);
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector3_value.y);
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector3_value.z);
                } else {
                    json["Overrides"][matb_entries[i]] = nlohmann::ordered_json::array();
                    json["Overrides"][matb_entries[i]].push_back(vector3_value.x);
                    json["Overrides"][matb_entries[i]].push_back(vector3_value.y);
                    json["Overrides"][matb_entries[i]].push_back(vector3_value.z);
                }

                //std::cout << matb_entries[i] << ": " << util::float_to_string(vector3_value.x) << ", " << util::float_to_string(vector3_value.y) << ", " << util::float_to_string(vector3_value.z) << std::endl;
            } else if (matt_entries[i].index() == 4) {
                vector4 vector4_value = std::get<vector4>(matt_entries[i]);

                if (matt_types[i] == 3) {
                    json["Overrides"]["Color"][matb_entries[i]] = nlohmann::ordered_json::array();
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector4_value.x);
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector4_value.y);
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector4_value.z);
                    json["Overrides"]["Color"][matb_entries[i]].push_back(vector4_value.w);
                } else {
                    json["Overrides"][matb_entries[i]] = nlohmann::ordered_json::array();
                    json["Overrides"][matb_entries[i]].push_back(vector4_value.x);
                    json["Overrides"][matb_entries[i]].push_back(vector4_value.y);
                    json["Overrides"][matb_entries[i]].push_back(vector4_value.z);
                    json["Overrides"][matb_entries[i]].push_back(vector4_value.w);
                }

                //std::cout << matb_entries[i] << ": " << util::float_to_string(vector4_value.x) << ", " << util::float_to_string(vector4_value.y) << ", " << util::float_to_string(vector4_value.z) << ", " << util::float_to_string(vector4_value.w) << std::endl;
            }
        }
    }

    json["Flags"]["Class"] = decode_flags(ClassFlags, class_flags);
    json["Flags"]["Instance"] = decode_flags(InstanceFlags, instance_flags);
}

void material::write_json_to_file(const std::string& output_path) const {
    std::ofstream json_file = std::ofstream(output_path, std::ofstream::binary);

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

nlohmann::ordered_json
material::read_mati_properties_json(nlohmann::ordered_json temp_json, uint32_t position, uint32_t parent,
                                    bool from_mati_file) {
    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    mati_property temp_mati_property;

    temp_mati_property.parent = parent;

    temp_mati_property.name[3] = mati_data[position];
    position++;
    temp_mati_property.name[2] = mati_data[position];
    position++;
    temp_mati_property.name[1] = mati_data[position];
    position++;
    temp_mati_property.name[0] = mati_data[position];
    position++;

    std::memcpy(&temp_mati_property.data, &mati_data[position], BYTES4);
    position += 0x4;

    std::memcpy(&temp_mati_property.size, &mati_data[position], BYTES4);
    position += 0x4;

    std::memcpy(&temp_mati_property.type, &mati_data[position], BYTES4);
    position += 0x4;

    std::string mati_property_name = temp_mati_property.name;

    auto it = mati_property_name_map.find(temp_mati_property.name);

    if (it != mati_property_name_map.end()) {
        mati_property_name = it->second;
    }

    if (log_output) {
        std::cout << "MATI temp_mati_property.name: " << temp_mati_property.name << " (" << mati_property_name << ")"
                  << std::endl;
        std::cout << "MATI temp_mati_property.data: " << temp_mati_property.data << std::endl;
        std::cout << "MATI temp_mati_property.size: " << temp_mati_property.size << std::endl;
        std::cout << "MATI temp_mati_property.type: " << temp_mati_property.type << std::endl;
    }

    if (temp_mati_property.type == 0) {
        if (log_output)
            std::cout << "MATI temp_property_type: FLOAT" << std::endl;

        if (temp_mati_property.size == 1) {
            float temp_float = 0;

            std::memcpy(&temp_float, &temp_mati_property.data, BYTES4);

            temp_mati_property.float_values.push_back(temp_float);

            temp_json[mati_property_name] = temp_mati_property.float_values[0];
        } else {
            for (uint32_t i = 0; i < temp_mati_property.size; i++) {
                float temp_float = 0;

                std::memcpy(&temp_float, &mati_data[temp_mati_property.data + i * 0x4], BYTES4);

                if (log_output)
                    std::cout << "MATI temp_mati_property.value (" << i << "): " << temp_float << std::endl;

                temp_mati_property.float_values.push_back(temp_float);
            }

            temp_json[mati_property_name] = temp_mati_property.float_values;
        }

        mati_properties.push_back(temp_mati_property);
    } else if (temp_mati_property.type == 1) {
        if (log_output)
            std::cout << "MATI temp_property_type: STRING" << std::endl;

        temp_mati_property.string_value = std::string(&mati_data[temp_mati_property.data]);

        if (log_output)
            std::cout << "MATI temp_mati_property.value: " << temp_mati_property.string_value << std::endl;

        mati_properties.push_back(temp_mati_property);

        temp_json[mati_property_name] = temp_mati_property.string_value;
    } else if (temp_mati_property.type == 2) {
        if (log_output)
            std::cout << "MATI temp_property_type: INT" << std::endl;

        temp_mati_property.int_value = temp_mati_property.data;

        mati_properties.push_back(temp_mati_property);

        if (temp_mati_property.name == "TXID") {
            if (temp_mati_property.int_value == -1) {
                temp_json[mati_property_name] = "";
            } else {
                temp_json[mati_property_name] = util::hash_to_ioi_string(
                        rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(
                                temp_mati_property.int_value), true);
            }
        } else {
            temp_json[mati_property_name] = temp_mati_property.int_value;
        }
    } else if (temp_mati_property.type == 3) {
        if (log_output)
            std::cout << "MATI temp_property_type: PROPERTY" << std::endl;

        position = temp_mati_property.data;

        parent = mati_properties.size();

        nlohmann::ordered_json temp_json_array = nlohmann::ordered_json::array();

        nlohmann::ordered_json temp_json_object = nlohmann::ordered_json::object();

        for (uint32_t p = 0; p < temp_mati_property.size; p++) {
            temp_json_object = read_mati_properties_json(temp_json_object, position, parent, from_mati_file);

            position += 0x10;
        }

        if (temp_json.contains(mati_property_name)) {
            temp_json[mati_property_name].push_back(temp_json_object);
        } else {
            temp_json_array.push_back(temp_json_object);

            temp_json[mati_property_name] = temp_json_array;
        }
    }

    return temp_json;
}

material::material(const std::string& json_path, const std::string& output_path) {
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
        uint64_t mati_hash_value = 0;
        uint64_t eres_hash_depends = 0;
        uint64_t mate_hash_depends = 0;
        std::vector<uint64_t> mati_text_depends;
        std::vector<uint32_t> mati_text_depends_txids;
        std::vector<std::string> mati_textures;
        std::vector<uint64_t> matt_text_depends;
        std::vector<bool> matt_textures;

        std::string temp_string = json["MATI"];

        mati_hash_value = util::ioi_string_to_hash(temp_string);

        temp_string = json["MATT"];

        matt_hash_value = util::ioi_string_to_hash(temp_string);

        temp_string = json["MATB"];

        matb_hash_value = util::ioi_string_to_hash(temp_string);

        temp_string = json["ERES"];

        eres_hash_depends = util::ioi_string_to_hash(temp_string);

        temp_string = json["MATE"];

        mate_hash_depends = util::ioi_string_to_hash(temp_string);

        type = json["TYPE"];
        uint32_t class_flags = encode_flags(ClassFlags, json["Flags"]["Class"]);
        uint32_t instance_flags = encode_flags(InstanceFlags, json["Flags"]["Instance"]);

        uint32_t texture_count = 0;

        if (json["Material"]["Instance"][0]["Binder"][0].contains("Texture")) {
            for (uint32_t i = 0; i < json["Material"]["Instance"][0]["Binder"][0]["Texture"].size(); i++) {
                if (json["Material"]["Instance"][0]["Binder"][0]["Texture"][i].contains("Texture Id")) {
                    if (json["Material"]["Instance"][0]["Binder"][0]["Texture"][i].contains("Name")) {
                        temp_string = json["Material"]["Instance"][0]["Binder"][0]["Texture"][i]["Texture Id"];

                        if (temp_string.empty()) {
                            mati_text_depends_txids.push_back(0xFFFFFFFF);
                        } else {
                            mati_text_depends_txids.push_back(texture_count);

                            mati_text_depends.push_back(util::ioi_string_to_hash(temp_string));

                            texture_count++;
                        }

                        mati_textures.emplace_back(json["Material"]["Instance"][0]["Binder"][0]["Texture"][i]["Name"]);
                    }
                }
            }
        }

        std::vector<char> mati_meta_data;
        std::vector<char> matt_meta_data;
        std::vector<char> matb_meta_data;

        for (uint32_t k = 0; k < sizeof(mati_hash_value); k++) {
            mati_meta_data.push_back(*((char*) &mati_hash_value + k));

            if (matt_hash_value && matb_hash_value) {
                matt_meta_data.push_back(*((char*) &matt_hash_value + k));
                matb_meta_data.push_back(*((char*) &matb_hash_value + k));
            }
        }

        write_uint32_t(mati_meta_data, 0);
        write_uint32_t(mati_meta_data, 0);
        write_uint32_t(mati_meta_data, 0x80000001);
        write_name(mati_meta_data, "ITAM");

        if (matt_hash_value && matb_hash_value) {
            write_uint32_t(matt_meta_data, 0);
            write_uint32_t(matt_meta_data, 0);
            write_uint32_t(matt_meta_data, 0x80000001);
            write_name(matt_meta_data, "TTAM");
            write_uint32_t(matb_meta_data, 0);
            write_uint32_t(matb_meta_data, 0);
            write_uint32_t(matb_meta_data, 0x80000001);
            write_name(matb_meta_data, "BTAM");
        }

        uint32_t mati_hash_depends_count = 0;
        uint32_t matt_hash_depends_count = 3;
        uint32_t matb_hash_depends_count = 1;

        if (eres_hash_depends != 0) {
            mati_hash_depends_count++;
        }

        if (mate_hash_depends != 0) {
            mati_hash_depends_count++;
        }

        mati_hash_depends_count += mati_text_depends.size();

        if (matt_hash_value && matb_hash_value) {
            if (json.contains("Overrides")) {
                if (json["Overrides"].contains("Texture")) {
                    for (const auto& it : json["Overrides"]["Texture"].items()) {
                        std::string text_name = "map" + std::string(it.key());
                        std::string text_value = std::string(it.value());

                        if (text_value.empty()) {
                            matt_textures.push_back(false);
                        } else {
                            if (std::find(mati_textures.begin(), mati_textures.end(), text_name) !=
                                mati_textures.end()) {
                                matt_textures.push_back(true);

                                matt_text_depends.push_back(util::ioi_string_to_hash(text_value));

                                matt_hash_depends_count++;
                            } else {
                                matt_textures.push_back(false);
                            }
                        }
                    }
                }
            }
        }

        const uint32_t mati_hash_reference_table_size = mati_hash_depends_count * 0x8 + mati_hash_depends_count + 0x4;
        mati_hash_depends_count = mati_hash_depends_count | 0xC0000000;
        write_uint32_t(mati_meta_data, mati_hash_reference_table_size);

        if (matt_hash_value && matb_hash_value) {
            const uint32_t matt_hash_reference_table_size =
                    matt_hash_depends_count * 0x8 + matt_hash_depends_count + 0x4;
            matt_hash_depends_count = matt_hash_depends_count | 0xC0000000;
            write_uint32_t(matt_meta_data, matt_hash_reference_table_size);

            const uint32_t matb_hash_reference_table_size =
                    matb_hash_depends_count * 0x8 + matb_hash_depends_count + 0x4;
            matb_hash_depends_count = matb_hash_depends_count | 0xC0000000;
            write_uint32_t(matb_meta_data, matb_hash_reference_table_size);
        }

        write_uint32_t(mati_meta_data, 0);

        if (matt_hash_value && matb_hash_value) {
            write_uint32_t(matt_meta_data, 0);
            write_uint32_t(matb_meta_data, 0);
        }

        const uint32_t mati_size_offset = mati_meta_data.size();
        const uint32_t matt_size_offset = matt_meta_data.size();
        const uint32_t matb_size_offset = matb_meta_data.size();

        write_uint32_t(mati_meta_data, 0);

        if (matt_hash_value && matb_hash_value) {
            write_uint32_t(matt_meta_data, 0);
            write_uint32_t(matb_meta_data, 0);
        }

        write_uint32_t(mati_meta_data, 0xFFFFFFFF);
        write_uint32_t(mati_meta_data, 0xFFFFFFFF);

        if (matt_hash_value && matb_hash_value) {
            write_uint32_t(matt_meta_data, 0xFFFFFFFF);
            write_uint32_t(matt_meta_data, 0xFFFFFFFF);
            write_uint32_t(matb_meta_data, 0xFFFFFFFF);
            write_uint32_t(matb_meta_data, 0xFFFFFFFF);
        }

        write_uint32_t(mati_meta_data, mati_hash_depends_count);

        if (matt_hash_value && matb_hash_value) {
            write_uint32_t(matt_meta_data, matt_hash_depends_count);
            write_uint32_t(matb_meta_data, matb_hash_depends_count);
        }

        if (matt_hash_value && matb_hash_value) {
            matt_meta_data.push_back(0x1F); //CPPT
            matt_meta_data.push_back(0x1F); //MATB
            matt_meta_data.push_back(0x5F); //MATI
            matb_meta_data.push_back(0x1F); //CBLU
        }

        for (uint32_t k = 0; k < mati_text_depends.size(); k++) {
            mati_meta_data.push_back(0x5F);
        }

        for (uint32_t k = 0; k < matt_text_depends.size(); k++) {
            matt_meta_data.push_back(0x5F);
        }

        if (mate_hash_depends != 0) {
            mati_meta_data.push_back(0x1F);
        }

        if (eres_hash_depends != 0) {
            mati_meta_data.push_back(0x5F);
        }

        if (matt_hash_value && matb_hash_value) {
            //00B4B11DA327CAD0.CPPT 1F [modules:/zrendermaterialentity.class].pc_entitytype
            constexpr uint64_t matt_cppt_hash = 0x00B4B11DA327CAD0;

            for (uint32_t k = 0; k < sizeof(matt_cppt_hash); k++) {
                matt_meta_data.push_back(*((char*) &matt_cppt_hash + k));
            }

            for (uint32_t k = 0; k < sizeof(matb_hash_value); k++) {
                matt_meta_data.push_back(*((char*) &matb_hash_value + k));
            }

            for (uint32_t k = 0; k < sizeof(mati_hash_value); k++) {
                matt_meta_data.push_back(*((char*) &mati_hash_value + k));
            }

            //00A1595C0918E2C9.CBLU 1F[modules:/zrendermaterialentity.class].pc_entityblueprint;
            constexpr uint64_t matb_cblu_hash = 0x00A1595C0918E2C9;

            for (uint32_t k = 0; k < sizeof(matb_cblu_hash); k++) {
                matb_meta_data.push_back(*((char*) &matb_cblu_hash + k));
            }
        }

        int32_t eres_index = 0;

        for (const auto& mati_text_depend : mati_text_depends) {
            for (uint32_t k = 0; k < sizeof(mati_text_depend); k++) {
                mati_meta_data.push_back(*((char*) &mati_text_depend + k));
            }

            eres_index++;
        }

        for (const auto& matt_text_depend : matt_text_depends) {
            for (uint32_t k = 0; k < sizeof(matt_text_depend); k++) {
                matt_meta_data.push_back(*((char*) &matt_text_depend + k));
            }
        }

        if (mate_hash_depends != 0) {
            for (uint32_t k = 0; k < sizeof(mate_hash_depends); k++) {
                mati_meta_data.push_back(*((char*) &mate_hash_depends + k));
            }

            eres_index++;
        }

        if (eres_hash_depends != 0) {
            for (uint32_t k = 0; k < sizeof(eres_hash_depends); k++) {
                mati_meta_data.push_back(*((char*) &eres_hash_depends + k));
            }
        } else {
            eres_index = -1;
        }

        uint32_t type_length = ((type.length() / 0x10) + 1) * 0x10;

        write_uint32_t(mati_data, (uint32_t) (type_length + 0x10));
        align(mati_data);

        write_string(mati_data, type);
        align(mati_data);

        const uint32_t instance_offset = (mati_data.size() + 0x30);

        write_uint32_t(mati_data, (uint32_t) 0x10);
        write_uint32_t(mati_data, texture_count);
        write_uint32_t(mati_data, class_flags);
        write_uint32_t(mati_data, instance_flags);
        write_uint32_t(mati_data, (uint32_t) eres_index);
        write_uint32_t(mati_data, (uint32_t) 0x0);
        write_uint32_t(mati_data, (uint32_t) 0x0);
        write_uint32_t(mati_data, (uint32_t) instance_offset);
        write_uint32_t(mati_data, (uint32_t) 0x3);
        write_uint32_t(mati_data, (uint32_t) 0x0);
        write_uint32_t(mati_data, (uint32_t) 0x0);
        write_uint32_t(mati_data, (uint32_t) 0x0);

        uint32_t temp_offset = mati_data.size();

        write_name(mati_data, "INST");
        write_uint32_t(mati_data, (uint32_t) (temp_offset + 0x10));
        write_uint32_t(mati_data, (uint32_t) json["Material"]["Instance"][0].size());
        write_property_type(mati_data, "INST");

        std::vector<uint32_t> offsets_values;
        std::vector<MATI_PROPERTY_TYPE> offsets_types;
        std::vector<nlohmann::ordered_json> offsets_data;

        temp_offset = mati_data.size();

        std::string name = json["Material"]["Instance"][0]["Name"];
        std::string tags = json["Material"]["Instance"][0]["Tags"];

        write_name(mati_data, "NAME");
        offsets_values.push_back((uint32_t) mati_data.size());
        offsets_types.push_back(MATI_PROPERTY_TYPE::STRING);
        offsets_data.push_back(json["Material"]["Instance"][0]["Name"]);
        write_uint32_t(mati_data, (uint32_t) 0x0);
        write_uint32_t(mati_data, (uint32_t) (name.length() + 1));
        write_property_type(mati_data, "NAME");

        write_name(mati_data, "TAGS");
        offsets_values.push_back((uint32_t) mati_data.size());
        offsets_types.push_back(MATI_PROPERTY_TYPE::STRING);
        offsets_data.push_back(json["Material"]["Instance"][0]["Tags"]);
        write_uint32_t(mati_data, (uint32_t) 0x0);
        write_uint32_t(mati_data, (uint32_t) (tags.length() + 1));
        write_property_type(mati_data, "TAGS");

        uint32_t bind_count = 0;

        for (const auto& it : json["Material"]["Instance"][0]["Binder"][0].items()) {
            for (const auto& it2 : it.value().items()) {
                bind_count++;
            }
        }

        write_name(mati_data, "BIND");
        write_uint32_t(mati_data, (uint32_t) (temp_offset + 0x30));
        write_uint32_t(mati_data, bind_count);
        write_property_type(mati_data, "BIND");

        for (const auto& it : json["Material"]["Instance"][0]["Binder"][0].items()) {
            for (const auto& it2 : it.value().items()) {
                write_name(mati_data, get_property_name(it.key()));
                offsets_values.push_back((uint32_t) mati_data.size());
                offsets_types.push_back(MATI_PROPERTY_TYPE::PROPERTY);
                offsets_data.push_back(it2.value());
                write_uint32_t(mati_data, (uint32_t) 0x0);
                write_uint32_t(mati_data, (uint32_t) it2.value().size());
                write_property_type(mati_data, get_property_name(it.key()));
            }
        }

        uint32_t txid_index = 0;

        for (uint32_t i = 0; i < offsets_values.size(); i++) {
            if (offsets_types.at(i) == MATI_PROPERTY_TYPE::PROPERTY) {
                temp_offset = mati_data.size();

                std::memcpy(&mati_data[offsets_values.at(i)], &temp_offset, 0x4);

                nlohmann::ordered_json temp_json = offsets_data.at(i);

                for (const auto& it : temp_json.items()) {
                    //std::cout << it.key() << "," << it.value() << "," << temp_json.size() << std::endl;
                    //std::cout << temp_json << std::endl;

                    if (get_property_type(get_property_name(it.key())) == MATI_PROPERTY_TYPE::FLOAT) {
                        if (it.value().is_array()) {
                            write_name(mati_data, get_property_name(it.key()));
                            offsets_values.push_back((uint32_t) mati_data.size());
                            offsets_types.push_back(MATI_PROPERTY_TYPE::FLOAT);
                            offsets_data.push_back(it.value());
                            write_uint32_t(mati_data, (uint32_t) 0x0);
                            write_uint32_t(mati_data, (uint32_t) it.value().size());
                            write_property_type(mati_data, get_property_name(it.key()));
                        } else {
                            write_name(mati_data, get_property_name(it.key()));
                            write_float(mati_data, (float) it.value());
                            write_uint32_t(mati_data, (uint32_t) 0x1);
                            write_property_type(mati_data, get_property_name(it.key()));
                        }
                    } else if (get_property_type(get_property_name(it.key())) == MATI_PROPERTY_TYPE::STRING) {
                        std::string temp_string = it.value();

                        write_name(mati_data, get_property_name(it.key()));
                        offsets_values.push_back((uint32_t) mati_data.size());
                        offsets_types.push_back(MATI_PROPERTY_TYPE::STRING);
                        offsets_data.push_back(it.value());
                        write_uint32_t(mati_data, (uint32_t) 0x0);
                        write_uint32_t(mati_data, (uint32_t) temp_string.size());
                        write_property_type(mati_data, get_property_name(it.key()));
                    } else if (get_property_type(get_property_name(it.key())) == MATI_PROPERTY_TYPE::INT) {
                        if (it.key() == "Texture Id") {
                            write_name(mati_data, get_property_name(it.key()));
                            write_uint32_t(mati_data, (uint32_t) mati_text_depends_txids.at(txid_index));
                            write_uint32_t(mati_data, (uint32_t) 0x1);
                            write_property_type(mati_data, get_property_name(it.key()));

                            txid_index++;
                        } else {
                            write_name(mati_data, get_property_name(it.key()));
                            write_uint32_t(mati_data, (uint32_t) it.value());
                            write_uint32_t(mati_data, (uint32_t) 0x1);
                            write_property_type(mati_data, get_property_name(it.key()));
                        }
                    }
                }
            }
        }

        for (uint32_t i = 0; i < offsets_values.size(); i++) {
            nlohmann::ordered_json temp_json = offsets_data.at(i);

            if (offsets_types.at(i) == MATI_PROPERTY_TYPE::FLOAT) {
                if (temp_json.is_array()) {
                    temp_offset = mati_data.size();

                    std::memcpy(&mati_data[offsets_values.at(i)], &temp_offset, 0x4);

                    for (const auto& i : temp_json) {
                        write_float(mati_data, i);
                    }

                    align(mati_data);
                }
            } else if (offsets_types.at(i) == MATI_PROPERTY_TYPE::STRING) {
                temp_offset = mati_data.size();

                std::memcpy(&mati_data[offsets_values.at(i)], &temp_offset, 0x4);

                std::string temp_string = temp_json;

                write_string(mati_data, temp_string);

                align(mati_data);
            }
        }

        if (matt_hash_value && matb_hash_value) {
            if (json.contains("Overrides")) {
                for (const auto& it : json["Overrides"].items()) {
                    if (it.key() == "Texture") {
                        uint32_t texture_index = 3;
                        uint32_t matt_texture = 0;

                        for (const auto& it2 : json["Overrides"]["Texture"].items()) {
                            if (matt_textures.at(matt_texture)) {
                                //std::cout << it2.key() << ", " << it2.value() << std::endl;

                                matt_data.push_back(0x1);
                                write_uint32_t(matt_data, texture_index);

                                matb_data.push_back(0x1);
                                std::string temp_name = std::string(it2.key());
                                write_uint32_t(matb_data, temp_name.length() + 1);
                                write_string(matb_data, temp_name);

                                texture_index++;
                            }

                            matt_texture++;
                        }
                    } else if (it.key() == "Color") {
                        for (const auto& it2 : json["Overrides"]["Color"].items()) {
                            //std::cout << it2.key() << ", " << it2.value().size() << std::endl;

                            if (it2.value().is_array()) {
                                if (it2.value().size() == 3) {
                                    matt_data.push_back(0x2);
                                    matb_data.push_back(0x2);
                                } else if (it2.value().size() == 4) {
                                    matt_data.push_back(0x3);
                                    matb_data.push_back(0x3);
                                }

                                for (const auto& it3 : it2.value().items()) {
                                    //std::cout << it3.key() << ", " << it3.value() << std::endl;

                                    write_float(matt_data, it3.value().get<float>());
                                }

                                std::string temp_name = std::string(it2.key());
                                write_uint32_t(matb_data, temp_name.length() + 1);
                                write_string(matb_data, temp_name);
                            }
                        }
                    } else if (it.value().is_array()) {
                        //std::cout << it.key() << ", " << it.value().size() << std::endl;

                        if (it.value().size() == 2) {
                            matt_data.push_back(0x5);
                            matb_data.push_back(0x5);
                        } else if (it.value().size() == 3) {
                            matt_data.push_back(0x6);
                            matb_data.push_back(0x6);
                        } else if (it.value().size() == 4) {
                            matt_data.push_back(0x7);
                            matb_data.push_back(0x7);
                        }

                        for (const auto& it2 : it.value().items()) {
                            //std::cout << it2.key() << ", " << it2.value() << std::endl;

                            write_float(matt_data, it2.value().get<float>());
                        }

                        std::string temp_name = std::string(it.key());
                        write_uint32_t(matb_data, temp_name.length() + 1);
                        write_string(matb_data, temp_name);
                    } else {
                        //std::cout << it.key() << ", " << it.value() << std::endl;

                        matt_data.push_back(0x4);
                        write_float(matt_data, it.value().get<float>());

                        matb_data.push_back(0x4);
                        std::string temp_name = std::string(it.key());
                        write_uint32_t(matb_data, temp_name.length() + 1);
                        write_string(matb_data, temp_name);
                    }
                }
            }
        }

        std::ofstream mati_file;
        std::ofstream mati_meta_file;
        std::ofstream matt_file;
        std::ofstream matt_meta_file;
        std::ofstream matb_file;
        std::ofstream matb_meta_file;

        file::create_directories(output_path);
        mati_file = std::ofstream(
                file::output_path_append(util::uint64_t_to_hex_string(mati_hash_value) + ".MATI", output_path),
                std::ofstream::binary);
        mati_meta_file = std::ofstream(
                file::output_path_append(util::uint64_t_to_hex_string(mati_hash_value) + ".MATI.meta", output_path),
                std::ofstream::binary);

        if (matt_hash_value && matb_hash_value) {
            matt_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(matt_hash_value) + ".MATT", output_path),
                    std::ofstream::binary);
            matt_meta_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(matt_hash_value) + ".MATT.meta", output_path),
                    std::ofstream::binary);
            matb_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(matb_hash_value) + ".MATB", output_path),
                    std::ofstream::binary);
            matb_meta_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(matb_hash_value) + ".MATB.meta", output_path),
                    std::ofstream::binary);
        }

        mati_file.write(mati_data.data(), mati_data.size());
        mati_file.close();
        uint32_t mati_data_size = mati_data.size();
        std::memcpy(&mati_meta_data[mati_size_offset], &mati_data_size, 0x4);
        mati_meta_file.write(mati_meta_data.data(), mati_meta_data.size());
        mati_meta_file.close();

        if (matt_hash_value && matb_hash_value) {
            matt_file.write(matt_data.data(), matt_data.size());
            matt_file.close();
            uint32_t matt_data_size = matt_data.size();
            std::memcpy(&matt_meta_data[matt_size_offset], &matt_data_size, 0x4);
            matt_meta_file.write(matt_meta_data.data(), matt_meta_data.size());
            matt_meta_file.close();

            matb_file.write(matb_data.data(), matb_data.size());
            matb_file.close();
            uint32_t matb_data_size = matb_data.size();
            std::memcpy(&matb_meta_data[matb_size_offset], &matb_data_size, 0x4);
            matb_meta_file.write(matb_meta_data.data(), matb_meta_data.size());
            matb_meta_file.close();
        }
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

MATI_PROPERTY_TYPE material::get_property_type(const std::string& key) const {
    auto it = mati_property_type_map.find(key);

    if (it != mati_property_type_map.end()) {
        return it->second;
    }

    return MATI_PROPERTY_TYPE::UNKNOWN;
}

void material::align(std::vector<char>& material_data) {
    while (material_data.size() % 0x10 != 0) {
        material_data.push_back(0x0);
    }
}

void material::write_uint32_t(std::vector<char>& material_data, uint32_t data) {
    char tmp_uint32_t[4];

    std::memcpy(&tmp_uint32_t, &data, 0x4);

    for (int i = 0; i < 0x4; i++) {
        material_data.push_back(*((char*) &tmp_uint32_t + i));
    }
}

void material::write_string(std::vector<char>& material_data, const std::string& data) {
    for (char i : data) {
        material_data.push_back(i);
    }

    material_data.push_back(0x0);
}

void material::write_float(std::vector<char>& material_data, float data) {
    char tmp_float[4];

    std::memcpy(&tmp_float, &data, 0x4);

    for (int i = 0; i < 0x4; i++) {
        material_data.push_back(*((char*) &tmp_float + i));
    }
}

void material::write_name(std::vector<char>& material_data, std::string name) {
    material_data.push_back(name[3]);
    material_data.push_back(name[2]);
    material_data.push_back(name[1]);
    material_data.push_back(name[0]);
}

void material::write_property_type(std::vector<char>& material_data, const std::string& name) {
    auto it = mati_property_type_map.find(name);

    if (it != mati_property_type_map.end()) {
        write_uint32_t(mati_data, (uint32_t) it->second);
    }
}

std::string material::get_property_name(std::string property) {
    auto it = mati_property_name_map_reversed.find(property);

    if (it != mati_property_name_map_reversed.end()) {
        return it->second;
    }

    return property;
}

uint32_t material::encode_flags(std::vector<std::string> flags, nlohmann::ordered_json temp_json) {
    uint32_t value = 0;

    for (uint32_t i = 0; i < flags.size(); i++) {
        if (temp_json.contains(flags.at(i))) {
            if (temp_json[flags.at(i)].get<bool>())
                value += 1 << i;
        }
    }

    return value;
}

nlohmann::ordered_json material::decode_flags(std::vector<std::string> flags, uint32_t value) {
    nlohmann::ordered_json temp_json = nlohmann::ordered_json::object();

    for (uint32_t i = 0; i < flags.size(); i++) {
        if ((value >> i) & 1)
            temp_json[flags.at(i)] = true;
        else
            temp_json[flags.at(i)] = false;
    }

    return temp_json;
}