#include "mrtr.h"
#include "rpkg_function.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "file.h"
#include "thirdparty/lz4/lz4.h"
#include "DirectXMath.h"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <typeinfo>

mrtr::mrtr() {

}

mrtr::mrtr(uint64_t rpkgs_index, uint64_t hash_index) {
    mrtr_rpkg_index = rpkgs_index;
    mrtr_hash_index = hash_index;

    mrtr_file_name = util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." +
                     rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type;

    uint64_t mrtr_hash_size;

    if (rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.lz4ed) {
        mrtr_hash_size = rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.header.data_size;

        if (rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.xored) {
            mrtr_hash_size &= 0x3FFFFFFF;
        }
    } else {
        mrtr_hash_size = rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.resource.size_final;
    }

    mrtr_input_data = std::vector<char>(mrtr_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(mrtr_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good()) {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(mrtr_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.header.data_offset, file.beg);
    file.read(mrtr_input_data.data(), mrtr_hash_size);
    file.close();

    if (rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.xored) {
        crypto::xor_data(mrtr_input_data.data(), (uint32_t) mrtr_hash_size);
    }

    const uint32_t mrtr_decompressed_size = rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.resource.size_final;

    mrtr_output_data = std::vector<char>(mrtr_decompressed_size, 0);

    if (rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).data.lz4ed) {
        LZ4_decompress_safe(mrtr_input_data.data(), mrtr_output_data.data(), (int) mrtr_hash_size,
                            mrtr_decompressed_size);

        mrtr_data = mrtr_output_data;
    } else {
        mrtr_data = mrtr_input_data;
    }

    std::vector<char>().swap(mrtr_output_data);
    std::vector<char>().swap(mrtr_input_data);
}

void mrtr::generate_json() {
    uint32_t uint32_t_bytes = 0;
    float float_bytes = 0;

    json = nlohmann::ordered_json::object();

    json["$schema"] = "https://wiki.glaciermodding.org/schemas/mrtr.schema.json";

    json["Bones"] = nlohmann::ordered_json::array();

    //std::memcpy(&uint32_t_bytes, &mrtr_data.data()[0x18], 0x4);
    //json["Unknown"] = uint32_t_bytes;
    std::memcpy(&uint32_t_bytes, &mrtr_data.data()[0x80], 0x4);
    mrtr_entry_count = uint32_t_bytes;
    uint32_t position = 0x90;

    for (uint32_t i = 0; i < mrtr_entry_count; i++) {
        mrtr_entry entry;

        entry.index = i;
        std::memcpy(&uint32_t_bytes, &mrtr_data.data()[position], 0x4);
        entry.parent = uint32_t_bytes;

        mrtr_entries.push_back(entry);

        position += 0x4;
    }

    std::memcpy(&uint32_t_bytes, &mrtr_data.data()[0x30], 0x4);
    position = uint32_t_bytes;

    for (uint32_t i = 0; i < mrtr_entry_count; i++) {
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].position.x = float_bytes;
        position += 0x4;
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].position.y = float_bytes;
        position += 0x4;
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].position.z = float_bytes;
        position += 0x8;
    }

    std::memcpy(&uint32_t_bytes, &mrtr_data.data()[0x28], 0x4);
    position = uint32_t_bytes;

    for (uint32_t i = 0; i < mrtr_entry_count; i++) {
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].rotation.x = float_bytes;
        position += 0x4;
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].rotation.y = float_bytes;
        position += 0x4;
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].rotation.z = float_bytes;
        position += 0x4;
        std::memcpy(&float_bytes, &mrtr_data.data()[position], 0x4);
        mrtr_entries[i].rotation.w = float_bytes;
        position += 0x4;
    }

    std::memcpy(&uint32_t_bytes, &mrtr_data.data()[0x20], 0x4);
    position = uint32_t_bytes + 0x20;
    uint32_t strings_offset = uint32_t_bytes;
    std::memcpy(&uint32_t_bytes, &mrtr_data.data()[position - 0x8], 0x4);
    strings_offset += uint32_t_bytes;

    for (uint32_t i = 0; i < mrtr_entry_count; i++) {
        std::memcpy(&uint32_t_bytes, &mrtr_data.data()[position], 0x4);
        uint32_t index = uint32_t_bytes;
        std::memcpy(&uint32_t_bytes, &mrtr_data.data()[position + mrtr_entry_count * 0x4], 0x4);
        uint32_t string_position = uint32_t_bytes;
        position += 0x4;

        mrtr_entries[index].name = std::string(&mrtr_data.data()[strings_offset + string_position]);
    }

    for (uint32_t i = 0; i < mrtr_entry_count; i++) {
        nlohmann::ordered_json temp_json = nlohmann::ordered_json::object();

        //temp_json["Index"] = mrtr_entries[i].index;

        if (mrtr_entries[i].parent == 0xFFFFFFFF)
            temp_json["Parent"] = nullptr;
        else
            temp_json["Parent"] = mrtr_entries[i].parent;

        temp_json["Name"] = mrtr_entries[i].name;

        temp_json["Position"]["x"] = mrtr_entries[i].position.x;
        temp_json["Position"]["y"] = mrtr_entries[i].position.y;
        temp_json["Position"]["z"] = mrtr_entries[i].position.z;

        doublematrix43 matrix;

        double x = mrtr_entries[i].rotation.x;
        double y = mrtr_entries[i].rotation.y;
        double z = mrtr_entries[i].rotation.z;
        double w = mrtr_entries[i].rotation.w;
        double x2 = x + x;
        double y2 = y + y;
        double z2 = z + z;
        double xx = x * x2;
        double xy = x * y2;
        double xz = x * z2;
        double yy = y * y2;
        double yz = y * z2;
        double zz = z * z2;
        double wx = w * x2;
        double wy = w * y2;
        double wz = w * z2;

        matrix.x_axis.x = 1.0 - (yy + zz);
        matrix.y_axis.x = xy + wz;
        matrix.z_axis.x = xz - wy;

        matrix.x_axis.y = xy - wz;
        matrix.y_axis.y = 1.0 - (xx + zz);
        matrix.z_axis.y = yz + wx;

        matrix.x_axis.z = xz + wy;
        matrix.y_axis.z = yz - wx;
        matrix.z_axis.z = 1.0 - (xx + yy);

        doublevector3 euler_angles;

        double radians_to_degrees = 180.0 / 3.14159265358979323846;

        euler_angles.y = std::asin(std::clamp(matrix.x_axis.z, -1.0, 1.0));

        if (std::abs(matrix.x_axis.z) < 0.9999999) {
            euler_angles.x = std::atan2(-matrix.y_axis.z, matrix.z_axis.z);
            euler_angles.z = std::atan2(-matrix.x_axis.y, matrix.x_axis.x);
        } else {
            euler_angles.x = std::atan2(matrix.z_axis.y, matrix.y_axis.y);
            euler_angles.z = 0;
        }

        if (euler_angles.x == 0.0 && std::signbit(euler_angles.x))
            euler_angles.x = 0.0;
        if (euler_angles.y == 0.0 && std::signbit(euler_angles.y))
            euler_angles.y = 0.0;
        if (euler_angles.z == 0.0 && std::signbit(euler_angles.z))
            euler_angles.z = 0.0;

        //std::cout << "euler_angles.x: " << euler_angles.x * radians_to_degrees << std::endl;
        //std::cout << "euler_angles.y: " << euler_angles.y * radians_to_degrees << std::endl;
        //std::cout << "euler_angles.z: " << euler_angles.z * radians_to_degrees << std::endl;

        temp_json["Rotation"]["x"] = euler_angles.x * radians_to_degrees;
        temp_json["Rotation"]["y"] = euler_angles.y * radians_to_degrees;
        temp_json["Rotation"]["z"] = euler_angles.z * radians_to_degrees;

        json["Bones"].push_back(temp_json);
    }
}


void mrtr::write_json_to_file(std::string output_path) {
    file::create_directories(output_path);

    std::ofstream json_file = std::ofstream(file::output_path_append(
                                                    util::uint64_t_to_hex_string(rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).hash_value) + "." +
                                                    rpkgs.at(mrtr_rpkg_index).hash.at(mrtr_hash_index).hash_resource_type + ".JSON", output_path),
                                            std::ofstream::binary);

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

mrtr::mrtr(std::string mrtr_path, std::string mrtr_meta_path, uint64_t hash_value, std::string output_path,
           bool output_path_is_file) {
    std::ifstream mrtr_file(mrtr_path, std::ifstream::binary);

    mrtr_file.seekg(0, mrtr_file.end);
    uint64_t mrtr_file_size = mrtr_file.tellg();
    mrtr_file.seekg(0, mrtr_file.beg);

    mrtr_data = std::vector<char>(mrtr_file_size, 0);

    mrtr_file.read(mrtr_data.data(), mrtr_file_size);

    mrtr_file.close();

    generate_json();

    std::ofstream json_file;

    if (output_path_is_file) {
        json_file = std::ofstream(output_path, std::ofstream::binary);
    } else {
        file::create_directories(output_path);
        json_file = std::ofstream(
                file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".mrtr.JSON", output_path),
                std::ofstream::binary);
    }

    json_file << std::setw(4) << json << std::endl;

    json_file.close();
}

mrtr::mrtr(std::string json_path, uint64_t hash_value, std::string output_path, bool output_path_is_file) {
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
        std::vector<char>().swap(mrtr_data);

        uint32_t temp_uint32_t = 0;

        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x80);
        mrtr_data.push_back(0x3F);
        mrtr_data.push_back(0x80);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);

        uint32_t ground_index_offset = mrtr_data.size();

        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x1);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);

        uint32_t names_offset = mrtr_data.size();

        for (uint32_t k = 0; k < 0x8; k++) {
            mrtr_data.push_back(0x0);
        }

        uint32_t rotations_offset = mrtr_data.size();

        for (uint32_t k = 0; k < 0x8; k++) {
            mrtr_data.push_back(0x0);
        }

        uint32_t positions_offset = mrtr_data.size();

        for (uint32_t k = 0; k < 0xC; k++) {
            mrtr_data.push_back(0x0);
        }

        for (uint32_t k = 0; k < 0x4; k++) {
            mrtr_data.push_back(0xCD);
        }

        for (uint32_t k = 0; k < 0xC; k++) {
            mrtr_data.push_back(0x0);
        }

        for (uint32_t k = 0; k < 0x34; k++) {
            mrtr_data.push_back(0xCD);
        }

        uint32_t bones_count = json["Bones"].size();

        for (uint32_t k = 0; k < sizeof(bones_count); k++) {
            mrtr_data.push_back(*((char*) &bones_count + k));
        }

        for (uint32_t k = 0; k < 0x4; k++) {
            mrtr_data.push_back(0xCD);
        }

        mrtr_data.push_back(0x10);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);

        for (auto& bone : json["Bones"]) {
            uint32_t parent = 0;

            if (bone["Parent"].is_null()) {
                parent = 0xFFFFFFFF;
            } else {
                parent = bone["Parent"];
            }

            for (uint32_t k = 0; k < sizeof(parent); k++) {
                mrtr_data.push_back(*((char*) &parent + k));
            }
        }

        while ((mrtr_data.size() % 0x10) != 0) {
            mrtr_data.push_back(0xCD);
        }

        temp_uint32_t = mrtr_data.size();
        std::memcpy(&mrtr_data.data()[positions_offset], &temp_uint32_t, sizeof(temp_uint32_t));

        for (auto& bone : json["Bones"]) {
            vector3 position;
            position.x = bone["Position"]["x"];
            position.y = bone["Position"]["y"];
            position.z = bone["Position"]["z"];

            for (uint32_t k = 0; k < sizeof(position.x); k++) {
                mrtr_data.push_back(*((char*) &position.x + k));
            }

            for (uint32_t k = 0; k < sizeof(position.y); k++) {
                mrtr_data.push_back(*((char*) &position.y + k));
            }

            for (uint32_t k = 0; k < sizeof(position.z); k++) {
                mrtr_data.push_back(*((char*) &position.z + k));
            }

            for (uint32_t k = 0; k < 0x4; k++) {
                mrtr_data.push_back(0xCD);
            }
        }

        temp_uint32_t = mrtr_data.size();
        std::memcpy(&mrtr_data.data()[rotations_offset], &temp_uint32_t, sizeof(temp_uint32_t));

        for (auto& bone : json["Bones"]) {
            doublematrix43 matrix;
            vector4 rotation;

            double degrees_to_radians = 3.14159265358979323846 / 180.0;

            double x = bone["Rotation"]["x"].get<double>() * degrees_to_radians;
            double y = bone["Rotation"]["y"].get<double>() * degrees_to_radians;
            double z = bone["Rotation"]["z"].get<double>() * degrees_to_radians;

            double c1 = std::cos(x / 2.0);
            double c2 = std::cos(y / 2.0);
            double c3 = std::cos(z / 2.0);

            double s1 = std::sin(x / 2.0);
            double s2 = std::sin(y / 2.0);
            double s3 = std::sin(z / 2.0);

            rotation.x = s1 * c2 * c3 + c1 * s2 * s3;
            rotation.y = c1 * s2 * c3 - s1 * c2 * s3;
            rotation.z = c1 * c2 * s3 + s1 * s2 * c3;
            rotation.w = c1 * c2 * c3 - s1 * s2 * s3;

            /*double a = std::cos(x);
            double b = std::sin(x);
            double c = std::cos(y);
            double d = std::sin(y);
            double e = std::cos(z);
            double f = std::sin(z);

            double ae = a * e;
            double af = a * f;
            double be = b * e;
            double bf = b * f;

            matrix.x_axis.x = c * e;
            matrix.y_axis.x = -c * f;
            matrix.z_axis.x = d;

            matrix.x_axis.y = af + be * d;
            matrix.y_axis.y = ae - bf * d;
            matrix.z_axis.y = -b * c;

            matrix.x_axis.z = bf - ae * d;
            matrix.y_axis.z = be + af * d;
            matrix.z_axis.z = a * c;

            double trace = matrix.x_axis.x + matrix.y_axis.y + matrix.z_axis.z;

            if (trace > 0)
            {
                double s = 0.5 / std::sqrt(trace + 1.0);

                rotation.x = (matrix.z_axis.y - matrix.y_axis.z) * s;
                rotation.y = (matrix.x_axis.z - matrix.z_axis.x) * s;
                rotation.z = (matrix.y_axis.x - matrix.x_axis.y) * s;
                rotation.w = 0.25 / s;
            }
            else if (matrix.x_axis.x > matrix.y_axis.y && matrix.x_axis.x > matrix.z_axis.z)
            {
                double s = 2.0 * std::sqrt(1.0 + matrix.x_axis.x - matrix.y_axis.y - matrix.z_axis.z);

                rotation.x = 0.25 * s;
                rotation.y = (matrix.x_axis.y + matrix.y_axis.x) / s;
                rotation.z = (matrix.x_axis.z + matrix.z_axis.x) / s;
                rotation.w = (matrix.z_axis.y - matrix.y_axis.z) / s;
            }
            else if (matrix.y_axis.y > matrix.z_axis.z)
            {
                double s = 2.0 * std::sqrt(1.0 + matrix.y_axis.y - matrix.x_axis.x - matrix.z_axis.z);

                rotation.x = (matrix.x_axis.y + matrix.y_axis.x) / s;
                rotation.y = 0.25 * s;
                rotation.z = (matrix.y_axis.z + matrix.z_axis.y) / s;
                rotation.w = (matrix.x_axis.z - matrix.z_axis.x) / s;
            }
            else
            {
                double s = 2.0 * std::sqrt(1.0 + matrix.z_axis.z - matrix.x_axis.x - matrix.y_axis.y);

                rotation.x = (matrix.x_axis.z + matrix.z_axis.x) / s;
                rotation.y = (matrix.y_axis.z + matrix.z_axis.y) / s;
                rotation.z = 0.25 * s;
                rotation.w = (matrix.y_axis.x - matrix.x_axis.y) / s;
            }*/

            for (uint32_t k = 0; k < sizeof(rotation.x); k++) {
                mrtr_data.push_back(*((char*) &rotation.x + k));
            }

            for (uint32_t k = 0; k < sizeof(rotation.y); k++) {
                mrtr_data.push_back(*((char*) &rotation.y + k));
            }

            for (uint32_t k = 0; k < sizeof(rotation.z); k++) {
                mrtr_data.push_back(*((char*) &rotation.z + k));
            }

            for (uint32_t k = 0; k < sizeof(rotation.w); k++) {
                mrtr_data.push_back(*((char*) &rotation.w + k));
            }
        }

        temp_uint32_t = mrtr_data.size();
        std::memcpy(&mrtr_data.data()[names_offset], &temp_uint32_t, sizeof(temp_uint32_t));
        std::memcpy(&mrtr_data.data()[names_offset + 0x18], &temp_uint32_t, sizeof(temp_uint32_t));

        for (uint32_t k = 0; k < sizeof(bones_count); k++) {
            mrtr_data.push_back(*((char*) &bones_count + k));
        }

        temp_uint32_t = 0;

        for (auto& bone : json["Bones"]) {
            temp_uint32_t += std::string(bone["Name"]).length() + 1;
        }

        for (uint32_t k = 0; k < sizeof(temp_uint32_t); k++) {
            mrtr_data.push_back(*((char*) &temp_uint32_t + k));
        }

        mrtr_data.push_back(0x20);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);

        temp_uint32_t = bones_count * 0x4 + 0x20;

        for (uint32_t k = 0; k < sizeof(temp_uint32_t); k++) {
            mrtr_data.push_back(*((char*) &temp_uint32_t + k));
        }

        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);

        temp_uint32_t += bones_count * 0x4;

        for (uint32_t k = 0; k < sizeof(temp_uint32_t); k++) {
            mrtr_data.push_back(*((char*) &temp_uint32_t + k));
        }

        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);
        mrtr_data.push_back(0x0);

        for (uint32_t i = 0; i < bones_count; i++) {
            for (uint32_t k = 0; k < sizeof(i); k++) {
                mrtr_data.push_back(*((char*) &i + k));
            }
        }

        temp_uint32_t = 0;

        for (auto& bone : json["Bones"]) {
            for (uint32_t k = 0; k < sizeof(temp_uint32_t); k++) {
                mrtr_data.push_back(*((char*) &temp_uint32_t + k));
            }

            temp_uint32_t += std::string(bone["Name"]).length() + 1;
        }

        uint32_t ground_bone_index = 1;

        uint32_t index = 0;

        for (auto& bone : json["Bones"]) {
            std::string bone_name = bone["Name"];

            if (bone_name == "Ground") {
                ground_bone_index = index;
            }

            for (uint32_t k = 0; k < bone_name.length(); k++) {
                mrtr_data.push_back(bone_name[k]);
            }

            mrtr_data.push_back(0x0);

            temp_uint32_t += std::string(bone["Name"]).length() + 1;

            index++;
        }

        std::memcpy(&mrtr_data.data()[ground_index_offset], &ground_bone_index, sizeof(ground_bone_index));

        while ((mrtr_data.size() % 0x4) != 0) {
            mrtr_data.push_back(0xCD);
        }

        std::ofstream mrtr_file;

        if (output_path_is_file) {
            mrtr_file = std::ofstream(output_path, std::ofstream::binary);
        } else {
            file::create_directories(output_path);
            mrtr_file = std::ofstream(
                    file::output_path_append(util::uint64_t_to_hex_string(hash_value) + ".MRTR", output_path),
                    std::ofstream::binary);
        }

        mrtr_file.write(mrtr_data.data(), mrtr_data.size());

        mrtr_file.close();
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