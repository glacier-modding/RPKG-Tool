#include "mati.h"
#include "rpkg_function.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "file.h"
#include <lz4.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <typeinfo>

mati::mati() = default;

mati::mati(uint64_t rpkgs_index, uint64_t hash_index) {
    mati_rpkg_index = rpkgs_index;
    mati_hash_index = hash_index;

    mati_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." +
                     rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    load_hash_depends();

    uint64_t mati_hash_size;

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.lz4ed) {
        mati_hash_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.header.data_size;

        if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.xored) {
            mati_hash_size &= 0x3FFFFFFF;
        }
    } else {
        mati_hash_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.resource.size_final;
    }

    mati_input_data = std::vector<char>(mati_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(mati_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(mati_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.header.data_offset, file.beg);
    file.read(mati_input_data.data(), mati_hash_size);
    file.close();

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.xored) {
        crypto::xor_data(mati_input_data.data(), (uint32_t) mati_hash_size);
    }

    const uint32_t mati_decompressed_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.resource.size_final;

    mati_output_data = std::vector<char>(mati_decompressed_size, 0);

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).data.lz4ed) {
        LZ4_decompress_safe(mati_input_data.data(), mati_output_data.data(), (int) mati_hash_size,
                            mati_decompressed_size);

        mati_data = mati_output_data;
    } else {
        mati_data = mati_input_data;
    }

    std::vector<char>().swap(mati_output_data);
    std::vector<char>().swap(mati_input_data);
}

void mati::load_hash_depends() {
    uint32_t mati_hash_reference_count =
            rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    if (mati_hash_reference_count > 0) {
        for (uint64_t i = 0; i < mati_hash_reference_count; i++) {
            std::vector<std::string> prim_text_depends_in_rpkgs;
            std::vector<uint32_t> prim_text_depends_rpkg_index;
            std::vector<uint32_t> prim_text_depends_hash_index;

            std::vector<std::string> prim_mate_depends_in_rpkgs;
            std::vector<uint32_t> prim_mate_depends_rpkg_index;
            std::vector<uint32_t> prim_mate_depends_hash_index;

            bool text_found = false;
            bool mate_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++) {
                std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(
                        rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end()) {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TEXT") {
                        if (!text_found) {
                            text_depends_file_name.push_back(
                                    util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                                    rpkgs.at(j).hash.at(it->second).hash_resource_type);

                            text_depends_index.push_back(i);

                            //LOG("Found TEXT hash depend:");
                            //LOG("  - File Name: " + text_depends_file_name.back());
                        }

                        text_found = true;

                        prim_text_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        prim_text_depends_rpkg_index.push_back(j);
                        prim_text_depends_hash_index.push_back(it->second);

                        //LOG("  - In RPKG: " + prim_text_depends_in_rpkgs.back());
                        //LOG("  - RPKG Index: " + util::uint64_t_to_string(prim_text_depends_rpkg_index.back()));
                        //LOG("  - Hash Index: " + util::uint64_t_to_string(prim_text_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "MATE") {
                        if (!mate_found) {
                            mate_depends_file_name.push_back(
                                    util::uint64_t_to_hex_string(rpkgs.at(j).hash.at(it->second).hash_value) + "." +
                                    rpkgs.at(j).hash.at(it->second).hash_resource_type);

                            mate_depends_index.push_back(i);

                            //LOG("Found MATE hash depend:");
                            //LOG("  - File Name: " + mate_depends_file_name.back());

                            mate_index = i;
                        }

                        mate_found = true;

                        prim_mate_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        prim_mate_depends_rpkg_index.push_back(j);
                        prim_mate_depends_hash_index.push_back(it->second);

                        //LOG("  - In RPKG: " + prim_mate_depends_in_rpkgs.back());
                        //LOG("  - RPKG Index: " + util::uint64_t_to_string(prim_mate_depends_rpkg_index.back()));
                        //LOG("  - Hash Index: " + util::uint64_t_to_string(prim_mate_depends_hash_index.back()));
                    }
                }
            }

            uint32_t text_value = 0;

            if (text_found) {
                text_depends_in_rpkgs.push_back(prim_text_depends_in_rpkgs);
                text_depends_rpkg_index.push_back(prim_text_depends_rpkg_index);
                text_depends_hash_index.push_back(prim_text_depends_hash_index);
                text_depends_in_rpkgs_index.push_back(text_value);
                text_depends_rpkg_index_index.push_back(text_value);
                text_depends_hash_index_index.push_back(text_value);
            }

            if (mate_found) {
                mate_depends_in_rpkgs.push_back(prim_mate_depends_in_rpkgs);
                mate_depends_rpkg_index.push_back(prim_mate_depends_rpkg_index);
                mate_depends_hash_index.push_back(prim_mate_depends_hash_index);
                mate_depends_in_rpkgs_index.push_back(text_value);
                mate_depends_rpkg_index_index.push_back(text_value);
                mate_depends_hash_index_index.push_back(text_value);
            }
        }
    }

    for (uint64_t k = 0; k < text_depends_file_name.size(); k++) {
        //LOG("  - MATI File Name: " + text_depends_file_name.at(k));

        bool text_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < text_depends_in_rpkgs.at(k).size(); d++) {
            std::string text_depends_in_rpkgs_upper_case = util::to_upper_case(text_depends_in_rpkgs.at(k).at(d));

            std::string_view text_depends_in_rpkgs_string_view(text_depends_in_rpkgs_upper_case.c_str(),
                                                               text_depends_in_rpkgs_upper_case.length());

            size_t pos1 = text_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos) {
                size_t pos2 = text_depends_in_rpkgs_string_view.substr(pos1).find('.');

                if (pos2 != std::string::npos) {
                    text_patch_name_found = true;

                    long new_patch_level = std::strtol(
                            std::string(text_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level) {
                        patch_level = new_patch_level;

                        text_depends_in_rpkgs_index.at(k) = d;
                        text_depends_rpkg_index_index.at(k) = d;
                        text_depends_hash_index_index.at(k) = d;
                    }
                }
            } else {
                if (!text_patch_name_found) {
                    text_depends_in_rpkgs_index.at(k) = d;
                    text_depends_rpkg_index_index.at(k) = d;
                    text_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - MATI File Name In RPKG: " + text_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - MATI File Name In RPKG Used: " + text_depends_in_rpkgs.at(k).at(text_depends_rpkg_index_index.at(k)));
    }

    for (uint64_t k = 0; k < mate_depends_file_name.size(); k++) {
        //LOG("  - MATE File Name: " + mate_depends_file_name.at(k));

        bool mate_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < mate_depends_in_rpkgs.at(k).size(); d++) {
            std::string mate_depends_in_rpkgs_upper_case = util::to_upper_case(mate_depends_in_rpkgs.at(k).at(d));

            std::string_view mate_depends_in_rpkgs_string_view(mate_depends_in_rpkgs_upper_case.c_str(),
                                                               mate_depends_in_rpkgs_upper_case.length());

            size_t pos1 = mate_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos) {
                size_t pos2 = mate_depends_in_rpkgs_string_view.substr(pos1).find('.');

                if (pos2 != std::string::npos) {
                    mate_patch_name_found = true;

                    long new_patch_level = std::strtol(
                            std::string(mate_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level) {
                        patch_level = new_patch_level;

                        mate_depends_in_rpkgs_index.at(k) = d;
                        mate_depends_rpkg_index_index.at(k) = d;
                        mate_depends_hash_index_index.at(k) = d;
                    }
                }
            } else {
                if (!mate_patch_name_found) {
                    mate_depends_in_rpkgs_index.at(k) = d;
                    mate_depends_rpkg_index_index.at(k) = d;
                    mate_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - MATE File Name In RPKG: " + mate_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - MATE File Name In RPKG Used: " + mate_depends_in_rpkgs.at(k).at(mate_depends_rpkg_index_index.at(k)));
    }
}

void mati::map_textures() {
    uint32_t position = 0;
    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    std::memcpy(&header_offset, &mati_data.data()[position], sizeof(bytes4));
    position = header_offset;

    std::memcpy(&type_offset, &mati_data.data()[position], sizeof(bytes4));
    position += 4;

    type = std::string(&mati_data.data()[type_offset]);

    std::memcpy(&texture_count, &mati_data.data()[position], sizeof(bytes4));
    position += 0x18;

    std::memcpy(&instance_offset, &mati_data.data()[position], sizeof(bytes4));
    position = instance_offset;

    if (log_output) {
        std::cout << "MATI header_offset: " << util::uint32_t_to_hex_string(header_offset) << std::endl;
        std::cout << "MATI type_offset: " << util::uint32_t_to_hex_string(type_offset) << std::endl;
        std::cout << "MATI type: " << type << std::endl;
        std::cout << "MATI texture_count: " << texture_count << std::endl;
        std::cout << "MATI instance_offset: " << util::uint32_t_to_hex_string(instance_offset) << std::endl;
    }

    read_properties(position, -1);

    uint32_t temp_hash_reference_count =
            rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    for (uint32_t p = 0; p < mati_properties.size(); p++) {
        if (!has_diffuse_texture) {
            if (mati_properties.at(p).string_value == "mapTexture2D_01") {
                for (uint32_t r = 0; r < mati_properties.size(); r++) {
                    if (mati_properties.at(p).parent == mati_properties.at(r).parent &&
                        mati_properties.at(r).name == "TXID") {
                        if (log_output)
                            std::cout << "Diffuse texture found at index: " << mati_properties.at(r).int_value
                                      << std::endl;

                        if (mati_properties.at(r).int_value < temp_hash_reference_count) {
                            has_diffuse_texture = true;

                            diffuse_texture_hash = rpkgs.at(mati_rpkg_index).hash.at(
                                    mati_hash_index).hash_reference_data.hash_reference.at(
                                    mati_properties.at(r).int_value);
                        }
                    }
                }
            }
        }

        if (!has_normal_texture) {
            if (mati_properties.at(p).string_value == "mapTexture2DNormal_01") {
                for (uint32_t r = 0; r < mati_properties.size(); r++) {
                    if (mati_properties.at(p).parent == mati_properties.at(r).parent &&
                        mati_properties.at(r).name == "TXID") {
                        if (log_output)
                            std::cout << "Normal texture found at index: " << mati_properties.at(r).int_value
                                      << std::endl;

                        if (mati_properties.at(r).int_value < temp_hash_reference_count) {
                            has_normal_texture = true;

                            normal_texture_hash = rpkgs.at(mati_rpkg_index).hash.at(
                                    mati_hash_index).hash_reference_data.hash_reference.at(
                                    mati_properties.at(r).int_value);
                        }
                    }
                }
            }
        }
    }

    if (!has_diffuse_texture || !has_normal_texture) {
        for (uint32_t d = 0; d < temp_hash_reference_count; d++) {
            uint32_t temp_rpkg_index = rpkg_function::get_latest_hash(
                    rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(d));

            if (temp_rpkg_index != UINT32_MAX) {
                std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(temp_rpkg_index).hash_map.find(
                        rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(d));

                if (it != rpkgs.at(temp_rpkg_index).hash_map.end()) {
                    if (!has_diffuse_texture || !has_normal_texture || !has_emissive_texture) {
                        if (rpkgs.at(temp_rpkg_index).hash.at(it->second).hash_resource_type == "TEXT") {
                            std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(
                                    rpkgs.at(temp_rpkg_index).hash.at(it->second).hash_value);

                            if (it2 != hash_list_hash_map.end()) {
                                if (!has_diffuse_texture) {
                                    if (hash_list_hash_strings.at(it2->second).find("diffuse") != std::string::npos) {
                                        has_diffuse_texture = true;

                                        diffuse_texture_hash = rpkgs.at(temp_rpkg_index).hash.at(it->second).hash_value;
                                    }
                                }

                                if (!has_normal_texture) {
                                    if (hash_list_hash_strings.at(it2->second).find("normal") != std::string::npos) {
                                        has_normal_texture = true;

                                        normal_texture_hash = rpkgs.at(temp_rpkg_index).hash.at(it->second).hash_value;
                                    }
                                }

                                if (!has_emissive_texture) {
                                    if (hash_list_hash_strings.at(it2->second).find("emissive") != std::string::npos) {
                                        has_emissive_texture = true;

                                        emissive_texture_hash = rpkgs.at(temp_rpkg_index).hash.at(
                                                it->second).hash_value;
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

void mati::read_properties(uint32_t position, uint32_t parent) {
    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    mati_property temp_mati_property;

    temp_mati_property.parent = parent;

    temp_mati_property.name[3] = mati_data.data()[position];
    position++;
    temp_mati_property.name[2] = mati_data.data()[position];
    position++;
    temp_mati_property.name[1] = mati_data.data()[position];
    position++;
    temp_mati_property.name[0] = mati_data.data()[position];
    position++;

    std::memcpy(&temp_mati_property.data, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&temp_mati_property.size, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&temp_mati_property.type, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    if (log_output) {
        std::unordered_map<std::string, std::string>::iterator it = mati_property_name_map.find(
                temp_mati_property.name);

        if (it != mati_property_name_map.end()) {
            std::cout << "MATI temp_mati_property.name: " << temp_mati_property.name << " (" << it->second << ")"
                      << std::endl;
        } else {
            std::cout << "MATI temp_mati_property.name: " << temp_mati_property.name << std::endl;
        }

        std::cout << "MATI temp_mati_property.data: " << temp_mati_property.data << std::endl;
        std::cout << "MATI temp_mati_property.size: " << temp_mati_property.size << std::endl;
        std::cout << "MATI temp_mati_property.type: " << temp_mati_property.type << std::endl;
    }

    if (temp_mati_property.type == 0) {
        if (log_output)
            std::cout << "MATI temp_property_type: FLOAT" << std::endl;

        if (temp_mati_property.size == 1) {
            float temp_float = 0;

            std::memcpy(&temp_float, &temp_mati_property.data, sizeof(bytes4));

            temp_mati_property.float_values.push_back(temp_float);
        } else {
            for (uint32_t i = 0; i < temp_mati_property.size; i++) {
                float temp_float = 0;

                std::memcpy(&temp_float, &mati_data.data()[temp_mati_property.data + i * 0x4], sizeof(bytes4));

                if (log_output)
                    std::cout << "MATI temp_mati_property.value (" << i << "): " << temp_float << std::endl;

                temp_mati_property.float_values.push_back(temp_float);
            }
        }

        mati_properties.push_back(temp_mati_property);
    } else if (temp_mati_property.type == 1) {
        if (log_output)
            std::cout << "MATI temp_property_type: STRING" << std::endl;

        temp_mati_property.string_value = std::string(&mati_data.data()[temp_mati_property.data]);

        if (log_output)
            std::cout << "MATI temp_mati_property.value: " << temp_mati_property.string_value << std::endl;

        mati_properties.push_back(temp_mati_property);
    } else if (temp_mati_property.type == 2) {
        if (log_output)
            std::cout << "MATI temp_property_type: INT" << std::endl;

        temp_mati_property.int_value = temp_mati_property.data;

        mati_properties.push_back(temp_mati_property);
    } else if (temp_mati_property.type == 3) {
        if (log_output)
            std::cout << "MATI temp_property_type: PROPERTY" << std::endl;

        position = temp_mati_property.data;

        parent = mati_properties.size();

        for (uint32_t p = 0; p < temp_mati_property.size; p++) {
            read_properties(position, parent);

            position += 0x10;
        }
    }
}

void mati::generate_json(std::string output_path) {
    uint32_t position = 0;
    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    std::memcpy(&header_offset, &mati_data.data()[position], sizeof(bytes4));
    position = header_offset;

    std::memcpy(&type_offset, &mati_data.data()[position], sizeof(bytes4));
    position += 4;

    type = std::string(&mati_data.data()[type_offset]);

    std::memcpy(&texture_count, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&unknown_flag_1, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&unknown_flag_2, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&eres_index, &mati_data.data()[position], sizeof(bytes4));
    position += 0xC;

    std::memcpy(&instance_offset, &mati_data.data()[position], sizeof(bytes4));
    position = instance_offset;

    if (log_output) {
        std::cout << "MATI header_offset: " << util::uint32_t_to_hex_string(header_offset) << std::endl;
        std::cout << "MATI type_offset: " << util::uint32_t_to_hex_string(type_offset) << std::endl;
        std::cout << "MATI type: " << type << std::endl;
        std::cout << "MATI texture_count: " << texture_count << std::endl;
        std::cout << "MATI unknown_flag_1: " << unknown_flag_1 << std::endl;
        std::cout << "MATI unknown_flag_2: " << unknown_flag_2 << std::endl;
        std::cout << "MATI eres_index: " << eres_index << std::endl;
        std::cout << "MATI instance_offset: " << util::uint32_t_to_hex_string(instance_offset) << std::endl;
    }

    json["$schema"] = "https://glaciermodding.org/schemas/mati.schema.json";
    json["TYPE"] = type;

    if (eres_index == -1) {
        json["ERES"] = "";
    } else {
        json["ERES"] = util::hash_to_ioi_string(
                rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(eres_index),
                true);
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

    json["UnknownFlags"]["Unk1"] = unknown_flag_1;
    json["UnknownFlags"]["Unk2"] = unknown_flag_2;

    json = read_properties_json(json, position, -1, false);

    file::create_directories(output_path);

    std::ofstream json_file = std::ofstream(file::output_path_append(
                                                    util::uint64_t_to_hex_string(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_value) + "." +
                                                    rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_resource_type + ".JSON", output_path),
                                            std::ofstream::binary);

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

nlohmann::ordered_json
mati::read_properties_json(nlohmann::ordered_json temp_json, uint32_t position, uint32_t parent, bool from_mati_file) {
    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    mati_property temp_mati_property;

    temp_mati_property.parent = parent;

    temp_mati_property.name[3] = mati_data.data()[position];
    position++;
    temp_mati_property.name[2] = mati_data.data()[position];
    position++;
    temp_mati_property.name[1] = mati_data.data()[position];
    position++;
    temp_mati_property.name[0] = mati_data.data()[position];
    position++;

    std::memcpy(&temp_mati_property.data, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&temp_mati_property.size, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&temp_mati_property.type, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    if (log_output) {
        std::unordered_map<std::string, std::string>::iterator it = mati_property_name_map.find(
                temp_mati_property.name);

        if (it != mati_property_name_map.end()) {
            std::cout << "MATI temp_mati_property.name: " << temp_mati_property.name << " (" << it->second << ")"
                      << std::endl;
        } else {
            std::cout << "MATI temp_mati_property.name: " << temp_mati_property.name << std::endl;
        }

        std::cout << "MATI temp_mati_property.data: " << temp_mati_property.data << std::endl;
        std::cout << "MATI temp_mati_property.size: " << temp_mati_property.size << std::endl;
        std::cout << "MATI temp_mati_property.type: " << temp_mati_property.type << std::endl;
    }

    if (temp_mati_property.type == 0) {
        if (log_output)
            std::cout << "MATI temp_property_type: FLOAT" << std::endl;

        if (temp_mati_property.size == 1) {
            float temp_float = 0;

            std::memcpy(&temp_float, &temp_mati_property.data, sizeof(bytes4));

            temp_mati_property.float_values.push_back(temp_float);

            temp_json[temp_mati_property.name] = temp_mati_property.float_values[0];
        } else {
            for (uint32_t i = 0; i < temp_mati_property.size; i++) {
                float temp_float = 0;

                std::memcpy(&temp_float, &mati_data.data()[temp_mati_property.data + i * 0x4], sizeof(bytes4));

                if (log_output)
                    std::cout << "MATI temp_mati_property.value (" << i << "): " << temp_float << std::endl;

                temp_mati_property.float_values.push_back(temp_float);
            }

            temp_json[temp_mati_property.name] = temp_mati_property.float_values;
        }

        mati_properties.push_back(temp_mati_property);
    } else if (temp_mati_property.type == 1) {
        if (log_output)
            std::cout << "MATI temp_property_type: STRING" << std::endl;

        temp_mati_property.string_value = std::string(&mati_data.data()[temp_mati_property.data]);

        if (log_output)
            std::cout << "MATI temp_mati_property.value: " << temp_mati_property.string_value << std::endl;

        mati_properties.push_back(temp_mati_property);

        temp_json[temp_mati_property.name] = temp_mati_property.string_value;
    } else if (temp_mati_property.type == 2) {
        if (log_output)
            std::cout << "MATI temp_property_type: INT" << std::endl;

        temp_mati_property.int_value = temp_mati_property.data;

        mati_properties.push_back(temp_mati_property);

        if (temp_mati_property.name == "TXID") {
            if (temp_mati_property.int_value == -1) {
                temp_json[temp_mati_property.name] = "";
            } else if (from_mati_file) {
                temp_json[temp_mati_property.name] = util::hash_to_ioi_string(
                        meta_data.hash_reference_data.hash_reference.at(temp_mati_property.int_value), true);
            } else {
                temp_json[temp_mati_property.name] = util::hash_to_ioi_string(
                        rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(
                                temp_mati_property.int_value), true);
            }
        } else {
            temp_json[temp_mati_property.name] = temp_mati_property.int_value;
        }
    } else if (temp_mati_property.type == 3) {
        if (log_output)
            std::cout << "MATI temp_property_type: PROPERTY" << std::endl;

        position = temp_mati_property.data;

        parent = mati_properties.size();

        nlohmann::ordered_json temp_json_array = nlohmann::ordered_json::array();

        nlohmann::ordered_json temp_json_object = nlohmann::ordered_json::object();

        for (uint32_t p = 0; p < temp_mati_property.size; p++) {
            temp_json_object = read_properties_json(temp_json_object, position, parent, from_mati_file);

            position += 0x10;
        }

        if (temp_json.contains(temp_mati_property.name)) {
            temp_json[temp_mati_property.name].push_back(temp_json_object);
        } else {
            temp_json_array.push_back(temp_json_object);

            temp_json[temp_mati_property.name] = temp_json_array;
        }
    }

    return temp_json;
}

mati::mati(std::string mati_path, std::string mati_meta_path, uint64_t hash_value, std::string output_path,
           bool output_path_is_file) {
    std::ifstream mati_file(mati_path, std::ifstream::binary);

    mati_file.seekg(0, mati_file.end);
    uint64_t mati_file_size = mati_file.tellg();
    mati_file.seekg(0, mati_file.beg);

    mati_data = std::vector<char>(mati_file_size, 0);

    mati_file.read(mati_data.data(), mati_file_size);

    mati_file.close();

    std::ifstream mati_meta_file(mati_meta_path, std::ifstream::binary);

    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    char input[1024];

    std::string json_string = "{";

    mati_meta_file.seekg(0, mati_meta_file.beg);

    mati_meta_file.read(input, sizeof(bytes8));
    std::memcpy(&bytes8, input, sizeof(bytes8));
    meta_data.hash_value = bytes8;

    mati_meta_file.read(input, sizeof(bytes8));
    std::memcpy(&bytes8, input, sizeof(bytes8));
    meta_data.data.header.data_offset = bytes8;

    mati_meta_file.read(input, sizeof(bytes4));
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

    mati_meta_file.read(input, sizeof(bytes4));
    input[4] = 0x0;
    meta_data.hash_resource_type = std::string(input);

    mati_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.reference_table_size = bytes4;

    mati_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.reference_table_dummy = bytes4;

    mati_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_final = bytes4;

    mati_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_in_memory = bytes4;

    mati_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&bytes4, input, sizeof(bytes4));
    meta_data.data.resource.size_in_video_memory = bytes4;

    hash_reference_variables temp_hash_reference_data;

    if (meta_data.data.resource.reference_table_size != 0x0) {
        temp_hash_reference_data.hash_value = meta_data.hash_value;

        mati_meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        temp_hash_reference_data.hash_reference_count = bytes4;

        uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

        for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
            mati_meta_file.read(input, sizeof(bytes1));
            std::memcpy(&bytes1, input, sizeof(bytes1));
            temp_hash_reference_data.hash_reference_type.push_back(bytes1);
        }

        for (uint64_t j = 0; j < temp_hash_reference_count; j++) {
            mati_meta_file.read(input, sizeof(bytes8));
            std::memcpy(&bytes8, input, sizeof(bytes8));
            temp_hash_reference_data.hash_reference.push_back(bytes8);
        }
    } else {
        temp_hash_reference_data.hash_reference_count = 0x0;
    }

    meta_data.hash_reference_data = temp_hash_reference_data;

    mati_meta_file.close();

    uint32_t position = 0;

    std::memcpy(&header_offset, &mati_data.data()[position], sizeof(bytes4));
    position = header_offset;

    std::memcpy(&type_offset, &mati_data.data()[position], sizeof(bytes4));
    position += 4;

    type = std::string(&mati_data.data()[type_offset]);

    std::memcpy(&texture_count, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&unknown_flag_1, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&unknown_flag_2, &mati_data.data()[position], sizeof(bytes4));
    position += 0x4;

    std::memcpy(&eres_index, &mati_data.data()[position], sizeof(bytes4));
    position += 0xC;

    std::memcpy(&instance_offset, &mati_data.data()[position], sizeof(bytes4));
    position = instance_offset;

    if (log_output) {
        std::cout << "MATI header_offset: " << util::uint32_t_to_hex_string(header_offset) << std::endl;
        std::cout << "MATI type_offset: " << util::uint32_t_to_hex_string(type_offset) << std::endl;
        std::cout << "MATI type: " << type << std::endl;
        std::cout << "MATI texture_count: " << texture_count << std::endl;
        std::cout << "MATI unknown_flag_1: " << unknown_flag_1 << std::endl;
        std::cout << "MATI unknown_flag_2: " << unknown_flag_2 << std::endl;
        std::cout << "MATI eres_index: " << eres_index << std::endl;
        std::cout << "MATI instance_offset: " << util::uint32_t_to_hex_string(instance_offset) << std::endl;
    }

    json["$schema"] = "https://glaciermodding.org/schemas/mati.schema.json";
    json["TYPE"] = type;

    if (eres_index == -1) {
        json["ERES"] = "";
    } else {
        json["ERES"] = util::hash_to_ioi_string(meta_data.hash_reference_data.hash_reference.at(eres_index), true);
    }

    for (uint32_t i = 0; i < meta_data.hash_reference_data.hash_reference.size(); i++) {
        if (util::hash_type(meta_data.hash_reference_data.hash_reference.at(i)) == "MATE") {
            mate_index = i;
        }
    }

    if (mate_index == -1) {
        json["MATE"] = "";
    } else {
        json["MATE"] = util::hash_to_ioi_string(meta_data.hash_reference_data.hash_reference.at(mate_index), true);
    }

    json["UnknownFlags"]["Unk1"] = unknown_flag_1;
    json["UnknownFlags"]["Unk2"] = unknown_flag_2;

    json = read_properties_json(json, position, -1, true);

    std::ofstream json_file;

    if (output_path_is_file) {
        json_file = std::ofstream(output_path, std::ofstream::binary);
    } else {
        file::create_directories(output_path);
        json_file = std::ofstream(
                file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".MATI.JSON", output_path),
                std::ofstream::binary);
    }

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

mati::mati(std::string json_path, uint64_t hash_value, std::string output_path, bool output_path_is_file) {
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
        uint64_t eres_hash_depends = 0;
        uint64_t mate_hash_depends = 0;
        std::vector<uint64_t> text_hash_depends;
        std::vector<uint32_t> text_hash_depends_txids;

        std::string temp_string = json["ERES"];

        eres_hash_depends = util::ioi_string_to_hash(temp_string);

        temp_string = json["MATE"];

        mate_hash_depends = util::ioi_string_to_hash(temp_string);

        type = json["TYPE"];
        unknown_flag_1 = json["UnknownFlags"]["Unk1"];
        unknown_flag_2 = json["UnknownFlags"]["Unk2"];

        texture_count = 0;

        if (json["INST"][0]["BIND"][0].contains("TEXT")) {
            for (uint32_t i = 0; i < json["INST"][0]["BIND"][0]["TEXT"].size(); i++) {
                if (json["INST"][0]["BIND"][0]["TEXT"][i].contains("TXID")) {
                    temp_string = json["INST"][0]["BIND"][0]["TEXT"][i]["TXID"];

                    if (temp_string == "") {
                        text_hash_depends_txids.push_back(0xFFFFFFFF);
                    } else {
                        text_hash_depends_txids.push_back(texture_count);

                        text_hash_depends.push_back(util::ioi_string_to_hash(temp_string));

                        texture_count++;
                    }
                }
            }
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
        meta_data.push_back('M');
        meta_data.push_back('A');
        meta_data.push_back('T');
        meta_data.push_back('I');

        uint32_t hash_depends_count = 0;

        if (eres_hash_depends != 0) {
            hash_depends_count++;
        }

        if (mate_hash_depends != 0) {
            hash_depends_count++;
        }

        hash_depends_count += text_hash_depends.size();

        uint32_t hash_reference_table_size = hash_depends_count * 0x8 + hash_depends_count + 0x4;

        hash_depends_count = hash_depends_count | 0xC0000000;

        for (uint32_t k = 0; k < sizeof(hash_reference_table_size); k++) {
            meta_data.push_back(*((char*) &hash_reference_table_size + k));
        }

        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);
        meta_data.push_back(0x0);

        uint32_t mati_size_offset = meta_data.size();
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

        for (uint32_t k = 0; k < sizeof(hash_depends_count); k++) {
            meta_data.push_back(*((char*) &hash_depends_count + k));
        }

        for (uint32_t k = 0; k < text_hash_depends.size(); k++) {
            meta_data.push_back(0x5F);
        }

        if (mate_hash_depends != 0) {
            meta_data.push_back(0x1F);
        }

        if (eres_hash_depends != 0) {
            meta_data.push_back(0x5F);
        }

        eres_index = 0;

        for (uint32_t i = 0; i < text_hash_depends.size(); i++) {
            for (uint32_t k = 0; k < sizeof(text_hash_depends.at(i)); k++) {
                meta_data.push_back(*((char*) &text_hash_depends.at(i) + k));
            }

            eres_index++;
        }

        if (mate_hash_depends != 0) {
            for (uint32_t k = 0; k < sizeof(mate_hash_depends); k++) {
                meta_data.push_back(*((char*) &mate_hash_depends + k));
            }

            eres_index++;
        }

        if (eres_hash_depends != 0) {
            for (uint32_t k = 0; k < sizeof(eres_hash_depends); k++) {
                meta_data.push_back(*((char*) &eres_hash_depends + k));
            }
        } else {
            eres_index = -1;
        }

        uint32_t type_length = ((type.length() / 0x10) + 1) * 0x10;

        write_uint32_t((uint32_t) (type_length + 0x10));
        align();

        write_string(type);
        align();

        uint32_t instance_offset = (mati_data.size() + 0x30);

        write_uint32_t((uint32_t) 0x10);
        write_uint32_t(texture_count);
        write_uint32_t(unknown_flag_1);
        write_uint32_t(unknown_flag_2);
        write_uint32_t((uint32_t) eres_index);
        write_uint32_t((uint32_t) 0x0);
        write_uint32_t((uint32_t) 0x0);
        write_uint32_t((uint32_t) instance_offset);
        write_uint32_t((uint32_t) 0x3);
        write_uint32_t((uint32_t) 0x0);
        write_uint32_t((uint32_t) 0x0);
        write_uint32_t((uint32_t) 0x0);

        uint32_t temp_offset = mati_data.size();

        write_name("INST");
        write_uint32_t((uint32_t) (temp_offset + 0x10));
        write_uint32_t((uint32_t) json["INST"][0].size());
        write_property_type("INST");

        std::vector<uint32_t> offsets_values;
        std::vector<MATI_PROPERTY_TYPE> offsets_types;
        std::vector<nlohmann::ordered_json> offsets_data;

        temp_offset = mati_data.size();

        std::string name = json["INST"][0]["NAME"];
        std::string tags = json["INST"][0]["TAGS"];

        write_name("NAME");
        offsets_values.push_back((uint32_t) mati_data.size());
        offsets_types.push_back(MATI_PROPERTY_TYPE::STRING);
        offsets_data.push_back(json["INST"][0]["NAME"]);
        write_uint32_t((uint32_t) 0x0);
        write_uint32_t((uint32_t) (name.length() + 1));
        write_property_type("NAME");

        write_name("TAGS");
        offsets_values.push_back((uint32_t) mati_data.size());
        offsets_types.push_back(MATI_PROPERTY_TYPE::STRING);
        offsets_data.push_back(json["INST"][0]["TAGS"]);
        write_uint32_t((uint32_t) 0x0);
        write_uint32_t((uint32_t) (tags.length() + 1));
        write_property_type("TAGS");

        uint32_t bind_count = 0;

        for (const auto& it : json["INST"][0]["BIND"][0].items()) {
            for (const auto& it2 : it.value().items()) {
                bind_count++;
            }
        }

        write_name("BIND");
        write_uint32_t((uint32_t) (temp_offset + 0x30));
        write_uint32_t(bind_count);
        write_property_type("BIND");

        for (const auto& it : json["INST"][0]["BIND"][0].items()) {
            for (const auto& it2 : it.value().items()) {
                write_name(it.key());
                offsets_values.push_back((uint32_t) mati_data.size());
                offsets_types.push_back(MATI_PROPERTY_TYPE::PROPERTY);
                offsets_data.push_back(it2.value());
                write_uint32_t((uint32_t) 0x0);
                write_uint32_t((uint32_t) it2.value().size());
                write_property_type(it.key());
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

                    if (get_property_type(it.key()) == MATI_PROPERTY_TYPE::FLOAT) {
                        if (it.value().is_array()) {
                            write_name(it.key());
                            offsets_values.push_back((uint32_t) mati_data.size());
                            offsets_types.push_back(MATI_PROPERTY_TYPE::FLOAT);
                            offsets_data.push_back(it.value());
                            write_uint32_t((uint32_t) 0x0);
                            write_uint32_t((uint32_t) it.value().size());
                            write_property_type(it.key());
                        } else {
                            write_name(it.key());
                            write_float((float) it.value());
                            write_uint32_t((uint32_t) 0x1);
                            write_property_type(it.key());
                        }
                    } else if (get_property_type(it.key()) == MATI_PROPERTY_TYPE::STRING) {
                        std::string temp_string = it.value();

                        write_name(it.key());
                        offsets_values.push_back((uint32_t) mati_data.size());
                        offsets_types.push_back(MATI_PROPERTY_TYPE::STRING);
                        offsets_data.push_back(it.value());
                        write_uint32_t((uint32_t) 0x0);
                        write_uint32_t((uint32_t) temp_string.size());
                        write_property_type(it.key());
                    } else if (get_property_type(it.key()) == MATI_PROPERTY_TYPE::INT) {
                        if (it.key() == "TXID") {
                            write_name(it.key());
                            write_uint32_t((uint32_t) text_hash_depends_txids.at(txid_index));
                            write_uint32_t((uint32_t) 0x1);
                            write_property_type(it.key());

                            txid_index++;
                        } else {
                            write_name(it.key());
                            write_uint32_t((uint32_t) it.value());
                            write_uint32_t((uint32_t) 0x1);
                            write_property_type(it.key());
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

                    for (uint32_t i = 0; i < temp_json.size(); i++) {
                        write_float(temp_json[i]);
                    }

                    align();
                }
            } else if (offsets_types.at(i) == MATI_PROPERTY_TYPE::STRING) {
                temp_offset = mati_data.size();

                std::memcpy(&mati_data[offsets_values.at(i)], &temp_offset, 0x4);

                std::string temp_string = temp_json;

                write_string(temp_string);

                align();
            }
        }

        std::ofstream mati_file;
        std::ofstream meta_file;

        if (output_path_is_file) {
            mati_file = std::ofstream(output_path, std::ofstream::binary);
            meta_file = std::ofstream(output_path + ".meta", std::ofstream::binary);
        } else {
            file::create_directories(output_path);
            mati_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".MATI", output_path),
                    std::ofstream::binary);
            meta_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".MATI.meta", output_path),
                    std::ofstream::binary);
        }

        mati_file.write(mati_data.data(), mati_data.size());

        mati_file.close();

        uint32_t mati_data_size = mati_data.size();

        std::memcpy(&meta_data.data()[mati_size_offset], &mati_data_size, 0x4);

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

MATI_PROPERTY_TYPE mati::get_property_type(std::string key) {
    std::unordered_map<std::string, MATI_PROPERTY_TYPE>::iterator it = mati_property_type_map.find(key);

    if (it != mati_property_type_map.end()) {
        return it->second;
    }

    return MATI_PROPERTY_TYPE::UNKNOWN;
}

void mati::align() {
    while (mati_data.size() % 0x10 != 0) {
        mati_data.push_back(0x0);
    }
}

void mati::write_uint32_t(uint32_t data) {
    char tmp_uint32_t[4];

    std::memcpy(&tmp_uint32_t, &data, 0x4);

    for (int i = 0; i < 0x4; i++) {
        mati_data.push_back(*((char*) &tmp_uint32_t + i));
    }
}

void mati::write_string(std::string data) {
    for (int i = 0; i < data.length(); i++) {
        mati_data.push_back(data[i]);
    }

    mati_data.push_back(0x0);
}

void mati::write_float(float data) {
    char tmp_float[4];

    std::memcpy(&tmp_float, &data, 0x4);

    for (int i = 0; i < 0x4; i++) {
        mati_data.push_back(*((char*) &tmp_float + i));
    }
}

void mati::write_name(std::string name) {
    mati_data.push_back(name[3]);
    mati_data.push_back(name[2]);
    mati_data.push_back(name[1]);
    mati_data.push_back(name[0]);
}

void mati::write_property_type(std::string name) {
    std::unordered_map<std::string, MATI_PROPERTY_TYPE>::iterator it = mati_property_type_map.find(name);

    if (it != mati_property_type_map.end()) {
        write_uint32_t((uint32_t) it->second);
    }
}
