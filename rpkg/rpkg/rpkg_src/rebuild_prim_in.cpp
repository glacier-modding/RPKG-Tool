#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/json/json.hpp"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

using json = nlohmann::ordered_json;

void rpkg_function::rebuild_prim_in(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;

    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path))
    {
        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        std::vector<std::string> prim_folders;

        std::string input_folder = input_rpkg_folder_path;

        std::filesystem::path input_folder_path = input_rpkg_folder_path;

        prim_folders.push_back(input_folder_path.string());

        if (input_folder.substr(input_folder.length() - 1, 1) == "\\")
        {
            input_folder = input_folder.substr(0, input_folder.length() - 1);
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                timing_string = ss.str();

                LOG_NO_ENDL("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

                period_count++;
            }

            if (std::filesystem::is_directory(entry.path().string()))
            {
                input_folder = entry.path().string();

                if (input_folder.substr(input_folder.length() - 1, 1) == "\\")
                {
                    input_folder = input_folder.substr(0, input_folder.length() - 1);
                }

                bool prim_folder_found = false;

                for (uint64_t i = 0; i < prim_folders.size(); i++)
                {
                    if (input_folder == prim_folders.at(i))
                    {
                        prim_folder_found = true;
                    }
                }

                if (!prim_folder_found)
                {
                    prim_folders.push_back(input_folder);
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        std::string message = "Rebuilding PRIM files from: ";

        for (uint64_t i = 0; i < prim_folders.size(); i++)
        {
            if (gui_control == ABORT_CURRENT_TASK)
            {
                return;
            }

            if (((i * (uint64_t)100000) / (uint64_t)prim_folders.size()) % (uint64_t)10 == 0 && i > 0)
            {
                stringstream_length = console::update_console(message, prim_folders.size(), i, start_time, stringstream_length);
            }

            std::vector<std::string> glb_file_names;
            std::vector<std::string> glb_file_paths;
            std::vector<uint64_t> prim_hashes;
            std::vector<std::string> prim_hash_strings;
            std::vector<std::string> prim_file_names;

            for (const auto& entry : std::filesystem::directory_iterator(prim_folders.at(i)))
            {
                if (std::filesystem::is_regular_file(entry.path().string()))
                {
                    std::size_t pos = entry.path().string().find_last_of("\\/");

                    std::string file_name = "";
                    std::string hash_file_name = "";
                    std::string hash_string = "";
                    std::string resource_type = "";

                    if (pos != std::string::npos)
                    {
                        file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    }
                    else
                    {
                        file_name = entry.path().string();
                    }

                    if (file_name.length() > 4)
                    {
                        if (util::to_upper_case(file_name.substr((file_name.length() - 4), 4)) == ".GLB")
                        {
                            hash_file_name = util::to_upper_case(file_name.substr(0, (file_name.length() - 4)));

                            pos = hash_file_name.find_last_of(".");

                            if (pos != std::string::npos)
                            {
                                hash_string = hash_file_name.substr(0, pos);
                                resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                            }

                            bool is_glb_hash_file = true;
                            bool glb_hash_meta = true;

                            if (hash_string.length() != 16)
                            {
                                is_glb_hash_file = false;
                            }

                            if (resource_type != "PRIM")
                            {
                                is_glb_hash_file = false;
                            }

                            if (!file::path_exists(prim_folders.at(i) + "\\meta"))
                            {
                                glb_hash_meta = false;

                                LOG("GLB file " + entry.path().string() + " found but meta file is missing, can't rebuild.");
                            }

                            if (is_glb_hash_file && glb_hash_meta)
                            {
                                glb_file_paths.push_back(entry.path().string());
                                glb_file_names.push_back(file_name);
                                prim_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                                prim_file_names.push_back(util::to_upper_case(hash_file_name));
                                prim_hash_strings.push_back(util::to_upper_case(hash_string));
                            }
                        }
                    }
                }
            }

            if (glb_file_paths.size() == 1)
            {
                std::ifstream meta_file = std::ifstream(prim_folders.at(i) + "\\meta", std::ifstream::binary);

                if (!meta_file.good())
                {
                    LOG_AND_EXIT("Error: meta file " + prim_folders.at(i) + "\\meta" + " could not be opened.");
                }

                meta_file.seekg(0, meta_file.end);

                uint64_t meta_file_size = meta_file.tellg();

                meta_file.seekg(0, meta_file.beg);

                std::vector<char> meta_data(meta_file_size, 0);
                std::vector<char> meta_data_footer;
                std::vector<char> prim_file_header_data;
                std::vector<char> prim_file_data;

                uint64_t glb_file_size = 0;

                char input[1024];
                char char4[4] = "";
                uint8_t bytes1 = 0;
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;
                uint64_t position = 0;

                meta_file.read(meta_data.data(), meta_file_size);

                meta_file.close();

                std::string prim_hash_file_name = prim_hash_strings.back() + ".PRIM";

                std::ifstream glb_file = std::ifstream(glb_file_paths.back(), std::ifstream::binary);

                if (!glb_file.good())
                {
                    LOG_AND_EXIT("Error: GLB file " + glb_file_paths.back() + " could not be created.");
                }

                glb_file.seekg(0, glb_file.end);
                glb_file_size = (uint64_t)glb_file.tellg();
                glb_file.seekg(0, glb_file.beg);

                std::vector<char> glb_file_data(glb_file_size, 0);

                glb_file.read(glb_file_data.data(), glb_file_size);

                uint32_t json_file_size = 0;

                position = 0xC;

                std::memcpy(&json_file_size, &glb_file_data[position], sizeof(bytes4));
                position += 0x8;

                std::vector<char> json_glb_data(json_file_size, 0);

                std::memcpy(json_glb_data.data(), &glb_file_data[position], json_file_size);

                json json_glb;

                std::string json_data_string = std::string(json_glb_data.data());

                std::stringstream ss;

                ss << json_data_string;

                ss >> json_glb;

                std::vector<std::string> mesh_names;
                std::vector<std::string> mesh_names_hash_file_name;
                std::vector<std::string> mesh_names_prim_object_index;
                std::vector<uint32_t> mesh_indexes;
                std::vector<uint32_t> mesh_indexes_count;
                std::vector<uint32_t> mesh_vertexes;
                std::vector<uint32_t> mesh_vertexes_count;
                std::vector<uint32_t> mesh_normals;
                std::vector<uint32_t> mesh_normals_count;
                std::vector<uint32_t> mesh_uvs;
                std::vector<uint32_t> mesh_uvs_count;
                std::vector<uint32_t> bufferviews_buffer;
                std::vector<uint32_t> bufferviews_byteOffset;
                std::vector<uint32_t> bufferviews_byteLength;
                std::vector<uint32_t> bufferviews_target;
                std::vector<uint32_t> accessors_bufferView;
                std::vector<uint32_t> accessors_byteOffset;
                std::vector<uint32_t> accessors_componentType;
                std::vector<uint32_t> accessors_count;
                std::vector<std::string> accessors_type;
                std::vector<float> accessors_float_min;
                std::vector<float> accessors_float_max;
                std::vector<vector2> accessors_vector2_min;
                std::vector<vector2> accessors_vector2_max;
                std::vector<vector3> accessors_vector3_min;
                std::vector<vector3> accessors_vector3_max;
                uint32_t buffers_byte_length = 0;

                for (const auto& it : json_glb.items())
                {
                    if (it.key() == "asset")
                    {
                        if (it.value().contains("version"))
                        {
                            if (it.value()["version"] == "2.0")
                            {
                                std::cout << "glTF version: " << it.value()["version"] << std::endl;
                            }
                            else
                            {
                                LOG_AND_EXIT("Error: glTF version must be 2.0 in order to rebuild PRIM from GLB file " + glb_file_paths.back() + ".");
                            }
                        }
                    }
                }

                for (const auto& it : json_glb.items())
                {
                    if (it.key() == "nodes")
                    {
                        for (const auto& it2 : it.value().items())
                        {
                            if (it2.value().contains("mesh"))
                            {
                                if (it2.value().contains("name"))
                                {
                                    std::cout << "mesh (" << it2.value()["mesh"] << "): " << it2.value()["name"] << std::endl;

                                    mesh_names.push_back(it2.value()["name"]);

                                    size_t pos = mesh_names.back().find("_");

                                    if (pos != std::string::npos && pos < (mesh_names.back().length() - 1))
                                    {
                                        std::cout << "mesh (" << it2.value()["mesh"] << ") hash file name: " << mesh_names.back().substr(0, pos) << std::endl;

                                        mesh_names_hash_file_name.push_back(mesh_names.back().substr(0, pos));

                                        std::cout << "mesh (" << it2.value()["mesh"] << ") PRIM object index: " << mesh_names.back().substr(pos + 1) << std::endl;

                                        mesh_names_prim_object_index.push_back(mesh_names.back().substr(pos + 1));
                                    }
                                    else
                                    {
                                        LOG_AND_EXIT("Error: glTF GLB JSON parsing error, malformed mesh name " + mesh_names.back() + ".");
                                    }
                                }
                                else
                                {
                                    LOG_AND_EXIT("Error: glTF GLB JSON parsing error, each mesh must be named for proper rebuilding.");
                                }
                            }
                        }
                    }
                }

                for (const auto& it : json_glb.items())
                {
                    if (it.key() == "meshes")
                    {
                        for (const auto& it2 : it.value().items())
                        {
                            if (it2.value().contains("primitives"))
                            {
                                for (const auto& it3 : it2.value().items())
                                {
                                    for (const auto& it4 : it3.value().items())
                                    {
                                        for (const auto& it5 : it4.value().items())
                                        {
                                            if (it5.key() == "attributes")
                                            {
                                                for (const auto& it6 : it5.value().items())
                                                {
                                                    if (it6.key() == "POSITION")
                                                    {
                                                        std::cout << "primitives attributes POSITION: " << it6.value() << std::endl;

                                                        mesh_vertexes.push_back(it6.value());
                                                    }
                                                    else if (it6.key() == "NORMAL")
                                                    {
                                                        std::cout << "primitives attributes NORMAL: " << it6.value() << std::endl;

                                                        mesh_normals.push_back(it6.value());
                                                    }
                                                    else if (it6.key() == "TEXCOORD_0")
                                                    {
                                                        std::cout << "primitives attributes TEXCOORD_0: " << it6.value() << std::endl;

                                                        mesh_uvs.push_back(it6.value());
                                                    }
                                                }
                                            }
                                            else if (it5.key() == "indices")
                                            {
                                                std::cout << "primitives indices: " << it5.value() << std::endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                for (const auto& it : json_glb.items())
                {
                    if (it.key() == "buffers")
                    {
                        for (const auto& it2 : it.value().items())
                        {
                            if (it2.value().contains("byteLength"))
                            {
                                std::cout << "buffers byteLength: " << it2.value()["byteLength"] << std::endl;

                                buffers_byte_length = it2.value()["byteLength"];
                            }
                            else
                            {
                                LOG_AND_EXIT("Error: glTF missing byteLength in buffers.");
                            }
                        }
                    }
                }

                for (const auto& it : json_glb.items())
                {
                    if (it.key() == "bufferViews")
                    {
                        for (const auto& it2 : it.value().items())
                        {
                            if (it2.value().contains("buffer") && it2.value().contains("byteOffset") && it2.value().contains("byteLength") && it2.value().contains("target"))
                            {
                                std::cout << "bufferViews buffer: " << it2.value()["buffer"] << std::endl;
                                std::cout << "bufferViews byteOffset: " << it2.value()["byteOffset"] << std::endl;
                                std::cout << "bufferViews byteLength: " << it2.value()["byteLength"] << std::endl;
                                std::cout << "bufferViews target: " << it2.value()["target"] << std::endl;

                                bufferviews_buffer.push_back(it2.value()["buffer"]);
                                bufferviews_byteOffset.push_back(it2.value()["byteOffset"]);
                                bufferviews_byteLength.push_back(it2.value()["byteLength"]);
                                bufferviews_target.push_back(it2.value()["target"]);
                            }
                            else
                            {
                                LOG_AND_EXIT("Error: glTF bufferViews is malformed.");
                            }
                        }
                    }
                }

                for (const auto& it : json_glb.items())
                {
                    if (it.key() == "accessors")
                    {
                        for (const auto& it2 : it.value().items())
                        {
                            if (it2.value().contains("bufferView") && it2.value().contains("byteOffset") && it2.value().contains("componentType") && it2.value().contains("count") && it2.value().contains("type"))
                            {
                                std::cout << "accessors bufferView: " << it2.value()["bufferView"] << std::endl;
                                std::cout << "accessors byteOffset: " << it2.value()["byteOffset"] << std::endl;
                                std::cout << "accessors componentType: " << it2.value()["componentType"] << std::endl;
                                std::cout << "accessors count: " << it2.value()["count"] << std::endl;
                                std::cout << "accessors type: " << it2.value()["type"] << std::endl;

                                accessors_bufferView.push_back(it2.value()["bufferView"]);
                                accessors_byteOffset.push_back(it2.value()["byteOffset"]);
                                accessors_componentType.push_back(it2.value()["componentType"]);
                                accessors_count.push_back(it2.value()["count"]);
                                accessors_type.push_back(it2.value()["type"]);

                                float float_min = 0;
                                float float_max = 0;
                                vector2 vector2_min;
                                vector2 vector2_max;
                                vector3 vector3_min;
                                vector3 vector3_max;

                                if (accessors_type.back() == "SCALAR")
                                {
                                    if (it2.value().contains("min") && it2.value().contains("max"))
                                    {
                                        for (const auto& it3 : it2.value()["min"].items())
                                        {
                                            std::cout << "accessors SCALAR min: " << it3.value() << std::endl;

                                            float_min = it3.value();
                                        }

                                        for (const auto& it3 : it2.value()["max"].items())
                                        {
                                            std::cout << "accessors SCALAR max: " << it3.value() << std::endl;

                                            float_max = it3.value();
                                        }
                                    }
                                }
                                else if (accessors_type.back() == "VEC2")
                                {
                                    if (it2.value().contains("min") && it2.value().contains("max"))
                                    {
                                        if (it2.value()["min"].size() == 2)
                                        {
                                            std::cout << "accessors VEC2 min 0: " << it2.value()["min"][0] << std::endl;
                                            std::cout << "accessors VEC2 min 1: " << it2.value()["min"][1] << std::endl;

                                            vector2_min.x = it2.value()["min"][0];
                                            vector2_min.y = it2.value()["min"][1];
                                        }

                                        if (it2.value()["max"].size() == 2)
                                        {
                                            std::cout << "accessors VEC2 max 0: " << it2.value()["max"][0] << std::endl;
                                            std::cout << "accessors VEC2 max 1: " << it2.value()["max"][1] << std::endl;

                                            vector2_max.x = it2.value()["max"][0];
                                            vector2_max.y = it2.value()["max"][1];
                                        }
                                    }
                                }
                                else if (accessors_type.back() == "VEC3")
                                {
                                    if (it2.value().contains("min") && it2.value().contains("max"))
                                    {
                                        if (it2.value()["min"].size() == 3)
                                        {
                                            std::cout << "accessors VEC3 min 0: " << it2.value()["min"][0] << std::endl;
                                            std::cout << "accessors VEC3 min 1: " << it2.value()["min"][1] << std::endl;
                                            std::cout << "accessors VEC3 min 2: " << it2.value()["min"][2] << std::endl;

                                            vector3_min.x = it2.value()["min"][0];
                                            vector3_min.y = it2.value()["min"][1];
                                            vector3_min.z = it2.value()["min"][2];
                                        }

                                        if (it2.value()["max"].size() == 3)
                                        {
                                            std::cout << "accessors VEC3 max 0: " << it2.value()["max"][0] << std::endl;
                                            std::cout << "accessors VEC3 max 1: " << it2.value()["max"][1] << std::endl;
                                            std::cout << "accessors VEC3 max 2: " << it2.value()["max"][2] << std::endl;

                                            vector3_max.x = it2.value()["max"][0];
                                            vector3_max.y = it2.value()["max"][1];
                                            vector3_max.z = it2.value()["max"][2];
                                        }
                                    }
                                }

                                accessors_float_min.push_back(float_min);
                                accessors_float_max.push_back(float_max);
                                accessors_vector2_min.push_back(vector2_min);
                                accessors_vector2_max.push_back(vector2_max);
                                accessors_vector3_min.push_back(vector3_min);
                                accessors_vector3_max.push_back(vector3_max);
                            }
                            else
                            {
                                LOG_AND_EXIT("Error: glTF accessors is malformed.");
                            }
                        }
                    }
                }








                prim_file_header_data.push_back(0x42);
                prim_file_header_data.push_back(0x49);
                prim_file_header_data.push_back(0x4E);
                prim_file_header_data.push_back(0x31);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x8);
                prim_file_header_data.push_back(0x1);
                prim_file_header_data.push_back(0x0);

                uint32_t header_value = glb_file_size + 0x44;

                std::memcpy(&char4, &header_value, sizeof(uint32_t));

                prim_file_header_data.push_back(char4[3]);
                prim_file_header_data.push_back(char4[2]);
                prim_file_header_data.push_back(char4[1]);
                prim_file_header_data.push_back(char4[0]);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x44);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);

                std::memcpy(&input, &glb_file_size, sizeof(uint32_t));

                for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                {
                    prim_file_header_data.push_back(input[k]);
                }

                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);
                prim_file_header_data.push_back(0x0);

                for (uint64_t k = 0; k < 0x30; k++)
                {
                    prim_file_header_data.push_back((char)0xFF);
                }

                for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                {
                    prim_file_header_data.push_back(input[k]);
                }

                unsigned char footer_header[] = { 0xED, 0xA5, 0xEB, 0x12 };
                unsigned char footer_serial_data[] = { 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00 };
                uint32_t footer_serial_data_count = 7;

                uint32_t footer_total_size = footer_serial_data_count * (uint32_t)0x4;

                for (uint64_t k = 0; k < sizeof(footer_header); k++)
                {
                    meta_data_footer.push_back(footer_header[k]);
                }

                std::memcpy(&input, &footer_total_size, sizeof(uint32_t));

                for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                {
                    meta_data_footer.push_back(input[k]);
                }

                std::memcpy(&input, &footer_serial_data_count, sizeof(uint32_t));

                for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                {
                    meta_data_footer.push_back(input[k]);
                }

                for (uint64_t k = 0; k < sizeof(footer_serial_data); k++)
                {
                    meta_data_footer.push_back(footer_serial_data[k]);
                }

                std::string current_path = prim_folders.at(i) + "\\PRIM.rebuilt";

                file::create_directories(current_path);

                std::ofstream prim_file = std::ofstream(current_path + "\\" + prim_hash_file_name, std::ofstream::binary);

                if (!prim_file.good())
                {
                    LOG_AND_EXIT("Error: PRIM file " + current_path + "\\" + prim_hash_file_name + " could not be created.");
                }

                prim_file.write(prim_file_header_data.data(), prim_file_header_data.size());

                prim_file.write(glb_file_data.data(), glb_file_data.size());

                prim_file.write(meta_data_footer.data(), meta_data_footer.size());
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

        percent_progress = (uint32_t)100;

        task_single_status = TASK_SUCCESSFUL;
    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path + " to rebuild the PRIMs does not exist.");
    }
}