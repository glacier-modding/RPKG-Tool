#include "gltf.h"
#include "file.h"
#include "util.h"
#include "thirdparty/json/json.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

using json = nlohmann::ordered_json;

void gltf::output_to_multiple_files(asset3d& asset3d_data, std::string& file_path, int type)
{
    char char2[2];
    char char4[4];

    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;

    std::vector<char> gltf_bin_file_data;

    uint16_t indexes_min = 0;
    uint16_t indexes_max = 0;

    vector3 vertexes_min;
    vector3 vertexes_max;

    vector3 normals_min;
    vector3 normals_max;

    uint32_t buffer_offset_aligned_1 = 0;
    uint32_t buffer_offset_aligned_2 = 0;

    for (uint32_t v = 0; v < asset3d_data.indexes->size(); v++)
    {
        if (v == 0)
        {
            indexes_min = asset3d_data.indexes->at(v);
            indexes_max = asset3d_data.indexes->at(v);
        }
        else
        {
            if (asset3d_data.indexes->at(v) < indexes_min)
                indexes_min = asset3d_data.indexes->at(v);
            if (asset3d_data.indexes->at(v) > indexes_max)
                indexes_max = asset3d_data.indexes->at(v);
        }

        std::memcpy(&char2, &asset3d_data.indexes->at(v), sizeof(bytes2));

        for (uint32_t d = 0; d < 2; d++)
        {
            gltf_bin_file_data.push_back(char2[d]);
        }
    }

    while (gltf_bin_file_data.size() % 4 != 0)
    {
        gltf_bin_file_data.push_back(0x0);
    }

    buffer_offset_aligned_1 = gltf_bin_file_data.size();

    for (uint32_t v = 0; v < asset3d_data.vertexes->size(); v++)
    {
        if (v == 0)
        {
            vertexes_min.x = asset3d_data.vertexes->at(v).x;
            vertexes_min.y = asset3d_data.vertexes->at(v).y;
            vertexes_min.z = asset3d_data.vertexes->at(v).z;
            vertexes_max.x = asset3d_data.vertexes->at(v).x;
            vertexes_max.y = asset3d_data.vertexes->at(v).y;
            vertexes_max.z = asset3d_data.vertexes->at(v).z;
        }
        else
        {
            if (asset3d_data.vertexes->at(v).x < vertexes_min.x)
                vertexes_min.x = asset3d_data.vertexes->at(v).x;
            if (asset3d_data.vertexes->at(v).x > vertexes_max.x)
                vertexes_max.x = asset3d_data.vertexes->at(v).x;
            if (asset3d_data.vertexes->at(v).y < vertexes_min.y)
                vertexes_min.y = asset3d_data.vertexes->at(v).y;
            if (asset3d_data.vertexes->at(v).y > vertexes_max.y)
                vertexes_max.y = asset3d_data.vertexes->at(v).y;
            if (asset3d_data.vertexes->at(v).z < vertexes_min.z)
                vertexes_min.z = asset3d_data.vertexes->at(v).z;
            if (asset3d_data.vertexes->at(v).z > vertexes_max.z)
                vertexes_max.z = asset3d_data.vertexes->at(v).z;
        }

        std::memcpy(&char4, &asset3d_data.vertexes->at(v).x, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }

        std::memcpy(&char4, &asset3d_data.vertexes->at(v).y, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }

        std::memcpy(&char4, &asset3d_data.vertexes->at(v).z, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }
    }

    while (gltf_bin_file_data.size() % 4 != 0)
    {
        gltf_bin_file_data.push_back(0x0);
    }

    buffer_offset_aligned_2 = gltf_bin_file_data.size() - buffer_offset_aligned_1;

    /*for (uint32_t v = 0; v < asset3d_data.normals->size(); v++)
    {
        float magnitude = std::sqrt(asset3d_data.normals->at(v).x * asset3d_data.normals->at(v).x + asset3d_data.normals->at(v).y * asset3d_data.normals->at(v).y + asset3d_data.normals->at(v).z * asset3d_data.normals->at(v).z);

        asset3d_data.normals->at(v).x /= magnitude;
        asset3d_data.normals->at(v).y /= magnitude;
        asset3d_data.normals->at(v).z /= magnitude;
    }**/

    for (uint32_t v = 0; v < asset3d_data.normals->size(); v++)
    {
        if (v == 0)
        {
            normals_min.x = asset3d_data.normals->at(v).x;
            normals_min.y = asset3d_data.normals->at(v).y;
            normals_min.z = asset3d_data.normals->at(v).z;
            normals_max.x = asset3d_data.normals->at(v).x;
            normals_max.y = asset3d_data.normals->at(v).y;
            normals_max.z = asset3d_data.normals->at(v).z;
        }
        else
        {
            if (asset3d_data.normals->at(v).x < normals_min.x)
                normals_min.x = asset3d_data.normals->at(v).x;
            if (asset3d_data.normals->at(v).x > normals_max.x)
                normals_max.x = asset3d_data.normals->at(v).x;
            if (asset3d_data.normals->at(v).y < normals_min.y)
                normals_min.y = asset3d_data.normals->at(v).y;
            if (asset3d_data.normals->at(v).y > normals_max.y)
                normals_max.y = asset3d_data.normals->at(v).y;
            if (asset3d_data.normals->at(v).z < normals_min.z)
                normals_min.z = asset3d_data.normals->at(v).z;
            if (asset3d_data.normals->at(v).z > normals_max.z)
                normals_max.z = asset3d_data.normals->at(v).z;
        }

        std::memcpy(&char4, &asset3d_data.normals->at(v).x, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }

        std::memcpy(&char4, &asset3d_data.normals->at(v).y, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }

        std::memcpy(&char4, &asset3d_data.normals->at(v).z, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }
    }

    /*for (uint32_t v = 0; v < vertexes_uvs.size(); v++)
    {
        char char4[2];

        std::memcpy(&char4, &vertexes_uvs.at(v).x, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }

        std::memcpy(&char4, &vertexes_uvs.at(v).y, sizeof(bytes4));

        for (uint32_t d = 0; d < 4; d++)
        {
            gltf_bin_file_data.push_back(char4[d]);
        }
    }*/

    json gltf_json_file;

    gltf_json_file["asset"]["generator"] = "RPKG";
    gltf_json_file["asset"]["version"] = "2.0";

    gltf_json_file["scene"] = 0;

    gltf_json_file["scenes"] = { { { "nodes", { 0 } } } };

    gltf_json_file["nodes"] = { { { "mesh", 0 } } };

    gltf_json_file["meshes"] = { { { "primitives", { { { "attributes", {
                                                { "POSITION", 1 },
                                                { "NORMAL", 2 } } },
                                                { "indices", 0 },
                                                { "material", 0 } } } } } };

    gltf_json_file["materials"] = { { { "pbrMetallicRoughness", { { "baseColorFactor", { 1.0, 1.0, 1.0, 1.0 } },
                                                                {"metallicFactor", 0.0},
                                                                {"roughnessFactor", 0.0 } } } } };

    if (type == GLB_MULTIPLE)
    {
        gltf_json_file["buffers"] = { { { "byteLength", gltf_bin_file_data.size() } } };
    }
    else if (type == GLTF_MULTIPLE)
    {
        std::string gltf_bin_file_name = file_path + ".bin";

        std::ofstream output_file = std::ofstream(gltf_bin_file_name, std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: glTF bin file " + gltf_bin_file_name + " could not be created.");
        }

        output_file.write(gltf_bin_file_data.data(), gltf_bin_file_data.size());

        output_file.close();

        gltf_json_file["buffers"] = { { { "uri", file_path + ".bin" },
                                        { "byteLength", gltf_bin_file_data.size() } } };
    }

    gltf_json_file["bufferViews"] = { { { "buffer", 0 },
                                        { "byteOffset", 0 },
                                        { "byteLength", asset3d_data.indexes->size() * 0x2 },
                                        { "target", 34963 } },
                                        { { "buffer", 0 },
                                        { "byteOffset", buffer_offset_aligned_1 },
                                        { "byteLength", asset3d_data.vertexes->size() * 0xC },
                                        { "target", 34962 } },
                                        { { "buffer", 0 },
                                        { "byteOffset", buffer_offset_aligned_2 },
                                        { "byteLength", asset3d_data.vertexes->size() * 0xC },
                                        { "target", 34962 } } };

    gltf_json_file["accessors"] = { { { "bufferView", 0 },
                                        { "byteOffset", 0 },
                                        { "componentType", 5123 },
                                        { "count", asset3d_data.indexes->size() },
                                        { "type", "SCALAR" },
                                        { "max", { indexes_max } },
                                        { "min", { indexes_min } } },
                                        { { "bufferView", 1 },
                                        { "byteOffset", 0 },
                                        { "componentType", 5126 },
                                        { "count", asset3d_data.vertexes->size() },
                                        { "type", "VEC3" },
                                        { "max", { vertexes_max.x, vertexes_max.y, vertexes_max.z } },
                                        { "min", { vertexes_min.x, vertexes_min.y, vertexes_min.z } } },
                                        { { "bufferView", 2 },
                                        { "byteOffset", 0 },
                                        { "componentType", 5126 },
                                        { "count", asset3d_data.vertexes->size() },
                                        { "type", "VEC3" },
                                        { "max", { normals_max.x, normals_max.y, normals_max.z } },
                                        { "min", { normals_min.x, normals_min.y, normals_min.z } } } };

    if (type == GLB_MULTIPLE)
    {
        std::ofstream output_file = std::ofstream(file_path, std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: glb file " + file_path + " could not be created.");
        }

        std::vector<char> glb_file_data_1;
        std::vector<char> glb_file_data_2;
        std::vector<char> json_data;
        std::vector<char> buffer_data;

        uint32_t json_size = gltf_json_file.dump().length();

        while (json_size % 4 != 0)
        {
            json_data.push_back(0x20);

            json_size++;
        }

        uint32_t buffer_size = gltf_bin_file_data.size();

        while (buffer_size % 4 != 0)
        {
            buffer_data.push_back(0x0);

            buffer_size++;
        }

        glb_file_data_1.push_back(0x67);
        glb_file_data_1.push_back(0x6C);
        glb_file_data_1.push_back(0x54);
        glb_file_data_1.push_back(0x46);
        glb_file_data_1.push_back(0x2);
        glb_file_data_1.push_back(0x0);
        glb_file_data_1.push_back(0x0);
        glb_file_data_1.push_back(0x0);

        uint32_t glb_file_size = (uint32_t)0x1C + json_size + buffer_size;

        std::memcpy(&char4, &glb_file_size, sizeof(bytes4));

        for (uint32_t j = 0; j < 4; j++)
        {
            glb_file_data_1.push_back(char4[j]);
        }

        std::memcpy(&char4, &json_size, sizeof(bytes4));

        for (uint32_t j = 0; j < 4; j++)
        {
            glb_file_data_1.push_back(char4[j]);
        }

        glb_file_data_1.push_back(0x4A);
        glb_file_data_1.push_back(0x53);
        glb_file_data_1.push_back(0x4F);
        glb_file_data_1.push_back(0x4E);

        std::memcpy(&char4, &buffer_size, sizeof(bytes4));

        for (uint32_t j = 0; j < 4; j++)
        {
            glb_file_data_2.push_back(char4[j]);
        }

        glb_file_data_2.push_back(0x42);
        glb_file_data_2.push_back(0x49);
        glb_file_data_2.push_back(0x4E);
        glb_file_data_2.push_back(0x0);

        output_file.write(glb_file_data_1.data(), glb_file_data_1.size());

        output_file.write(gltf_json_file.dump().c_str(), gltf_json_file.dump().length());

        output_file.write(json_data.data(), json_data.size());

        output_file.write(glb_file_data_2.data(), glb_file_data_2.size());

        output_file.write(gltf_bin_file_data.data(), gltf_bin_file_data.size());

        output_file.write(buffer_data.data(), buffer_data.size());

        output_file.close();
    }
    else if (type == GLTF_MULTIPLE)
    {
        std::ofstream output_file = std::ofstream(file_path, std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: glTF file " + file_path + " could not be created.");
        }

        output_file << std::setw(4) << gltf_json_file;

        output_file.close();
    }
}

void gltf::output_to_single_file(asset3ds& asset3ds_data, std::string& file_path, int type)
{
    char char2[2];
    char char4[4];

    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;

    std::vector<std::vector<char>> gltf_bin_file_data;

    uint64_t gltf_bin_file_data_total_size = 0;

    std::vector<uint16_t> indexes_min;
    std::vector<uint16_t> indexes_max;

    std::vector<vector3> vertexes_min;
    std::vector<vector3> vertexes_max;

    std::vector<vector3> normals_min;
    std::vector<vector3> normals_max;

    std::vector<uint32_t> buffer_offset_aligned_1;
    std::vector<uint32_t> buffer_offset_aligned_2;

    for (uint32_t a = 0; a < asset3ds_data.indexes->size(); a++)
    {
        std::vector<char> temp_gltf_bin_file_data;

        uint16_t temp_indexes_min = 0;
        uint16_t temp_indexes_max = 0;

        vector3 temp_vertexes_min;
        vector3 temp_vertexes_max;

        vector3 temp_normals_min;
        vector3 temp_normals_max;

        uint32_t temp_buffer_offset_aligned_1 = 0;
        uint32_t temp_buffer_offset_aligned_2 = 0;

        for (uint32_t v = 0; v < asset3ds_data.indexes->at(a).size(); v++)
        {
            if (v == 0)
            {
                temp_indexes_min = asset3ds_data.indexes->at(a).at(v);
                temp_indexes_max = asset3ds_data.indexes->at(a).at(v);
            }
            else
            {
                if (asset3ds_data.indexes->at(a).at(v) < temp_indexes_min)
                    temp_indexes_min = asset3ds_data.indexes->at(a).at(v);
                if (asset3ds_data.indexes->at(a).at(v) > temp_indexes_max)
                    temp_indexes_max = asset3ds_data.indexes->at(a).at(v);
            }

            std::memcpy(&char2, &asset3ds_data.indexes->at(a).at(v), sizeof(bytes2));

            for (uint32_t d = 0; d < 2; d++)
            {
                temp_gltf_bin_file_data.push_back(char2[d]);
            }
        }

        while (temp_gltf_bin_file_data.size() % 4 != 0)
        {
            temp_gltf_bin_file_data.push_back(0x0);
        }

        temp_buffer_offset_aligned_1 = temp_gltf_bin_file_data.size();

        for (uint32_t v = 0; v < asset3ds_data.vertexes->at(a).size(); v++)
        {
            if (v == 0)
            {
                temp_vertexes_min.x = asset3ds_data.vertexes->at(a).at(v).x;
                temp_vertexes_min.y = asset3ds_data.vertexes->at(a).at(v).y;
                temp_vertexes_min.z = asset3ds_data.vertexes->at(a).at(v).z;
                temp_vertexes_max.x = asset3ds_data.vertexes->at(a).at(v).x;
                temp_vertexes_max.y = asset3ds_data.vertexes->at(a).at(v).y;
                temp_vertexes_max.z = asset3ds_data.vertexes->at(a).at(v).z;
            }
            else
            {
                if (asset3ds_data.vertexes->at(a).at(v).x < temp_vertexes_min.x)
                    temp_vertexes_min.x = asset3ds_data.vertexes->at(a).at(v).x;
                if (asset3ds_data.vertexes->at(a).at(v).x > temp_vertexes_max.x)
                    temp_vertexes_max.x = asset3ds_data.vertexes->at(a).at(v).x;
                if (asset3ds_data.vertexes->at(a).at(v).y < temp_vertexes_min.y)
                    temp_vertexes_min.y = asset3ds_data.vertexes->at(a).at(v).y;
                if (asset3ds_data.vertexes->at(a).at(v).y > temp_vertexes_max.y)
                    temp_vertexes_max.y = asset3ds_data.vertexes->at(a).at(v).y;
                if (asset3ds_data.vertexes->at(a).at(v).z < temp_vertexes_min.z)
                    temp_vertexes_min.z = asset3ds_data.vertexes->at(a).at(v).z;
                if (asset3ds_data.vertexes->at(a).at(v).z > temp_vertexes_max.z)
                    temp_vertexes_max.z = asset3ds_data.vertexes->at(a).at(v).z;
            }

            std::memcpy(&char4, &asset3ds_data.vertexes->at(a).at(v).x, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }

            std::memcpy(&char4, &asset3ds_data.vertexes->at(a).at(v).y, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }

            std::memcpy(&char4, &asset3ds_data.vertexes->at(a).at(v).z, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }
        }

        while (temp_gltf_bin_file_data.size() % 4 != 0)
        {
            temp_gltf_bin_file_data.push_back(0x0);
        }

        temp_buffer_offset_aligned_2 = temp_gltf_bin_file_data.size() - temp_buffer_offset_aligned_1;

        /*for (uint32_t v = 0; v < asset3ds_data.normals->at(a).size(); v++)
        {
            float magnitude = std::sqrt(asset3ds_data.normals->at(a).at(v).x * asset3ds_data.normals->at(a).at(v).x + asset3ds_data.normals->at(a).at(v).y * asset3ds_data.normals->at(a).at(v).y + asset3ds_data.normals->at(a).at(v).z * asset3ds_data.normals->at(a).at(v).z);

            asset3ds_data.normals->at(a).at(v).x /= magnitude;
            asset3ds_data.normals->at(a).at(v).y /= magnitude;
            asset3ds_data.normals->at(a).at(v).z /= magnitude;
        }*/

        for (uint32_t v = 0; v < asset3ds_data.normals->at(a).size(); v++)
        {
            if (v == 0)
            {
                temp_normals_min.x = asset3ds_data.normals->at(a).at(v).x;
                temp_normals_min.y = asset3ds_data.normals->at(a).at(v).y;
                temp_normals_min.z = asset3ds_data.normals->at(a).at(v).z;
                temp_normals_max.x = asset3ds_data.normals->at(a).at(v).x;
                temp_normals_max.y = asset3ds_data.normals->at(a).at(v).y;
                temp_normals_max.z = asset3ds_data.normals->at(a).at(v).z;
            }
            else
            {
                if (asset3ds_data.normals->at(a).at(v).x < temp_normals_min.x)
                    temp_normals_min.x = asset3ds_data.normals->at(a).at(v).x;
                if (asset3ds_data.normals->at(a).at(v).x > temp_normals_max.x)
                    temp_normals_max.x = asset3ds_data.normals->at(a).at(v).x;
                if (asset3ds_data.normals->at(a).at(v).y < temp_normals_min.y)
                    temp_normals_min.y = asset3ds_data.normals->at(a).at(v).y;
                if (asset3ds_data.normals->at(a).at(v).y > temp_normals_max.y)
                    temp_normals_max.y = asset3ds_data.normals->at(a).at(v).y;
                if (asset3ds_data.normals->at(a).at(v).z < temp_normals_min.z)
                    temp_normals_min.z = asset3ds_data.normals->at(a).at(v).z;
                if (asset3ds_data.normals->at(a).at(v).z > temp_normals_max.z)
                    temp_normals_max.z = asset3ds_data.normals->at(a).at(v).z;
            }

            std::memcpy(&char4, &asset3ds_data.normals->at(a).at(v).x, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }

            std::memcpy(&char4, &asset3ds_data.normals->at(a).at(v).y, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }

            std::memcpy(&char4, &asset3ds_data.normals->at(a).at(v).z, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }
        }

        /*for (uint32_t v = 0; v < vertexes_uvs.size(); v++)
        {
            char char4[2];

            std::memcpy(&char4, &vertexes_uvs.at(v).x, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }

            std::memcpy(&char4, &vertexes_uvs.at(v).y, sizeof(bytes4));

            for (uint32_t d = 0; d < 4; d++)
            {
                temp_gltf_bin_file_data.push_back(char4[d]);
            }
        }*/

        gltf_bin_file_data_total_size += temp_gltf_bin_file_data.size();

        gltf_bin_file_data.push_back(temp_gltf_bin_file_data);

        indexes_min.push_back(temp_indexes_min);
        indexes_max.push_back(temp_indexes_max);

        vertexes_min.push_back(temp_vertexes_min);
        vertexes_max.push_back(temp_vertexes_max);

        normals_min.push_back(temp_normals_min);
        normals_max.push_back(temp_normals_max);

        buffer_offset_aligned_1.push_back(temp_buffer_offset_aligned_1);
        buffer_offset_aligned_2.push_back(temp_buffer_offset_aligned_2);
    }

    std::string gltf_json_file = "{\"asset\":{\"generator\":\"RPKG\",\"version\":\"2.0\"},\"scene\":0,\"scenes\":[{\"nodes\":[";

    for (int i = 0; i < gltf_bin_file_data.size(); i++)
    {
        gltf_json_file.append(std::to_string(i));

        if (i < (gltf_bin_file_data.size() - 1))
        {
            gltf_json_file.push_back(',');
        }
    }

    gltf_json_file += "]}],\"nodes\":[";

    for (int i = 0; i < gltf_bin_file_data.size(); i++)
    {
        gltf_json_file.append("{\"mesh\":" + std::to_string(i) + "}");

        if (i < (gltf_bin_file_data.size() - 1))
        {
            gltf_json_file.push_back(',');
        }
    }

    gltf_json_file += "],\"meshes\":[";

    for (int i = 0; i < gltf_bin_file_data.size(); i++)
    {
        int index = i * 3;

        gltf_json_file.append("{\"primitives\":[{\"attributes\":{\"POSITION\":" + std::to_string(index + 1) + ",\"NORMAL\":" + std::to_string(index + 2) + "},\"indices\":" + std::to_string(index) + ",\"material\":0}]}");

        if (i < (gltf_bin_file_data.size() - 1))
        {
            gltf_json_file.push_back(',');
        }
    }

    gltf_json_file += "],\"materials\":[{\"pbrMetallicRoughness\":{\"baseColorFactor\":[1.0,1.0,1.0,1.0],\"metallicFactor\":0.0,\"roughnessFactor\":0.0}}],";

    if (type == GLB_SINGLE)
    {
        gltf_json_file += "\"buffers\":[{\"byteLength\":" + util::uint64_t_to_string(gltf_bin_file_data_total_size) + "}],\"bufferViews\":[";
    }
    else if (type == GLTF_SINGLE)
    {
        gltf_json_file += "\"buffers\":[{\"uri\":\"" + file_path + ".bin\",\"byteLength\":" + util::uint64_t_to_string(gltf_bin_file_data_total_size) + "}],\"bufferViews\":[";
    }

    uint64_t buffer_offset = 0;

    for (int i = 0; i < gltf_bin_file_data.size(); i++)
    {
        gltf_json_file.append("{\"buffer\":0,\"byteOffset\":" + util::uint64_t_to_string(buffer_offset) + ",\"byteLength\":" + util::uint64_t_to_string(asset3ds_data.indexes->at(i).size() * 0x2) + ",\"target\":34963},");

        std::string temp_string = util::uint64_t_to_string(asset3ds_data.vertexes->at(i).size() * 0xC);

        buffer_offset += buffer_offset_aligned_1.at(i);

        gltf_json_file.append("{\"buffer\":0,\"byteOffset\":" + util::uint64_t_to_string(buffer_offset) + ",\"byteLength\":" + temp_string + ",\"target\":34962},");

        buffer_offset += buffer_offset_aligned_2.at(i);

        gltf_json_file.append("{\"buffer\":0,\"byteOffset\":" + util::uint64_t_to_string(buffer_offset) + ",\"byteLength\":" + temp_string + ",\"target\":34962}");

        buffer_offset += buffer_offset_aligned_2.at(i);

        if (i < (gltf_bin_file_data.size() - 1))
        {
            gltf_json_file.push_back(',');
        }
    }

    gltf_json_file += "],\"accessors\":[";

    for (int i = 0; i < gltf_bin_file_data.size(); i++)
    {
        int index = i * 3;

        gltf_json_file.append("{\"bufferView\":" + std::to_string(index) + ",\"byteOffset\":0,\"componentType\":5123,\"count\":" + util::uint64_t_to_string(asset3ds_data.indexes->at(i).size()) + ",\"type\":\"SCALAR\",\"max\":[" + util::uint16_t_to_string(indexes_max.at(i)) + "],\"min\":[" + util::uint16_t_to_string(indexes_min.at(i)) + "]},");

        std::string temp_string = util::uint64_t_to_string(asset3ds_data.vertexes->at(i).size());

        gltf_json_file.append("{\"bufferView\":" + std::to_string(index + 1) + ",\"byteOffset\":0,\"componentType\":5126,\"count\":" + temp_string + ",\"type\":\"VEC3\",\"max\":[" + util::float_to_string(vertexes_max.at(i).x) + "," + util::float_to_string(vertexes_max.at(i).y) + "," + util::float_to_string(vertexes_max.at(i).z) + "],\"min\":[" + util::float_to_string(vertexes_min.at(i).x) + "," + util::float_to_string(vertexes_min.at(i).y) + "," + util::float_to_string(vertexes_min.at(i).z) + "]},");

        gltf_json_file.append("{\"bufferView\":" + std::to_string(index + 2) + ",\"byteOffset\":0,\"componentType\":5126,\"count\":" + temp_string + ",\"type\":\"VEC3\",\"max\":[" + util::float_to_string(normals_max.at(i).x) + "," + util::float_to_string(normals_max.at(i).y) + "," + util::float_to_string(normals_max.at(i).z) + "],\"min\":[" + util::float_to_string(normals_min.at(i).x) + "," + util::float_to_string(normals_min.at(i).y) + "," + util::float_to_string(normals_min.at(i).z) + "]}");

        if (i < (gltf_bin_file_data.size() - 1))
        {
            gltf_json_file.push_back(',');
        }
    }

    gltf_json_file += "]}";

    if (type == GLB_SINGLE)
    {
        std::ofstream output_file = std::ofstream(file_path, std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: glb file " + file_path + " could not be created.");
        }

        std::vector<char> glb_file_data_1;
        std::vector<char> glb_file_data_2;
        std::vector<char> json_data;
        std::vector<char> buffer_data;

        uint32_t json_size = gltf_json_file.length();

        while (json_size % 4 != 0)
        {
            json_data.push_back(0x20);

            json_size++;
        }

        uint32_t buffer_size_total = 0;

        for (uint32_t i = 0; i < gltf_bin_file_data.size(); i++)
        {
            uint32_t buffer_size = gltf_bin_file_data.at(i).size();

            buffer_size_total += buffer_size;
        }

        while (buffer_size_total % 4 != 0)
        {
            buffer_data.push_back(0x0);

            buffer_size_total++;
        }

        glb_file_data_1.push_back(0x67);
        glb_file_data_1.push_back(0x6C);
        glb_file_data_1.push_back(0x54);
        glb_file_data_1.push_back(0x46);
        glb_file_data_1.push_back(0x2);
        glb_file_data_1.push_back(0x0);
        glb_file_data_1.push_back(0x0);
        glb_file_data_1.push_back(0x0);

        uint32_t glb_file_size = (uint32_t)0x1C + json_size + buffer_size_total;

        std::memcpy(&char4, &glb_file_size, sizeof(bytes4));

        for (uint32_t j = 0; j < 4; j++)
        {
            glb_file_data_1.push_back(char4[j]);
        }

        std::memcpy(&char4, &json_size, sizeof(bytes4));

        for (uint32_t j = 0; j < 4; j++)
        {
            glb_file_data_1.push_back(char4[j]);
        }

        glb_file_data_1.push_back(0x4A);
        glb_file_data_1.push_back(0x53);
        glb_file_data_1.push_back(0x4F);
        glb_file_data_1.push_back(0x4E);

        std::memcpy(&char4, &buffer_size_total, sizeof(bytes4));

        for (uint32_t j = 0; j < 4; j++)
        {
            glb_file_data_2.push_back(char4[j]);
        }

        glb_file_data_2.push_back(0x42);
        glb_file_data_2.push_back(0x49);
        glb_file_data_2.push_back(0x4E);
        glb_file_data_2.push_back(0x0);

        output_file.write(glb_file_data_1.data(), glb_file_data_1.size());

        output_file.write(gltf_json_file.c_str(), gltf_json_file.length());

        output_file.write(json_data.data(), json_data.size());

        output_file.write(glb_file_data_2.data(), glb_file_data_2.size());

        for (uint32_t i = 0; i < gltf_bin_file_data.size(); i++)
        {
            output_file.write(gltf_bin_file_data.at(i).data(), gltf_bin_file_data.at(i).size());
        }

        output_file.write(buffer_data.data(), buffer_data.size());

        output_file.close();
    }
    else if (type == GLTF_SINGLE)
    {
        std::string gltf_bin_file_name = file_path + ".bin";

        std::ofstream output_file = std::ofstream(gltf_bin_file_name, std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: glTF bin file " + gltf_bin_file_name + " could not be created.");
        }

        uint32_t buffer_size_total = 0;

        for (uint32_t i = 0; i < gltf_bin_file_data.size(); i++)
        {
            uint32_t buffer_size = gltf_bin_file_data.at(i).size();

            buffer_size_total += buffer_size;

            output_file.write(gltf_bin_file_data.at(i).data(), gltf_bin_file_data.at(i).size());
        }

        while (buffer_size_total % 4 != 0)
        {
            output_file.write(0x0, 1);

            buffer_size_total++;
        }

        output_file.close();

        output_file = std::ofstream(file_path, std::ofstream::binary);

        if (!output_file.good())
        {
            LOG_AND_EXIT("Error: glTF file " + file_path + " could not be created.");
        }

        output_file << std::setw(4) << gltf_json_file;

        output_file.close();
    }
}