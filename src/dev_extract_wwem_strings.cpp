#include "rpkg_function.h"
#include "dev_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <regex>

void dev_function::dev_extract_wwem_strings(std::string& input_path, std::string& output_path) {
    input_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_path);

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    input_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_path);

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    std::unordered_map<std::string, uint64_t> fxas_to_wwem_ioi_path_map;
    std::vector<std::string> fxas_wwem_ioi_path;
    std::vector<std::string> fxas_file_name;
    std::unordered_map<uint64_t, uint64_t> fxas_hash;

    uint64_t total_wwem_fxas_linked = 0;
    uint64_t total_wwem_fxas_not_linked = 0;

    uint64_t wwem_count = 0;

    std::unordered_map<uint64_t, uint64_t> dlge_wwem_map;
    std::unordered_map<uint64_t, uint64_t> dlge_fxas_map;
    std::vector<uint64_t> wwem_hashes;
    std::vector<std::string> wwem_ioi_paths;
    std::vector<std::string> wwem_ioi_short_paths;

    for (uint64_t i = 0; i < rpkgs.size(); i++) {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++) {
            if (rpkgs.at(i).hash_resource_types.at(r) == "WWEM") {
                wwem_count += rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size();
            }

            if (rpkgs.at(i).hash_resource_types.at(r) != "DLGE")
                continue;

            for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                uint32_t hash_reference_count =
                        rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint32_t k = 0; k < hash_reference_count; k++) {
                    if (k + 1 >= hash_reference_count)
                        continue;

                    bool already_found = false;

                    for (uint64_t x = 0; x < rpkgs.size(); x++) {
                        if (already_found)
                            continue;

                        auto it = rpkgs.at(x).hash_map.find(
                                rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference.at(k));

                        if (it == rpkgs.at(x).hash_map.end())
                            continue;

                        std::string hash_resource_type = rpkgs.at(x).hash.at(it->second).hash_resource_type;

                        if (hash_resource_type != "WWEM")
                            continue;

                        auto it3 = rpkgs.at(x).hash_map.find(
                                rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference.at(k + 1));

                        if (it3 == rpkgs.at(x).hash_map.end())
                            continue;

                        std::string hash_resource_type_2 = rpkgs.at(x).hash.at(it3->second).hash_resource_type;

                        if (hash_resource_type_2 != "FXAS")
                            continue;

                        std::string hash_file_name =
                                util::uint64_t_to_hex_string(rpkgs.at(x).hash.at(it3->second).hash_value) + "." +
                                rpkgs.at(x).hash.at(it3->second).hash_resource_type;

                        uint64_t hash_size;

                        if (rpkgs.at(x).hash.at(it3->second).data.lz4ed) {
                            hash_size = rpkgs.at(x).hash.at(it3->second).data.header.data_size;

                            if (rpkgs.at(x).hash.at(it3->second).data.xored) {
                                hash_size &= 0x3FFFFFFF;
                            }
                        } else {
                            hash_size = rpkgs.at(x).hash.at(it3->second).data.resource.size_final;
                        }

                        std::vector<char> input_data(hash_size, 0);

                        std::ifstream file = std::ifstream(rpkgs.at(x).rpkg_file_path, std::ifstream::binary);

                        if (!file.good()) {
                            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(x).rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkgs.at(x).hash.at(it3->second).data.header.data_offset, std::ifstream::beg);
                        file.read(input_data.data(), hash_size);
                        file.close();

                        if (rpkgs.at(x).hash.at(it3->second).data.xored) {
                            crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                        }

                        uint32_t decompressed_size = rpkgs.at(x).hash.at(it3->second).data.resource.size_final;

                        std::vector<char> output_data(decompressed_size, 0);

                        std::vector<char>* fxas_data;

                        if (rpkgs.at(x).hash.at(it3->second).data.lz4ed) {
                            LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                                decompressed_size);

                            fxas_data = &output_data;
                        } else {
                            fxas_data = &input_data;
                        }

                        uint64_t position = 0;

                        bool done_searching = false;

                        const std::string text_search = ".wav";

                        std::string fxas_string;

                        while (!done_searching) {
                            if ((position + text_search.length()) > decompressed_size) {
                                done_searching = true;
                                break;
                            }

                            for (uint64_t l = 0; l < text_search.length(); l++) {
                                if (std::tolower((*fxas_data)[position + l]) != std::tolower(text_search[l]))
                                    break;

                                if (l != text_search.length() - 1)
                                    continue;

                                bool done_searching_start = false;

                                uint64_t position_start = position;

                                while (!done_searching_start) {
                                    if (position_start == 0) {
                                        done_searching_start = true;
                                        break;
                                    }

                                    if ((*fxas_data)[position_start] < 0x20 || (*fxas_data)[position_start] > 0x7E) {
                                        done_searching_start = true;
                                        break;
                                    }

                                    position_start--;
                                }

                                fxas_string = std::string(fxas_data->data() + position_start + 1);

                                std::smatch m;
                                std::regex re("(Assembly.+)\\.wav");

                                std::regex_search(fxas_string, m, re);

                                if (m.empty()) {
                                    LOG_AND_EXIT("Error: Could not find FXAS IOI path name.");
                                }

                                std::smatch m2;
                                re.assign(R"(\\([^\\\/]+)\.wav)");

                                std::regex_search(fxas_string, m2, re);

                                if (m2.empty()) {
                                    LOG_AND_EXIT("Error: Could not parse FXAS IOI path name.");
                                }

                                std::string lowercase;

                                for (int z = 0; z < m2[1].str().length(); z++) {
                                    lowercase.push_back(std::tolower(m2[1].str()[z]));
                                }

                                fxas_wwem_ioi_path.push_back(m[1].str());

                                dlge_wwem_map[rpkgs.at(x).hash.at(it->second).hash_value] = dlge_wwem_map.size();

                                wwem_hashes.push_back(rpkgs.at(x).hash.at(it->second).hash_value);

                                dlge_fxas_map[rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference.at(
                                        k + 1)] = dlge_fxas_map.size();

                                wwem_ioi_paths.push_back(m[1].str());

                                wwem_ioi_short_paths.push_back(lowercase);

                                fxas_file_name.push_back(hash_file_name);

                                fxas_hash[rpkgs.at(x).hash.at(it3->second).hash_value] = (fxas_wwem_ioi_path.size() -
                                                                                          1);

                                fxas_to_wwem_ioi_path_map[lowercase] = (fxas_wwem_ioi_path.size() - 1);

                                already_found = true;

                                break;
                            }

                            position++;
                        }
                    }
                }
            }
        }
    }

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "FXAS")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                uint32_t hash_reference_count =
                        rpkg.hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                for (uint32_t k = 0; k < hash_reference_count; k++) {
                    auto it = fxas_hash.find(rpkg.hash.at(hash_index).hash_reference_data.hash_reference.at(k));

                    if (it == fxas_hash.end()) {
                        std::string hash_file_name =
                                util::uint64_t_to_hex_string(rpkg.hash.at(it->second).hash_value) + "." +
                                rpkg.hash.at(it->second).hash_resource_type;

                        uint64_t hash_size;

                        if (rpkg.hash.at(it->second).data.lz4ed) {
                            hash_size = rpkg.hash.at(it->second).data.header.data_size;

                            if (rpkg.hash.at(it->second).data.xored) {
                                hash_size &= 0x3FFFFFFF;
                            }
                        } else {
                            hash_size = rpkg.hash.at(it->second).data.resource.size_final;
                        }

                        std::vector<char> input_data(hash_size, 0);

                        std::ifstream file = std::ifstream(rpkg.rpkg_file_path, std::ifstream::binary);

                        if (!file.good()) {
                            LOG_AND_EXIT("Error: RPKG file " + rpkg.rpkg_file_path + " could not be read.");
                        }

                        file.seekg(rpkg.hash.at(it->second).data.header.data_offset, std::ifstream::beg);
                        file.read(input_data.data(), hash_size);
                        file.close();

                        if (rpkg.hash.at(it->second).data.xored) {
                            crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                        }

                        uint32_t decompressed_size = rpkg.hash.at(it->second).data.resource.size_final;

                        std::vector<char> output_data(decompressed_size, 0);

                        std::vector<char>* fxas_data;

                        if (rpkg.hash.at(it->second).data.lz4ed) {
                            LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                                decompressed_size);

                            fxas_data = &output_data;
                        } else {
                            fxas_data = &input_data;
                        }

                        uint64_t position = 0;

                        bool done_searching = false;

                        std::string text_search = ".wav";

                        std::string fxas_string;

                        while (!done_searching) {
                            if ((position + text_search.length()) > decompressed_size) {
                                done_searching = true;
                                break;
                            }

                            for (uint64_t l = 0; l < text_search.length(); l++) {
                                if (std::tolower((*fxas_data)[position + l]) != std::tolower(text_search[l]))
                                    break;

                                if (l != text_search.length() - 1)
                                    continue;

                                bool done_searching_start = false;

                                uint64_t position_start = position;

                                while (!done_searching_start) {
                                    if (position_start == 0) {
                                        done_searching_start = true;
                                        break;
                                    }

                                    if ((*fxas_data)[position_start] < 0x20 || (*fxas_data)[position_start] > 0x7E) {
                                        done_searching_start = true;
                                        break;
                                    }

                                    position_start--;
                                }

                                fxas_string = std::string(fxas_data->data() + position_start + 1);

                                std::smatch m;
                                std::regex re("(Assembly.+)\\.wav");

                                std::regex_search(fxas_string, m, re);

                                if (m.empty()) {
                                    LOG_AND_EXIT("Error: Could not find FXAS IOI path name.");
                                }

                                std::smatch m2;
                                re.assign(R"(\\([^\\\/]+)\.wav)");

                                std::regex_search(fxas_string, m2, re);

                                if (m2.empty()) {
                                    LOG_AND_EXIT("Error: Could not parse FXAS IOI path name.");
                                }
                                std::string lowercase;

                                for (int z = 0; z < m2[1].str().length(); z++) {
                                    lowercase.push_back(std::tolower(m2[1].str()[z]));
                                }

                                fxas_wwem_ioi_path.push_back(m[1].str());

                                fxas_file_name.push_back(hash_file_name);

                                fxas_to_wwem_ioi_path_map[lowercase] = (fxas_wwem_ioi_path.size() - 1);

                                break;
                            }

                            position++;
                        }
                    }
                }
            }
        }
    }

    std::unordered_map<std::string, uint32_t> wwem_name_map;

    for (uint64_t i = 0; i < rpkgs.size(); i++) {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++) {
            if (rpkgs.at(i).hash_resource_types.at(r) != "WWEM")
                continue;

            for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                std::string hash_file_name =
                        util::uint64_t_to_hex_string(rpkgs.at(i).hash.at(hash_index).hash_value) + "." +
                        rpkgs.at(i).hash.at(hash_index).hash_resource_type;

                bool is_unknown = true;

                auto it = std::find(wwem_hashes.begin(), wwem_hashes.end(), rpkgs.at(i).hash.at(hash_index).hash_value);

                uint64_t wwem_index = 0;

                if (it != wwem_hashes.end()) {
                    is_unknown = false;

                    wwem_index = std::distance(wwem_hashes.begin(), it);
                }

                uint64_t hash_size;

                if (rpkgs.at(i).hash.at(hash_index).data.lz4ed) {
                    hash_size = rpkgs.at(i).hash.at(hash_index).data.header.data_size;

                    if (rpkgs.at(i).hash.at(hash_index).data.xored) {
                        hash_size &= 0x3FFFFFFF;
                    }
                } else {
                    hash_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;
                }

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                if (!file.good()) {
                    LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                }

                file.seekg(rpkgs.at(i).hash.at(hash_index).data.header.data_offset, std::ifstream::beg);
                file.read(input_data.data(), hash_size);
                file.close();

                if (rpkgs.at(i).hash.at(hash_index).data.xored) {
                    crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                }

                uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).data.resource.size_final;

                std::vector<char> output_data(decompressed_size, 0);

                std::vector<char>* wwem_data;

                if (rpkgs.at(i).hash.at(hash_index).data.lz4ed) {
                    LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                        decompressed_size);

                    wwem_data = &output_data;
                } else {
                    wwem_data = &input_data;
                }

                uint32_t wwem_file_name_length = 0;

                uint32_t position = 0;

                char input[1024];

                bool adtllabl_not_found = true;

                std::string adtllabl = "adtllabl";

                while (adtllabl_not_found && position <= (decompressed_size - adtllabl.length())) {
                    for (int k = 0; k < adtllabl.length(); k++) {
                        if ((*wwem_data)[position + k] != adtllabl[k]) {
                            break;
                        }

                        if (k == (adtllabl.length() - 1)) {
                            adtllabl_not_found = false;
                        }
                    }

                    if (adtllabl_not_found) {
                        position++;
                    }
                }

                std::string wem_file_name = "";
                std::string wwem_meta_data_file_name = "";
                std::string ogg_file = "";
                std::string output_meta_file_path = "";
                std::string wwem_string = "";
                std::string directory = "";

                if (!adtllabl_not_found) {
                    position += static_cast<uint32_t>(adtllabl.length());

                    std::memcpy(&wwem_file_name_length, (&(*wwem_data)[0] + position), BYTES4);
                    position += 0x8;

                    wwem_file_name_length -= 0x4;

                    std::vector<char> wwem_file_name;

                    std::memcpy(&input, (&(*wwem_data)[0] + position), wwem_file_name_length);
                    for (uint64_t k = 0; k < wwem_file_name_length; k++) {
                        if (input[k] != 0) {
                            wwem_file_name.push_back(input[k]);
                        }
                    }

                    wwem_file_name_length = wwem_file_name.size();

                    wwem_string = std::string(wwem_file_name.begin(), wwem_file_name.end());

                    std::string lowercase;

                    for (char z : wwem_string) {
                        lowercase.push_back(std::tolower(z));
                    }

                    bool short_path_found = false;

                    if (!wwem_hashes.empty()) {
                        if (wwem_ioi_short_paths.at(wwem_index) == lowercase) {
                            short_path_found = true;
                        }
                    }

                    if (short_path_found) {
                        total_wwem_fxas_linked++;

                        std::size_t pos = wwem_ioi_paths.at(wwem_index).find_last_of("\\/");

                        directory = file::output_path_append("WWEM\\", output_path);

                        if (pos != std::string::npos) {
                            directory.append(wwem_ioi_paths.at(wwem_index).substr(0, pos));
                        } else {
                            LOG_AND_EXIT("Error: Couldn't parse directory from FXAS IOI path.");
                        }

                        std::string wem_base_name = file::output_path_append("WWEM\\" + wwem_ioi_paths.at(wwem_index),
                                                                             output_path);

                        wem_file_name = wem_base_name + ".wav].pc_wem";

                        wwem_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                        ogg_file = wem_base_name + ".ogg";

                        output_meta_file_path = wem_base_name;

                        if (output_meta_file_path.substr(output_meta_file_path.length() - 1, 1) == "\\") {
                            output_meta_file_path = output_meta_file_path.substr(0, output_meta_file_path.length() - 1);
                        }

                        pos = output_meta_file_path.find_last_of("\\/");

                        if (pos != std::string::npos) {
                            output_meta_file_path = output_meta_file_path.substr(0, pos + 1);
                        }
                    } else if (is_unknown) {
                        total_wwem_fxas_not_linked++;

                        directory = file::output_path_append("[assembly:/sound/wwise/originals/voices/unknown",
                                                             output_path);

                        if (wwem_string.empty()) {
                            wwem_string = "NOLABEL";
                        }

                        wwem_string = util::remove_all_string_from_string(wwem_string, ":");

                        std::string wem_base_name = directory + "/" + wwem_string;

                        //rpkg_function::get_unique_wwem_name(wwem_name_map, wem_base_name, rpkgs.at(i).rpkg_file_name);

                        wem_file_name = wem_base_name + ".wav].pc_wem";

                        wwem_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                        ogg_file = wem_base_name + ".ogg";

                        output_meta_file_path = directory;
                    } else {
                        auto it = fxas_to_wwem_ioi_path_map.find(lowercase);

                        if (it != fxas_to_wwem_ioi_path_map.end()) {
                            total_wwem_fxas_linked++;

                            std::size_t pos = fxas_wwem_ioi_path.at(it->second).find_last_of("\\/");

                            directory = file::output_path_append("WWEM\\", output_path);

                            if (pos != std::string::npos) {
                                directory.append(fxas_wwem_ioi_path.at(it->second).substr(0, pos));
                            } else {
                                LOG_AND_EXIT("Error: Couldn't parse directory from FXAS IOI path.");
                            }

                            std::string wem_base_name = file::output_path_append(
                                    "WWEM\\" + fxas_wwem_ioi_path.at(it->second), output_path);

                            //rpkg_function::get_unique_wwem_name(wwem_name_map, wem_base_name, rpkgs.at(i).rpkg_file_name);

                            wem_file_name = wem_base_name + ".wav].pc_wem";

                            wwem_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                            ogg_file = wem_base_name + ".ogg";

                            output_meta_file_path = wem_base_name;

                            if (output_meta_file_path.substr(output_meta_file_path.length() - 1, 1) == "\\") {
                                output_meta_file_path = output_meta_file_path.substr(0, output_meta_file_path.length() -
                                                                                        1);
                            }

                            pos = output_meta_file_path.find_last_of("\\/");

                            if (pos != std::string::npos) {
                                output_meta_file_path = output_meta_file_path.substr(0, pos + 1);
                            }
                        } else {
                            total_wwem_fxas_not_linked++;

                            directory = file::output_path_append("[assembly:/sound/wwise/originals/voices/unknown",
                                                                 output_path);

                            std::string wem_base_name = directory + "/" + wwem_string;

                            //rpkg_function::get_unique_wwem_name(wwem_name_map, wem_base_name, rpkgs.at(i).rpkg_file_name);

                            wem_file_name = wem_base_name + ".wav].pc_wem";

                            wwem_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                            ogg_file = wem_base_name + ".ogg";

                            output_meta_file_path = directory;
                        }

                        LOG(wwem_ioi_short_paths.at(wwem_index) + " does not equal " + lowercase);
                        LOG("Outputing to " + wem_file_name);
                        LOG("Error: " + hash_file_name + " has no FXAS linked to it.");
                    }
                } else {
                    bool found_wwev = false;

                    for (uint64_t a = 0; a < rpkgs.size(); a++) {
                        for (uint64_t b = 0; b < rpkgs.at(a).hash_resource_types.size(); b++) {
                            if (rpkgs.at(a).hash_resource_types.at(b) != "WWEV")
                                continue;

                            for (uint64_t c = 0;
                                 c < rpkgs.at(a).hashes_indexes_based_on_resource_types.at(b).size(); c++) {
                                uint64_t hash_index2 = rpkgs.at(a).hashes_indexes_based_on_resource_types.at(b).at(c);

                                uint32_t hash_reference_count =
                                        rpkgs.at(a).hash.at(hash_index2).hash_reference_data.hash_reference_count &
                                        0x3FFFFFFF;

                                for (uint32_t k = 0; k < hash_reference_count; k++) {
                                    if (rpkgs.at(a).hash.at(hash_index2).hash_reference_data.hash_reference.at(k) !=
                                        rpkgs.at(i).hash.at(hash_index).hash_value || found_wwev)
                                        continue;

                                    uint64_t hash_size;

                                    if (rpkgs.at(a).hash.at(hash_index2).data.lz4ed) {
                                        hash_size = rpkgs.at(a).hash.at(hash_index2).data.header.data_size;

                                        if (rpkgs.at(a).hash.at(hash_index2).data.xored) {
                                            hash_size &= 0x3FFFFFFF;
                                        }
                                    } else {
                                        hash_size = rpkgs.at(a).hash.at(hash_index2).data.resource.size_final;
                                    }

                                    std::vector<char> input_data2(hash_size, 0);

                                    std::ifstream file2 = std::ifstream(rpkgs.at(a).rpkg_file_path,
                                                                        std::ifstream::binary);

                                    if (!file2.good()) {
                                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(a).rpkg_file_path +
                                                     " could not be read.");
                                    }

                                    file2.seekg(rpkgs.at(a).hash.at(hash_index2).data.header.data_offset, std::ifstream::beg);
                                    file2.read(input_data2.data(), hash_size);
                                    file2.close();

                                    if (rpkgs.at(a).hash.at(hash_index2).data.xored) {
                                        crypto::xor_data(input_data2.data(), static_cast<uint32_t>(hash_size));
                                    }

                                    uint32_t decompressed_size = rpkgs.at(a).hash.at(
                                            hash_index2).data.resource.size_final;

                                    std::vector<char> output_data2(decompressed_size, 0);

                                    std::vector<char>* wwev_data;

                                    if (rpkgs.at(a).hash.at(hash_index2).data.lz4ed) {
                                        LZ4_decompress_safe(input_data2.data(), output_data2.data(),
                                                            static_cast<int>(hash_size), decompressed_size);

                                        wwev_data = &output_data2;
                                    } else {
                                        wwev_data = &input_data2;
                                    }

                                    uint32_t wwev_file_name_length = 0;

                                    uint32_t position = 0;

                                    char input[1024];

                                    std::memcpy(&wwev_file_name_length, &(*wwev_data)[position], BYTES4);

                                    std::vector<char> wwev_meta_data;

                                    char hash[8];

                                    std::memcpy(&hash, &rpkgs.at(a).hash.at(hash_index2).hash_value, 0x8);

                                    for (char& k : hash) {
                                        wwev_meta_data.push_back(k);
                                    }

                                    std::memcpy(&input, &(*wwev_data)[position],
                                                (wwev_file_name_length + static_cast<uint64_t>(0xC)));
                                    for (uint64_t k = 0;
                                         k < (wwev_file_name_length + static_cast<uint64_t>(0xC)); k++) {
                                        wwev_meta_data.push_back(input[k]);
                                    }

                                    position += 0x4;

                                    std::vector<char> wwev_file_name(
                                            static_cast<uint64_t>(wwev_file_name_length) + static_cast<uint64_t>(1), 0);
                                    wwev_file_name[wwev_file_name_length] = 0;

                                    std::memcpy(wwev_file_name.data(), &(*wwev_data)[position], wwev_file_name_length);

                                    wwem_string = std::string(wwev_file_name.data());

                                    found_wwev = true;
                                }
                            }
                        }
                    }

                    if (found_wwev) {
                        directory = file::output_path_append("[assembly:/sound/wwise/originals/voices/unknown",
                                                             output_path);

                        std::string wem_base_name = directory + "/" + wwem_string;

                        wem_file_name = wem_base_name + ".wav].pc_wem";

                        wwem_meta_data_file_name = wem_base_name + "_" + hash_file_name;

                        ogg_file = wem_base_name + ".ogg";

                        output_meta_file_path = directory;
                    }
                }

                std::replace(wem_file_name.begin(), wem_file_name.end(), ' ', '_');

                std::cout << hash_file_name << "," << wem_file_name << std::endl;
            }
        }
    }
}