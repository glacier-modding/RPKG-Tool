#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include <fstream>

void rpkg_function::extract_rpkg_meta(uint64_t i, std::string& hash_file_path)
{
    std::ofstream rpkg_meta_data_file;

    file::create_directories(hash_file_path);

    std::string final_path = hash_file_path + "\\" + file::get_root_file_name(rpkgs.at(i).rpkg_file_name) + ".meta";

    rpkg_meta_data_file = std::ofstream(final_path, std::ofstream::binary);

    if (!rpkg_meta_data_file.good())
    {
        LOG_AND_EXIT("Error: Meta data file " + final_path + " could not be created.");
    }

    std::vector<char> rpkg_meta_data;
    char char1[1];
    char char4[4];
    char char8[8];

    if (rpkgs.at(i).rpkg_file_version == 1)
    {
        rpkg_meta_data.push_back('G');
        rpkg_meta_data.push_back('K');
        rpkg_meta_data.push_back('P');
        rpkg_meta_data.push_back('R');
    }
    else
    {
        rpkg_meta_data.push_back('2');
        rpkg_meta_data.push_back('K');
        rpkg_meta_data.push_back('P');
        rpkg_meta_data.push_back('R');
    }

    if (rpkgs.at(i).rpkg_file_version == 2)
    {
        for (char& j : rpkgs.at(i).rpkgv2_header)
        {
            std::memcpy(&char1, &j, sizeof(uint8_t));

            rpkg_meta_data.push_back(char1[0]);
        }
    }

    std::memcpy(&char4, &rpkgs.at(i).header.hash_count, sizeof(uint32_t));

    for (char& j : char4)
    {
        rpkg_meta_data.push_back(j);
    }

    std::memcpy(&char4, &rpkgs.at(i).header.hash_header_table_size, sizeof(uint32_t));

    for (char& j : char4)
    {
        rpkg_meta_data.push_back(j);
    }

    std::memcpy(&char4, &rpkgs.at(i).header.hash_resource_table_size, sizeof(uint32_t));

    for (char& j : char4)
    {
        rpkg_meta_data.push_back(j);
    }

    if (rpkgs.at(i).is_patch_file)
    {
        std::memcpy(&char4, &rpkgs.at(i).header.patch_count, sizeof(uint32_t));

        for (char& j : char4)
        {
            rpkg_meta_data.push_back(j);
        }

        if (rpkgs.at(i).header.patch_count > 0)
        {
            uint32_t patch_entry_count = 0;

            for (uint64_t j = 0; j < rpkgs.at(i).header.patch_count; j++)
            {
                std::memcpy(&char8, &rpkgs.at(i).patch_entry_list.at(j), sizeof(uint64_t));
                for (char& k : char8)
                {
                    rpkg_meta_data.push_back(k);
                }

                patch_entry_count++;
            }

            if (patch_entry_count != rpkgs.at(i).header.patch_count)
            {
                LOG_AND_EXIT("Error: Hash meta data for " + rpkgs.at(i).rpkg_file_name + " is corrupt.");
            }
        }
    }

    for (auto& j : rpkgs.at(i).hash)
    {
        std::memcpy(&char8, &j.hash_value, sizeof(uint64_t));

        for (char& j : char8)
        {
            rpkg_meta_data.push_back(j);
        }

        std::memcpy(&char8, &j.data.header.data_offset, sizeof(uint64_t));

        for (char& j : char8)
        {
            rpkg_meta_data.push_back(j);
        }

        std::memcpy(&char4, &j.data.header.data_size, sizeof(uint32_t));

        for (char& j : char4)
        {
            rpkg_meta_data.push_back(j);
        }
    }

    rpkg_meta_data_file.write(rpkg_meta_data.data(), rpkg_meta_data.size());

    rpkg_meta_data_file.close();
}