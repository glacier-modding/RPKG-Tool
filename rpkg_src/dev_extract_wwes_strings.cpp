#include "dev_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include "thirdparty/ww2ogg/packed_codebooks_aoTuV_603.h"
#include "thirdparty/ww2ogg/wwriff.h"
#include "thirdparty/revorb/revorb.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

void dev_function::dev_extract_wwes_strings(std::string& input_path, std::string& filter, std::string& output_path)
{
    std::unordered_map<std::string, uint64_t> fxas_to_wwes_ioi_path_map;
    std::vector<std::string> fxas_wwes_ioi_path;
    std::vector<std::string> fxas_file_name;
    std::vector<uint64_t> fxas_hash;

    uint64_t total_wwes_fxas_linked = 0;
    uint64_t total_wwes_fxas_not_linked = 0;

    uint64_t wwes_count = 0;

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    double console_update_rate = 1.0 / 2.0;
    int period_count = 1;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "WWES")
            {
                wwes_count += rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size();
            }

            if (rpkgs.at(i).hash_resource_types.at(r) == "FXAS")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." + rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    uint64_t hash_size;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).data.header.data_size;

                        if (rpkgs.at(i).hash.at(hash_index).data.xored)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkgs.at(i).hash.at(hash_index).data.header.data_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkgs.at(i).hash.at(hash_index).data.xored)
                    {
                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                    }

                    uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* fxas_data;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                        fxas_data = &output_data;
                    }
                    else
                    {
                        fxas_data = &input_data;
                    }

                    uint64_t position = 0;

                    bool done_searching = false;

                    std::string text_search = ".wav";

                    while (!done_searching)
                    {
                        if ((position + text_search.length()) > decompressed_size)
                        {
                            done_searching = true;
                            break;
                        }

                        for (uint64_t k = 0; k < text_search.length(); k++)
                        {
                            if (std::tolower(fxas_data->data()[position + k]) == std::tolower(text_search[k]))
                            {
                                if (k == (text_search.length() - 1))
                                {
                                    bool done_searching_start = false;

                                    uint64_t position_start = position;

                                    while (!done_searching_start)
                                    {
                                        if (position_start == 0)
                                        {
                                            done_searching_start = true;
                                            break;
                                        }

                                        if (fxas_data->data()[position_start] < 0x20 || fxas_data->data()[position_start] > 0x7E)
                                        {
                                            done_searching_start = true;
                                            break;
                                        }

                                        position_start--;
                                    }

                                    std::string fxas_string = std::string(fxas_data->data() + position_start + 1);

                                    std::smatch m;
                                    std::regex re("(Assembly.+)\\.wav");

                                    std::regex_search(fxas_string, m, re);

                                    if (m.size() > 0)
                                    {
                                        std::smatch m2;
                                        re.assign("\\\\([^\\\\\\/]+)\\.wav");

                                        std::regex_search(fxas_string, m2, re);

                                        if (m2.size() > 0)
                                        {
                                            std::string lowercase;

                                            for (int z = 0; z < m2[1].str().length(); z++)
                                            {
                                                lowercase.push_back(std::tolower(m2[1].str()[z]));
                                            }

                                            std::string fxas_output = "[assembly:/_pro/facefx/exported_animation/" + util::to_lower_case(m[1].str()).substr(38) + ".animset].pc_animset";

                                            std::replace(fxas_output.begin(), fxas_output.end(), '\\', '/');

                                            std::replace(fxas_output.begin(), fxas_output.end(), ' ', '_');

                                            std::cout << hash_file_name << "," << fxas_output << std::endl;

                                            fxas_wwes_ioi_path.push_back(m[1].str());

                                            fxas_file_name.push_back(hash_file_name);

                                            fxas_hash.push_back(rpkgs.at(i).hash.at(hash_index).hash_value);

                                            fxas_to_wwes_ioi_path_map[lowercase] = (fxas_wwes_ioi_path.size() - 1);
                                        }
                                        else
                                        {
                                            LOG_AND_EXIT("Error: Could not parse FXAS IOI path name.");
                                        }
                                    }
                                    else
                                    {
                                        LOG_AND_EXIT("Error: Could not find FXAS IOI path name.");
                                    }

                                    break;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }

                        position++;
                    }
                }
            }
        }
    }

    uint64_t wwes_count_current = 0;

    std::unordered_map<std::string, uint32_t> wwes_name_map;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "WWES")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    std::string hash_file_name = util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." + rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                    uint64_t hash_size;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).data.header.data_size;

                        if (rpkgs.at(i).hash.at(hash_index).data.xored)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkgs.at(i).hash.at(hash_index).data.header.data_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkgs.at(i).hash.at(hash_index).data.xored)
                    {
                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                    }

                    uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* wwes_data;

                    if (rpkgs.at(i).hash.at(hash_index).data.lz4ed)
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                        wwes_data = &output_data;
                    }
                    else
                    {
                        wwes_data = &input_data;
                    }

                    uint32_t wwes_file_name_length = 0;
                    uint32_t wwes_file_count = 0;
                    uint32_t wwes_file_count_test = 0;

                    uint32_t position = 0;

                    char input[1024];
                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    bool adtllabl_not_found = true;

                    std::string adtllabl = "adtllabl";

                    while (adtllabl_not_found && position <= (decompressed_size - adtllabl.length()))
                    {
                        for (int k = 0; k < adtllabl.length(); k++)
                        {
                            if (wwes_data->data()[position + k] != adtllabl[k])
                            {
                                break;
                            }

                            if (k == (adtllabl.length() - 1))
                            {
                                adtllabl_not_found = false;
                            }
                        }

                        if (adtllabl_not_found)
                        {
                            position++;
                        }
                    }

                    if (!adtllabl_not_found)
                    {
                        position += (uint32_t)adtllabl.length();

                        std::memcpy(&wwes_file_name_length, (&wwes_data->data()[0] + position), sizeof(bytes4));
                        position += 0x8;

                        wwes_file_name_length -= 0x4;

                        std::vector<char> wwes_file_name;

                        std::memcpy(&input, (&wwes_data->data()[0] + position), wwes_file_name_length);
                        for (uint64_t k = 0; k < wwes_file_name_length; k++)
                        {
                            if (input[k] != 0)
                            {
                                wwes_file_name.push_back(input[k]);
                            }
                        }

                        wwes_file_name_length = wwes_file_name.size();

                        std::string wwes_string = std::string(wwes_file_name.begin(), wwes_file_name.end());

                        std::string lowercase;

                        for (int z = 0; z < wwes_string.length(); z++)
                        {
                            lowercase.push_back(std::tolower(wwes_string[z]));
                        }

                        std::unordered_map<std::string, uint64_t>::iterator it = fxas_to_wwes_ioi_path_map.find(lowercase);

                        std::string wem_file_name = "";
                        std::string wwes_meta_data_file_name = "";
                        std::string ogg_file = "";
                        std::string output_meta_file_path = "";

                        if (it != fxas_to_wwes_ioi_path_map.end())
                        {

                            total_wwes_fxas_linked++;

                            std::size_t pos = fxas_wwes_ioi_path.at(it->second).find_last_of("\\/");

                            std::string directory = file::output_path_append("WWES\\", output_path);

                            if (pos != std::string::npos)
                            {
                                directory.append(fxas_wwes_ioi_path.at(it->second).substr(0, pos));
                            }
                            else
                            {
                                LOG_AND_EXIT("Error: Couldn't parse directory from FXAS IOI path.");
                            }

                            std::string wem_base_name = file::output_path_append("[assembly:/" + util::remove_all_string_from_string(util::to_upper_case(fxas_wwes_ioi_path.at(it->second)), "ASSEMBLY\\"), output_path);

                            if (wwes_name_map.size() > 0)
                            {
                                std::unordered_map<std::string, uint32_t>::iterator it = wwes_name_map.find(wem_base_name);

                                if (it == wwes_name_map.end())
                                {
                                    wwes_name_map[wem_base_name] = wwes_name_map.size();
                                }
                                else
                                {
                                    bool found_new_wem_name = false;

                                    uint32_t count = 0;

                                    while (!found_new_wem_name)
                                    {
                                        std::string test_name = wem_base_name + "-" + std::to_string(count) + "-" + rpkgs.at(i).rpkg_file_name;

                                        std::unordered_map<std::string, uint32_t>::iterator it2 = wwes_name_map.find(test_name);

                                        if (it2 == wwes_name_map.end())
                                        {
                                            wem_base_name = test_name;

                                            wwes_name_map[wem_base_name] = wwes_name_map.size();

                                            found_new_wem_name = true;
                                        }

                                        count++;
                                    }
                                }
                            }
                            else
                            {
                                wwes_name_map[wem_base_name] = wwes_name_map.size();
                            }

                            wem_file_name = wem_base_name + ".wav].pc_wem";

                            wwes_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                            ogg_file = wem_base_name + ".ogg";

                            output_meta_file_path = wem_base_name;

                        }
                        else
                        {
                            total_wwes_fxas_not_linked++;

                            std::string directory = file::output_path_append("[assembly:/sound/wwise/originals/voices/unknown", output_path);

                            std::string wem_base_name = directory + "/" + wwes_string;

                            if (wwes_name_map.size() > 0)
                            {
                                std::unordered_map<std::string, uint32_t>::iterator it = wwes_name_map.find(wem_base_name);

                                if (it == wwes_name_map.end())
                                {
                                    wwes_name_map[wem_base_name] = wwes_name_map.size();
                                }
                                else
                                {
                                    bool found_new_wem_name = false;

                                    uint32_t count = 0;

                                    while (!found_new_wem_name)
                                    {
                                        std::string test_name = wem_base_name + "-" + std::to_string(count) + "-" + rpkgs.at(i).rpkg_file_name;

                                        std::unordered_map<std::string, uint32_t>::iterator it2 = wwes_name_map.find(test_name);

                                        if (it2 == wwes_name_map.end())
                                        {
                                            wem_base_name = test_name;

                                            wwes_name_map[wem_base_name] = wwes_name_map.size();

                                            found_new_wem_name = true;
                                        }

                                        count++;
                                    }
                                }
                            }
                            else
                            {
                                wwes_name_map[wem_base_name] = wwes_name_map.size();
                            }

                            wem_file_name = wem_base_name + ".wav].pc_wem";

                            wwes_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                            ogg_file = wem_base_name + ".ogg";

                            output_meta_file_path = directory;
                        }

                        std::replace(wem_file_name.begin(), wem_file_name.end(), '\\', '/');

                        std::replace(wem_file_name.begin(), wem_file_name.end(), ' ', '_');

                        std::cout << hash_file_name << "," << wem_file_name << std::endl;
                    }
                }
            }
        }
    }
}