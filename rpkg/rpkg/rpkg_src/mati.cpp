#include "mati.h"
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

mati::mati()
{

}

mati::mati(uint64_t rpkgs_index, uint64_t hash_index)
{
    mati_rpkg_index = rpkgs_index;
    mati_hash_index = hash_index;

    mati_file_name = rpkgs.at(rpkgs_index).hash.at(hash_index).hash_file_name;

    load_hash_depends();

    uint64_t mati_hash_size;

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).is_lz4ed == 1)
    {
        mati_hash_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_size;

        if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).is_xored == 1)
        {
            mati_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        mati_hash_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_size_final;
    }

    mati_input_data = std::vector<char>(mati_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(mati_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(mati_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_offset, file.beg);
    file.read(mati_input_data.data(), mati_hash_size);
    file.close();

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).is_xored == 1)
    {
        crypto::xor_data(mati_input_data.data(), (uint32_t)mati_hash_size);
    }

    uint32_t mati_decompressed_size = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_size_final;

    mati_output_data = std::vector<char>(mati_decompressed_size, 0);

    if (rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).is_lz4ed)
    {
        LZ4_decompress_safe(mati_input_data.data(), mati_output_data.data(), (int)mati_hash_size, mati_decompressed_size);

        mati_data = mati_output_data;
    }
    else
    {
        mati_data = mati_input_data;
    }

    std::vector<char>().swap(mati_output_data);
    std::vector<char>().swap(mati_input_data);
}

void mati::load_hash_depends()
{
    uint32_t mati_hash_reference_count = rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    if (mati_hash_reference_count > 0)
    {
        for (uint64_t i = 0; i < mati_hash_reference_count; i++)
        {
            std::vector<std::string> prim_text_depends_in_rpkgs;
            std::vector<uint32_t> prim_text_depends_rpkg_index;
            std::vector<uint32_t> prim_text_depends_hash_index;

            std::vector<std::string> prim_mate_depends_in_rpkgs;
            std::vector<uint32_t> prim_mate_depends_rpkg_index;
            std::vector<uint32_t> prim_mate_depends_hash_index;

            bool text_found = false;
            bool mate_found = false;

            for (uint64_t j = 0; j < rpkgs.size(); j++)
            {
                std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(j).hash_map.find(rpkgs.at(mati_rpkg_index).hash.at(mati_hash_index).hash_reference_data.hash_reference.at(i));

                if (it != rpkgs.at(j).hash_map.end())
                {
                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "TEXT")
                    {
                        if (!text_found)
                        {
                            text_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            text_depends_index.push_back(i);

                            //LOG("Found TEXT hash depend:");
                            //LOG("  - File Name: " + text_depends_file_name.back());
                        }

                        text_found = true;

                        prim_text_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        prim_text_depends_rpkg_index.push_back(j);
                        prim_text_depends_hash_index.push_back(it->second);

                        //LOG("  - In RPKG: " + prim_text_depends_in_rpkgs.back());
                        //LOG("  - RPKG Index: " + util::uint64_t_to_string(prim_text_depends_rpkg_index.back()));
                        //LOG("  - Hash Index: " + util::uint64_t_to_string(prim_text_depends_hash_index.back()));
                    }

                    if (rpkgs.at(j).hash.at(it->second).hash_resource_type == "MATE")
                    {
                        if (!mate_found)
                        {
                            mate_depends_file_name.push_back(rpkgs.at(j).hash.at(it->second).hash_file_name);

                            mate_depends_index.push_back(i);

                            //LOG("Found MATE hash depend:");
                            //LOG("  - File Name: " + mate_depends_file_name.back());
                        }

                        mate_found = true;

                        prim_mate_depends_in_rpkgs.push_back(rpkgs.at(j).rpkg_file_path);
                        prim_mate_depends_rpkg_index.push_back(j);
                        prim_mate_depends_hash_index.push_back(it->second);

                        //LOG("  - In RPKG: " + prim_mate_depends_in_rpkgs.back());
                        //LOG("  - RPKG Index: " + util::uint64_t_to_string(prim_mate_depends_rpkg_index.back()));
                        //LOG("  - Hash Index: " + util::uint64_t_to_string(prim_mate_depends_hash_index.back()));
                    }
                }
            }

            uint32_t text_value = 0;

            if (text_found)
            {
                text_depends_in_rpkgs.push_back(prim_text_depends_in_rpkgs);
                text_depends_rpkg_index.push_back(prim_text_depends_rpkg_index);
                text_depends_hash_index.push_back(prim_text_depends_hash_index);
                text_depends_in_rpkgs_index.push_back(text_value);
                text_depends_rpkg_index_index.push_back(text_value);
                text_depends_hash_index_index.push_back(text_value);
            }

            if (mate_found)
            {
                mate_depends_in_rpkgs.push_back(prim_mate_depends_in_rpkgs);
                mate_depends_rpkg_index.push_back(prim_mate_depends_rpkg_index);
                mate_depends_hash_index.push_back(prim_mate_depends_hash_index);
                mate_depends_in_rpkgs_index.push_back(text_value);
                mate_depends_rpkg_index_index.push_back(text_value);
                mate_depends_hash_index_index.push_back(text_value);
            }
        }
    }

    for (uint64_t k = 0; k < text_depends_file_name.size(); k++)
    {
        //LOG("  - MATI File Name: " + text_depends_file_name.at(k));

        bool text_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < text_depends_in_rpkgs.at(k).size(); d++)
        {
            std::string text_depends_in_rpkgs_upper_case = util::to_upper_case(text_depends_in_rpkgs.at(k).at(d));

            std::string_view text_depends_in_rpkgs_string_view(text_depends_in_rpkgs_upper_case.c_str(), text_depends_in_rpkgs_upper_case.length());

            size_t pos1 = text_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos)
            {
                size_t pos2 = text_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos)
                {
                    text_patch_name_found = true;

                    long new_patch_level = std::strtol(std::string(text_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level)
                    {
                        patch_level = new_patch_level;

                        text_depends_in_rpkgs_index.at(k) = d;
                        text_depends_rpkg_index_index.at(k) = d;
                        text_depends_hash_index_index.at(k) = d;
                    }
                }
            }
            else
            {
                if (!text_patch_name_found)
                {
                    text_depends_in_rpkgs_index.at(k) = d;
                    text_depends_rpkg_index_index.at(k) = d;
                    text_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - MATI File Name In RPKG: " + text_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - MATI File Name In RPKG Used: " + text_depends_in_rpkgs.at(k).at(text_depends_rpkg_index_index.at(k)));
    }

    for (uint64_t k = 0; k < mate_depends_file_name.size(); k++)
    {
        //LOG("  - MATE File Name: " + mate_depends_file_name.at(k));

        bool mate_patch_name_found = false;

        long patch_level = 0;

        for (uint64_t d = 0; d < mate_depends_in_rpkgs.at(k).size(); d++)
        {
            std::string mate_depends_in_rpkgs_upper_case = util::to_upper_case(mate_depends_in_rpkgs.at(k).at(d));

            std::string_view mate_depends_in_rpkgs_string_view(mate_depends_in_rpkgs_upper_case.c_str(), mate_depends_in_rpkgs_upper_case.length());

            size_t pos1 = mate_depends_in_rpkgs_string_view.find("PATCH");

            if (pos1 != std::string::npos)
            {
                size_t pos2 = mate_depends_in_rpkgs_string_view.substr(pos1).find(".");

                if (pos2 != std::string::npos)
                {
                    mate_patch_name_found = true;

                    long new_patch_level = std::strtol(std::string(mate_depends_in_rpkgs_string_view.substr(pos1 + 5, pos2)).c_str(), nullptr, 10);

                    if (new_patch_level > patch_level)
                    {
                        patch_level = new_patch_level;

                        mate_depends_in_rpkgs_index.at(k) = d;
                        mate_depends_rpkg_index_index.at(k) = d;
                        mate_depends_hash_index_index.at(k) = d;
                    }
                }
            }
            else
            {
                if (!mate_patch_name_found)
                {
                    mate_depends_in_rpkgs_index.at(k) = d;
                    mate_depends_rpkg_index_index.at(k) = d;
                    mate_depends_hash_index_index.at(k) = d;
                }
            }

            //LOG("  - MATE File Name In RPKG: " + mate_depends_in_rpkgs.at(k).at(d));
        }

        //LOG("  - MATE File Name In RPKG Used: " + mate_depends_in_rpkgs.at(k).at(mate_depends_rpkg_index_index.at(k)));
    }
}