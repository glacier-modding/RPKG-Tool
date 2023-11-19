#include "dev_function.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include <iostream>
#include <fstream>

void dev_function::dev_extract_wwise_ids(std::string& input_path, std::string& output_path) {
    input_path = file::parse_input_folder_path(input_path);

    rpkg_function::import_rpkg_files_in_folder(input_path);

    uint32_t bytes4 = 0;

    std::string wemids_txt = "";

    for (auto& rpkg : rpkgs) {
        for (uint64_t r = 0; r < rpkg.hash_resource_types.size(); r++) {
            if (rpkg.hash_resource_types.at(r) != "WWEV")
                continue;

            for (uint64_t j = 0; j < rpkg.hashes_indexes_based_on_resource_types.at(r).size(); j++) {
                uint64_t hash_index = rpkg.hashes_indexes_based_on_resource_types.at(r).at(j);

                std::string hash_file_name = util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) + "." +
                                             rpkg.hash.at(hash_index).hash_resource_type;

                uint32_t hash_reference_count =
                        rpkg.hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                std::string current_path = file::output_path_append("WWEV\\" + rpkg.rpkg_file_name, output_path);

                uint64_t hash_size;

                if (rpkg.hash.at(hash_index).data.lz4ed) {
                    hash_size = rpkg.hash.at(hash_index).data.header.data_size;

                    if (rpkg.hash.at(hash_index).data.xored) {
                        hash_size &= 0x3FFFFFFF;
                    }
                } else {
                    hash_size = rpkg.hash.at(hash_index).data.resource.size_final;
                }

                std::vector<char> input_data(hash_size, 0);

                std::ifstream file = std::ifstream(rpkg.rpkg_file_path, std::ifstream::binary);

                if (!file.good()) {
                    LOG_AND_EXIT("Error: RPKG file " + rpkg.rpkg_file_path + " could not be read.");
                }

                file.seekg(rpkg.hash.at(hash_index).data.header.data_offset, std::ifstream::beg);
                file.read(input_data.data(), hash_size);
                file.close();

                if (rpkg.hash.at(hash_index).data.xored) {
                    crypto::xor_data(input_data.data(), static_cast<uint32_t>(hash_size));
                }

                uint32_t decompressed_size = rpkg.hash.at(hash_index).data.resource.size_final;

                std::vector<char> output_data(decompressed_size, 0);

                std::vector<char>* wwev_data;

                if (rpkg.hash.at(hash_index).data.lz4ed) {
                    LZ4_decompress_safe(input_data.data(), output_data.data(), static_cast<int>(hash_size),
                                        decompressed_size);

                    wwev_data = &output_data;
                } else {
                    wwev_data = &input_data;
                }

                uint32_t wwev_name_length = 0;
                uint32_t wwev_embedded_count = 0;
                uint32_t wwev_wwem_count = 0;
                uint32_t wwev_version_test = 0;
                uint32_t position = 0;

                std::memcpy(&wwev_name_length, &(*wwev_data)[position], BYTES4);
                position += wwev_name_length + 8;

                std::memcpy(&wwev_version_test, &(*wwev_data)[position], BYTES4);

                if (wwev_version_test == 0xFFFFFFFF) {
                    position += 4;
                }
                
                std::memcpy(&wwev_embedded_count, &(*wwev_data)[position], BYTES4);

                //std::cout << "Hash: " << util::uint64_t_to_hex_string(rpkg.hash.at(hash_index).hash_value) << std::endl;
                //std::cout << "Embedded Count: " << wwev_embedded_count << std::endl;
                position += 4;

                for (uint32_t i = 0; i < wwev_embedded_count; i++) {
                    uint32_t embedded_id = 0;
                    uint32_t embedded_length = 0;
                    std::memcpy(&embedded_id, &(*wwev_data)[position], BYTES4);
                    position += 4;
                    std::memcpy(&embedded_length, &(*wwev_data)[position], BYTES4);
                    position += 4 + embedded_length;
                    //std::cout << "  - Embedded Id: " << util::uint32_t_to_hex_string(embedded_id) << std::endl;
                    //std::cout << "  - Embedded Length: " << util::uint32_t_to_hex_string(embedded_length) << std::endl; 
                }

                std::memcpy(&wwev_wwem_count, &(*wwev_data)[position], BYTES4);
                //std::cout << "WWEM Count: " << wwev_wwem_count << std::endl;
                position += 4;

                for (uint32_t i = 0; i < wwev_wwem_count; i++) {
                    uint32_t wwem_index = 0;
                    uint32_t wwem_id = 0;
                    uint32_t wwem_length = 0;
                    std::memcpy(&wwem_index, &(*wwev_data)[position], BYTES4);
                    position += 4;
                    std::memcpy(&wwem_id, &(*wwev_data)[position], BYTES4);
                    position += 4;
                    std::memcpy(&wwem_length, &(*wwev_data)[position], BYTES4);
                    position += 4 + wwem_length;

                    //std::cout << "  - WWEM Index: " << util::uint32_t_to_hex_string(wwem_index) << std::endl;
                    //std::cout << "  - WWEM Id: " << util::uint32_t_to_hex_string(wwem_id) << std::endl;
                    //std::cout << "  - WWEM Length: " << util::uint32_t_to_hex_string(wwem_length) << std::endl; 

                    if (wwem_index < hash_reference_count) {
                        bool already_found = false;

                        for (uint64_t x = 0; x < rpkgs.size(); x++) {
                            if (already_found)
                                continue;

                            auto it = rpkgs.at(x).hash_map.find(
                                    rpkg.hash.at(hash_index).hash_reference_data.hash_reference.at(wwem_index));

                            if (it == rpkgs.at(x).hash_map.end())
                                continue;

                            if (rpkgs.at(x).hash.at(it->second).hash_resource_type != "WWEM")
                                continue;

                            already_found = true;

                            wemids_txt += util::uint64_t_to_hex_string(rpkgs.at(x).hash.at(it->second).hash_value);
                            wemids_txt += ".WWEM,";
                            wemids_txt += util::uint32_t_to_string(wwem_id);
                            wemids_txt += "\n";
                        }
                    }
                }
            }
        }
    }

    file::write_to_file("wemids.txt", wemids_txt);
}