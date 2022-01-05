#pragma once
#include "global.h"

class gltf
{
public:
    static void output_to_single_file(asset3ds& asset3ds_data, std::string& file_path, int type, bool rotate);
    static void output_to_single_file_with_textures(asset3ds& asset3ds_data, std::string& file_path, int type, bool rotate, std::vector<png_textures> textures);
};
