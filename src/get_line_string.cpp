#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "crypto.h"
#include <lz4.h>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <TonyTools/Languages.h>

void rpkg_function::get_line_string(std::string& input_path, std::string& filter, std::string& output_path) {
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (!file::path_exists(input_rpkg_folder_path)) {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path +
                     " to search for RPKG files for latest hash mode does not exist.");
    }

    if (!hash_list_loaded) {
        LOG("Loading Hash List...");
        generic_function::load_hash_list(false);
        LOG("Loading Hash List: Done");
    }

    rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

    std::stringstream ss;

    ss << "Scanning folder: Done";

    //LOG("\r" << ss.str() << std::string((80 - ss.str().length()), ' '));

    std::vector<std::string> filters = util::parse_input_filter(filter);

    for (auto& filter : filters) {
        uint64_t hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        std::unordered_map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(hash_value);

        if (it2 != hash_list_hash_map.end()) {
            LOG("Scanning RPKG files for: " + filter);
            LOG(filter + "'s IOI string is: " + hash_list_hash_strings.at(it2->second));
        } else {
            LOG("Scanning RPKG files for: " + filter);
        }

        uint32_t rpkg_index = rpkg_function::get_latest_hash(hash_value);

        if (rpkg_index == UINT32_MAX)
            continue;

        std::unordered_map<uint64_t, uint64_t>::iterator it = rpkgs.at(rpkg_index).hash_map.find(hash_value);

        if (it == rpkgs.at(rpkg_index).hash_map.end())
            continue;

        uint64_t hash_size;

        if (rpkgs.at(rpkg_index).hash.at(it->second).data.lz4ed) {
            hash_size = rpkgs.at(rpkg_index).hash.at(it->second).data.header.data_size;

            if (rpkgs.at(rpkg_index).hash.at(it->second).data.xored) {
                hash_size &= 0x3FFFFFFF;
            }
        } else {
            hash_size = rpkgs.at(rpkg_index).hash.at(it->second).data.resource.size_final;
        }

        std::vector<char> input_data(hash_size, 0);

        std::ifstream file = std::ifstream(rpkgs.at(rpkg_index).rpkg_file_path, std::ifstream::binary);

        if (!file.good()) {
            LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(rpkg_index).rpkg_file_path + " could not be read.");
        }

        file.seekg(rpkgs.at(rpkg_index).hash.at(it->second).data.header.data_offset, file.beg);
        file.read(input_data.data(), hash_size);
        file.close();

        if (rpkgs.at(rpkg_index).hash.at(it->second).data.xored) {
            crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
        }

        uint32_t decompressed_size = rpkgs.at(rpkg_index).hash.at(it->second).data.resource.size_final;

        std::vector<char> output_data(decompressed_size, 0);

        std::vector<char>* line_data;

        if (rpkgs.at(rpkg_index).hash.at(it->second).data.lz4ed) {
            LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size), decompressed_size);

            line_data = &output_data;
        } else {
            line_data = &input_data;
        }

        uint32_t line_crc32 = 0;

        std::memcpy(&line_crc32, &line_data->data()[0], sizeof(uint32_t));

        for (uint32_t i = 0;
             i < rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.size(); i++) {
            if (util::hash_type(rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i)) !=
                "LOCR")
                continue;

            uint32_t rpkg_index2 = rpkg_function::get_latest_hash(
                    rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i));

            if (rpkg_index2 == UINT32_MAX)
                continue;

            std::unordered_map<uint64_t, uint64_t>::iterator it6 = rpkgs.at(rpkg_index2).hash_map.find(
                    rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i));

            std::string hash_string = util::uint64_t_to_hex_string(
                    rpkgs.at(rpkg_index).hash.at(it->second).hash_reference_data.hash_reference.at(i));

            if (it6 != rpkgs.at(rpkg_index2).hash_map.end()) {
                rpkg_function::extract_locr_to_json_from(rpkgs.at(rpkg_index2).rpkg_file_path, hash_string, output_path,
                                                         true, "HM3");

                for (const auto& [language, translations] : localization_json["languages"].items()) {
                    std::string locrKey = TonyTools::Language::HashList::GetLine(line_crc32);
                    std::string hash = util::uint32_t_to_hex_string(line_crc32);

                    if (!translations.contains(locrKey)) {
                        continue;
                    }

                    std::string found_translation = translations.at(locrKey).get<std::string>();

                    if (found_translation != "") {
                        if (localization_line_string == "") {
                            localization_line_string = "\n" + (hash != locrKey ? "LINE: " + locrKey + "\n" : "") + "Hash: " + hash + "\nLOCR Strings:\n";
                        }

                        localization_line_string += "  - " + language + ": " + found_translation + "\n";
                    }
                }

            }
        }
    }

    LOG(localization_line_string);
}
