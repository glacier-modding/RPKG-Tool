#include "temp.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <iostream>
#include <map>
#include <fstream>
#include <set>

temp::temp()
{

}

temp::temp(uint64_t rpkgs_index, uint64_t hash_index)
{
    initialize_property_map();
    initialize_enum_map();

    temp_rpkg_index = rpkgs_index;
    temp_hash_index = hash_index;

    temp_file_name = rpkgs.at(rpkgs_index).hash.at(hash_index).hash_file_name;

    tblu_return_value = TEMP_TBLU_NOT_FOUND_IN_DEPENDS;

    load_hash_depends();

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        load_temp_tblu_data();
    }
    else
    {
        //std::cout << temp_file_name << " will not be loaded due to a missing TBLU." << std::endl;
    }

    //load_temp_hash_depends();

    //load_prim_hash_depends();

    ////LOG("Found TEMP Depends Count: " + util::uint64_t_to_string(temp_depends_file_name.size()));

    ////LOG("Found PRIM Depends Count: " + util::uint64_t_to_string(prim_depends_file_name.size()));
}

void temp::load_temp_hash_depends()
{
    uint32_t temp_hash_reference_count = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    //LOG(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(temp_rpkg_index).rpkg_file_path);

    if (temp_hash_reference_count > 0)
    {
        for (uint64_t i = 0; i < temp_hash_reference_count; i++)
        {
            std::vector<std::string> temp_temp_depends_in_rpkgs;
            std::vector<uint32_t> temp_temp_depends_rpkg_index;
            std::vector<uint32_t> temp_temp_depends_hash_index;

            bool found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end())
                {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TEMP")
                    {
                        if (!found)
                        {
                            temp_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            ////LOG("Found TEMP hash depend:");
                            ////LOG("  - File Name: " + temp_depends_file_name.back());
                        }

                        found = true;

                        temp_temp_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_temp_depends_rpkg_index.push_back(j);
                        temp_temp_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_temp_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_temp_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_temp_depends_hash_index.back()));
                    }
                }
            }

            if (found)
            {
                temp_depends_in_rpkgs.push_back(temp_temp_depends_in_rpkgs);
                temp_depends_rpkg_index.push_back(temp_temp_depends_rpkg_index);
                temp_depends_hash_index.push_back(temp_temp_depends_hash_index);
            }
        }
    }
}

void temp::load_prim_hash_depends()
{
    uint32_t temp_hash_reference_count = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    //LOG(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(temp_rpkg_index).rpkg_file_path);

    if (temp_hash_reference_count > 0)
    {
        for (uint64_t i = 0; i < temp_hash_reference_count; i++)
        {
            std::vector<std::string> temp_prim_depends_in_rpkgs;
            std::vector<uint32_t> temp_prim_depends_rpkg_index;
            std::vector<uint32_t> temp_prim_depends_hash_index;

            bool found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end())
                {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "PRIM")
                    {
                        if (!found)
                        {
                            prim_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            ////LOG("Found PRIM hash depend:");
                            ////LOG("  - File Name: " + prim_depends_file_name.back());
                        }

                        found = true;

                        temp_prim_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_prim_depends_rpkg_index.push_back(j);
                        temp_prim_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_prim_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_prim_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_prim_depends_hash_index.back()));
                    }
                }
            }

            if (found)
            {
                prim_depends_in_rpkgs.push_back(temp_prim_depends_in_rpkgs);
                prim_depends_rpkg_index.push_back(temp_prim_depends_rpkg_index);
                prim_depends_hash_index.push_back(temp_prim_depends_hash_index);
            }
        }
    }
}

void temp::load_hash_depends()
{
    uint32_t temp_hash_reference_count = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    //LOG(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(temp_rpkg_index).rpkg_file_path);

    if (temp_hash_reference_count > 0)
    {
        for (uint64_t i = 0; i < temp_hash_reference_count; i++)
        {
            std::vector<std::string> temp_temp_depends_in_rpkgs;
            std::vector<uint32_t> temp_temp_depends_rpkg_index;
            std::vector<uint32_t> temp_temp_depends_hash_index;

            std::vector<std::string> temp_tblu_depends_in_rpkgs;
            std::vector<uint32_t> temp_tblu_depends_rpkg_index;
            std::vector<uint32_t> temp_tblu_depends_hash_index;

            std::vector<std::string> temp_prim_depends_in_rpkgs;
            std::vector<uint32_t> temp_prim_depends_rpkg_index;
            std::vector<uint32_t> temp_prim_depends_hash_index;

            bool temp_found = false;
            bool tblu_found = false;
            bool prim_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end())
                {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TEMP")
                    {
                        if (!temp_found)
                        {
                            temp_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            temp_depends_index.push_back(i);

                            ////LOG("Found TEMP hash depend:");
                            ////LOG("  - File Name: " + temp_depends_file_name.back());
                        }

                        temp_found = true;

                        temp_temp_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_temp_depends_rpkg_index.push_back(j);
                        temp_temp_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_temp_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_temp_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_temp_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TBLU")
                    {
                        if (!tblu_found)
                        {
                            tblu_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            tblu_depends_index.push_back(i);

                            tblu_return_value = TEMP_TBLU_FOUND;

                            ////LOG("Found TEMP hash depend:");
                            ////LOG("  - File Name: " + temp_depends_file_name.back());
                        }
                        else
                        {
                            if (rpkgs.at(j).hash.at(it->second).hash_file_name != tblu_depends_file_name.back())
                            {
                                tblu_return_value = TEMP_TBLU_TOO_MANY;
                            }
                        }

                        tblu_found = true;

                        temp_tblu_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_tblu_depends_rpkg_index.push_back(j);
                        temp_tblu_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_tblu_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_tblu_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_tblu_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "PRIM")
                    {
                        if (!prim_found)
                        {
                            prim_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            prim_depends_index.push_back(i);

                            ////LOG("Found PRIM hash depend:");
                            ////LOG("  - File Name: " + prim_depends_file_name.back());
                        }

                        prim_found = true;

                        temp_prim_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        temp_prim_depends_rpkg_index.push_back(j);
                        temp_prim_depends_hash_index.push_back(it->second);

                        ////LOG("  - In RPKG: " + temp_prim_depends_in_rpkgs.back());
                        ////LOG("  - RPKG Index: " + util::uint64_t_to_string(temp_prim_depends_rpkg_index.back()));
                        ////LOG("  - Hash Index: " + util::uint64_t_to_string(temp_prim_depends_hash_index.back()));
                    }
                }
            }

            uint32_t temp_value = 0;

            if (temp_found)
            {
                temp_depends_in_rpkgs.push_back(temp_temp_depends_in_rpkgs);
                temp_depends_rpkg_index.push_back(temp_temp_depends_rpkg_index);
                temp_depends_hash_index.push_back(temp_temp_depends_hash_index);
                temp_depends_in_rpkgs_index.push_back(temp_value);
                temp_depends_rpkg_index_index.push_back(temp_value);
                temp_depends_hash_index_index.push_back(temp_value);
            }

            if (tblu_found)
            {
                tblu_depends_in_rpkgs.push_back(temp_tblu_depends_in_rpkgs);
                tblu_depends_rpkg_index.push_back(temp_tblu_depends_rpkg_index);
                tblu_depends_hash_index.push_back(temp_tblu_depends_hash_index);
                tblu_depends_in_rpkgs_index.push_back(temp_value);
                tblu_depends_rpkg_index_index.push_back(temp_value);
                tblu_depends_hash_index_index.push_back(temp_value);
            }

            if (prim_found)
            {
                prim_depends_in_rpkgs.push_back(temp_prim_depends_in_rpkgs);
                prim_depends_rpkg_index.push_back(temp_prim_depends_rpkg_index);
                prim_depends_hash_index.push_back(temp_prim_depends_hash_index);
                prim_depends_in_rpkgs_index.push_back(temp_value);
                prim_depends_rpkg_index_index.push_back(temp_value);
                prim_depends_hash_index_index.push_back(temp_value);
            }
        }
    }

    for (uint64_t k = 0; k < temp_depends_file_name.size(); k++)
    {
        //LOG("  - TEMP File Name: " + temp_depends_file_name.at(k));

        bool temp_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < temp_depends_in_rpkgs.at(k).size(); d++)
        {
            std::string temp_depends_in_rpkgs_upper_case = util::to_upper_case(temp_depends_in_rpkgs.at(k).at(d));

            std::string_view temp_depends_in_rpkgs_string_view(temp_depends_in_rpkgs_upper_case.c_str(), temp_depends_in_rpkgs_upper_case.length());

            size_t pos1 = temp_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos)
            {
                size_t pos2 = temp_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos)
                {
                    temp_patch_name_found = true;

                    long new_patch_level = std::strtol(std::string(temp_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level)
                    {
                        patch_level = new_patch_level;

                        temp_depends_in_rpkgs_index.at(k) = d;
                        temp_depends_rpkg_index_index.at(k) = d;
                        temp_depends_hash_index_index.at(k) = d;
                    }
                }
            }
            else
            {
                if (!temp_patch_name_found)
                {
                    temp_depends_in_rpkgs_index.at(k) = d;
                    temp_depends_rpkg_index_index.at(k) = d;
                    temp_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - TEMP File Name In RPKG: " + temp_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - TEMP File Name In RPKG Used: " + temp_depends_in_rpkgs.at(k).at(temp_depends_rpkg_index_index.at(k)));
    }

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        //LOG("  - TBLU Return Value: TEMP_TBLU_FOUND");

        for (uint64_t k = 0; k < tblu_depends_file_name.size(); k++)
        {
            //LOG("  - TBLU File Name: " + tblu_depends_file_name.at(k));

            bool tblu_patch_name_found = false;

            long patch_level = 0;

            for (uint64_t d = 0; d < tblu_depends_in_rpkgs.at(k).size(); d++)
            {
                std::string tblu_depends_in_rpkgs_upper_case = util::to_upper_case(tblu_depends_in_rpkgs.at(k).at(d));

                std::string_view tblu_depends_in_rpkgs_string_view(tblu_depends_in_rpkgs_upper_case.c_str(), tblu_depends_in_rpkgs_upper_case.length());

                size_t pos1 = tblu_depends_in_rpkgs_string_view.find("PATCH");

                if (pos1 != std::string::npos)
                {
                    size_t pos2 = tblu_depends_in_rpkgs_string_view.substr(pos1).find(".");

                    if (pos2 != std::string::npos)
                    {
                        tblu_patch_name_found = true;

                        long new_patch_level = std::strtol(std::string(tblu_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                        if (new_patch_level > patch_level)
                        {
                            patch_level = new_patch_level;

                            tblu_depends_in_rpkgs_index.at(k) = d;
                            tblu_depends_rpkg_index_index.at(k) = d;
                            tblu_depends_hash_index_index.at(k) = d;
                        }
                    }
                }
                else
                {
                    if (!tblu_patch_name_found)
                    {
                        tblu_depends_in_rpkgs_index.at(k) = d;
                        tblu_depends_rpkg_index_index.at(k) = d;
                        tblu_depends_hash_index_index.at(k) = d;
                    }
                }

                //LOG("  - TBLU File Name In RPKG: " + tblu_depends_in_rpkgs.at(k).at(d));
            }

            //LOG("  - TBLU File Name In RPKG Used: " + tblu_depends_in_rpkgs.at(k).at(tblu_depends_rpkg_index_index.at(k)));

            tblu_rpkg_index = tblu_depends_rpkg_index.at(k).at(tblu_depends_rpkg_index_index.at(k));
            tblu_hash_index = tblu_depends_hash_index.at(k).at(tblu_depends_hash_index_index.at(k));
        }
    }

    for (uint64_t k = 0; k < prim_depends_file_name.size(); k++)
    {
        //LOG("  - PRIM File Name: " + prim_depends_file_name.at(k));

        bool prim_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < prim_depends_in_rpkgs.at(k).size(); d++)
        {
            std::string prim_depends_in_rpkgs_upper_case = util::to_upper_case(prim_depends_in_rpkgs.at(k).at(d));

            std::string_view prim_depends_in_rpkgs_string_view(prim_depends_in_rpkgs_upper_case.c_str(), prim_depends_in_rpkgs_upper_case.length());

            size_t pos1 = prim_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos)
            {
                size_t pos2 = prim_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos)
                {
                    prim_patch_name_found = true;

                    long new_patch_level = std::strtol(std::string(prim_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level)
                    {
                        patch_level = new_patch_level;

                        prim_depends_in_rpkgs_index.at(k) = d;
                        prim_depends_rpkg_index_index.at(k) = d;
                        prim_depends_hash_index_index.at(k) = d;
                    }
                }
            }
            else
            {
                if (!prim_patch_name_found)
                {
                    prim_depends_in_rpkgs_index.at(k) = d;
                    prim_depends_rpkg_index_index.at(k) = d;
                    prim_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - PRIM File Name In RPKG: " + prim_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - PRIM File Name In RPKG Used: " + prim_depends_in_rpkgs.at(k).at(prim_depends_rpkg_index_index.at(k)));
    }
}

void temp::load_temp_tblu_data()
{
    //std::cout << rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_file_name << std::endl;
    //std::cout << rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_file_name << std::endl;

    uint64_t temp_hash_size;

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_lz4ed == 1)
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size;

        if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_xored == 1)
        {
            temp_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        temp_hash_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size_final;
    }

    temp_input_data = std::vector<char>(temp_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(temp_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(temp_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_offset, file.beg);
    file.read(temp_input_data.data(), temp_hash_size);
    file.close();

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_xored == 1)
    {
        crypto::xor_data(temp_input_data.data(), (uint32_t)temp_hash_size);
    }

    uint32_t temp_decompressed_size = rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_size_final;

    temp_output_data = std::vector<char>(temp_decompressed_size, 0);

    if (rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).is_lz4ed)
    {
        LZ4_decompress_safe(temp_input_data.data(), temp_output_data.data(), (int)temp_hash_size, temp_decompressed_size);

        temp_data = temp_output_data;
    }
    else
    {
        temp_data = temp_input_data;
    }

    std::vector<char>().swap(temp_output_data);
    std::vector<char>().swap(temp_input_data);

    std::string tblu_hash_file_name = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_file_name;

    uint64_t tblu_hash_size;

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_lz4ed == 1)
    {
        tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_size;

        if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_xored == 1)
        {
            tblu_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        tblu_hash_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_size_final;
    }

    tblu_input_data = std::vector<char>(tblu_hash_size, 0);

    std::ifstream file2 = std::ifstream(rpkgs.at(tblu_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file2.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(tblu_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file2.seekg(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_offset, file2.beg);
    file2.read(tblu_input_data.data(), tblu_hash_size);
    file2.close();

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_xored == 1)
    {
        crypto::xor_data(tblu_input_data.data(), (uint32_t)tblu_hash_size);
    }

    uint32_t tblu_decompressed_size = rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_size_final;

    tblu_output_data = std::vector<char>(tblu_decompressed_size, 0);

    if (rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).is_lz4ed)
    {
        LZ4_decompress_safe(tblu_input_data.data(), tblu_output_data.data(), (int)tblu_hash_size, tblu_decompressed_size);

        tblu_data = tblu_output_data;
    }
    else
    {
        tblu_data = tblu_input_data;
    }

    std::vector<char>().swap(tblu_output_data);
    std::vector<char>().swap(tblu_input_data);

    //LOG(temp_decompressed_size);
    //LOG(tblu_decompressed_size);

    char input[1024];
    char char4[4];
    uint8_t bytes1 = 0;
    uint16_t bytes2 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;

    uint64_t temp_position = 0;
    uint64_t tblu_position = 0;

    uint32_t temp_entry_count = 0;
    uint32_t tblu_entry_count = 0;

    uint32_t temp_entry_size = 0xA8;
    uint32_t tblu_entry_size = 0x70;

    temp_position = 0x8;
    tblu_position = 0x18;

    std::memcpy(&input, &temp_data.data()[temp_position], sizeof(bytes4));
    char4[0] = input[3];
    char4[1] = input[2];
    char4[2] = input[1];
    char4[3] = input[0];
    std::memcpy(&temp_footer_offset, &char4, sizeof(bytes4));
    //temp_position += 0x64;
    check_pointer_map(temp_position);
    temp_position += 0x20;
    check_pointer_map(temp_position);

    std::memcpy(&temp_after_header_table_offset, &temp_data.data()[temp_position], sizeof(bytes4));
    temp_position += 0x44;
    check_pointer_map(temp_position);

    temp_after_header_table_offset += 0x10;

    temp_footer_offset += 0x10;

    //LOG("TEMP footer offset: " + util::uint32_t_to_string(temp_footer_offset));

    std::memcpy(&temp_entry_count, &temp_data.data()[temp_position], sizeof(bytes4));

    //LOG("TEMP entry count: " + util::uint32_t_to_string(temp_entry_count));

    std::memcpy(&tblu_header_table_offset, &tblu_data.data()[tblu_position], sizeof(bytes4));

    if (tblu_header_table_offset == 0xA0)
    {
        tblu_file_version = 0;

        tblu_position = 0xAC;

        //LOG("TEMP version: H1");
    }
    else if (tblu_header_table_offset == 0xD0)
    {
        tblu_file_version = 1;

        tblu_position = 0xDC;

        //LOG("TEMP version: H2/H3");
    }
    else
    {
        LOG("Error: TBLU_VERSION_UNKNOWN");

        tblu_return_value = TBLU_VERSION_UNKNOWN;

        return;
    }

    std::memcpy(&tblu_entry_count, &tblu_data.data()[tblu_position], sizeof(bytes4));

    //LOG("TBLU entry count: " + util::uint32_t_to_string(tblu_entry_count));

    if (temp_entry_count != tblu_entry_count)
    {
        LOG("Error: TEMP_TBLU_ENTRY_COUNT_MISMATCH");

        tblu_return_value = TEMP_TBLU_ENTRY_COUNT_MISMATCH;

        return;
    }

    uint32_t temp_version_check = (temp_after_header_table_offset - 0x70) / temp_entry_count;

    if (temp_version_check == 0x58)
    {
        temp_file_version = 0;

        //LOG("TEMP version: H1/H2");
    }
    else if (temp_version_check == 0x70)
    {
        temp_file_version = 1;

        //LOG("TEMP version: H3");
    }
    else
    {
        LOG("Error: TEMP_VERSION_UNKNOWN");

        tblu_return_value = TEMP_VERSION_UNKNOWN;

        return;
    }

    temp_position += 0x4;
    check_pointer_map(temp_position);
    tblu_position += 0x4;

    /*
    H1/H2 TEMP:
        SEntityTemplateReference logicalParent; // 0x0
        int32 entityTypeResourceIndex; // 0x20
        TArray<SEntityTemplateProperty> propertyValues; // 0x28
        TArray<SEntityTemplateProperty> postInitPropertyValues; // 0x40
    H3 TEMP:
        SEntityTemplateReference logicalParent; // 0x0
        int32 entityTypeResourceIndex; // 0x20
        TArray<SEntityTemplateProperty> propertyValues; // 0x28
        TArray<SEntityTemplateProperty> postInitPropertyValues; // 0x40
        TArray<SEntityTemplatePlatformSpecificProperty> platformSpecificPropertyValues; // 0x58
    H1/H2/H3 TBLU:
        SEntityTemplateReference logicalParent; // 0x0
        int32 entityTypeResourceIndex; // 0x20
        uint64 entityId; // 0x28
        bool editorOnly; // 0x30
        ZString entityName; // 0x38
        TArray<SEntityTemplatePropertyAlias> propertyAliases; // 0x48
        TArray<SEntityTemplateExposedEntity> exposedEntities; // 0x60
        TArray<TPair<ZString, int32>> exposedInterfaces; // 0x78
        TArray<TPair<ZString, SEntityTemplateEntitySubset>> entitySubsets; // 0x90
    */

    for (uint32_t e = 0; e < tblu_entry_count; e++)
    {
        tblu_entry_index.push_back((uint32_t)e);

        //LOG("tblu_entry_index: " + util::uint32_t_to_hex_string(tblu_entry_index.back()));

        std::memcpy(&bytes8, &tblu_data.data()[tblu_position], sizeof(bytes8));
        tblu_position += 0xC;

        tblu_logicalParent_entityId.push_back(bytes8);

        tblu_logicalParent_is_entityId.push_back(false);

        if (bytes8 != 0xFFFFFFFFFFFFFFFF)
        {
            tblu_logicalParent_is_entityId.push_back(true);
        }

        std::memcpy(&bytes4, &tblu_data.data()[tblu_position], sizeof(bytes4));
        tblu_position += 0x14;

        tblu_logicalParent.push_back(bytes4);

        //LOG("tblu_logicalParent: " + util::uint32_t_to_hex_string(tblu_logicalParent.back()));

        std::memcpy(&bytes4, &tblu_data.data()[tblu_position], sizeof(bytes4));
        tblu_position += 0x8;

        tblu_entityTypeResourceIndex.push_back(bytes4);

        //LOG("tblu_entityTypeResourceIndex: " + util::uint32_t_to_hex_string(tblu_entityTypeResourceIndex.back()));

        std::memcpy(&bytes8, &tblu_data.data()[tblu_position], sizeof(bytes8));
        tblu_position += 0x8;

        tblu_entityId.push_back(bytes8);

        //LOG("tblu_entityId: " + util::uint64_t_to_hex_string(tblu_entityId.back()));

        if (tblu_file_version == 1)
        {
            std::memcpy(&bytes4, &tblu_data.data()[tblu_position], sizeof(bytes4));
            tblu_position += 0x8;

            tblu_editorOnly.push_back(bytes4);

            //LOG("tblu_editorOnly: " + util::uint32_t_to_hex_string(tblu_editorOnly.back()));
        }

        uint32_t string_size = 0;
        uint32_t string_position = 0;

        std::memcpy(&string_size, &tblu_data.data()[tblu_position], sizeof(bytes4));
        tblu_position += 0x8;

        string_size &= 0x3FFFFFFF;

        //LOG("string_size: " + util::uint32_t_to_string(string_size));

        std::memcpy(&string_position, &tblu_data.data()[tblu_position], sizeof(bytes4));

        string_position += 0x10;

        std::memcpy(&input, &tblu_data.data()[string_position], string_size);
        input[string_size] = 0;

        tblu_entityName.push_back(std::string(input));

        //LOG("tblu_entityName: " + tblu_entityName.back());

        tblu_position += 0x68;
    }

    uint64_t temp_temp_position = temp_position;

    temp_position = temp_footer_offset;

    uint32_t crc32_header_value = 0;

    bool header_found = false;

    while (crc32_header_value != 0x12EBA5ED && temp_position < temp_data.size())
    {
        std::memcpy(&crc32_header_value, &temp_data.data()[temp_position], sizeof(bytes4));

        if (crc32_header_value != 0x12EBA5ED)
        {
            temp_position += 0x4;
            check_pointer_map(temp_position);
            std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
            temp_position += 0x4;
            check_pointer_map(temp_position);
            temp_position += bytes4;
            check_pointer_map(temp_position);
        }
        else
        {
            header_found = true;
        }
    }

    if (header_found)
    {
        temp_position += 0x4;
        check_pointer_map(temp_position);

        std::memcpy(&pointers_size, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x4;
        check_pointer_map(temp_position);

        std::cout << "Pointer table size: " << pointers_size << std::endl;

        std::memcpy(&pointers_count, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x4;
        check_pointer_map(temp_position);

        std::cout << "Pointer table count: " << pointers_count << std::endl;

        for (uint64_t p = 0; p < pointers_count; p++)
        {
            std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
            temp_position += 0x4;
            check_pointer_map(temp_position);

            pointers.push_back(bytes4);

            pointers_map[pointers.back()] = pointers_map.size();

            //std::cout << "Pointer (" << p << "): 0x" << util::uint32_t_to_hex_string(pointers.back()) << std::endl;
        }
    }
    else
    {
        LOG("Error: TEMP_HEADER_NOT_FOUND");

        tblu_return_value = TEMP_HEADER_NOT_FOUND;

        return;
    }

    temp_position = temp_temp_position;

    for (uint32_t e = 0; e < temp_entry_count; e++)
    {
        temp_entry_index.push_back((uint32_t)e);

        //LOG("temp_entry_index: " + util::uint32_t_to_hex_string(temp_entry_index.back()));

        std::memcpy(&bytes8, &temp_data.data()[temp_position], sizeof(bytes8));
        temp_position += 0xC;
        check_pointer_map(temp_position);

        temp_logicalParent_entityId.push_back(bytes8);

        temp_logicalParent_is_entityId.push_back(false);

        if (bytes8 != 0xFFFFFFFFFFFFFFFF)
        {
            temp_logicalParent_is_entityId.push_back(true);
        }

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x14;
        check_pointer_map(temp_position);

        temp_logicalParent.push_back(bytes4);

        if (temp_logicalParent.at(e) < temp_entry_count)
        {
            //LOG("temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.back()) + " (" + tblu_entityName.at(temp_logicalParent.back()) + ")");
        }
        else
        {
            //LOG("temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.back()));
        }

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x8;
        check_pointer_map(temp_position);

        temp_entityTypeResourceIndex.push_back(bytes4);

        //LOG("temp_entityTypeResourceIndex: " + util::uint32_t_to_hex_string(temp_entityTypeResourceIndex.back()));

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x8;
        check_pointer_map(temp_position);

        temp_propertyValues_start_offsets.push_back(bytes4);

        //LOG("temp_propertyValues_start_offsets: " + util::uint32_t_to_hex_string(temp_propertyValues_start_offsets.back()));

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x10;
        check_pointer_map(temp_position);

        temp_propertyValues_end_offsets.push_back(bytes4);

        //LOG("temp_propertyValues_end_offsets: " + util::uint32_t_to_hex_string(temp_propertyValues_end_offsets.back()));

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x8;
        check_pointer_map(temp_position);

        temp_postInitPropertyValues_start_offsets.push_back(bytes4);

        //LOG("temp_postInitPropertyValues_start_offsets: " + util::uint32_t_to_hex_string(temp_postInitPropertyValues_start_offsets.back()));

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x10;
        check_pointer_map(temp_position);

        temp_postInitPropertyValues_end_offsets.push_back(bytes4);

        //LOG("temp_postInitPropertyValues_end_offsets: " + util::uint32_t_to_hex_string(temp_postInitPropertyValues_end_offsets.back()));

        if (temp_file_version == 1)
        {
            std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
            temp_position += 0x8;
            check_pointer_map(temp_position);

            temp_platformSpecificPropertyValues_start_offsets.push_back(bytes4);

            //LOG("temp_platformSpecificPropertyValues_start_offsets: " + util::uint32_t_to_hex_string(temp_platformSpecificPropertyValues_start_offsets.back()));

            std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
            temp_position += 0x10;
            check_pointer_map(temp_position);

            temp_platformSpecificPropertyValues_end_offsets.push_back(bytes4);

            //LOG("temp_platformSpecificPropertyValues_end_offsets: " + util::uint32_t_to_hex_string(temp_platformSpecificPropertyValues_end_offsets.back()));
        }
    }

    temp_position = temp_footer_offset;

    crc32_header_value = 0;

    header_found = false;

    while (crc32_header_value != 0x3989BF9F && temp_position < temp_decompressed_size)
    {
        std::memcpy(&crc32_header_value, &temp_data.data()[temp_position], sizeof(bytes4));

        if (crc32_header_value != 0x3989BF9F)
        {
            temp_position += 0x4;
            check_pointer_map(temp_position);
            std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
            temp_position += 0x4;
            check_pointer_map(temp_position);
            temp_position += bytes4;
            check_pointer_map(temp_position);
        }
        else
        {
            header_found = true;
        }
    }

    if (header_found)
    {
        //LOG("Header (0x3989BF9F) found at offset: " + util::uint32_t_to_hex_string((uint32_t)temp_position));

        temp_position += 0x8;
        check_pointer_map(temp_position);

        std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x4;
        check_pointer_map(temp_position);

        temp_position += (uint64_t)bytes4 * (uint64_t)0x4;
        check_pointer_map(temp_position);

        uint32_t property_types_count = 0;

        std::memcpy(&property_types_count, &temp_data.data()[temp_position], sizeof(bytes4));
        temp_position += 0x4;
        check_pointer_map(temp_position);

        uint64_t alignment_offset = 0;

        for (uint32_t e = 0; e < property_types_count; e++)
        {
            temp_position += 0x8;
            check_pointer_map(temp_position);
            alignment_offset += 0x8;

            std::memcpy(&bytes4, &temp_data.data()[temp_position], sizeof(bytes4));
            temp_position += 0x4;
            check_pointer_map(temp_position);
            alignment_offset += 0x4;

            std::memcpy(&input, &temp_data.data()[temp_position], bytes4);
            temp_position += bytes4;
            check_pointer_map(temp_position);
            alignment_offset += bytes4;

            temp_property_types.push_back(std::string(input));

            //LOG("temp_property_types: " + temp_property_types.back());

            while (alignment_offset % 0x4 != 0)
            {
                temp_position++;
                alignment_offset++;
            }
        }

        for (uint32_t e = 0; e < property_types_count; e++)
        {
            std::map<uint32_t, uint32_t> temp_temp_property_types_offsets_map;

            temp_property_types_offsets_map.push_back(temp_temp_property_types_offsets_map);

            std::vector<uint32_t> temp_temp_property_types_offsets;

            temp_property_types_offsets.push_back(temp_temp_property_types_offsets);

            std::vector<std::string> temp_temp_property_types_values;

            temp_property_types_values.push_back(temp_temp_property_types_values);

            std::vector<bool> temp_temp_property_types_shared;

            temp_property_types_shared.push_back(temp_temp_property_types_shared);

            std::vector<uint32_t> temp_temp_property_types_shared_count;

            temp_property_types_shared_count.push_back(temp_temp_property_types_shared_count);
        }

        for (uint32_t e = 0; e < temp_entry_count; e++)
        {
            uint32_t start_offset = temp_propertyValues_start_offsets.at(e);
            uint32_t end_offset = temp_propertyValues_end_offsets.at(e);
            start_offset += 0x10;
            end_offset += 0x10;

            temp_position = start_offset;

            //LOG("tblu_entityName: " + tblu_entityName.at(e));
            //LOG("  temp_entry_index: " + util::uint32_t_to_hex_string(temp_entry_index.at(e)));

            if (temp_logicalParent.at(e) < temp_entry_count)
            {
                //LOG("  temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.at(e)) + " (" + tblu_entityName.at(temp_logicalParent.at(e)) + ")");
            }
            else
            {
                //LOG("  temp_logicalParent: " + util::uint32_t_to_hex_string(temp_logicalParent.at(e)));
            }

            std::vector<uint32_t> temp_property_crc32_values;
            std::vector<uint32_t> temp_property_type_indexes;
            std::vector<uint32_t> temp_property_offsets;
            std::vector<uint32_t> temp_property_pointer_offsets;

            if ((end_offset - start_offset) >= 0x18)
            {
                while (temp_position < end_offset)
                {
                    uint32_t property_crc32_value = 0;
                    uint32_t property_type_index = 0;
                    uint32_t property_offset = 0;
                    uint32_t property_pointer_offset = 0;

                    std::memcpy(&property_crc32_value, &temp_data.data()[temp_position], sizeof(bytes4));
                    temp_position += 0x8;
                    check_pointer_map(temp_position);

                    temp_property_crc32_values.push_back(property_crc32_value);

                    std::memcpy(&property_type_index, &temp_data.data()[temp_position], sizeof(bytes4));
                    temp_position += 0x8;
                    check_pointer_map(temp_position);

                    temp_property_type_indexes.push_back(property_type_index);

                    LOG("position: " + util::uint32_t_to_hex_string(temp_position));

                    std::memcpy(&property_offset, &temp_data.data()[temp_position], sizeof(bytes4));
                    property_pointer_offset = temp_position;
                    temp_position += 0x8;
                    check_pointer_map(temp_position);

                    temp_property_offsets.push_back(property_offset);

                    temp_property_pointer_offsets.push_back(property_pointer_offset);

                    property_offset += 0x10;
                    check_pointer_map(property_offset);

                    bool shared_value_found = false;

                    std::map<uint32_t, uint32_t>::iterator it3 = temp_property_types_offsets_map.at(property_type_index).find(property_offset);

                    if (it3 != temp_property_types_offsets_map.at(property_type_index).end())
                    {
                        shared_value_found = true;

                        temp_property_types_shared.at(property_type_index).at(it3->second) = true;
                        temp_property_types_shared_count.at(property_type_index).at(it3->second)++;
                    }
                    else
                    {
                        temp_property_types_shared.at(property_type_index).push_back(false);
                        temp_property_types_shared_count.at(property_type_index).push_back((uint32_t)1);
                        temp_property_types_offsets.at(property_type_index).push_back(property_offset);
                        temp_property_types_values.at(property_type_index).push_back(std::string(""));
                        temp_property_types_offsets_map.at(property_type_index)[property_offset] = temp_property_types_offsets_map.at(property_type_index).size();
                    }

                    std::string property_string = "";

                    std::map<uint32_t, std::string>::iterator it2 = property_map->find(property_crc32_value);

                    if (it2 != property_map->end())
                    {
                        property_string = it2->second;

                        //LOG("  property_crc32_value: " + util::uint32_t_to_hex_string(property_crc32_value) + " (" + property_string + ")");
                        //LOG("  property_type_index: " + util::uint32_t_to_hex_string(property_type_index));
                        //LOG("  property_offset: " + util::uint32_t_to_hex_string(property_offset));
                        //LOG("  property_pointer_offset: " + util::uint32_t_to_hex_string(property_pointer_offset));
                    }
                    else
                    {
                        //LOG("  property_crc32_value: " + util::uint32_t_to_hex_string(property_crc32_value));
                        //LOG("  property_type_index: " + util::uint32_t_to_hex_string(property_type_index));
                        //LOG("  property_offset: " + util::uint32_t_to_hex_string(property_offset));
                        //LOG("  property_pointer_offset: " + util::uint32_t_to_hex_string(property_pointer_offset));
                    }

                    LOG("    - " + property_string + "'s type: " + temp_property_types.at(property_type_index));

                    std::map<std::string, std::map<uint32_t, std::string>>::iterator it = enum_map->find(temp_property_types.at(property_type_index));

                    if (it != enum_map->end())
                    {
                        std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint32_t_to_string(bytes4);
                        }

                        //LOG("    - enum: " + util::uint32_t_to_string(bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "bool")
                    {
                        std::memcpy(&bytes1, &temp_data.data()[property_offset], sizeof(bytes1));
                        property_offset += 0x1;
                        check_pointer_map(property_offset);

                        if (bytes1 == 0)
                        {
                            if (!shared_value_found)
                            {
                                temp_property_types_values.at(property_type_index).back() = "False";
                            }
                        }
                        else if (bytes1 == 1)
                        {
                            if (!shared_value_found)
                            {
                                temp_property_types_values.at(property_type_index).back() = "True";
                            }
                        }

                        //LOG("    - bool: " + util::uint8_t_to_string((bool)bytes1));
                    }
                    else if (temp_property_types.at(property_type_index) == "uint8")
                    {
                        std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint8_t_to_string((uint8_t)bytes4);
                        }

                        //LOG("    - uint8: " + util::uint8_t_to_string((uint8_t)bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "int32")
                    {
                        int32_t temp_int32 = 0;

                        std::memcpy(&temp_int32, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::int32_t_to_string(temp_int32);
                        }

                        //LOG("    - int32: " + util::int32_t_to_string(temp_int32));
                    }
                    else if (temp_property_types.at(property_type_index) == "uint32")
                    {
                        std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint32_t_to_string(bytes4);
                        }

                        //LOG("    - uint32: " + util::uint32_t_to_string(bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "SEntityTemplateReference")
                    {
                        std::memcpy(&bytes8, &temp_data.data()[property_offset], sizeof(bytes8));
                        property_offset += 0x8;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint64_t_to_string(bytes8);
                        }

                        //LOG("    - uint32: " + util::uint32_t_to_string(bytes4));
                    }
                    else if (temp_property_types.at(property_type_index) == "float32")
                    {
                        float temp_float = 0;

                        std::memcpy(&temp_float, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::float_to_string(temp_float);
                        }

                        //LOG("    - float32: " + util::float_to_string(temp_float));
                    }
                    else if (temp_property_types.at(property_type_index) == "SMatrix43")
                    {
                        matrix43 temp_matrix43;

                        std::memcpy(&temp_matrix43.x_axis.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.x_axis.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.x_axis.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        std::memcpy(&temp_matrix43.y_axis.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.y_axis.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.y_axis.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        std::memcpy(&temp_matrix43.z_axis.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.z_axis.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.z_axis.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        std::memcpy(&temp_matrix43.transform.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.transform.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_matrix43.transform.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.x_axis.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.x_axis.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.x_axis.z));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.y_axis.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.y_axis.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.y_axis.z));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.z_axis.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.z_axis.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.z_axis.z));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.transform.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.transform.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_matrix43.transform.z));
                        }

                        //LOG("    - SMatrix43: ");
                        //LOG("      - X Axis: " + util::float_to_string(temp_matrix43.x_axis.x) + ", " + util::float_to_string(temp_matrix43.x_axis.y) + ", " + util::float_to_string(temp_matrix43.x_axis.z));
                        //LOG("      - Y Axis: " + util::float_to_string(temp_matrix43.y_axis.x) + ", " + util::float_to_string(temp_matrix43.y_axis.y) + ", " + util::float_to_string(temp_matrix43.y_axis.z));
                        //LOG("      - Z Axis: " + util::float_to_string(temp_matrix43.z_axis.x) + ", " + util::float_to_string(temp_matrix43.z_axis.y) + ", " + util::float_to_string(temp_matrix43.z_axis.z));
                        //LOG("      - Transform: " + util::float_to_string(temp_matrix43.transform.x) + ", " + util::float_to_string(temp_matrix43.transform.y) + ", " + util::float_to_string(temp_matrix43.transform.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "SVector2")
                    {
                        vector2 temp_vector2;

                        std::memcpy(&temp_vector2.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector2.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector2.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector2.y));
                        }

                        //LOG("    - SVector2: " + util::float_to_string(temp_vector2.x) + ", " + util::float_to_string(temp_vector2.y));
                    }
                    else if (temp_property_types.at(property_type_index) == "SVector3")
                    {
                        vector3 temp_vector3;

                        std::memcpy(&temp_vector3.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector3.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector3.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.z));
                        }

                        //LOG("    - SVector3: " + util::float_to_string(temp_vector3.x) + ", " + util::float_to_string(temp_vector3.y) + ", " + util::float_to_string(temp_vector3.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "SVector4")
                    {
                        vector4 temp_vector4;

                        std::memcpy(&temp_vector4.w, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector4.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector4.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector4.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.w));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector4.z));
                        }

                        //LOG("    - SVector4: " + util::float_to_string(temp_vector4.w) + ", " + util::float_to_string(temp_vector4.x) + ", " + util::float_to_string(temp_vector4.y) + ", " + util::float_to_string(temp_vector4.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "ZGuid")
                    {
                        uint32_t temp_ZGuid1;
                        uint16_t temp_ZGuid2;
                        uint16_t temp_ZGuid3;
                        uint16_t temp_ZGuid4;
                        uint16_t temp_ZGuid5;
                        uint32_t temp_ZGuid6;
                        char char2[2];
                        char char4[4];

                        std::memcpy(&temp_ZGuid1, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_ZGuid2, &temp_data.data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_ZGuid3, &temp_data.data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        check_pointer_map(property_offset);
                        std::memcpy(&input, &temp_data.data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        check_pointer_map(property_offset);
                        char2[0] = input[1];
                        char2[1] = input[0];
                        std::memcpy(&temp_ZGuid4, &char2, sizeof(bytes2));
                        std::memcpy(&input, &temp_data.data()[property_offset], sizeof(bytes2));
                        property_offset += 0x2;
                        check_pointer_map(property_offset);
                        char2[0] = input[1];
                        char2[1] = input[0];
                        std::memcpy(&temp_ZGuid5, &char2, sizeof(bytes2));
                        std::memcpy(&input, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        char4[0] = input[3];
                        char4[1] = input[2];
                        char4[2] = input[1];
                        char4[3] = input[0];
                        std::memcpy(&temp_ZGuid6, &char4, sizeof(bytes4));

                        std::string guid_string = util::uint32_t_to_hex_string(temp_ZGuid1);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid2);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid3);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid4);
                        guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid5);
                        guid_string += util::uint32_t_to_hex_string(temp_ZGuid6);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = guid_string;
                        }

                        //LOG("    - ZGuid: " + guid_string);
                    }
                    else if (temp_property_types.at(property_type_index) == "ZRuntimeResourceID")
                    {
                        uint64_t temp_ZRuntimeResourceID;

                        std::memcpy(&temp_ZRuntimeResourceID, &temp_data.data()[property_offset], sizeof(bytes8));
                        property_offset += 0x8;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = util::uint64_t_to_hex_string(temp_ZRuntimeResourceID);
                        }

                        //LOG("    - ZRuntimeResourceID: " + util::uint64_t_to_hex_string(temp_ZRuntimeResourceID));
                    }
                    else if (temp_property_types.at(property_type_index) == "SColorRGB")
                    {
                        vector3 temp_vector3;

                        std::memcpy(&temp_vector3.x, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector3.y, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);
                        std::memcpy(&temp_vector3.z, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.x));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.y));
                            temp_property_types_values.at(property_type_index).back().push_back(',');
                            temp_property_types_values.at(property_type_index).back().append(util::float_to_string(temp_vector3.z));
                        }

                        //LOG("    - SColorRGB: " + util::float_to_string(temp_vector3.x) + ", " + util::float_to_string(temp_vector3.y) + ", " + util::float_to_string(temp_vector3.z));
                    }
                    else if (temp_property_types.at(property_type_index) == "ZString")
                    {
                        property_offset += 0x10;
                        check_pointer_map(property_offset);

                        std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                        property_offset += 0x4;
                        check_pointer_map(property_offset);

                        std::memcpy(&input, &temp_data.data()[property_offset], bytes4);
                        property_offset += bytes4;
                        check_pointer_map(property_offset);

                        if (!shared_value_found)
                        {
                            temp_property_types_values.at(property_type_index).back() = std::string(input);
                        }

                        //LOG("    - ZString: " + std::string(input));
                    }
                }

                start_offset = temp_postInitPropertyValues_start_offsets.at(e);
                end_offset = temp_postInitPropertyValues_end_offsets.at(e);
                start_offset += 0x10;
                end_offset += 0x10;

                if (temp_file_version == 1)
                {
                    start_offset = temp_platformSpecificPropertyValues_start_offsets.at(e);
                    end_offset = temp_platformSpecificPropertyValues_end_offsets.at(e);
                    start_offset += 0x10;
                    end_offset += 0x10;
                }
            }

            property_crc32_values.push_back(temp_property_crc32_values);
            property_type_indexes.push_back(temp_property_type_indexes);
            property_offsets.push_back(temp_property_offsets);
            property_pointer_offsets.push_back(temp_property_pointer_offsets);
        }

        for (uint32_t e = 0; e < property_types_count; e++)
        {
            //LOG("temp_property_type: " + temp_property_types.at(e));

            for (uint32_t p = 0; p < temp_property_types_offsets.at(e).size(); p++)
            {
                //LOG("  - temp_property_types_offsets: " + util::uint32_t_to_hex_string(temp_property_types_offsets.at(e).at(p)));
                //LOG("  - temp_property_types_values: " + temp_property_types_values.at(e).at(p));
                //LOG("  - temp_property_types_shared: " + util::uint32_t_to_string(temp_property_types_shared.at(e).at(p)));
                //LOG("  - temp_property_types_shared_count: " + util::uint32_t_to_string(temp_property_types_shared_count.at(e).at(p)));
            }
        }
    }
    else
    {
        LOG("Error: TEMP_HEADER_NOT_FOUND");

        tblu_return_value = TEMP_HEADER_NOT_FOUND;

        return;
    }
}

void temp::get_entries_with_logical_parent(uint32_t logical_parent)
{
    response_string = "";

    for (uint32_t e = 0; e < tblu_logicalParent.size(); e++)
    {
        if (tblu_logicalParent.at(e) == logical_parent)
        {
            response_string.append(util::uint32_t_to_string(tblu_entry_index.at(e)));
            response_string.push_back('|');
            response_string.append(tblu_entityName.at(e));
            response_string.push_back(',');
        }
    }
}

void temp::get_entries_data(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < temp_entry_index.size(); e++)
    {
        if (temp_entry_index.at(e) == entry_index)
        {
            for (uint32_t p = 0; p < property_crc32_values.at(e).size(); p++)
            {
                response_string.append(util::uint32_t_to_string(property_crc32_values.at(e).at(p)));
                response_string.push_back('|');

                std::string property_string = "";

                std::map<uint32_t, std::string>::iterator it = property_map->find(property_crc32_values.at(e).at(p));

                if (it != property_map->end())
                {
                    property_string = it->second;
                }

                response_string.append(property_string);
                response_string.push_back('|');
                response_string.append(temp_property_types.at(property_type_indexes.at(e).at(p)));
                response_string.push_back('|');
                response_string.append(util::uint32_t_to_string(property_type_indexes.at(e).at(p)));
                response_string.push_back('|');
                response_string.append(util::uint32_t_to_string(p));
                response_string.push_back('|');

                uint32_t property_type_index = property_type_indexes.at(e).at(p);
                uint32_t property_offset = property_offsets.at(e).at(p);

                property_offset += 0x10;
                check_pointer_map(property_offset);

                response_string.append(util::uint32_t_to_string(property_offset));
                response_string.push_back('|');

                if (temp_logicalParent.at(e) < temp_entry_index.size())
                {
                    response_string.append(tblu_entityName.at(temp_logicalParent.at(e)));
                    response_string.push_back('|');
                }
                else
                {
                    response_string.append(util::uint32_t_to_hex_string(temp_logicalParent.at(e)));
                    response_string.push_back('|');
                }

                char input[1024];
                uint8_t bytes1 = 0;
                uint16_t bytes2 = 0;
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;

                std::map<std::string, std::map<uint32_t, std::string>>::iterator it2 = enum_map->find(temp_property_types.at(property_type_index));

                if (it2 != enum_map->end())
                {
                    std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::uint32_t_to_string((uint32_t)bytes4));
                }
                else if (temp_property_types.at(property_type_index) == "bool")
                {
                    std::memcpy(&bytes1, &temp_data.data()[property_offset], sizeof(bytes1));
                    property_offset += 0x1;
                    check_pointer_map(property_offset);

                    response_string.append(util::uint8_t_to_string((uint8_t)((bool)bytes1)));
                }
                else if (temp_property_types.at(property_type_index) == "uint8")
                {
                    std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::uint32_t_to_string(bytes4));
                }
                else if (temp_property_types.at(property_type_index) == "int32")
                {
                    int32_t temp_int32 = 0;

                    std::memcpy(&temp_int32, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::int32_t_to_string(temp_int32));
                }
                else if (temp_property_types.at(property_type_index) == "uint32")
                {
                    std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::uint32_t_to_string(bytes4));
                }
                else if (temp_property_types.at(property_type_index) == "float32")
                {
                    float temp_float = 0;

                    std::memcpy(&temp_float, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::float_to_string(temp_float));
                }
                else if (temp_property_types.at(property_type_index) == "SMatrix43")
                {
                    matrix43 temp_matrix43;

                    std::memcpy(&temp_matrix43.x_axis.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.x_axis.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.x_axis.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    std::memcpy(&temp_matrix43.y_axis.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.y_axis.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.y_axis.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    std::memcpy(&temp_matrix43.z_axis.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.z_axis.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.z_axis.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    std::memcpy(&temp_matrix43.transform.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.transform.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_matrix43.transform.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::float_to_string(temp_matrix43.x_axis.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.x_axis.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.x_axis.z));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.y_axis.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.y_axis.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.y_axis.z));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.z_axis.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.z_axis.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.z_axis.z));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.transform.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.transform.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_matrix43.transform.z));
                }
                else if (temp_property_types.at(property_type_index) == "SVector2")
                {
                    vector2 temp_vector2;

                    std::memcpy(&temp_vector2.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector2.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::float_to_string(temp_vector2.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector2.y));
                }
                else if (temp_property_types.at(property_type_index) == "SVector3")
                {
                    vector3 temp_vector3;

                    std::memcpy(&temp_vector3.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector3.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector3.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::float_to_string(temp_vector3.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector3.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector3.z));
                }
                else if (temp_property_types.at(property_type_index) == "SVector4")
                {
                    vector4 temp_vector4;

                    std::memcpy(&temp_vector4.w, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector4.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector4.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector4.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::float_to_string(temp_vector4.w));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector4.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector4.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector4.z));
                }
                else if (temp_property_types.at(property_type_index) == "ZGuid")
                {
                    uint32_t temp_ZGuid1;
                    uint16_t temp_ZGuid2;
                    uint16_t temp_ZGuid3;
                    uint16_t temp_ZGuid4;
                    uint16_t temp_ZGuid5;
                    uint32_t temp_ZGuid6;
                    char char2[2];
                    char char4[4];

                    std::memcpy(&temp_ZGuid1, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_ZGuid2, &temp_data.data()[property_offset], sizeof(bytes2));
                    property_offset += 0x2;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_ZGuid3, &temp_data.data()[property_offset], sizeof(bytes2));
                    property_offset += 0x2;
                    check_pointer_map(property_offset);
                    std::memcpy(&input, &temp_data.data()[property_offset], sizeof(bytes2));
                    property_offset += 0x2;
                    check_pointer_map(property_offset);
                    char2[0] = input[1];
                    char2[1] = input[0];
                    std::memcpy(&temp_ZGuid4, &char2, sizeof(bytes2));
                    std::memcpy(&input, &temp_data.data()[property_offset], sizeof(bytes2));
                    property_offset += 0x2;
                    check_pointer_map(property_offset);
                    char2[0] = input[1];
                    char2[1] = input[0];
                    std::memcpy(&temp_ZGuid5, &char2, sizeof(bytes2));
                    std::memcpy(&input, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    char4[0] = input[3];
                    char4[1] = input[2];
                    char4[2] = input[1];
                    char4[3] = input[0];
                    std::memcpy(&temp_ZGuid6, &char4, sizeof(bytes4));

                    std::string guid_string = util::uint32_t_to_hex_string(temp_ZGuid1);
                    guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid2);
                    guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid3);
                    guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid4);
                    guid_string += "-" + util::uint16_t_to_hex_string(temp_ZGuid5);
                    guid_string += util::uint32_t_to_hex_string(temp_ZGuid6);

                    response_string.append(guid_string);
                }
                else if (temp_property_types.at(property_type_index) == "ZRuntimeResourceID")
                {
                    uint64_t temp_ZRuntimeResourceID;

                    std::memcpy(&temp_ZRuntimeResourceID, &temp_data.data()[property_offset], sizeof(bytes8));
                    property_offset += 0x8;
                    check_pointer_map(property_offset);

                    response_string.append(util::uint64_t_to_hex_string(temp_ZRuntimeResourceID));
                }
                else if (temp_property_types.at(property_type_index) == "SColorRGB")
                {
                    vector3 temp_vector3;

                    std::memcpy(&temp_vector3.x, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector3.y, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);
                    std::memcpy(&temp_vector3.z, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    response_string.append(util::float_to_string(temp_vector3.x));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector3.y));
                    response_string.push_back('!');
                    response_string.append(util::float_to_string(temp_vector3.z));
                }
                else if (temp_property_types.at(property_type_index) == "ZString")
                {
                    property_offset += 0x10;
                    check_pointer_map(property_offset);

                    std::memcpy(&bytes4, &temp_data.data()[property_offset], sizeof(bytes4));
                    property_offset += 0x4;
                    check_pointer_map(property_offset);

                    std::memcpy(&input, &temp_data.data()[property_offset], bytes4);
                    property_offset += bytes4;
                    check_pointer_map(property_offset);

                    std::string zstring = std::string(input);
                    std::string to_replace = ",";
                    std::string replace_with = "@";

                    util::replace_all_string_in_string(zstring, to_replace, replace_with);

                    response_string.append(zstring);
                }

                response_string.push_back(',');
            }
        }
    }
}

void temp::get_entries_hash_reference_data(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < temp_entry_index.size(); e++)
    {
        if (temp_entry_index.at(e) == entry_index)
        {
            response_string.append("  - ");

            std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
                response_string.push_back(' ');
                response_string.append(hash_list_hash_strings.at(it->second));
            }
            else
            {
                response_string.append(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_string.at(temp_entityTypeResourceIndex.at(e)));
            }

            response_string.append("\n  - ");

            it = hash_list_hash_map.find(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
                response_string.push_back(' ');
                response_string.append(hash_list_hash_strings.at(it->second));
            }
            else
            {
                response_string.append(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference_string.at(tblu_entityTypeResourceIndex.at(e)));
            }

        }
    }
}

void temp::get_entries_hash_references(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < temp_entry_index.size(); e++)
    {
        if (temp_entry_index.at(e) == entry_index)
        {
            std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
            }
            else
            {
                response_string.append(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference_string.at(temp_entityTypeResourceIndex.at(e)));
            }

            response_string.append(",");

            it = hash_list_hash_map.find(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference.at(tblu_entityTypeResourceIndex.at(e)));

            if (it != hash_list_hash_map.end())
            {
                response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it->second)));
            }
            else
            {
                response_string.append(rpkgs.at(tblu_rpkg_index).hash.at(tblu_hash_index).hash_reference_data.hash_reference_string.at(tblu_entityTypeResourceIndex.at(e)));
            }

        }
    }
}

int temp::update_temp_file(std::string& offset_string, std::string& type_string, std::string& value_string)
{
    uint32_t offset_value = std::strtoul(offset_string.c_str(), nullptr, 10);

    if (type_string == "enum")
    {
        uint32_t uint32_temp = std::strtoul(value_string.c_str(), nullptr, 10);

        uint32_t uint32_temp_existing = 0;
        std::memcpy(&uint32_temp_existing, &temp_data.data()[offset_value], 0x4);

        if (uint32_temp != uint32_temp_existing)
        {
            std::memcpy(&temp_data.data()[offset_value], &uint32_temp, 0x4);

            file_has_been_changed = true;
        }
    }
    else if (type_string == "bool")
    {
        uint8_t uint8_temp = 0x0;

        if (value_string == "True")
        {
            uint8_temp = 0x1;
        }
        else if (value_string == "False")
        {
            uint8_temp = 0x0;
        }

        uint8_t uint8_temp_existing = 0;
        std::memcpy(&uint8_temp_existing, &temp_data.data()[offset_value], 0x1);

        if (uint8_temp != uint8_temp_existing)
        {
            std::memcpy(&temp_data.data()[offset_value], &uint8_temp, 0x1);

            file_has_been_changed = true;
        }
    }
    else if (type_string == "uint8")
    {
        uint32_t uint32_temp = std::strtoul(value_string.c_str(), nullptr, 10);

        uint32_t uint32_temp_existing = 0;
        std::memcpy(&uint32_temp_existing, &temp_data.data()[offset_value], 0x4);

        if (uint32_temp != uint32_temp_existing)
        {
            std::memcpy(&temp_data.data()[offset_value], &uint32_temp, 0x4);

            file_has_been_changed = true;
        }
    }
    else if (type_string == "int32")
    {
        uint32_t uint32_temp = std::strtoul(value_string.c_str(), nullptr, 10);

        int32_t int32_temp = (int32_t)uint32_temp;

        int32_t int32_temp_existing = 0;
        std::memcpy(&int32_temp_existing, &temp_data.data()[offset_value], 0x4);

        if (int32_temp != int32_temp_existing)
        {
            std::memcpy(&temp_data.data()[offset_value], &int32_temp, 0x4);

            file_has_been_changed = true;
        }
    }
    else if (type_string == "uint32")
    {
        uint32_t uint32_temp = std::strtoul(value_string.c_str(), nullptr, 10);

        uint32_t uint32_temp_existing = 0;
        std::memcpy(&uint32_temp_existing, &temp_data.data()[offset_value], 0x4);

        if (uint32_temp != uint32_temp_existing)
        {
            std::memcpy(&temp_data.data()[offset_value], &uint32_temp, 0x4);

            file_has_been_changed = true;
        }
    }
    else if (type_string == "float32")
    {
        float tolerance = 1E-5;

        float temp_float = std::strtof(value_string.c_str(), nullptr);

        float temp_float_existing = 0;
        std::memcpy(&temp_float_existing, &temp_data.data()[offset_value], 0x4);

        float temp_float_abs = std::abs(temp_float);
        float temp_float_existing_abs = std::abs(temp_float_existing);
        float difference = 0;

        if (temp_float_abs > temp_float_existing_abs)
        {
            difference = temp_float_abs * tolerance;
        }
        else
        {
            difference = temp_float_existing_abs * tolerance;
        }

        if (std::abs(temp_float - temp_float_existing) > difference)
        {
            std::memcpy(&temp_data.data()[offset_value], &temp_float, 0x4);

            file_has_been_changed = true;
        }
    }

    return 0;
}

int temp::generate_temp_file_from_data(std::string& temp_path)
{
    std::ofstream temp_file = std::ofstream(temp_path, std::ofstream::binary);

    if (!temp_file.good())
    {
        return 1;
    }

    temp_file.write(temp_data.data(), temp_data.size());

    temp_file.close();

    rpkg_function::extract_hash_meta(temp_rpkg_index, temp_hash_index, temp_path);

    return 0;
}

int temp::is_offset_shared(std::string& offset_string, std::string& property_type_index_string)
{
    uint32_t offset_value = std::strtoul(offset_string.c_str(), nullptr, 10);
    uint32_t property_type_index_value = std::strtoul(property_type_index_string.c_str(), nullptr, 10);

    std::map<uint32_t, uint32_t>::iterator it = temp_property_types_offsets_map.at(property_type_index_value).find(offset_value);

    if (it != temp_property_types_offsets_map.at(property_type_index_value).end())
    {
        if (temp_property_types_shared.at(property_type_index_value).at(it->second))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    return 2;
}

void temp::get_all_shared_values(std::string& offset_string, std::string& property_type_index_string)
{
    response_string = "";

    uint32_t offset_value = std::strtoul(offset_string.c_str(), nullptr, 10);
    uint32_t property_type_index_value = std::strtoul(property_type_index_string.c_str(), nullptr, 10);

    for (uint32_t p = 0; p < temp_property_types_offsets.at(property_type_index_value).size(); p++)
    {
        if (temp_property_types_shared.at(property_type_index_value).at(p))
        {
            response_string.append(util::uint32_t_to_string(temp_property_types_offsets.at(property_type_index_value).at(p)));
            response_string.push_back('|');
            response_string.append(temp_property_types_values.at(property_type_index_value).at(p));
            response_string.push_back(',');
        }
    }
}

int temp::get_shared_index(std::string& offset_string, std::string& property_type_index_string)
{
    response_string = "";

    uint32_t offset_value = std::strtoul(offset_string.c_str(), nullptr, 10);
    uint32_t property_type_index_value = std::strtoul(property_type_index_string.c_str(), nullptr, 10);

    int index = 0;

    for (uint32_t p = 0; p < temp_property_types_offsets.at(property_type_index_value).size(); p++)
    {
        if (temp_property_types_shared.at(property_type_index_value).at(p))
        {
            if (temp_property_types_offsets.at(property_type_index_value).at(p) == offset_value)
            {
                return index;
            }

            index++;
        }
    }

    return 0;
}

int temp::get_shared_count(std::string& offset_string, std::string& property_type_index_string)
{
    response_string = "";

    uint32_t offset_value = std::strtoul(offset_string.c_str(), nullptr, 10);
    uint32_t property_type_index_value = std::strtoul(property_type_index_string.c_str(), nullptr, 10);

    for (uint32_t p = 0; p < temp_property_types_offsets.at(property_type_index_value).size(); p++)
    {
        if (temp_property_types_shared.at(property_type_index_value).at(p))
        {
            if (temp_property_types_offsets.at(property_type_index_value).at(p) == offset_value)
            {
                return temp_property_types_shared_count.at(property_type_index_value).at(p);
            }
        }
    }

    return 0;
}

int temp::update_temp_file_pointer(std::string& entry_index_string, std::string& property_index_string, std::string& offset_string)
{
    uint32_t entry_index_value = std::strtoul(entry_index_string.c_str(), nullptr, 10);
    uint32_t property_index_value = std::strtoul(property_index_string.c_str(), nullptr, 10);
    uint32_t offset_value = std::strtoul(offset_string.c_str(), nullptr, 16);

    offset_value -= 0x10;

    for (uint32_t e = 0; e < temp_entry_index.size(); e++)
    {
        if (temp_entry_index.at(e) == entry_index_value)
        {
            property_offsets.at(e).at(property_index_value) = offset_value;

            uint32_t offset_value_existing = 0;
            std::memcpy(&offset_value_existing, &temp_data.data()[property_pointer_offsets.at(e).at(property_index_value)], 0x4);

            if (offset_value != offset_value_existing)
            {
                std::memcpy(&temp_data.data()[property_pointer_offsets.at(e).at(property_index_value)], &offset_value, 0x4);

                file_has_been_changed = true;
            }
        }
    }

    return 0;
}

void temp::get_enum_values(std::string& property_type_string)
{
    response_string = "";

    std::map<std::string, std::map<uint32_t, std::string>>::iterator it = enum_map->find(property_type_string);

    if (it != enum_map->end())
    {
        for (std::map<uint32_t, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            response_string.append(util::uint32_t_to_string(it2->first));
            response_string.push_back('|');
            response_string.append(it2->second);
            response_string.push_back(',');
        }
    }
}

void temp::get_prim_from_temp(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < temp_entry_index.size(); e++)
    {
        if (temp_entry_index.at(e) == entry_index)
        {
            std::map<uint64_t, uint32_t>::iterator it = temps_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_entityTypeResourceIndex.at(e)));

            if (it != temps_map.end())
            {
                for (uint64_t k = 0; k < temps.at(it->second).prim_depends_file_name.size(); k++)
                {
                    if (temps.at(it->second).prim_depends_in_rpkgs.at(k).size() > 0)
                    {
                        response_string.append(temps.at(it->second).prim_depends_file_name.at(k));
                        response_string.push_back('|');
                        response_string.append(temps.at(it->second).prim_depends_in_rpkgs.at(k).at(temps.at(it->second).prim_depends_in_rpkgs_index.at(k)));
                        response_string.push_back(',');
                    }
                }
            }
        }
    }
}

void temp::get_entry_name_string(uint32_t entry_index)
{
    response_string = "";

    for (uint32_t e = 0; e < temp_entry_index.size(); e++)
    {
        if (temp_entry_index.at(e) == entry_index)
        {
            response_string.append(tblu_entityName.at(e));
        }
    }
}

void temp::get_top_level_logical_parents()
{
    response_string = "";

    std::set<std::string> logical_parents_set;

    for (uint32_t e = 0; e < tblu_logicalParent.size(); e++)
    {
        if (tblu_logicalParent.at(e) >= tblu_logicalParent.size())
        {
            logical_parents_set.insert(util::uint32_t_to_string(tblu_logicalParent.at(e)));
        }
    }

    for (std::set<std::string>::iterator it = logical_parents_set.begin(); it != logical_parents_set.end(); it++)
    {
        response_string.append(*it);
        response_string.push_back(',');
    }
}

void temp::get_all_bricks()
{
    response_string = "";

    if (tblu_return_value == TEMP_TBLU_FOUND)
    {
        for (uint32_t i = 0; i < temp_depends_file_name.size(); i++)
        {
            if (temp_depends_index.at(i) > tblu_depends_index.at(0))
            {
                uint64_t temp_hash_value = std::strtoull(temp_depends_file_name.at(i).c_str(), nullptr, 16);

                std::map<uint64_t, uint32_t>::iterator it = temps_map.find(temp_hash_value);

                if (it != temps_map.end())
                {
                    if (temps.at(it->second).tblu_return_value == TEMP_TBLU_FOUND)
                    {
                        std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(rpkgs.at(temp_rpkg_index).hash.at(temp_hash_index).hash_reference_data.hash_reference.at(temp_depends_index.at(i)));

                        if (it2 != hash_list_hash_map.end())
                        {
                            response_string.append(util::to_upper_case(hash_list_hash_file_names.at(it2->second)));
                            response_string.push_back(' ');
                            response_string.append(hash_list_hash_strings.at(it2->second));
                            response_string.push_back(',');
                        }
                        else
                        {
                            response_string.append(temp_depends_file_name.at(i));
                            response_string.push_back(' ');
                            response_string.push_back(',');
                        }
                    }
                }
            }
        }
    }
}

void temp::check_pointer_map(uint32_t position)
{
    position -= 0x10;

    std::map<uint32_t, uint32_t>::iterator it = pointers_map.find(position);

    if (it != pointers_map.end())
    {
        std::cout << "Current position: " << position << ", found in pointer map: " << util::uint32_t_to_hex_string(it->first) << ", " << it->second << std::endl;
    }
}