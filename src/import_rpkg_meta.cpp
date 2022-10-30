#include "rpkg_function.h"
#include "rpkg.h"
#include "file.h"
#include "global.h"
#include <iostream>
#include <fstream>

bool rpkg_function::import_rpkg_meta(rpkg& rpkg_meta_data, const std::string& rpkg_meta_file_path, const std::string& input_rpkg_folder_path)
{
    rpkg_meta_data.is_patch_file = false;

    uint8_t bytes1 = 0;
    uint32_t bytes4 = 0;
    uint64_t bytes8 = 0;
    char input[1024];

    uint64_t rpkg_meta_file_size = 0;

    rpkg_meta_data.rpkg_file_version = 1;

    if (!file::path_exists(rpkg_meta_file_path))
    {
        rpkg_meta_data.is_patch_file = true;
        rpkg_meta_data.header.patch_count = 0x0;

        LOG("RPKG meta file matching the name of the folder was not found");
        LOG("   in it's root:" << input_rpkg_folder_path);
        LOG("Defaulting to RPKGv1 patch file creation settings:");
        LOG("  - Patch entry (deletion list) count is set to 0");
        LOG("  - No patch entries (deletion entries) will be appended");

        return false;
    }

    LOG("Using RPKG meta file: " << rpkg_meta_file_path);

    std::ifstream rpkg_meta_file = std::ifstream(rpkg_meta_file_path, std::ifstream::binary);

    if (!rpkg_meta_file.good())
    {
        LOG_AND_EXIT_NOP("Error: RPKG meta file " + rpkg_meta_file_path + " could not be read.");
    }

    rpkg_meta_file.seekg(0, std::ifstream::end);
    rpkg_meta_file_size = rpkg_meta_file.tellg();
    rpkg_meta_file.seekg(0, std::ifstream::beg);

    rpkg_meta_file.read(input, 4);
    input[4] = 0;

    if (std::string(input) == "GKPR")
    {
        rpkg_meta_data.rpkg_file_version = 1;

        LOG("Valid RPKGv1 file magic signature found.");
    }
    else if (std::string(input) == "2KPR")
    {
        rpkg_meta_data.rpkg_file_version = 2;

        LOG("Valid RPKGv2 file magic signature found.");
    }
    else
    {
        LOG_AND_EXIT_NOP("Error: " + rpkg_meta_file_path + " is not a valid RPKG file.");
    }

    if (rpkg_meta_data.rpkg_file_version == 2)
    {
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
        rpkg_meta_file.read(input, sizeof(bytes1));
        rpkg_meta_data.rpkgv2_header.push_back(input[0]);
    }

    rpkg_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&rpkg_meta_data.header.hash_count, input, sizeof(bytes4));

    rpkg_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&rpkg_meta_data.header.hash_header_table_size, input, sizeof(bytes4));

    rpkg_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&rpkg_meta_data.header.hash_resource_table_size, input, sizeof(bytes4));

    uint64_t position = rpkg_meta_file.tellg();

    rpkg_meta_file.read(input, sizeof(bytes4));
    std::memcpy(&rpkg_meta_data.header.patch_count, input, sizeof(bytes4));

    if (rpkg_meta_data.rpkg_file_version == 1 && (static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) + static_cast<uint64_t>(0x14) + static_cast<uint64_t>(0x10)) >= rpkg_meta_file_size)
    {
        rpkg_meta_data.is_patch_file = false;

        LOG("RPKGv1 file " << rpkg_meta_file_path << " is not a patch file.");
    }
    else if (rpkg_meta_data.rpkg_file_version == 2 && (static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) + static_cast<uint64_t>(0x1D) + static_cast<uint64_t>(0x10)) >= rpkg_meta_file_size)
    {
        rpkg_meta_data.is_patch_file = false;

        LOG("RPKGv2 file " << rpkg_meta_file_path << " is not a patch file.");
    }
    else
    {
        char patchZeroTest = 0;
        uint64_t patchValue = 0;

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            rpkg_meta_file.seekg((static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) + static_cast<uint64_t>(0x14)), rpkg_meta_file.beg);
        }
        else
        {
            rpkg_meta_file.seekg((static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) + static_cast<uint64_t>(0x1D)), rpkg_meta_file.beg);
        }

        rpkg_meta_file.read(input, 0x7);
        rpkg_meta_file.read(input, sizeof(bytes1));
        std::memcpy(&patchZeroTest, input, sizeof(bytes1));
        rpkg_meta_file.read(input, sizeof(bytes8));
        std::memcpy(&patchValue, input, sizeof(bytes8));

        if (rpkg_meta_data.rpkg_file_version == 1 && patchValue == (static_cast<uint64_t>(rpkg_meta_data.header.hash_header_table_size) + static_cast<uint64_t>(rpkg_meta_data.header.hash_resource_table_size) + static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) + static_cast<uint64_t>(0x14)) && patchZeroTest == 0x0)
        {
            rpkg_meta_data.is_patch_file = true;

            LOG("RPKGv1 file " << rpkg_meta_file_path << " is a patch file.");
        }
        else if (rpkg_meta_data.rpkg_file_version == 2 && patchValue == (static_cast<uint64_t>(rpkg_meta_data.header.hash_header_table_size) + static_cast<uint64_t>(rpkg_meta_data.header.hash_resource_table_size) + static_cast<uint64_t>(rpkg_meta_data.header.patch_count) * static_cast<uint64_t>(0x8) + static_cast<uint64_t>(0x1D)) && patchZeroTest == 0x0)
        {
            rpkg_meta_data.is_patch_file = true;

            LOG("RPKGv2 file " << rpkg_meta_file_path << " is a patch file.");
        }
        else
        {
            rpkg_meta_data.is_patch_file = false;

            if (rpkg_meta_data.rpkg_file_version == 1)
            {
                LOG("RPKGv1 file " << rpkg_meta_file_path << " is not a patch file.");
            }
            else
            {
                LOG("RPKGv2 file " << rpkg_meta_file_path << " is not a patch file.");
            }
        }
    }

    rpkg_meta_file.seekg(position, std::ifstream::beg);

    if (rpkg_meta_data.is_patch_file)
    {
        rpkg_meta_file.read(input, sizeof(bytes4));
        std::memcpy(&rpkg_meta_data.header.patch_count, input, sizeof(bytes4));

        for (uint64_t i = 0; i < rpkg_meta_data.header.patch_count; i++)
        {
            rpkg_meta_file.read(input, sizeof(bytes8));
            std::memcpy(&bytes8, input, sizeof(bytes8));
            rpkg_meta_data.patch_entry_list.push_back(bytes8);
        }
    }

    for (uint64_t i = 0; i < rpkg_meta_data.header.patch_count; i++)
    {
        rpkg_meta_file.read(input, sizeof(bytes8));
        std::memcpy(&bytes8, input, sizeof(bytes8));
        rpkg_meta_data.meta_hash_value.push_back(bytes8);
        rpkg_meta_data.hash_map[bytes8] = static_cast<int>(rpkg_meta_data.meta_hash_value.size() - 1);

        rpkg_meta_file.read(input, sizeof(bytes8));
        std::memcpy(&bytes8, input, sizeof(bytes8));
        rpkg_meta_data.meta_hash_offset.push_back(bytes8);

        rpkg_meta_file.read(input, sizeof(bytes4));
        std::memcpy(&bytes4, input, sizeof(bytes4));
        rpkg_meta_data.meta_hash_size.push_back(bytes4);
    }

    return true;
}
