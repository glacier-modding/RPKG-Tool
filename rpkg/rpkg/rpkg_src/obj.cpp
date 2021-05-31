#include "obj.h"
#include "file.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include <filesystem>

void obj::output_to_single_file(asset3ds asset3ds_data, std::string& file_path)
{
    std::ofstream output_file = std::ofstream(file_path, std::ofstream::binary);

    if (!output_file.good())
    {
        LOG_AND_EXIT("Error: OBJ file " + file_path + " could not be created.");
    }

    std::string obj_file_data = "";

    for (uint32_t a = 0; a < asset3ds_data.vertexes.size(); a++)
    {
        for (uint32_t v = 0; v < asset3ds_data.vertexes.at(a).size(); v++)
        {
            obj_file_data.append("v ");
            obj_file_data.append(util::float_to_string(asset3ds_data.vertexes.at(a).at(v).x));
            obj_file_data.push_back(' ');
            obj_file_data.append(util::float_to_string(asset3ds_data.vertexes.at(a).at(v).y));
            obj_file_data.push_back(' ');
            obj_file_data.append(util::float_to_string(asset3ds_data.vertexes.at(a).at(v).z));
            //obj_file_data.push_back(' ');
            //obj_file_data.append(util::float_to_string(asset3d_data.vertexes->at(v).w));
            obj_file_data.push_back('\n');
        }
    }

    for (uint32_t a = 0; a < asset3ds_data.normals.size(); a++)
    {
        for (uint32_t v = 0; v < asset3ds_data.normals.at(a).size(); v++)
        {
            obj_file_data.append("vn ");
            obj_file_data.append(util::float_to_string(asset3ds_data.normals.at(a).at(v).x));
            obj_file_data.push_back(' ');
            obj_file_data.append(util::float_to_string(asset3ds_data.normals.at(a).at(v).y));
            obj_file_data.push_back(' ');
            obj_file_data.append(util::float_to_string(asset3ds_data.normals.at(a).at(v).z));
            //obj_file_data.push_back(' ');
            //obj_file_data.append(util::float_to_string(asset3d_data.normals->at(v).w));
            obj_file_data.push_back('\n');
        }
    }

    for (uint32_t a = 0; a < asset3ds_data.uvs.size(); a++)
    {
        for (uint32_t v = 0; v < asset3ds_data.uvs.at(a).size(); v++)
        {
            obj_file_data.append("vt ");
            obj_file_data.append(util::float_to_string(asset3ds_data.uvs.at(a).at(v).x));
            obj_file_data.push_back(' ');
            obj_file_data.append(util::float_to_string(asset3ds_data.uvs.at(a).at(v).y));
            obj_file_data.push_back('\n');
        }
    }

    uint32_t v_count = 0;

    for (uint32_t a = 0; a < asset3ds_data.indexes.size(); a++)
    {
        for (uint32_t v = 0; v < (asset3ds_data.indexes.at(a).size() / 3); v++)
        {
            uint32_t one = asset3ds_data.indexes.at(a).at((uint64_t)v * (uint64_t)3);
            one++;
            one += v_count;
            uint32_t two = asset3ds_data.indexes.at(a).at((uint64_t)v * (uint64_t)3 + (uint64_t)1);
            two++;
            two += v_count;
            uint32_t three = asset3ds_data.indexes.at(a).at((uint64_t)v * (uint64_t)3 + (uint64_t)2);
            three++;
            three += v_count;
            //uint32_t four = asset3d_data.indexes->at(v * (uint32_t)4 + (uint32_t)3);
            //four++;

            std::string one_string = util::uint32_t_to_string(one);
            std::string two_string = util::uint32_t_to_string(two);
            std::string three_string = util::uint32_t_to_string(three);
            //std::string four_string = util::uint32_t_to_string(four);

            obj_file_data.append("f ");
            obj_file_data.append(one_string);
            obj_file_data.push_back('/');
            //obj_file_data.push_back('/');
            obj_file_data.append(one_string);
            obj_file_data.push_back('/');
            obj_file_data.append(one_string);
            obj_file_data.push_back(' ');
            obj_file_data.append(two_string);
            obj_file_data.push_back('/');
            //obj_file_data.push_back('/');
            obj_file_data.append(two_string);
            obj_file_data.push_back('/');
            obj_file_data.append(two_string);
            obj_file_data.push_back(' ');
            obj_file_data.append(three_string);
            obj_file_data.push_back('/');
            //obj_file_data.push_back('/');
            obj_file_data.append(three_string);
            obj_file_data.push_back('/');
            obj_file_data.append(three_string);
            //obj_file_data.push_back(' ');
            //obj_file_data.append(four_string);
            //obj_file_data.push_back('/');
            //obj_file_data.append(four_string);
            //obj_file_data.push_back('/');
            //obj_file_data.append(four_string);
            obj_file_data.push_back('\n');
        }

        v_count += asset3ds_data.vertexes.at(a).size();
    }

    output_file.write(obj_file_data.c_str(), obj_file_data.length());

    output_file.close();
}