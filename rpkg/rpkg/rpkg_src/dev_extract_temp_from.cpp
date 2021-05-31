#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "temp.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <string_view>
/*
void rpkg_function::dev_extract_temp_from(std::string& input_path, std::string& filter, std::string& output_path)
{
    task_single_status = TASK_EXECUTING;
    task_multiple_status = TASK_EXECUTING;

    initialize_enum_map();

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path))
    {
        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        uint64_t temp_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path == input_path || !input_path_is_rpkg_file)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(temp_hash_value);

                if (it != rpkgs.at(rpkg_index).hash_map.end())
                {
                    std::map<uint32_t, uint32_t> parents_map;

                    std::vector<uint32_t> temps_indexes;

                    recursive_temp_loader(rpkg_index, it->second, parents_map, temps_indexes, 0, 0, 0);

                    for (uint64_t j = 0; j < temps.size(); j++)
                    {
                        for (uint64_t a = 0; a < temps.at(j).prim_depends_file_name.size(); a++)
                        {
                            if (temps.at(j).prim_depends_in_rpkgs.at(a).size() > 0)
                            {
                                for (uint64_t b = 0; b < temps.at(j).children.size(); b++)
                                {
                                    for (uint64_t c = 0; c < temps.at(temps.at(j).children.at(b)).prim_depends_file_name.size(); c++)
                                    {
                                        if (temps.at(temps.at(j).children.at(b)).prim_depends_in_rpkgs.at(c).size() > 0)
                                        {
                                            //temps.at(j).prim_depends_in_children = true;

                                            if (temps.at(j).prim_depends_file_name.at(a) == temps.at(temps.at(j).children.at(b)).prim_depends_file_name.at(c))
                                            {
                                                //temps.at(j).prim_depends_is_godot_node.at(a) = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    std::string map_name1 = "lol.tscn";
                    std::string map_name2 = "r:\\h3";

                    //export_all_map_data_to_folder(0, map_name1, map_name2);

                    std::exit(0);

                    //temps.at(0).export_map_data_to_folder(map_name1, map_name2);

                    //map_export_map_data_to_folder(0, "output.tscn", "R:\\output");

                    LOG("Recursively loaded TEMPs: " + util::uint64_t_to_string(temps.size()));

                    for (uint64_t j = 0; j < temps.size(); j++)
                    {
                        std::cout << "TEMP File Name: " << temps.at(j).temp_file_name << std::endl;
                        std::cout << "  - In RPKG: " << rpkgs.at(temps.at(j).temp_rpkg_index).rpkg_file_path << std::endl;

                        //std::cout << "  - TBLU return value: " << temps.at(j).tblu_return_value << std::endl;

                        for (uint64_t a = 0; a < temps.at(j).prim_depends_file_name.size(); a++)
                        {
                            if (temps.at(j).prim_depends_in_rpkgs.at(a).size() > 0)
                            {
                                for (uint64_t b = 0; b < temps.at(j).children.size(); b++)
                                {
                                    //std::cout << "    - Child: " << temps.at(j).children.at(b) << std::endl;
                                    //std::cout << "    - Child TEMP File Name: " << temps.at(temps.at(j).children.at(b)).temp_file_name << std::endl;

                                    for (uint64_t c = 0; c < temps.at(temps.at(j).children.at(b)).prim_depends_file_name.size(); c++)
                                    {
                                        if (temps.at(temps.at(j).children.at(b)).prim_depends_in_rpkgs.at(c).size() > 0)
                                        {
                                            if (temps.at(j).prim_depends_file_name.at(a) == temps.at(temps.at(j).children.at(b)).prim_depends_file_name.at(c))
                                            {
                                                std::cout << "Duplicate found! Main TEMP's PRIM matches child PRIM!" << std::endl;

                                                std::cout << "      - Child PRIM File Name Found: " << temps.at(temps.at(j).children.at(b)).prim_depends_file_name.at(c) << std::endl;
                                                std::cout << "      - Child In RPKG: " << temps.at(temps.at(j).children.at(b)).prim_depends_in_rpkgs.at(c).at(temps.at(temps.at(j).children.at(b)).prim_depends_in_rpkgs_index.at(c)) << std::endl;

                                                std::cout << "  - PRIM File Name Found: " << temps.at(j).prim_depends_file_name.at(a) << std::endl;
                                                std::cout << "  - In RPKG: " << temps.at(j).prim_depends_in_rpkgs.at(a).at(temps.at(j).prim_depends_in_rpkgs_index.at(a)) << std::endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        //std::cout << "  - Top Level Logical Parents: " << std::endl;

                        temps.at(j).get_top_level_logical_parents();

                        //std::cout << "  - Parents: " << std::endl;

                        for (uint64_t k = 0; k < temps.at(j).parents.size(); k++)
                        {
                            //std::cout << "    - Parent: " << temps.at(j).parents.at(k) << std::endl;
                            //std::cout << "    - Parent TEMP File Name: " << temps.at(temps.at(j).parents.at(k)).temp_file_name << std::endl;
                        }

                        //std::cout << "  - Children: " << std::endl;

                        for (uint64_t k = 0; k < temps.at(j).children.size(); k++)
                        {
                            //std::cout << "    - Child: " << temps.at(j).children.at(k) << std::endl;
                            //std::cout << "    - Child TEMP File Name: " << temps.at(temps.at(j).children.at(k)).temp_file_name << std::endl;

                            for (uint64_t c = 0; c < temps.at(temps.at(j).children.at(k)).prim_depends_file_name.size(); c++)
                            {
                                if (temps.at(temps.at(j).children.at(k)).prim_depends_in_rpkgs.at(c).size() > 0)
                                {
                                    //std::cout << "      - Child PRIM File Name Found: " << temps.at(temps.at(j).children.at(k)).prim_depends_file_name.at(c) << std::endl;
                                    //std::cout << "      - Child Hash Depends Index: " << temps.at(temps.at(j).children.at(k)).prim_depends_index.at(c) << std::endl;
                                    //std::cout << "      - Child In RPKG: " << temps.at(temps.at(j).children.at(k)).prim_depends_in_rpkgs.at(c).at(temps.at(temps.at(j).children.at(k)).prim_depends_in_rpkgs_index.at(c)) << std::endl;
                                    //std::cout << "      - Child rpkg_index: " << temps.at(temps.at(j).children.at(k)).prim_depends_rpkg_index_index.at(c) << std::endl;
                                    //std::cout << "      - Child hash_index: " << temps.at(temps.at(j).children.at(k)).prim_depends_hash_index_index.at(c) << std::endl;
                                    //std::cout << "      - Child rpkg_index (rpkgs): " << temps.at(temps.at(j).children.at(k)).prim_depends_rpkg_index.at(c).at(temps.at(temps.at(j).children.at(k)).prim_depends_rpkg_index_index.at(c)) << std::endl;
                                    //std::cout << "      - Child hash_index (rpkgs): " << temps.at(temps.at(j).children.at(k)).prim_depends_hash_index.at(c).at(temps.at(temps.at(j).children.at(k)).prim_depends_hash_index_index.at(c)) << std::endl;
                                }
                            }
                        }

                        for (uint64_t k = 0; k < temps.at(j).temp_depends_file_name.size(); k++)
                        {
                            if (temps.at(j).temp_depends_in_rpkgs.at(k).size() > 0)
                            {
                                //std::cout << "  - TEMP File Name Found: " << temps.at(j).temp_depends_file_name.at(k) << std::endl;
                                //std::cout << "  - Hash Depends Index: " << temps.at(j).temp_depends_index.at(k) << std::endl;
                                //std::cout << "  - In RPKG: " << temps.at(j).temp_depends_in_rpkgs.at(k).at(temps.at(j).temp_depends_in_rpkgs_index.at(k)) << std::endl;
                                //std::cout << "  - rpkg_index: " << temps.at(j).temp_depends_rpkg_index_index.at(k) << std::endl;
                                //std::cout << "  - hash_index: " << temps.at(j).temp_depends_hash_index_index.at(k) << std::endl;
                                //std::cout << "  - rpkg_index (rpkgs): " << temps.at(j).temp_depends_rpkg_index.at(k).at(temps.at(j).temp_depends_rpkg_index_index.at(k)) << std::endl;
                                //std::cout << "  - hash_index (rpkgs): " << temps.at(j).temp_depends_hash_index.at(k).at(temps.at(j).temp_depends_hash_index_index.at(k)) << std::endl;
                            }
                        }

                        for (uint64_t k = 0; k < temps.at(j).tblu_depends_file_name.size(); k++)
                        {
                            if (temps.at(j).tblu_depends_in_rpkgs.at(k).size() > 0)
                            {
                                //std::cout << "  - TBLU File Name Found: " << temps.at(j).tblu_depends_file_name.at(k) << std::endl;
                                //std::cout << "  - Hash Depends Index: " << temps.at(j).tblu_depends_index.at(k) << std::endl;
                                //std::cout << "  - In RPKG: " << temps.at(j).tblu_depends_in_rpkgs.at(k).at(temps.at(j).tblu_depends_in_rpkgs_index.at(k)) << std::endl;
                                //std::cout << "  - rpkg_index: " << temps.at(j).tblu_depends_rpkg_index_index.at(k) << std::endl;
                                //std::cout << "  - hash_index: " << temps.at(j).tblu_depends_hash_index_index.at(k) << std::endl;
                                //std::cout << "  - rpkg_index (rpkgs): " << temps.at(j).tblu_depends_rpkg_index.at(k).at(temps.at(j).tblu_depends_rpkg_index_index.at(k)) << std::endl;
                                //std::cout << "  - hash_index (rpkgs): " << temps.at(j).tblu_depends_hash_index.at(k).at(temps.at(j).tblu_depends_hash_index_index.at(k)) << std::endl;
                            }
                        }

                        for (uint64_t k = 0; k < temps.at(j).prim_depends_file_name.size(); k++)
                        {
                            if (temps.at(j).prim_depends_in_rpkgs.at(k).size() > 0)
                            {
                                //std::cout << "  - PRIM File Name Found: " << temps.at(j).prim_depends_file_name.at(k) << std::endl;
                                //std::cout << "  - Hash Depends Index: " << temps.at(j).prim_depends_index.at(k) << std::endl;
                                //std::cout << "  - In RPKG: " << temps.at(j).prim_depends_in_rpkgs.at(k).at(temps.at(j).prim_depends_in_rpkgs_index.at(k)) << std::endl;
                                //std::cout << "  - rpkg_index: " << temps.at(j).prim_depends_rpkg_index_index.at(k) << std::endl;
                                //std::cout << "  - hash_index: " << temps.at(j).prim_depends_hash_index_index.at(k) << std::endl;
                                //std::cout << "  - rpkg_index (rpkgs): " << temps.at(j).prim_depends_rpkg_index.at(k).at(temps.at(j).prim_depends_rpkg_index_index.at(k)) << std::endl;
                                //std::cout << "  - hash_index (rpkgs): " << temps.at(j).prim_depends_hash_index.at(k).at(temps.at(j).prim_depends_hash_index_index.at(k)) << std::endl;
                            }
                        }
                    }
                }
            }
        }

        std::vector<temp>().swap(temps);

    }
    else
    {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }
}*/