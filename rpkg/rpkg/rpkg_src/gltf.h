#pragma once
#include "global.h"

class gltf
{
public:
    static void output_to_multiple_files(asset3d& asset3d_data, std::string& file_path, int type);
    static void output_to_single_file(asset3ds& asset3ds_data, std::string& file_path, int type);
};
