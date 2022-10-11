#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "gltf.h"
#include "prim.h"
#include "mati.h"
#include "util.h"
#include "text.h"
#include "generic_function.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <regex>
#include <filesystem>

void rpkg_function::extract_prim_textured_from(std::string& input_path, std::string& filter, std::string& output_path, bool rotate)
{
    task_single_status = TASK_EXECUTING;
    //task_multiple_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    //else
    //{
        //input_path = file::parse_input_folder_path(input_path);
    //}

    if (file::path_exists(input_path))
    {
        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path, false);

            std::filesystem::path base_folder_path = input_path;

            std::string parent_path = base_folder_path.parent_path().string();

            rpkg_function::import_rpkg_files_in_folder(parent_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        //timing_string = "Calculating Entity World Coordinate Data And Extracting Associated PRIMs to glTFs...";

        //LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        if (!hash_list_loaded)
        {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(true);
            LOG("Loading Hash List: Done");
        }

        //std::vector<std::string>().swap(prim_asset_file_names);

        std::vector<std::string> filters = util::parse_input_filter(filter);

        for (uint64_t f = 0; f < filters.size(); f++)
        {
            uint64_t temp_hash_value = std::strtoull(filters.at(f).c_str(), nullptr, 16);

            uint32_t rpkg_index = rpkg_function::get_latest_hash(temp_hash_value);

            if (rpkg_index == UINT32_MAX)
            {
                return;
            }

            std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

            if (it != rpkgs.at(rpkg_index).hash_map.end())
            {
                if (gui_control == ABORT_CURRENT_TASK)
                {
                    return;
                }

                prim temp_prim(rpkg_index, it->second);

                if (temp_prim.asset3ds_data.vertexes.size() > 0 && temp_prim.success)
                {
                    for (uint32_t c = 0; c < temp_prim.asset3ds_data.colors.size(); c++)
                    {
                        std::fill(temp_prim.asset3ds_data.colors.at(c).begin(), temp_prim.asset3ds_data.colors.at(c).end(), 0);
                    }

                    std::string asset_file_name = "";

                    if (output_path == "")
                    {
                        asset_file_name = std::filesystem::current_path().generic_string() + "/" + util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + ".glb";
                    }
                    else
                    {
                        asset_file_name = file::output_path_append(util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + ".glb", output_path);

                        file::create_directories(output_path);
                    }

                    prim_asset_file_names.push_back(asset_file_name);

                    //std::string meta_path = output_path + "\\" + "metas";

                    //file::create_directories(meta_path);

                    //temp_prim.extract_meta(meta_path);

                    //std::string hash_meta_file_name = file::output_path_append(util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) + "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type, meta_path);

                    //rpkg_function::extract_hash_meta(rpkg_index, it->second, hash_meta_file_name);

                    std::vector<jpg_textures> all_jpg_textures;

                    uint32_t temp_hash_reference_count = rpkgs.at(temp_prim.prim_rpkg_index).hash.at(temp_prim.prim_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                    for (uint32_t d = 0; d < temp_hash_reference_count; d++)
                    {
                        uint32_t temp_rpkg_index = rpkg_function::get_latest_hash(rpkgs.at(temp_prim.prim_rpkg_index).hash.at(temp_prim.prim_hash_index).hash_reference_data.hash_reference.at(d));

                        if (temp_rpkg_index == UINT32_MAX)
                        {
                            return;
                        }

                        std::unordered_map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(temp_rpkg_index).hash_map.find(rpkgs.at(temp_prim.prim_rpkg_index).hash.at(temp_prim.prim_hash_index).hash_reference_data.hash_reference.at(d));

                        if (it2 != rpkgs.at(temp_rpkg_index).hash_map.end())
                        {
                            if (rpkgs.at(temp_rpkg_index).hash.at(it2->second).hash_resource_type == "MATI")
                            {
                                jpg_textures temp_jpg_textures;

                                temp_jpg_textures.material_id = d;

                                mati temp_mati(temp_rpkg_index, it2->second);

                                temp_mati.map_textures();

                                std::unordered_map<uint64_t, uint64_t>::iterator it3;

                                if (temp_mati.has_diffuse_texture)
                                {
                                    uint32_t temp_rpkg_index2 = rpkg_function::get_latest_hash(temp_mati.diffuse_texture_hash);

                                    if (temp_rpkg_index2 == UINT32_MAX)
                                    {
                                        return;
                                    }

                                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(temp_rpkg_index2).hash_map.find(temp_mati.diffuse_texture_hash);

                                    if (it3 != rpkgs.at(temp_rpkg_index2).hash_map.end())
                                    {
                                        text temp_text(temp_rpkg_index2, it3->second);

                                        if (temp_text.texd_found)
                                        {
                                            if (temp_text.save_texd_to_jpg(""))
                                            {
                                                temp_jpg_textures.has_diffuse_texture = true;
                                                temp_jpg_textures.texture_diffuse_jpg = temp_text.texd_data_jpg;
                                            }
                                        }
                                        else
                                        {
                                            if (temp_text.save_text_to_jpg(""))
                                            {
                                                temp_jpg_textures.has_diffuse_texture = true;
                                                temp_jpg_textures.texture_diffuse_jpg = temp_text.text_data_jpg;
                                            }
                                        }
                                    }
                                }
                                
                                if (temp_mati.has_normal_texture)
                                {
                                    uint32_t temp_rpkg_index2 = rpkg_function::get_latest_hash(temp_mati.normal_texture_hash);

                                    if (temp_rpkg_index2 == UINT32_MAX)
                                    {
                                        return;
                                    }

                                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(temp_rpkg_index2).hash_map.find(temp_mati.normal_texture_hash);

                                    if (it3 != rpkgs.at(temp_rpkg_index2).hash_map.end())
                                    {
                                        text temp_text(temp_rpkg_index2, it3->second);

                                        if (temp_text.texd_found)
                                        {
                                            if (temp_text.save_texd_to_jpg(""))
                                            {
                                                temp_jpg_textures.has_normal_texture = true;
                                                temp_jpg_textures.texture_normal_jpg = temp_text.texd_data_jpg;
                                            }
                                        }
                                        else
                                        {
                                            if (temp_text.save_text_to_jpg(""))
                                            {
                                                temp_jpg_textures.has_normal_texture = true;
                                                temp_jpg_textures.texture_normal_jpg = temp_text.text_data_jpg;
                                            }
                                        }
                                    }
                                }

                                /*if (temp_mati.has_emissive_texture)
                                {
                                    uint32_t temp_rpkg_index2 = rpkg_function::get_latest_hash(temp_mati.emissive_texture_hash, false);

                                    if (temp_rpkg_index2 == UINT32_MAX)
                                    {
                                        return;
                                    }

                                    std::unordered_map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(temp_rpkg_index2).hash_map.find(temp_mati.emissive_texture_hash);

                                    if (it3 != rpkgs.at(temp_rpkg_index2).hash_map.end())
                                    {
                                        text temp_text(temp_rpkg_index2, it3->second);

                                        if (temp_text.texd_found)
                                        {
                                            if (temp_text.save_texd_to_jpg(""))
                                            {
                                                temp_jpg_textures.has_emissive_texture = true;
                                                temp_jpg_textures.texture_emissive_jpg = temp_text.texd_data_jpg;
                                            }
                                        }
                                        else
                                        {
                                            if (temp_text.save_text_to_jpg(""))
                                            {
                                                temp_png_textures.has_emissive_texture = true;
                                                temp_png_textures.texture_emissive_png = temp_text.text_data_png;
                                            }
                                        }
                                    }
                                }*/

                                all_jpg_textures.push_back(temp_jpg_textures);
                            }
                        }
                    }

                    gltf::output_to_single_file_with_textures(temp_prim.asset3ds_data, asset_file_name, GLB_SINGLE, false, all_jpg_textures);
                }
            }
        }

        task_single_status = TASK_SUCCESSFUL;
        //task_multiple_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}