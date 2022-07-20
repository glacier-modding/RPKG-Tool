#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "console.h"
#include "util.h"
#include "rpkg.h"
#include "hash.h"
#include <iostream>
#include <map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>

uint32_t rpkg_function::get_latest_hash(uint64_t hash_value)
{
    uint32_t hash_in_rpkgs_index_by_chunk_number = 0;
    uint32_t hash_in_rpkgs_index_by_patch_number = 0;

    std::vector<std::string> hash_in_rpkgs;
    std::vector<bool> hash_in_rpkgs_patch_list;
    std::vector<bool> hash_in_rpkgs_deleted;
    std::vector<uint32_t> rpkg_indexes;
    std::vector<uint32_t> chunk_numbers;
    std::vector<uint32_t> patch_numbers;
    std::vector<uint32_t> chunk_numbers_deleted;
    std::vector<uint32_t> patch_numbers_deleted;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(i).hash_map.find(hash_value);

        if (it2 != rpkgs.at(i).hash_map.end())
        {
            std::string hash_in_rpkgs_upper_case = util::to_upper_case(rpkgs.at(i).rpkg_file_name);

            std::string_view hash_in_rpkgs_string_view(hash_in_rpkgs_upper_case.c_str(), hash_in_rpkgs_upper_case.length());

            size_t pos_chunk = hash_in_rpkgs_string_view.find("CHUNK");
            size_t pos_dlc = hash_in_rpkgs_string_view.find("DLC");
            size_t pos_patch = hash_in_rpkgs_string_view.find("PATCH");
            size_t pos_ext = hash_in_rpkgs_string_view.find(".RPKG");

            if (pos_chunk != std::string::npos && pos_patch != std::string::npos)
            {
                //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_patch)) << std::endl;
                //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)) << std::endl;
                chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_patch)).c_str(), nullptr, 10));
                patch_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)).c_str(), nullptr, 10));

                //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
                //std::cout << "Patch number: " << patch_numbers.back() << std::endl;
            }
            else if (pos_chunk != std::string::npos)
            {
                //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_ext)) << std::endl;
                chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5)).c_str(), nullptr, 10));
                patch_numbers.push_back(0);

                //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
            }
            else if (pos_dlc != std::string::npos && pos_patch != std::string::npos)
            {
                //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_patch)) << std::endl;
                //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)) << std::endl;
                chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_dlc + 3, pos_patch)).c_str(), nullptr, 10));
                patch_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)).c_str(), nullptr, 10));

                //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
                //std::cout << "Patch number: " << patch_numbers.back() << std::endl;
            }
            else if (pos_dlc != std::string::npos)
            {
                //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_ext)) << std::endl;
                chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_dlc + 3)).c_str(), nullptr, 10));
                patch_numbers.push_back(0);

                //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
            }

            if (log_output)
                LOG(rpkgs.at(i).hash.at(it2->second).hash_file_name + " was found in RPKG file: " + rpkgs.at(i).rpkg_file_name);

            hash_in_rpkgs.push_back(rpkgs.at(i).rpkg_file_name);
            hash_in_rpkgs_patch_list.push_back(false);
            hash_in_rpkgs_deleted.push_back(false);
            rpkg_indexes.push_back(i);
        }
        else
        {
            bool found_in_patch_list = false;

            if (rpkgs.at(i).is_patch_file)
            {
                if (rpkgs.at(i).patch_entry_count > 0)
                {
                    uint32_t patch_entry_count = 0;

                    for (uint64_t j = 0; j < rpkgs.at(i).patch_entry_count; j++)
                    {
                        if (hash_value == rpkgs.at(i).patch_entry_list.at(j))
                        {
                            found_in_patch_list = true;
                        }
                    }
                }
            }

            if (found_in_patch_list)
            {
                std::string hash_in_rpkgs_upper_case = util::to_upper_case(rpkgs.at(i).rpkg_file_name);

                std::string_view hash_in_rpkgs_string_view(hash_in_rpkgs_upper_case.c_str(), hash_in_rpkgs_upper_case.length());

                size_t pos_chunk = hash_in_rpkgs_string_view.find("CHUNK");
                size_t pos_dlc = hash_in_rpkgs_string_view.find("DLC");
                size_t pos_patch = hash_in_rpkgs_string_view.find("PATCH");
                size_t pos_ext = hash_in_rpkgs_string_view.find(".RPKG");

                if (pos_chunk != std::string::npos && pos_patch != std::string::npos)
                {
                    //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_patch)) << std::endl;
                    //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)) << std::endl;
                    chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_patch)).c_str(), nullptr, 10));
                    patch_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)).c_str(), nullptr, 10));

                    //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
                    //std::cout << "Patch number: " << patch_numbers.back() << std::endl;
                }
                else if (pos_chunk != std::string::npos)
                {
                    //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_ext)) << std::endl;
                    chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5)).c_str(), nullptr, 10));
                    patch_numbers.push_back(0);

                    //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
                }
                else if (pos_dlc != std::string::npos && pos_patch != std::string::npos)
                {
                    //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_patch)) << std::endl;
                    //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)) << std::endl;
                    chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_dlc + 3, pos_patch)).c_str(), nullptr, 10));
                    patch_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_patch + 5, pos_ext)).c_str(), nullptr, 10));

                    //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
                    //std::cout << "Patch number: " << patch_numbers.back() << std::endl;
                }
                else if (pos_dlc != std::string::npos)
                {
                    //std::cout << std::string(hash_in_rpkgs_string_view.substr(pos_chunk + 5, pos_ext)) << std::endl;
                    chunk_numbers.push_back(std::strtol(std::string(hash_in_rpkgs_string_view.substr(pos_dlc + 3)).c_str(), nullptr, 10));
                    patch_numbers.push_back(0);

                    //std::cout << "Chunk number: " << chunk_numbers.back() << std::endl;
                }

                if (log_output)
                    LOG(util::uint64_t_to_hex_string(hash_value) + " was found in the patch deletion list of RPKG file: " + rpkgs.at(i).rpkg_file_name);

                hash_in_rpkgs.push_back(rpkgs.at(i).rpkg_file_name);
                hash_in_rpkgs_patch_list.push_back(true);
                hash_in_rpkgs_deleted.push_back(true);
                chunk_numbers_deleted.push_back(chunk_numbers.back());
                patch_numbers_deleted.push_back(patch_numbers.back());
                rpkg_indexes.push_back(i);
            }
        }
    }

    for (uint64_t i = 0; i < hash_in_rpkgs.size(); i++)
    {
        for (uint64_t j = 0; j < chunk_numbers_deleted.size(); j++)
        {
            if (chunk_numbers.at(i) == chunk_numbers_deleted.at(j))
            {
                if (patch_numbers.at(i) <= patch_numbers_deleted.at(j))
                {
                    hash_in_rpkgs_deleted.at(i) = true;
                }
            }
        }
    }

    uint32_t hash_in_rpkg_index = 0;
    uint32_t hash_in_rpkg_index_patch = 0;

    for (uint64_t i = 0; i < hash_in_rpkgs.size(); i++)
    {
        if (!hash_in_rpkgs_deleted.at(i))
        {
            if (patch_numbers.at(i) > hash_in_rpkg_index_patch)
            {
                hash_in_rpkg_index = i;
                hash_in_rpkg_index_patch = patch_numbers.at(i);
            }
        }
    }

    if (rpkg_indexes.size() == 0)
    {
        return UINT32_MAX;
    }
    else
    {
        return rpkg_indexes.at(hash_in_rpkg_index);
    }
}