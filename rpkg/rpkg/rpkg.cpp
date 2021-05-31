#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <memory>
#include <set>
#include <chrono>
#include <map>
#include <unordered_map>
#include <regex>
#include <io.h>
#include <numeric>
#include "lib/lz4/lz4.h"
#include "lib/lz4/lz4hc.h"
#include "md5.h"
#include "wwriff.h"
#include "packed_codebooks_aoTuV_603.h"
#include "lib/json/json.hpp"

#ifdef _WIN64
#include <io.h>
#else
#include <unistd.h>
#define _isatty isatty
#define _fileno fileno
#endif

using json = nlohmann::ordered_json;

extern "C" void MD5Init(struct MD5Context* ctx);
extern "C" void MD5Update(struct MD5Context* ctx, unsigned char* buf, unsigned len);
extern "C" void MD5Final(unsigned char digest[16], struct MD5Context* ctx);
extern "C" void MD5Transform(uint32 buf[4], uint32 in[16]);

struct hash_depends_variables
{
    uint64_t hash = 0;
    std::string hash_string = "";
    std::string rpkg_file_name = "";
    std::vector<std::string> hash_dependency;
    std::vector<std::string> hash_dependency_file_name;
    std::map<uint64_t, uint64_t> hash_dependency_map;
    std::vector<std::vector<std::string>> hash_dependency_in_rpkg;
    std::vector<std::vector<std::string>> hash_dependency_patch_list;
};

struct hash_reference_variables
{
    uint64_t hash = 0;
    uint32_t hash_reference_count = 0;
    std::vector<uint8_t> hash_reference_type;
    std::vector<uint64_t> hash_reference;
    std::vector<std::string> hash_reference_string;
};

struct rpkg_variables
{
    int rpkg_file_version = 0;
    int rpkgv2_chunk_number = 0;
    int rpkgv2_chunk_patch_number = 0;
    std::string rpkg_file_name = "";
    std::string rpkg_file_path = "";
    bool is_patch_file = false;
    uint32_t patch_entry_count = 0;
    std::vector<uint64_t> patch_entry_list;
    uint32_t rpkg_file_count = 0;
    uint32_t rpkg_table_offset = 0;
    uint32_t rpkg_table_size = 0;
    std::vector<uint64_t> hash;
    std::map<uint64_t, uint64_t> hash_map;
    std::map<uint64_t, uint64_t> hash_unordered_map;
    std::vector<std::string> hash_string;
    std::vector<std::string> hash_file_name;
    std::vector<uint64_t> hash_offset;
    std::vector<uint32_t> hash_size;
    std::vector<int> is_lz4ed;
    std::vector<int> is_xored;
    std::vector<std::string> hash_resource_type;
    std::vector<uint32_t> hash_reference_table_size;
    std::vector<uint32_t> hash_reference_table_dummy;
    std::vector<uint32_t> hash_size_final;
    std::vector<uint32_t> hash_size_in_memory;
    std::vector<uint32_t> hash_size_in_video_memory;
    std::vector<hash_reference_variables> hash_reference_data;
};

struct main_variables
{
    std::vector<rpkg_variables> rpkg_data;
    std::string input_rpkg_file_path = "";
    std::string input_rpkg_file_name = "";
    std::string input_rpkg_folder_path = "";
    std::vector<std::string> input_filter;
    std::string input_filter_string;
    std::string input_command_json_file_path = "";
    std::string input_command_json_file_name = "";
    std::string input_text_search = "";
    std::string input_hex_search = "";
    std::string input_regex_search = "";
    std::string input_ores_path = "";
    std::string input_output_path = "";
    std::string input_to_ioi_hash = "";
    std::string input_packagedefinitions_thumbs_file_path = "";
    std::string input_packagedefinitions_thumbs_file_name = "";
    std::string console_prefix = "";
    bool mode_filter = false;
    bool mode_extract_from_rpkg = false;
    bool mode_generate_rpkg_file = false;
    bool mode_command_json = false;
    bool mode_use_lz4hc = true;
    bool mode_xor_bytes = false;
    bool mode_hash_depends = false;
    bool mode_hash_probe = false;
    bool mode_search_rpkg = false;
    bool mode_text_search = false;
    bool mode_hex_search = false;
    bool mode_regex_search = false;
    bool mode_extract_all_ores = false;
    bool mode_extract_all_wwev_to_ogg = false;
    bool mode_rebuild_all_wwev = false;
    bool mode_extract_all_wwes_to_ogg = false;
    bool mode_locr_to_json = false;
    bool mode_json_to_locr = false;
    bool mode_rtlv_to_json = false;
    bool mode_json_to_rtlv = false;
    bool mode_dlge_to_json = false;
    bool mode_json_to_dlge = false;
    bool mode_output_path = false;
    bool mode_compute_ioi_hash = false;
    bool mode_decrypt_packagedefinition_thumbs = false;
    bool mode_encrypt_packagedefinition_thumbs = false;
    bool suppress_console_output = false;
    bool debug = false;
};

int update_console(std::string& message, uint64_t indexMax, uint64_t index, std::chrono::time_point<std::chrono::high_resolution_clock> start_time, int stringstream_length)
{
    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();
    double timeInSecondsFromstart_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

    double percent = ((double)index / (double)indexMax) * 100.0;
    std::stringstream ss;
    ss << message << std::fixed << std::setprecision(1) << percent << "% Done" << " in " << timeInSecondsFromstart_time << "s, estimated completion in " << (timeInSecondsFromstart_time / (double)index) * (indexMax - index) << "s";

    if (_isatty(_fileno(stdout)))
    {
        if (ss.str().length() > stringstream_length)
        {
            stringstream_length = (int)ss.str().length();
        }

        std::cout << "\r" << ss.str() << std::string((stringstream_length - ss.str().length()), ' ') << std::flush;
    }

    return stringstream_length;
}

std::string replace_slashes(const std::string& s)
{
    std::string replacedS = s;
    std::replace(begin(replacedS), end(replacedS), '\\', '/');
    return replacedS;
}

bool path_exists(const std::string& s)
{
    return std::filesystem::exists(replace_slashes(s));
}

std::string to_uppercase(std::string s)
{
    for (uint64_t i = 0; i < s.length(); i++)
    {
        s[i] = std::toupper(s[i]);
    }

    return s;
}

std::string remove_all_string_from_string(std::string input_string, std::string to_remove)
{
    bool done = false;

    while (!done)
    {
        size_t pos = input_string.find(to_remove);

        if (pos != std::string::npos)
        {
            input_string.erase(pos, to_remove.length());
        }
        else
        {
            done = true;
        }
    }

    return input_string;
}

std::string uint64_t_to_hex_string(uint64_t bytes8)
{
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << std::uppercase << bytes8;
    return ss.str();
}

void xor_data(char* input_data, uint32_t input_data_length)
{
    unsigned char xor_array[] = { 0xDC, 0x45, 0xA6, 0x9C, 0xD3, 0x72, 0x4C, 0xAB };
    int xor_length = sizeof(xor_array);

    for (uint64_t i = 0; i < input_data_length; i++)
    {
        input_data[i] ^= xor_array[i % xor_length];
    }
}

void xtea_encrypt_locr(uint32_t v[2])
{
    const uint32_t key[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    v[0] = v0; v[1] = v1;
}

void xtea_decrypt_locr(uint32_t v[2])
{
    const uint32_t key[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = 0xC6EF3720;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v1 -= (uint32_t)((uint32_t)((v0 << 4) ^ (v0 >> 5)) + v0 ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
        sum -= (uint32_t)delta;
        v0 -= (uint32_t)((uint32_t)((v1 << 4) ^ (v1 >> 5)) + v1 ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
    }
    v[0] = v0; v[1] = v1;
}

void xtea_encrypt_rtlv(uint32_t v[2])
{
    const uint32_t key[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    v[0] = v0; v[1] = v1;
}

void xtea_decrypt_rtlv(uint32_t v[2])
{
    const uint32_t key[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = 0xC6EF3720;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v1 -= (uint32_t)((uint32_t)((v0 << 4) ^ (v0 >> 5)) + v0 ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
        sum -= (uint32_t)delta;
        v0 -= (uint32_t)((uint32_t)((v1 << 4) ^ (v1 >> 5)) + v1 ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
    }
    v[0] = v0; v[1] = v1;
}

void xtea_encrypt_dlge(uint32_t v[2])
{
    const uint32_t key[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    v[0] = v0; v[1] = v1;
}

void xtea_decrypt_dlge(uint32_t v[2])
{
    const uint32_t key[4] = { 0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268 };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = 0xC6EF3720;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v1 -= (uint32_t)((uint32_t)((v0 << 4) ^ (v0 >> 5)) + v0 ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
        sum -= (uint32_t)delta;
        v0 -= (uint32_t)((uint32_t)((v1 << 4) ^ (v1 >> 5)) + v1 ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
    }
    v[0] = v0; v[1] = v1;
}

void xtea_encrypt_pd(uint32_t v[2])
{
    const uint32_t key[4] = { 0x30F95282, 0x1F48C419, 0x295F8548, 0x2A78366D };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x61C88647;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v0 += (uint32_t)((uint32_t)(((v1 << 4) ^ (v1 >> 5)) + v1) ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
        sum -= delta;
        v1 += (uint32_t)((uint32_t)(((v0 << 4) ^ (v0 >> 5)) + v0) ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
    }
    v[0] = v0; v[1] = v1;
}

void xtea_decrypt_pd(uint32_t v[2])
{
    const uint32_t key[4] = { 0x30F95282, 0x1F48C419, 0x295F8548, 0x2A78366D };
    unsigned int num_rounds = 32;
    uint32_t v0 = v[0], v1 = v[1], delta = 0x61C88647, sum = 0xC6EF3720;
    for (uint64_t i = 0; i < num_rounds; i++) {
        v1 -= (uint32_t)((uint32_t)(((v0 << 4) ^ (v0 >> 5)) + v0) ^ (uint32_t)((uint32_t)sum + key[(sum >> 11) & 3]));
        sum += (uint32_t)delta;
        v0 -= (uint32_t)((uint32_t)(((v1 << 4) ^ (v1 >> 5)) + v1) ^ (uint32_t)((uint32_t)sum + key[sum & 3]));
    }
    v[0] = v0; v[1] = v1;
}

void parse_rpkg_file_input(std::string input, main_variables* main_data)
{
    main_data->input_rpkg_file_path = remove_all_string_from_string(replace_slashes(input), "\"");
    main_data->input_rpkg_file_path = remove_all_string_from_string(main_data->input_rpkg_file_path, "\'");

    if (main_data->input_rpkg_file_path.substr(main_data->input_rpkg_file_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_file_path = main_data->input_rpkg_file_path.substr(0, main_data->input_rpkg_file_path.length() - 1);
    }

    size_t pos = main_data->input_rpkg_file_path.find_last_of("\\/");

    if (pos != std::string::npos)
    {
        main_data->input_rpkg_file_name = main_data->input_rpkg_file_path.substr(pos + 1, main_data->input_rpkg_file_path.length() - (pos + 1));
    }
    else
    {
        main_data->input_rpkg_file_name = main_data->input_rpkg_file_path;
    }

    if (to_uppercase(main_data->input_rpkg_file_name.substr((main_data->input_rpkg_file_name.length() - 5), 5)) != ".RPKG")
    {
        std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
        std::exit(0);
    }
}

void parse_packagedefinitions_thumbs_file_input(std::string input, main_variables* main_data)
{
    main_data->input_packagedefinitions_thumbs_file_path = remove_all_string_from_string(replace_slashes(input), "\"");
    main_data->input_packagedefinitions_thumbs_file_path = remove_all_string_from_string(main_data->input_packagedefinitions_thumbs_file_path, "\'");

    if (main_data->input_packagedefinitions_thumbs_file_path.substr(main_data->input_packagedefinitions_thumbs_file_path.length() - 1, 1) == "/")
    {
        main_data->input_packagedefinitions_thumbs_file_path = main_data->input_packagedefinitions_thumbs_file_path.substr(0, main_data->input_packagedefinitions_thumbs_file_path.length() - 1);
    }

    size_t pos = main_data->input_packagedefinitions_thumbs_file_path.find_last_of("\\/");

    if (pos != std::string::npos)
    {
        main_data->input_packagedefinitions_thumbs_file_name = main_data->input_packagedefinitions_thumbs_file_path.substr(pos + 1, main_data->input_packagedefinitions_thumbs_file_path.length() - (pos + 1));
    }
    else
    {
        main_data->input_packagedefinitions_thumbs_file_name = main_data->input_packagedefinitions_thumbs_file_path;
    }
}

void parse_command_json_file_input(std::string input, main_variables* main_data)
{
    main_data->input_command_json_file_path = remove_all_string_from_string(replace_slashes(input), "\"");
    main_data->input_command_json_file_path = remove_all_string_from_string(main_data->input_command_json_file_path, "\'");

    if (main_data->input_command_json_file_path.substr(main_data->input_command_json_file_path.length() - 1, 1) == "/")
    {
        main_data->input_command_json_file_path = main_data->input_command_json_file_path.substr(0, main_data->input_command_json_file_path.length() - 1);
    }

    size_t pos = main_data->input_command_json_file_path.find_last_of("\\/");

    if (pos != std::string::npos)
    {
        main_data->input_command_json_file_name = main_data->input_command_json_file_path.substr(pos + 1, main_data->input_command_json_file_path.length() - (pos + 1));
    }
    else
    {
        main_data->input_command_json_file_name = main_data->input_command_json_file_path;
    }

    if (to_uppercase(main_data->input_command_json_file_name.substr((main_data->input_command_json_file_name.length() - 5), 5)) != ".JSON")
    {
        std::cout << main_data->console_prefix << "Error: Invalid rpkg.exe command JSON file path." << std::endl;
        std::exit(0);
    }
}

void process_command_line(int argc, char* argv[], main_variables* main_data)
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i] == std::string("-extract_from_rpkg"))
            {
                main_data->mode_extract_from_rpkg = true;

                if (argc > (i + 1))
                {
                    parse_rpkg_file_input(std::string(argv[i + 1]), main_data);

                    if (argv[i + 1][0] == '-' || main_data->input_rpkg_file_path == "" || main_data->input_rpkg_file_name == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-filter"))
            {
                main_data->mode_filter = true;

                if (argc > (i + 1))
                {
                    if (argv[i + 1] != "-" && argv[i + 1] != "")
                    {
                        std::string input = std::string(argv[i + 1]);

                        std::smatch m;
                        std::regex re("^([^, ]+)[, ]+");
                        std::regex_search(input, m, re);

                        if (m.size() > 0)
                        {
                            main_data->input_filter_string = input;

                            //std::cout << main_data->console_prefix << m[1].str() << std::endl;

                            main_data->input_filter.push_back(m[1].str());

                            std::smatch m2;
                            re.assign("[, ]+([^, ]+)");

                            while (std::regex_search(input, m2, re))
                            {
                                main_data->input_filter.push_back(m2[1].str());

                                input = m2.suffix().str();

                                //std::cout << main_data->console_prefix << m2[1].str() << std::endl;
                            }
                        }
                        else
                        {
                            main_data->input_filter_string = input;

                            main_data->input_filter.push_back(input);
                        }
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid filter." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-search_rpkg"))
            {
                if (argc > (i + 1))
                {
                    parse_rpkg_file_input(std::string(argv[i + 1]), main_data);

                    if (argv[i + 1][0] == '-' || main_data->input_rpkg_file_path == "" || main_data->input_rpkg_file_name == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-compute_ioi_hash"))
            {
                main_data->mode_compute_ioi_hash = true;

                if (argc > (i + 1))
                {
                    main_data->input_to_ioi_hash = argv[i + 1];

                    if (main_data->input_to_ioi_hash[0] == '-' || main_data->input_to_ioi_hash == "")
                    {
                        std::cout << main_data->console_prefix << "Error: No input string to hash specified." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: No input string to hash specified." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-rpkg_command_json"))
            {
                main_data->mode_command_json = true;

                if (argc > (i + 1))
                {
                    parse_command_json_file_input(std::string(argv[i + 1]), main_data);

                    if (argv[i + 1][0] == '-' || main_data->input_command_json_file_path == "" || main_data->input_command_json_file_name == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid rpkg.exe command JSON file path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid rpkg.exe command JSON file path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-decrypt_packagedefinition_thumbs") || argv[i] == std::string("-encrypt_packagedefinition_thumbs"))
            {
                if (argv[i] == std::string("-decrypt_packagedefinition_thumbs"))
                {
                    main_data->mode_decrypt_packagedefinition_thumbs = true;
                }
                else
                {
                    main_data->mode_encrypt_packagedefinition_thumbs = true;
                }

                if (argc > (i + 1))
                {
                    parse_packagedefinitions_thumbs_file_input(std::string(argv[i + 1]), main_data);

                    if (argv[i + 1][0] == '-' || main_data->input_packagedefinitions_thumbs_file_path == "" || main_data->input_packagedefinitions_thumbs_file_name == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid packagedefinitions.txt / thumbs.dat file path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid packagedefinitions.txt / thumbs.dat file path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-text_search"))
            {
                main_data->mode_text_search = true;

                if (argc > (i + 1))
                {
                    main_data->input_text_search = argv[i + 1];

                    if (main_data->input_text_search[0] == '-' || main_data->input_text_search == "")
                    {
                        std::cout << main_data->console_prefix << "Error: No text search string specified." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: No text search string specified." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-hex_search"))
            {
                main_data->mode_hex_search = true;

                if (argc > (i + 1))
                {
                    main_data->input_hex_search = argv[i + 1];

                    if (main_data->input_hex_search[0] == '-' || main_data->input_hex_search == "")
                    {
                        std::cout << main_data->console_prefix << "Error: No hex search string specified." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: No hex search string specified." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-regex_search"))
            {
                main_data->mode_regex_search = true;

                if (argc > (i + 1))
                {
                    main_data->input_regex_search = argv[i + 1];

                    if (main_data->input_regex_search[0] == '-' || main_data->input_regex_search == "")
                    {
                        std::cout << main_data->console_prefix << "Error: No regex search string specified." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: No regex search string specified." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-hash_depends"))
            {
                main_data->mode_hash_depends = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-hash_probe"))
            {
                main_data->mode_hash_probe = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-generate_rpkg_from"))
            {
                main_data->mode_generate_rpkg_file = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-extract_all_ores_from"))
            {
                main_data->mode_extract_all_ores = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-extract_all_wwev_to_ogg_from"))
            {
                main_data->mode_extract_all_wwev_to_ogg = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-extract_all_wwes_to_ogg_from"))
            {
                main_data->mode_extract_all_wwes_to_ogg = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-extract_locr_to_json_from"))
            {
                main_data->mode_locr_to_json = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-rebuild_locr_from_json_from"))
            {
                main_data->mode_json_to_locr = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-extract_rtlv_to_json_from"))
            {
                main_data->mode_rtlv_to_json = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-rebuild_rtlv_from_json_from"))
            {
                main_data->mode_json_to_rtlv = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-extract_dlge_to_json_from"))
            {
                main_data->mode_dlge_to_json = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-rebuild_dlge_from_json_from"))
            {
                main_data->mode_json_to_dlge = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-rebuild_all_wwev_in"))
            {
                main_data->mode_rebuild_all_wwev = true;

                if (argc > (i + 1))
                {
                    main_data->input_rpkg_folder_path = argv[i + 1];

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG folder path." << std::endl;
                    std::exit(0);
                }
            }

            if (argv[i] == std::string("-output_path"))
            {
                main_data->mode_output_path = true;

                main_data->input_output_path = argv[i + 1];

                if (argc > (i + 1))
                {
                    if (main_data->input_output_path != "-" && main_data->input_output_path != "")
                    {
                        main_data->input_output_path = remove_all_string_from_string(main_data->input_output_path, "\"");
                        main_data->input_output_path = remove_all_string_from_string(main_data->input_output_path, "\'");
                        main_data->input_output_path = replace_slashes(main_data->input_output_path);

                        if (main_data->input_output_path.substr(main_data->input_output_path.length() - 1, 1) == "\\")
                        {
                            main_data->input_output_path = main_data->input_output_path.substr(0, main_data->input_output_path.length() - 1);
                        }

                        if (main_data->input_output_path.substr(main_data->input_output_path.length() - 1, 1) == "/")
                        {
                            main_data->input_output_path = main_data->input_output_path.substr(0, main_data->input_output_path.length() - 1);
                        }

                        //std::cout << main_data->console_prefix << "input: " << main_data->input_output_path << std::endl;

                        if (!path_exists(main_data->input_output_path))
                        {
                            std::vector<std::string> output_file_path_elements;

                            //std::cout << main_data->console_prefix << "does not exist: " << main_data->input_output_path << std::endl;

                            std::smatch m;
                            std::regex re("^([^\\/]+)\\/");
                            std::regex_search(main_data->input_output_path, m, re);

                            if (m.size() > 0)
                            {
                                output_file_path_elements.push_back(m[1].str());

                                std::smatch m2;
                                re.assign("\\/([^\\/]+)");
                                std::string output_path = main_data->input_output_path;

                                while (std::regex_search(output_path, m2, re))
                                {
                                    output_file_path_elements.push_back(m2[1].str());

                                    output_path = m2.suffix().str();
                                }
                            }
                            else
                            {
                                output_file_path_elements.push_back(main_data->input_output_path);
                            }

                            std::string path = "";

                            for (uint64_t l = 0; l < output_file_path_elements.size(); l++)
                            {
                                path.append(output_file_path_elements.at(l));

                                if (!path_exists(path))
                                {
                                    if (!std::filesystem::create_directories(path))
                                    {
                                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << path << std::endl;
                                        std::exit(0);
                                    }
                                }

                                path.append("/");

                                //std::cout << main_data->console_prefix << "directory: " << path << std::endl;
                            }
                        }

                        main_data->input_output_path.append("/");
                    }
                    else
                    {
                        std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
                    std::exit(0);
                }
            }
        }

        if (main_data->mode_extract_from_rpkg && main_data->mode_search_rpkg)
        {
            std::cout << main_data->console_prefix << "Error: RPKG extraction and search can not be used together." << std::endl;
            std::exit(0);
        }

        if (main_data->mode_extract_from_rpkg && main_data->mode_generate_rpkg_file)
        {
            std::cout << main_data->console_prefix << "Error: RPKG extraction and generate can not be used together." << std::endl;
            std::exit(0);
        }

        if (main_data->mode_hash_depends && !main_data->mode_filter)
        {
            std::cout << main_data->console_prefix << "Error: Filter (-filter) must be specified when using hash depends mode." << std::endl;
            std::exit(0);
        }

        if (main_data->mode_hash_probe && !main_data->mode_filter)
        {
            std::cout << main_data->console_prefix << "Error: Filter (-filter) must be specified when using hash probe mode." << std::endl;
            std::exit(0);
        }
    }
}

void import_rpkg_file(main_variables* main_data, std::string rpkg_file_path, std::string rpkg_file_name, bool continue_on_failure)
{
    std::ifstream file = std::ifstream(rpkg_file_path, std::ifstream::binary);
    char input[1024];
    char bytes1 = 0;
    uint32_t bytes4 = 0, file_count, table_offset, table_size, patch_entry_count;
    uint64_t bytes8 = 0, offset1 = 0, offset2 = 0, fileSize = 0;
    std::string value;
    std::string message = main_data->console_prefix + "Importing RPKG file data: ";

    if (!file.good())
    {
        std::cout << main_data->console_prefix << "Error: RPKG file " << rpkg_file_path << " could not be read." << std::endl;
        std::exit(0);
    }

    file.seekg(0, file.end);
    fileSize = file.tellg();
    file.seekg(0, file.beg);

    if (fileSize <= 0x4)
    {
        if (continue_on_failure)
        {
            std::cout << main_data->console_prefix << "Error: " << rpkg_file_path << " is not a valid RPKG file." << std::endl;
            return;
        }
        else
        {
            std::cout << main_data->console_prefix << "Error: " << rpkg_file_path << " is not a valid RPKG file." << std::endl;
            std::exit(0);
        }
    }

    rpkg_variables temp_rpkg_data;
    temp_rpkg_data.rpkg_file_name = rpkg_file_name;
    temp_rpkg_data.rpkg_file_path = rpkg_file_path;

    file.read(input, 4);
    input[4] = 0;

    if (std::string(input) == "GKPR")
    {
        temp_rpkg_data.rpkg_file_version = 1;

        std::cout << main_data->console_prefix << "Valid RPKGv1 file magic signature found." << std::endl;
    }
    else if (std::string(input) == "2KPR")
    {
        temp_rpkg_data.rpkg_file_version = 2;

        std::cout << main_data->console_prefix << "Valid RPKGv2 file magic signature found." << std::endl;
    }
    else
    {
        if (continue_on_failure)
        {
            std::cout << main_data->console_prefix << "Error: " << main_data->input_rpkg_file_path << " is not a valid RPKG file." << std::endl;
            return;
        }
        else
        {
            std::cout << main_data->console_prefix << "Error: " << main_data->input_rpkg_file_path << " is not a valid RPKG file." << std::endl;
            std::exit(0);
        }
    }

    if ((temp_rpkg_data.rpkg_file_version == 1 && fileSize <= 0x14) || (temp_rpkg_data.rpkg_file_version == 2 && fileSize <= 0x1D))
    {
        if (continue_on_failure)
        {
            std::cout << main_data->console_prefix << "Error: " << main_data->input_rpkg_file_path << " is a empty RPKG file." << std::endl;
            return;
        }
        else
        {
            std::cout << main_data->console_prefix << "Error: " << main_data->input_rpkg_file_path << " is a empty RPKG file." << std::endl;
            std::exit(0);
        }
    }

    if (temp_rpkg_data.rpkg_file_version == 2)
    {
        file.read(input, sizeof(bytes4));

        file.read(input, sizeof(bytes1));
        std::memcpy(&bytes1, input, sizeof(bytes1));
        temp_rpkg_data.rpkgv2_chunk_number = bytes1;

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "rpkgv2_chunk_number: " << bytes1 << std::endl;
        }

        file.read(input, sizeof(bytes4));
    }

    file.read(input, sizeof(file_count));
    std::memcpy(&file_count, input, sizeof(file_count));
    temp_rpkg_data.rpkg_file_count = file_count;

    if (main_data->debug)
    {
        std::cout << main_data->console_prefix << "file_count: " << file_count << std::endl;
    }

    file.read(input, sizeof(table_offset));
    std::memcpy(&table_offset, input, sizeof(table_offset));
    temp_rpkg_data.rpkg_table_offset = table_offset;

    if (main_data->debug)
    {
        std::cout << main_data->console_prefix << "table_offset: " << table_offset << std::endl;
    }

    file.read(input, sizeof(table_size));
    std::memcpy(&table_size, input, sizeof(table_size));
    temp_rpkg_data.rpkg_table_size = table_size;

    if (main_data->debug)
    {
        std::cout << main_data->console_prefix << "table_size: " << table_size << std::endl;
    }

    uint64_t position = file.tellg();

    file.read(input, sizeof(patch_entry_count));
    std::memcpy(&patch_entry_count, input, sizeof(patch_entry_count));

    temp_rpkg_data.patch_entry_count = patch_entry_count;

    if (main_data->debug)
    {
        std::cout << main_data->console_prefix << "patch_entry_count: " << patch_entry_count << std::endl;
        std::cout << main_data->console_prefix << "file.tellg(): " << file.tellg() << std::endl;
    }

    if (temp_rpkg_data.rpkg_file_version == 1 && ((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14 + (uint64_t)0x10) >= fileSize)
    {
        temp_rpkg_data.is_patch_file = false;

        std::cout << main_data->console_prefix << "RPKGv1 file " << temp_rpkg_data.rpkg_file_name << " is not a patch file." << std::endl;
    }
    else if (temp_rpkg_data.rpkg_file_version == 2 && ((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D + (uint64_t)0x10) >= fileSize)
    {
        temp_rpkg_data.is_patch_file = false;

        std::cout << main_data->console_prefix << "RPKGv2 file " << temp_rpkg_data.rpkg_file_name << " is not a patch file." << std::endl;
    }
    else
    {
        char patchZeroTest = 0;
        uint64_t patchValue = 0;

        if (temp_rpkg_data.rpkg_file_version == 1)
        {
            file.seekg(((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14), file.beg);
        }
        else
        {
            file.seekg(((uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D), file.beg);
        }

        file.read(input, 0x7);
        file.read(input, sizeof(bytes1));
        std::memcpy(&patchZeroTest, input, sizeof(bytes1));
        file.read(input, sizeof(bytes8));
        std::memcpy(&patchValue, input, sizeof(bytes8));

        if (temp_rpkg_data.rpkg_file_version == 1 && patchValue == ((uint64_t)table_offset + (uint64_t)table_size + (uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14) && patchZeroTest == 0x0)
        {
            temp_rpkg_data.is_patch_file = true;

            std::cout << main_data->console_prefix << "RPKGv1 file " << temp_rpkg_data.rpkg_file_name << " is a patch file." << std::endl;
        }
        else if (temp_rpkg_data.rpkg_file_version == 2 && patchValue == ((uint64_t)table_offset + (uint64_t)table_size + (uint64_t)patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D) && patchZeroTest == 0x0)
        {
            temp_rpkg_data.is_patch_file = true;

            std::cout << main_data->console_prefix << "RPKGv2 file " << temp_rpkg_data.rpkg_file_name << " is a patch file." << std::endl;
        }
        else
        {
            temp_rpkg_data.is_patch_file = false;

            if (temp_rpkg_data.rpkg_file_version == 1)
            {
                std::cout << main_data->console_prefix << "RPKGv1 file " << temp_rpkg_data.rpkg_file_name << " is not a patch file." << std::endl;
            }
            else
            {
                std::cout << main_data->console_prefix << "RPKGv2 file " << temp_rpkg_data.rpkg_file_name << " is not a patch file." << std::endl;
            }
        }
    }

    file.seekg(position, file.beg);

    std::unique_ptr<char[]> input_file_data;
    uint64_t input_file_size = 0;

    if (temp_rpkg_data.is_patch_file)
    {
        if (temp_rpkg_data.rpkg_file_version == 1)
        {
            std::cout << main_data->console_prefix << "Importing index from RPKGv1 file: " << temp_rpkg_data.rpkg_file_name << std::endl;
        }
        else
        {
            std::cout << main_data->console_prefix << "Importing index from RPKGv2 file: " << temp_rpkg_data.rpkg_file_name << std::endl;
        }

        file.read(input, sizeof(patch_entry_count));
        std::memcpy(&patch_entry_count, input, sizeof(patch_entry_count));
        temp_rpkg_data.patch_entry_count = patch_entry_count;

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "patch_entry_count: " << patch_entry_count << std::endl;
            std::cout << main_data->console_prefix << "file.tellg(): " << file.tellg() << std::endl;
        }

        for (uint64_t i = 0; i < patch_entry_count; i++)
        {
            file.read(input, sizeof(bytes8));
            std::memcpy(&bytes8, input, sizeof(bytes8));
            temp_rpkg_data.patch_entry_list.push_back(bytes8);

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "patchEntryList(" << i << "): " << bytes8 << std::endl;
            }
        }

        if (temp_rpkg_data.rpkg_file_version == 1)
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x14 + (uint64_t)patch_entry_count * (uint64_t)0x8;
        }
        else
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x1D + (uint64_t)patch_entry_count * (uint64_t)0x8;
        }
    }
    else
    {
        if (temp_rpkg_data.rpkg_file_version == 1)
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x10;

            std::cout << main_data->console_prefix << "Importing index from RPKGv1 file: " << temp_rpkg_data.rpkg_file_name << std::endl;
        }
        else
        {
            input_file_size = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x19;

            std::cout << main_data->console_prefix << "Importing index from RPKGv2 file: " << temp_rpkg_data.rpkg_file_name << std::endl;
        }
    }

    if (main_data->debug)
    {
        std::cout << main_data->console_prefix << "input_file_size: " << input_file_size << std::endl;
    }

    position = (uint64_t)file.tellg();

    offset2 = position + table_offset;

    input_file_data = std::make_unique<char[]>(input_file_size);

    file.seekg(0x0, file.beg);

    file.read(input_file_data.get(), input_file_size);

    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
    int stringstream_length = 80;

    for (uint64_t i = 0; i < file_count; i++)
    {
        if (((i * (uint64_t)100000) / (uint64_t)file_count) % (uint64_t)100 == 0 && i > 0)
        {
            stringstream_length = update_console(message, file_count, i, start_time, stringstream_length);
        }

        std::memcpy(&bytes8, (input_file_data.get() + position), sizeof(bytes8));
        position += sizeof(bytes8);
        value = uint64_t_to_hex_string(bytes8);
        temp_rpkg_data.hash.push_back(bytes8);
        temp_rpkg_data.hash_string.push_back(value);
        temp_rpkg_data.hash_map[bytes8] = (int)(temp_rpkg_data.hash.size() - 1);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "hash: " << value << std::endl;
        }

        std::memcpy(&bytes8, (input_file_data.get() + position), sizeof(bytes8));
        position += sizeof(bytes8);
        temp_rpkg_data.hash_offset.push_back(bytes8);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "hashOffset: " << bytes8 << std::endl;
        }

        std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        temp_rpkg_data.hash_size.push_back(bytes4);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "hashSize: " << bytes4 << std::endl;
        }

        if ((temp_rpkg_data.hash_size.back() & 0x3FFFFFFF) != 0)
        {
            temp_rpkg_data.is_lz4ed.push_back(true);

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "LZ4ed." << std::endl;
                std::cout << main_data->console_prefix << "hashSize: " << temp_rpkg_data.hash_size.back() << std::endl;
            }
        }
        else
        {
            temp_rpkg_data.is_lz4ed.push_back(false);

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "Not LZ4ed." << std::endl;
                std::cout << main_data->console_prefix << "hashSize: " << temp_rpkg_data.hash_size.back() << std::endl;
            }
        }

        if ((temp_rpkg_data.hash_size.back() & 0x80000000) == 0x80000000)
        {
            temp_rpkg_data.is_xored.push_back(true);

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "XORed." << std::endl;
                std::cout << main_data->console_prefix << "hashSize: " << temp_rpkg_data.hash_size.back() << std::endl;
            }
        }
        else
        {
            temp_rpkg_data.is_xored.push_back(false);

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "Not XORed." << std::endl;
                std::cout << main_data->console_prefix << "hashSize: " << temp_rpkg_data.hash_size.back() << std::endl;
            }
        }

        offset1 = position;

        position = offset2;

        std::memcpy(&input, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        input[4] = 0;
        value = std::string(input);
        std::reverse(value.begin(), value.end());
        temp_rpkg_data.hash_resource_type.push_back(value);
        temp_rpkg_data.hash_file_name.push_back(temp_rpkg_data.hash_string.back() + "." + temp_rpkg_data.hash_resource_type.back());

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "fileType: " << temp_rpkg_data.hash_file_name.back() << std::endl;
        }

        std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        temp_rpkg_data.hash_reference_table_size.push_back(bytes4);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "chunkSize1: " << bytes4 << std::endl;
        }

        std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        temp_rpkg_data.hash_reference_table_dummy.push_back(bytes4);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "chunkSize2: " << bytes4 << std::endl;
        }

        std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        temp_rpkg_data.hash_size_final.push_back(bytes4);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "sizeFinal: " << bytes4 << std::endl;
        }

        std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        temp_rpkg_data.hash_size_in_memory.push_back(bytes4);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "sizeMem: " << bytes4 << std::endl;
        }

        std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
        position += sizeof(bytes4);
        temp_rpkg_data.hash_size_in_video_memory.push_back(bytes4);

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "sizeVideoMem: " << bytes4 << std::endl;
        }

        hash_reference_variables temp_hash_reference_data;

        if (temp_rpkg_data.hash_reference_table_size.back() != 0x0)
        {
            temp_hash_reference_data.hash = temp_rpkg_data.hash.back();

            std::memcpy(&bytes4, (input_file_data.get() + position), sizeof(bytes4));
            position += sizeof(bytes4);
            temp_hash_reference_data.hash_reference_count = bytes4;

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "hashLinkCount: " << bytes4 << std::endl;
            }

            uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

            for (uint64_t i = 0; i < temp_hash_reference_count; i++)
            {
                std::memcpy(&bytes1, (input_file_data.get() + position), sizeof(bytes1));
                position += sizeof(bytes1);
                temp_hash_reference_data.hash_reference_type.push_back(bytes1);

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "hashLinkTypes(" << i << "): " << bytes1 << std::endl;
                }
            }

            for (uint64_t i = 0; i < temp_hash_reference_count; i++)
            {
                std::memcpy(&bytes8, (input_file_data.get() + position), sizeof(bytes8));
                position += sizeof(bytes8);
                temp_hash_reference_data.hash_reference.push_back(bytes8);

                value = uint64_t_to_hex_string(bytes8);
                temp_hash_reference_data.hash_reference_string.push_back(value);

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "hashLinks(" << i << "): " << bytes8 << std::endl;
                    std::cout << main_data->console_prefix << "hashLinkNames(" << i << "): " << value << std::endl;
                }
            }

            offset2 = position;
        }
        else
        {
            temp_hash_reference_data.hash_reference_count = 0x0;

            offset2 = position;

            offset2 += temp_rpkg_data.hash_reference_table_size.back();
        }

        temp_rpkg_data.hash_reference_data.push_back(temp_hash_reference_data);

        position = offset1;
    }

    main_data->rpkg_data.push_back(temp_rpkg_data);

    file.close();

    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

    std::stringstream ss;

    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

    std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
}

void import_rpkg_file_if_not_already(main_variables* main_data, std::string rpkg_file_path, std::string rpkg_file_name, bool continue_on_failure)
{
    bool found = false;

    for (uint64_t j = 0; j < main_data->rpkg_data.size(); j++)
    {
        if (main_data->rpkg_data.at(j).rpkg_file_name == rpkg_file_name)
        {
            found = true;
        }
    }

    if (!found)
    {
        import_rpkg_file(main_data, rpkg_file_path, rpkg_file_name, continue_on_failure);
    }
}

void generate_rpkg_file(main_variables* main_data)
{
    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::string rpkg_file_name;
        std::string rpkg_meta_file_name;

        uint64_t position = 0;
        uint8_t bytes1 = 0;
        uint32_t bytes4 = 0;
        uint64_t bytes8 = 0;
        char input[1024];

        std::string base_folder_name = "";

        std::size_t pos = main_data->input_rpkg_folder_path.find_last_of("\\/");

        if (pos == (main_data->input_rpkg_folder_path.length() - 1))
        {
            main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);

            pos = main_data->input_rpkg_folder_path.find_last_of("\\/");

            if (pos != std::string::npos)
            {
                base_folder_name = main_data->input_rpkg_folder_path.substr(pos + 1, main_data->input_rpkg_folder_path.length() - (pos + 1));
                rpkg_file_name = base_folder_name + ".rpkg";
                rpkg_meta_file_name = base_folder_name + ".meta";
            }
            else
            {
                rpkg_file_name = main_data->input_rpkg_folder_path + ".rpkg";
                rpkg_meta_file_name = main_data->input_rpkg_folder_path + ".meta";
            }
        }
        else
        {
            if (pos != std::string::npos)
            {
                base_folder_name = main_data->input_rpkg_folder_path.substr(pos + 1, main_data->input_rpkg_folder_path.length() - (pos + 1));
                rpkg_file_name = base_folder_name + ".rpkg";
                rpkg_meta_file_name = base_folder_name + ".meta";
            }
            else
            {
                rpkg_file_name = main_data->input_rpkg_folder_path + ".rpkg";
                rpkg_meta_file_name = main_data->input_rpkg_folder_path + ".meta";
            }
        }

        uint64_t rpkg_meta_file_size = 0;
        rpkg_variables rpkg_meta_data;

        std::string rpkg_meta_file_path = replace_slashes(main_data->input_rpkg_folder_path);
        rpkg_meta_file_path.append("/" + rpkg_meta_file_name);

        rpkg_meta_data.is_patch_file = false;

        bool use_rpkg_file_meta_data = false;

        rpkg_meta_data.rpkg_file_version = 1;

        if (path_exists(rpkg_meta_file_path))
        {
            std::cout << main_data->console_prefix << "Using RPKG meta file: " << rpkg_meta_file_name << std::endl;

            std::ifstream rpkg_meta_file = std::ifstream(rpkg_meta_file_path, std::ifstream::binary);

            if (!rpkg_meta_file.good())
            {
                std::cout << main_data->console_prefix << "Error: RPKG meta file " << rpkg_meta_file_path << " could not be read." << std::endl;
                std::exit(0);
            }

            use_rpkg_file_meta_data = true;

            rpkg_meta_file.seekg(0, rpkg_meta_file.end);
            rpkg_meta_file_size = rpkg_meta_file.tellg();
            rpkg_meta_file.seekg(0, rpkg_meta_file.beg);

            rpkg_meta_file.read(input, 4);
            input[4] = 0;

            if (std::string(input) == "GKPR")
            {
                rpkg_meta_data.rpkg_file_version = 1;

                std::cout << main_data->console_prefix << "Valid RPKGv1 file magic signature found." << std::endl;
            }
            else if (std::string(input) == "2KPR")
            {
                rpkg_meta_data.rpkg_file_version = 2;

                std::cout << main_data->console_prefix << "Valid RPKGv2 file magic signature found." << std::endl;
            }
            else
            {
                std::cout << main_data->console_prefix << "Error: " << rpkg_meta_file_path << " is not a valid RPKG file." << std::endl;
                std::exit(0);
            }

            if (rpkg_meta_data.rpkg_file_version == 2)
            {
                rpkg_meta_file.read(input, sizeof(bytes4));

                rpkg_meta_file.read(input, sizeof(bytes1));
                std::memcpy(&bytes1, input, sizeof(bytes1));
                rpkg_meta_data.rpkgv2_chunk_number = bytes1;

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "rpkgv2_chunk_number: " << bytes1 << std::endl;
                }

                rpkg_meta_file.read(input, sizeof(bytes4));
            }

            rpkg_meta_file.read(input, sizeof(bytes4));
            std::memcpy(&rpkg_meta_data.rpkg_file_count, input, sizeof(bytes4));

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "rpkg_meta_data.rpkg_file_count: " << rpkg_meta_data.rpkg_file_count << std::endl;
            }

            rpkg_meta_file.read(input, sizeof(bytes4));
            std::memcpy(&rpkg_meta_data.rpkg_table_offset, input, sizeof(bytes4));

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "rpkg_meta_data.rpkg_table_offset: " << rpkg_meta_data.rpkg_table_offset << std::endl;
            }

            rpkg_meta_file.read(input, sizeof(bytes4));
            std::memcpy(&rpkg_meta_data.rpkg_table_size, input, sizeof(bytes4));

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "rpkg_meta_data.rpkg_table_size: " << rpkg_meta_data.rpkg_table_size << std::endl;
            }

            uint64_t position = rpkg_meta_file.tellg();

            rpkg_meta_file.read(input, sizeof(bytes4));
            std::memcpy(&rpkg_meta_data.patch_entry_count, input, sizeof(bytes4));

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "patch_entry_count: " << rpkg_meta_data.patch_entry_count << std::endl;
                std::cout << main_data->console_prefix << "file.tellg(): " << rpkg_meta_file.tellg() << std::endl;
            }

            if (rpkg_meta_data.rpkg_file_version == 1 && ((uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14 + (uint64_t)0x10) >= rpkg_meta_file_size)
            {
                rpkg_meta_data.is_patch_file = false;

                std::cout << main_data->console_prefix << "RPKGv1 file " << rpkg_meta_file_path << " is not a patch file." << std::endl;
            }
            else if (rpkg_meta_data.rpkg_file_version == 2 && ((uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D + (uint64_t)0x10) >= rpkg_meta_file_size)
            {
                rpkg_meta_data.is_patch_file = false;

                std::cout << main_data->console_prefix << "RPKGv2 file " << rpkg_meta_file_path << " is not a patch file." << std::endl;
            }
            else
            {
                char patchZeroTest = 0;
                uint64_t patchValue = 0;

                if (rpkg_meta_data.rpkg_file_version == 1)
                {
                    rpkg_meta_file.seekg(((uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14), rpkg_meta_file.beg);
                }
                else
                {
                    rpkg_meta_file.seekg(((uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D), rpkg_meta_file.beg);
                }

                rpkg_meta_file.read(input, 0x7);
                rpkg_meta_file.read(input, sizeof(bytes1));
                std::memcpy(&patchZeroTest, input, sizeof(bytes1));
                rpkg_meta_file.read(input, sizeof(bytes8));
                std::memcpy(&patchValue, input, sizeof(bytes8));

                if (rpkg_meta_data.rpkg_file_version == 1 && patchValue == ((uint64_t)rpkg_meta_data.rpkg_table_offset + (uint64_t)rpkg_meta_data.rpkg_table_size + (uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14) && patchZeroTest == 0x0)
                {
                    rpkg_meta_data.is_patch_file = true;

                    std::cout << main_data->console_prefix << "RPKGv1 file " << rpkg_meta_file_path << " is a patch file." << std::endl;
                }
                else if (rpkg_meta_data.rpkg_file_version == 2 && patchValue == ((uint64_t)rpkg_meta_data.rpkg_table_offset + (uint64_t)rpkg_meta_data.rpkg_table_size + (uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D) && patchZeroTest == 0x0)
                {
                    rpkg_meta_data.is_patch_file = true;

                    std::cout << main_data->console_prefix << "RPKGv2 file " << rpkg_meta_file_path << " is a patch file." << std::endl;
                }
                else
                {
                    rpkg_meta_data.is_patch_file = false;

                    if (rpkg_meta_data.rpkg_file_version == 1)
                    {
                        std::cout << main_data->console_prefix << "RPKGv1 file " << rpkg_meta_file_path << " is not a patch file." << std::endl;
                    }
                    else
                    {
                        std::cout << main_data->console_prefix << "RPKGv2 file " << rpkg_meta_file_path << " is not a patch file." << std::endl;
                    }
                }
            }

            rpkg_meta_file.seekg(position, rpkg_meta_file.beg);

            if (rpkg_meta_data.is_patch_file)
            {
                rpkg_meta_file.read(input, sizeof(bytes4));
                std::memcpy(&rpkg_meta_data.patch_entry_count, input, sizeof(bytes4));

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "rpkg_meta_data.patch_entry_count: " << rpkg_meta_data.patch_entry_count << std::endl;
                }

                for (uint64_t i = 0; i < rpkg_meta_data.patch_entry_count; i++)
                {
                    rpkg_meta_file.read(input, sizeof(bytes8));
                    std::memcpy(&bytes8, input, sizeof(bytes8));
                    rpkg_meta_data.patch_entry_list.push_back(bytes8);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "rpkg_meta_data.patch_entry_list(" << i << "): " << rpkg_meta_data.patch_entry_list.back() << std::endl;
                    }
                }
            }

            for (uint64_t i = 0; i < rpkg_meta_data.rpkg_file_count; i++)
            {
                rpkg_meta_file.read(input, sizeof(bytes8));
                std::memcpy(&bytes8, input, sizeof(bytes8));
                rpkg_meta_data.hash.push_back(bytes8);
                rpkg_meta_data.hash_map[bytes8] = (int)(rpkg_meta_data.hash.size() - 1);

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "rpkg_meta_data.hash: " << rpkg_meta_data.hash.back() << std::endl;
                }

                rpkg_meta_file.read(input, sizeof(bytes8));
                std::memcpy(&bytes8, input, sizeof(bytes8));
                rpkg_meta_data.hash_offset.push_back(bytes8);

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "rpkg_meta_data.hash_offset: " << rpkg_meta_data.hash_offset.back() << std::endl;
                }

                rpkg_meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                rpkg_meta_data.hash_size.push_back(bytes4);

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "rpkg_meta_data.hash_size: " << rpkg_meta_data.hash_size.back() << std::endl;
                }
            }
        }
        else
        {
            rpkg_meta_data.is_patch_file = true;
            rpkg_meta_data.patch_entry_count = 0x0;

            std::cout << main_data->console_prefix << "RPKG meta file matching the name of the folder was not found" << std::endl;
            std::cout << main_data->console_prefix << "   in it's root:" << main_data->input_rpkg_folder_path << std::endl;
            std::cout << main_data->console_prefix << "Defaulting to RPKGv1 patch file creation settings:" << std::endl;
            std::cout << main_data->console_prefix << "  - Patch entry (deletion list) count is set to 0" << std::endl;
            std::cout << main_data->console_prefix << "  - No patch entries (deletion entries) will be appended" << std::endl;
        }

        std::vector<std::string> files;
        std::vector<uint64_t> hashes;
        std::vector<bool> hash_in_rpkg_meta;
        std::map<uint64_t, uint64_t> hash_map;
        std::vector<std::string> hash_file_names;
        std::vector<std::string> hash_strings;
        std::vector<std::string> hash_resource_types;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos)
                {
                    hash_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    hash_file_name = entry.path().string();
                }

                pos = hash_file_name.find_last_of(".");

                if (pos != std::string::npos)
                {
                    hash_string = hash_file_name.substr(0, pos);
                    resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << entry.path().string() << "," << hash_file_name << "," << hash_string << "," << resource_type << std::endl;
                }

                bool is_hash_file = true;

                if (hash_string.length() != 16)
                {
                    is_hash_file = false;
                }

                if (resource_type.length() != 4)
                {
                    is_hash_file = false;
                }

                if (is_hash_file)
                {
                    files.push_back(entry.path().string());
                    hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                    hash_map[hashes.back()] = (int)(hashes.size() - 1);
                    hash_in_rpkg_meta.push_back(false);
                    hash_file_names.push_back(to_uppercase(hash_file_name));
                    hash_strings.push_back(to_uppercase(hash_string));
                    hash_resource_types.push_back(to_uppercase(resource_type));

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid hash file for RPKG patch file generation." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid hash file for RPKG patch file generation." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "RPKG patch file " << rpkg_file_name << " will include these files: " << std::endl;
        }

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < files.size(); i++)
            {
                std::cout << main_data->console_prefix << "  File: " << files.at(i) << std::endl;
                std::cout << main_data->console_prefix << "    Hash file name: " << hash_file_names.at(i) << std::endl;
                std::cout << main_data->console_prefix << "    Resource type: " << hash_resource_types.at(i) << std::endl;
            }
        }

        std::vector<uint64_t> input_data_length;
        std::vector<uint64_t> output_data_length;

        std::string temp_file_name = main_data->input_output_path + rpkg_file_name + ".tmp";

        std::ofstream temp_file_output = std::ofstream(temp_file_name, std::ofstream::binary);

        if (!temp_file_output.good())
        {
            std::cout << main_data->console_prefix << "Error: Temporary file " << temp_file_name << " could not be read." << std::endl;
            std::exit(0);
        }

        std::cout << main_data->console_prefix << "Writing to temporary RPKG file: " << temp_file_name << std::endl;

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        std::string message = main_data->console_prefix + "Writing RPKG data: ";

        bool found_all_meta_files = true;

        std::vector<uint64_t> files_index;

        if (use_rpkg_file_meta_data)
        {
            for (uint64_t i = 0; i < rpkg_meta_data.hash.size(); i++)
            {
                std::map<uint64_t, uint64_t>::iterator it = hash_map.find(rpkg_meta_data.hash.at(i));

                if (it != hash_map.end())
                {
                    files_index.push_back(it->second);

                    hash_in_rpkg_meta.at(it->second) = true;

                    //std::cout << main_data->console_prefix << uint64_t_to_hex_string(rpkg_meta_data.hash.at(i)) << " found at index " << it->second << " of hash_map" << std::endl;
                }
            }
        }

        for (uint64_t i = 0; i < hashes.size(); i++)
        {
            //std::cout << main_data->console_prefix << files.at(i) << ", " << hashes.at(i) << ", " << hash_in_rpkg_meta.at(i) << std::endl;

            if (!hash_in_rpkg_meta.at(i))
            {
                files_index.push_back(i);
            }
        }

        //for (uint64_t i = 0; i < files_index.size(); i++)
        //{
            //std::cout << main_data->console_prefix << "files_index.at(" << i << "): " << files_index.at(i) << ", files.at(" << files_index.at(i) << "): " << files.at(files_index.at(i)) << ", hashes.at(" << files_index.at(i) << "): " << uint64_t_to_hex_string(hashes.at(files_index.at(i))) << std::endl;
        //}

        for (uint64_t i = 0; i < files_index.size(); i++)
        {
            if (((i * (uint64_t)100000) / (uint64_t)files_index.size()) % (uint64_t)100 == 0 && i > 0)
            {
                stringstream_length = update_console(message, files_index.size(), i, start_time, stringstream_length);
            }

            bool use_hash_file_meta_data = false;

            std::ifstream file = std::ifstream(files.at(files_index.at(i)), std::ifstream::binary);

            if (!file.good())
            {
                std::cout << main_data->console_prefix << "Error: Hash file " << files.at(files_index.at(i)) << " could not be read." << std::endl;
                std::exit(0);
            }

            file.seekg(0, file.end);
            uint64_t input_file_size = file.tellg();
            file.seekg(0, file.beg);

            std::unique_ptr<char[]> input_file_data;
            std::unique_ptr<char[]> output_file_data;

            input_file_data = std::make_unique<char[]>(input_file_size);

            input_data_length.push_back(input_file_size);

            file.read(input_file_data.get(), input_file_size);

            file.close();

            rpkg_variables meta_data;

            if (path_exists(files.at(files_index.at(i)) + ".meta"))
            {
                std::ifstream meta_file = std::ifstream(files.at(files_index.at(i)) + ".meta", std::ifstream::binary);

                if (!meta_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: Hash meta file " << files.at(files_index.at(i)) + ".meta" << " could not be read." << std::endl;
                    std::exit(0);
                }

                use_hash_file_meta_data = true;

                meta_file.seekg(0, meta_file.beg);

                meta_file.read(input, sizeof(bytes8));
                std::memcpy(&bytes8, input, sizeof(bytes8));
                meta_data.hash.push_back(bytes8);

                meta_file.read(input, sizeof(bytes8));
                std::memcpy(&bytes8, input, sizeof(bytes8));
                meta_data.hash_offset.push_back(bytes8);

                meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                meta_data.hash_size.push_back(bytes4);

                if ((meta_data.hash_size.back() & 0x3FFFFFFF) != 0)
                {
                    meta_data.is_lz4ed.push_back(true);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "LZ4ed." << std::endl;
                        std::cout << main_data->console_prefix << "hashSize: " << meta_data.hash_size.back() << std::endl;
                    }
                }
                else
                {
                    meta_data.is_lz4ed.push_back(false);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "Not LZ4ed." << std::endl;
                        std::cout << main_data->console_prefix << "hashSize: " << meta_data.hash_size.back() << std::endl;
                    }
                }

                if ((meta_data.hash_size.back() & 0x80000000) == 0x80000000)
                {
                    meta_data.is_xored.push_back(true);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "XORed." << std::endl;
                        std::cout << main_data->console_prefix << "hashSize: " << meta_data.hash_size.back() << std::endl;
                    }
                }
                else
                {
                    meta_data.is_xored.push_back(false);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "Not XORed." << std::endl;
                        std::cout << main_data->console_prefix << "hashSize: " << meta_data.hash_size.back() << std::endl;
                    }
                }

                meta_file.read(input, sizeof(bytes4));
                input[4] = 0x0;
                meta_data.hash_resource_type.push_back(std::string(input));

                meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                meta_data.hash_reference_table_size.push_back(bytes4);

                meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                meta_data.hash_reference_table_dummy.push_back(bytes4);

                meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                meta_data.hash_size_final.push_back(bytes4);

                meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                meta_data.hash_size_in_memory.push_back(bytes4);

                meta_file.read(input, sizeof(bytes4));
                std::memcpy(&bytes4, input, sizeof(bytes4));
                meta_data.hash_size_in_video_memory.push_back(bytes4);

                hash_reference_variables temp_hash_reference_data;

                if (meta_data.hash_reference_table_size.back() != 0x0)
                {
                    temp_hash_reference_data.hash = meta_data.hash.back();

                    meta_file.read(input, sizeof(bytes4));
                    std::memcpy(&bytes4, input, sizeof(bytes4));
                    temp_hash_reference_data.hash_reference_count = bytes4;

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "temp_hash_reference_data.hash_reference_count: " << temp_hash_reference_data.hash_reference_count << std::endl;
                    }

                    uint32_t temp_hash_reference_count = temp_hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                    for (uint64_t j = 0; j < temp_hash_reference_count; j++)
                    {
                        meta_file.read(input, sizeof(bytes1));
                        std::memcpy(&bytes1, input, sizeof(bytes1));
                        temp_hash_reference_data.hash_reference_type.push_back(bytes1);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "temp_hash_reference_data.hash_reference_type(" << j << "): " << temp_hash_reference_data.hash_reference_type.back() << std::endl;
                        }
                    }

                    for (uint64_t j = 0; j < temp_hash_reference_count; j++)
                    {
                        meta_file.read(input, sizeof(bytes8));
                        std::memcpy(&bytes8, input, sizeof(bytes8));
                        temp_hash_reference_data.hash_reference.push_back(bytes8);

                        temp_hash_reference_data.hash_reference_string.push_back(uint64_t_to_hex_string(bytes8));

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "temp_hash_reference_data.hash_reference(" << j << "): " << temp_hash_reference_data.hash_reference.back() << std::endl;
                            std::cout << main_data->console_prefix << "temp_hash_reference_data.hash_reference_string(" << j << "): " << temp_hash_reference_data.hash_reference_string.back() << std::endl;
                        }
                    }
                }
                else
                {
                    temp_hash_reference_data.hash_reference_count = 0x0;
                }

                meta_data.hash_reference_data.push_back(temp_hash_reference_data);
                temp_rpkg_data.hash_reference_data.push_back(temp_hash_reference_data);

                meta_file.close();
            }
            else
            {
                found_all_meta_files = false;

                meta_data.is_lz4ed.push_back(true);
                meta_data.is_xored.push_back(true);

                hash_reference_variables temp_hash_reference_data;

                temp_rpkg_data.hash_reference_data.push_back(temp_hash_reference_data);
            }

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "Adding " << hash_file_names.at(i) << " to " << rpkg_file_name << std::endl;
            }

            if (meta_data.is_lz4ed.back())
            {
                temp_rpkg_data.is_lz4ed.push_back(true);

                uint32_t compressed_size = LZ4_compressBound((int)input_file_size);

                output_file_data = std::make_unique<char[]>(compressed_size);

                uint64_t compressed_size_final;

                if (main_data->mode_use_lz4hc)
                {
                    compressed_size_final = LZ4_compress_HC(input_file_data.get(), output_file_data.get(), (int)input_file_size, compressed_size, LZ4HC_CLEVEL_MAX);
                }
                else
                {
                    compressed_size_final = LZ4_compress_default(input_file_data.get(), output_file_data.get(), (int)input_file_size, compressed_size);
                }

                output_data_length.push_back(compressed_size_final);

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "LZ4_compressBound: " << compressed_size << std::endl;
                    std::cout << main_data->console_prefix << "LZ4_compress_default: " << compressed_size_final << std::endl;
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << hash_file_names.at(files_index.at(i)) << " has been LZ4 compressed." << std::endl;
                    std::cout << main_data->console_prefix << "Original size of " << hash_file_names.at(files_index.at(i)) << ": " << input_file_size << std::endl;
                    std::cout << main_data->console_prefix << "LZ4 compressed size of " << hash_file_names.at(files_index.at(i)) << ": " << compressed_size_final << std::endl;
                    std::cout << main_data->console_prefix << "LZ4 compression ratio: " << ((double)input_file_size / (double)compressed_size_final) << std::endl;
                }

                if (meta_data.is_xored.back())
                {
                    xor_data(output_file_data.get(), (uint32_t)compressed_size_final);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "XORing output_file_data with a input_file_size of " << input_file_size << std::endl;
                    }

                    temp_rpkg_data.is_xored.push_back(1);
                    temp_rpkg_data.hash_size.push_back((uint32_t)compressed_size_final | (uint32_t)0x80000000);
                }
                else
                {
                    temp_rpkg_data.is_xored.push_back(0);
                    temp_rpkg_data.hash_size.push_back((uint32_t)compressed_size_final);
                }

                if (use_hash_file_meta_data)
                {
                    temp_rpkg_data.hash.push_back(meta_data.hash.back());
                    temp_rpkg_data.hash_string.push_back(uint64_t_to_hex_string(meta_data.hash.back()));
                    temp_rpkg_data.hash_resource_type.push_back(meta_data.hash_resource_type.back());
                    temp_rpkg_data.hash_reference_table_size.push_back(meta_data.hash_reference_table_size.back());
                    temp_rpkg_data.hash_reference_table_dummy.push_back(meta_data.hash_reference_table_dummy.back());
                    temp_rpkg_data.hash_size_final.push_back((uint32_t)input_file_size);

                    if (meta_data.hash_size_in_memory.back() == 0xFFFFFFFF)
                    {
                        temp_rpkg_data.hash_size_in_memory.push_back(meta_data.hash_size_in_memory.back());
                        temp_rpkg_data.hash_size_in_video_memory.push_back(meta_data.hash_size_in_video_memory.back());
                    }
                    else
                    {
                        uint32_t resized_size_in_memory = (uint32_t)((double)input_file_size * ((double)meta_data.hash_size_in_memory.back() / (double)meta_data.hash_size_final.back()));
                        uint32_t resized_difference_value = meta_data.hash_size_in_memory.back() - (uint32_t)((double)meta_data.hash_size_final.back() * ((double)meta_data.hash_size_in_memory.back() / (double)meta_data.hash_size_final.back()));

                        resized_size_in_memory += resized_difference_value;

                        temp_rpkg_data.hash_size_in_memory.push_back(resized_size_in_memory);
                        temp_rpkg_data.hash_size_in_video_memory.push_back(meta_data.hash_size_in_video_memory.back());
                    }

                }
                else
                {
                    uint64_t hash = std::strtoll(hash_strings.at(files_index.at(i)).c_str(), nullptr, 16);
                    temp_rpkg_data.hash.push_back(hash);
                    temp_rpkg_data.hash_string.push_back(hash_strings.at(files_index.at(i)));
                    temp_rpkg_data.hash_resource_type.push_back(hash_resource_types.at(files_index.at(i)));
                    temp_rpkg_data.hash_reference_table_size.push_back(0x0);
                    temp_rpkg_data.hash_reference_table_dummy.push_back(0x0);
                    temp_rpkg_data.hash_size_final.push_back((uint32_t)input_file_size);
                    temp_rpkg_data.hash_size_in_memory.push_back((uint32_t)compressed_size_final);
                    temp_rpkg_data.hash_size_in_video_memory.push_back(0x0);
                }
            }
            else
            {
                temp_rpkg_data.is_lz4ed.push_back(false);

                output_file_data = std::make_unique<char[]>(input_file_size);

                output_data_length.push_back(input_file_size);

                std::memcpy(output_file_data.get(), input_file_data.get(), input_file_size);

                if (meta_data.is_xored.back())
                {
                    xor_data(output_file_data.get(), (uint32_t)input_file_size);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << "XORing output_file_data with a input_file_size of " << input_file_size << std::endl;
                    }

                    temp_rpkg_data.is_xored.push_back(1);
                    temp_rpkg_data.hash_size.push_back(0x80000000);
                }
                else
                {
                    temp_rpkg_data.is_xored.push_back(0);
                    temp_rpkg_data.hash_size.push_back(0x0);
                }

                if (use_hash_file_meta_data)
                {
                    temp_rpkg_data.hash.push_back(meta_data.hash.back());
                    temp_rpkg_data.hash_string.push_back(uint64_t_to_hex_string(meta_data.hash.back()));
                    temp_rpkg_data.hash_resource_type.push_back(meta_data.hash_resource_type.back());
                    temp_rpkg_data.hash_reference_table_size.push_back(meta_data.hash_reference_table_size.back());
                    temp_rpkg_data.hash_reference_table_dummy.push_back(meta_data.hash_reference_table_dummy.back());
                    temp_rpkg_data.hash_size_final.push_back((uint32_t)input_file_size);

                    if (meta_data.hash_size_in_memory.back() == 0xFFFFFFFF)
                    {
                        temp_rpkg_data.hash_size_in_memory.push_back(meta_data.hash_size_in_memory.back());
                        temp_rpkg_data.hash_size_in_video_memory.push_back(meta_data.hash_size_in_video_memory.back());
                    }
                    else
                    {
                        uint32_t resized_size_in_memory = (uint32_t)((double)input_file_size * ((double)meta_data.hash_size_in_memory.back() / (double)meta_data.hash_size_final.back()));
                        uint32_t resized_difference_value = meta_data.hash_size_in_memory.back() - (uint32_t)((double)meta_data.hash_size_final.back() * ((double)meta_data.hash_size_in_memory.back() / (double)meta_data.hash_size_final.back()));

                        resized_size_in_memory += resized_difference_value;

                        temp_rpkg_data.hash_size_in_memory.push_back(resized_size_in_memory);
                        temp_rpkg_data.hash_size_in_video_memory.push_back(meta_data.hash_size_in_video_memory.back());
                    }
                }
                else
                {
                    uint64_t hash = std::strtoll(hash_strings.at(files_index.at(i)).c_str(), nullptr, 16);
                    temp_rpkg_data.hash.push_back(hash);
                    temp_rpkg_data.hash_string.push_back(hash_strings.at(files_index.at(i)));
                    temp_rpkg_data.hash_resource_type.push_back(hash_resource_types.at(files_index.at(i)));
                    temp_rpkg_data.hash_reference_table_size.push_back(0x0);
                    temp_rpkg_data.hash_reference_table_dummy.push_back(0x0);
                    temp_rpkg_data.hash_size_final.push_back((uint32_t)input_file_size);
                    temp_rpkg_data.hash_size_in_memory.push_back((uint32_t)input_file_size);
                    temp_rpkg_data.hash_size_in_video_memory.push_back(0x0);
                }
            }

            temp_file_output.write(output_file_data.get(), output_data_length.back());

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "Writing to temporary file: " << temp_file_name << std::endl;
            }

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "hash: " << temp_rpkg_data.hash.back() << std::endl;
                std::cout << main_data->console_prefix << "hashString: " << temp_rpkg_data.hash_string.back() << std::endl;
                std::cout << main_data->console_prefix << "hashSize: " << temp_rpkg_data.hash_size.back() << std::endl;
                std::cout << main_data->console_prefix << "isLZ4ed: " << temp_rpkg_data.is_lz4ed.back() << std::endl;
                std::cout << main_data->console_prefix << "isXORed: " << temp_rpkg_data.is_xored.back() << std::endl;
                std::cout << main_data->console_prefix << "fileType: " << temp_rpkg_data.hash_resource_type.back() << std::endl;
                std::cout << main_data->console_prefix << "chunkSize1: " << temp_rpkg_data.hash_reference_table_size.back() << std::endl;
                std::cout << main_data->console_prefix << "chunkSize2: " << temp_rpkg_data.hash_reference_table_dummy.back() << std::endl;
                std::cout << main_data->console_prefix << "sizeFinal: " << temp_rpkg_data.hash_size_final.back() << std::endl;
                std::cout << main_data->console_prefix << "sizeMem: " << temp_rpkg_data.hash_size_in_memory.back() << std::endl;
                std::cout << main_data->console_prefix << "sizeVideoMem: " << temp_rpkg_data.hash_size_in_video_memory.back() << std::endl;
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (!found_all_meta_files)
        {
            std::cout << main_data->console_prefix << "Some hash files did not have meta files." << std::endl;
            std::cout << main_data->console_prefix << "When a hash file does not have a meta file the defaults used are:" << std::endl;
            std::cout << main_data->console_prefix << "  - The hash file is XORed and LZ4ed" << std::endl;
            std::cout << main_data->console_prefix << "  - The hash file's ref table is set to 0, meaning the hash file's" << std::endl;
            std::cout << main_data->console_prefix << "    reference data linking to other hashes will not be included." << std::endl;
        }

        temp_file_output.close();

        uint32_t hash_count = (uint32_t)temp_rpkg_data.hash.size();
        uint32_t table_offset = hash_count * 0x14;
        uint32_t table_size = 0x0;
        uint64_t hash_offset = 0x0;
        uint64_t total_hash_size = 0x0;

        table_size = 0x0;

        for (uint64_t j = 0; j < temp_rpkg_data.hash.size(); j++)
        {
            table_size += 0x18;
            table_size += temp_rpkg_data.hash_reference_table_size.at(j);
        }

        if (rpkg_meta_data.is_patch_file)
        {
            if (rpkg_meta_data.rpkg_file_version == 1)
            {
                hash_offset = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x14;
            }
            else
            {
                hash_offset = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)rpkg_meta_data.patch_entry_count * (uint64_t)0x8 + (uint64_t)0x1D;
            }
        }
        else
        {
            if (rpkg_meta_data.rpkg_file_version == 1)
            {
                hash_offset = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x10;
            }
            else
            {
                hash_offset = (uint64_t)table_offset + (uint64_t)table_size + (uint64_t)0x19;
            }
        }

        if (main_data->debug)
        {
            std::cout << main_data->console_prefix << "hash_count: " << hash_count << std::endl;
            std::cout << main_data->console_prefix << "table_offset: " << table_offset << std::endl;
            std::cout << main_data->console_prefix << "table_size: " << table_size << std::endl;
            std::cout << main_data->console_prefix << "hash_offset: " << hash_offset << std::endl;
            std::cout << main_data->console_prefix << "total_hash_size: " << total_hash_size << std::endl;
        }

        std::ofstream file = std::ofstream(main_data->input_output_path + rpkg_file_name, std::ifstream::binary);

        if (!file.good())
        {
            std::cout << main_data->console_prefix << "Error: RPKG file " << rpkg_file_name << " could not be created." << std::endl;
            std::exit(0);
        }

        char char1[1];
        char char4[4];
        char char8[8];

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            file.write("G", 0x1);
            file.write("K", 0x1);
            file.write("P", 0x1);
            file.write("R", 0x1);
        }
        else
        {
            file.write("2", 0x1);
            file.write("K", 0x1);
            file.write("P", 0x1);
            file.write("R", 0x1);
        }

        if (rpkg_meta_data.rpkg_file_version == 2)
        {
            char1[0] = 0x01;
            file.write(char1, 0x1);
            char1[0] = 0x00;
            file.write(char1, 0x1);
            file.write(char1, 0x1);
            file.write(char1, 0x1);
            char1[0] = rpkg_meta_data.rpkgv2_chunk_number;
            file.write(char1, 0x1);
            char1[0] = 0x00;
            file.write(char1, 0x1);
            file.write(char1, 0x1);
            char1[0] = 0x78;
            file.write(char1, 0x1);
            file.write(char1, 0x1);
        }

        std::memcpy(&char4, &hash_count, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &table_offset, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));
        std::memcpy(&char4, &table_size, sizeof(uint32_t));
        file.write(char4, sizeof(uint32_t));

        if (rpkg_meta_data.is_patch_file)
        {
            std::memcpy(&char4, &rpkg_meta_data.patch_entry_count, sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));

            for (uint64_t i = 0; i < rpkg_meta_data.patch_entry_count; i++)
            {
                std::memcpy(&char8, &rpkg_meta_data.patch_entry_list.at(i), sizeof(uint64_t));
                file.write(char8, sizeof(uint64_t));
            }
        }

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            message = "Writing RPKGv1 offset index: ";
        }
        else
        {
            message = "Writing RPKGv2 offset index: ";
        }

        start_time = std::chrono::high_resolution_clock::now();
        stringstream_length = 80;

        for (uint64_t i = 0; i < temp_rpkg_data.hash.size(); i++)
        {
            if (((i * (uint64_t)100000) / (uint64_t)temp_rpkg_data.hash.size()) % (uint64_t)100 == 0 && i > 0)
            {
                stringstream_length = update_console(message, temp_rpkg_data.hash.size(), i, start_time, stringstream_length);
            }

            std::memcpy(&char8, &temp_rpkg_data.hash.at(i), sizeof(uint64_t));
            file.write(char8, sizeof(uint64_t));
            std::memcpy(&char8, &hash_offset, sizeof(uint64_t));
            file.write(char8, sizeof(uint64_t));

            std::memcpy(&char4, &temp_rpkg_data.hash_size.at(i), sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));

            if (temp_rpkg_data.is_lz4ed.at(i))
            {
                hash_offset += temp_rpkg_data.hash_size.at(i) & 0x3FFFFFFF;
            }
            else
            {
                hash_offset += temp_rpkg_data.hash_size_final.at(i);
            }
        }

        end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            message = "Writing RPKGv1 info index: ";
        }
        else
        {
            message = "Writing RPKGv2 info index: ";
        }

        start_time = std::chrono::high_resolution_clock::now();
        stringstream_length = 80;

        for (uint64_t i = 0; i < temp_rpkg_data.hash.size(); i++)
        {
            if (((i * (uint64_t)100000) / temp_rpkg_data.hash.size()) % (uint64_t)100 == 0 && i > 0)
            {
                stringstream_length = update_console(message, temp_rpkg_data.hash.size(), i, start_time, stringstream_length);
            }

            file.write(&temp_rpkg_data.hash_resource_type.at(i)[3], 0x1);
            file.write(&temp_rpkg_data.hash_resource_type.at(i)[2], 0x1);
            file.write(&temp_rpkg_data.hash_resource_type.at(i)[1], 0x1);
            file.write(&temp_rpkg_data.hash_resource_type.at(i)[0], 0x1);

            std::memcpy(&char4, &temp_rpkg_data.hash_reference_table_size.at(i), sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));
            std::memcpy(&char4, &temp_rpkg_data.hash_reference_table_dummy.at(i), sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));
            std::memcpy(&char4, &temp_rpkg_data.hash_size_final.at(i), sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));
            std::memcpy(&char4, &temp_rpkg_data.hash_size_in_memory.at(i), sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));
            std::memcpy(&char4, &temp_rpkg_data.hash_size_in_video_memory.at(i), sizeof(uint32_t));
            file.write(char4, sizeof(uint32_t));

            uint32_t temp_hash_reference_count = temp_rpkg_data.hash_reference_data.at(i).hash_reference_count & 0x3FFFFFFF;

            if (temp_rpkg_data.hash_reference_table_size.at(i) > 0)
            {
                std::memcpy(&char4, &temp_rpkg_data.hash_reference_data.at(i).hash_reference_count, sizeof(uint32_t));
                file.write(char4, sizeof(uint32_t));
            }

            if (temp_hash_reference_count > 0)
            {
                for (uint64_t j = 0; j < temp_hash_reference_count; j++)
                {
                    std::memcpy(&char1, &temp_rpkg_data.hash_reference_data.at(i).hash_reference_type.at(j), sizeof(uint8_t));
                    file.write(char1, sizeof(uint8_t));
                }

                for (uint64_t j = 0; j < temp_hash_reference_count; j++)
                {
                    std::memcpy(&char8, &temp_rpkg_data.hash_reference_data.at(i).hash_reference.at(j), sizeof(uint64_t));
                    file.write(char8, sizeof(uint64_t));
                }
            }
        }

        end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        std::ifstream temp_file_input = std::ifstream(temp_file_name, std::ifstream::binary);

        if (!temp_file_input.good())
        {
            std::cout << main_data->console_prefix << "Error: Temporary file " << temp_file_name << " could not be read." << std::endl;
            std::exit(0);
        }

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            std::cout << main_data->console_prefix << "Merging RPKGv1 file and temporary RPKG files..." << std::endl;
        }
        else
        {
            std::cout << main_data->console_prefix << "Merging RPKGv2 file and temporary RPKG files..." << std::endl;
        }

        start_time = std::chrono::high_resolution_clock::now();

        file << temp_file_input.rdbuf();

        end_time = std::chrono::high_resolution_clock::now();

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            std::cout << main_data->console_prefix << "Merged RPKGv1 file and temporary RPKGv1 file in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s" << std::endl;
        }
        else
        {
            std::cout << main_data->console_prefix << "Merged RPKGv2 file and temporary RPKGv2 file in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s" << std::endl;
        }

        temp_file_input.close();

        std::remove(temp_file_name.c_str());

        file.close();

        if (rpkg_meta_data.rpkg_file_version == 1)
        {
            std::cout << main_data->console_prefix << "Successfully generated RPKGv1 file: " << rpkg_file_name << std::endl;
        }
        else
        {
            std::cout << main_data->console_prefix << "Successfully generated RPKGv2 file: " << rpkg_file_name << std::endl;
        }
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void extract_from_rpkg(main_variables* main_data)
{
    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    std::cout << main_data->console_prefix << "Extract from RPKG file: " << main_data->input_rpkg_file_name << std::endl;

    std::string message = main_data->console_prefix + "Extracting from RPKG: ";

    if (main_data->mode_filter)
    {
        std::cout << main_data->console_prefix << "Extract: All hash files with filter \"" << main_data->input_filter_string << "\"" << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Extract: All hash files" << std::endl;
    }

    import_rpkg_file_if_not_already(main_data, main_data->input_rpkg_file_path, main_data->input_rpkg_file_name, true);

    std::string rpkg_base_name = "";

    size_t pos = to_uppercase(main_data->input_rpkg_file_name).find(".RPKG");

    if (pos != std::string::npos)
    {
        rpkg_base_name = main_data->input_rpkg_file_name.substr(0, pos);
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: RPKG file name " << main_data->input_rpkg_file_name << " does not end in .rpkg" << std::endl;
        std::exit(0);
    }

    for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
    {
        if (main_data->rpkg_data.at(i).rpkg_file_name == main_data->input_rpkg_file_name)
        {
            std::string hash_file_path = main_data->input_output_path + rpkg_base_name;
            std::string final_path;

            std::ofstream rpkg_meta_data_file;

            if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
            {
                if (!path_exists(hash_file_path))
                {
                    final_path = replace_slashes(hash_file_path);

                    if (!std::filesystem::create_directories(final_path))
                    {
                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << hash_file_path << std::endl;
                        std::exit(0);
                    }
                }

                hash_file_path.append("/" + rpkg_base_name + ".meta");
                final_path = replace_slashes(hash_file_path);
                rpkg_meta_data_file = std::ofstream(final_path, std::ifstream::binary);

                if (!rpkg_meta_data_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: Meta data file " << final_path << " could not be created." << std::endl;
                    std::exit(0);
                }
            }

            std::vector<char> rpkg_meta_data;
            char char1[1];
            char char4[4];
            char char8[8];

            if (main_data->rpkg_data.at(i).rpkg_file_version == 1)
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

            if (main_data->rpkg_data.at(i).rpkg_file_version == 2)
            {
                rpkg_meta_data.push_back(0x01);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back((char)main_data->rpkg_data.at(i).rpkgv2_chunk_number);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x78);
                rpkg_meta_data.push_back(0x78);
            }

            std::memcpy(&char4, &main_data->rpkg_data.at(i).rpkg_file_count, sizeof(uint32_t));

            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
            {
                rpkg_meta_data.push_back(char4[j]);
            }

            std::memcpy(&char4, &main_data->rpkg_data.at(i).rpkg_table_offset, sizeof(uint32_t));

            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
            {
                rpkg_meta_data.push_back(char4[j]);
            }

            std::memcpy(&char4, &main_data->rpkg_data.at(i).rpkg_table_size, sizeof(uint32_t));

            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
            {
                rpkg_meta_data.push_back(char4[j]);
            }

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "main_data->rpkg_data.at(i).fileCount: " << main_data->rpkg_data.at(i).rpkg_file_count << std::endl;
                std::cout << main_data->console_prefix << "main_data->rpkg_data.at(i).tableOffset: " << main_data->rpkg_data.at(i).rpkg_table_offset << std::endl;
                std::cout << main_data->console_prefix << "main_data->rpkg_data.at(i).tableSize: " << main_data->rpkg_data.at(i).rpkg_table_size << std::endl;
            }

            if (main_data->rpkg_data.at(i).is_patch_file)
            {
                std::memcpy(&char4, &main_data->rpkg_data.at(i).patch_entry_count, sizeof(uint32_t));

                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                {
                    rpkg_meta_data.push_back(char4[j]);
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "main_data->rpkg_data.at(" << i << ").patch_entry_count: " << main_data->rpkg_data.at(i).patch_entry_count << std::endl;
                }

                if (main_data->rpkg_data.at(i).patch_entry_count > 0)
                {
                    uint32_t patchEntryCount = 0;

                    for (uint64_t j = 0; j < main_data->rpkg_data.at(i).patch_entry_count; j++)
                    {
                        std::memcpy(&char8, &main_data->rpkg_data.at(i).patch_entry_list.at(j), sizeof(uint64_t));
                        for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                        {
                            rpkg_meta_data.push_back(char8[k]);
                        }

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "main_data->rpkg_data.at(" << i << ").patch_entry_list.at(" << j << "): " << main_data->rpkg_data.at(i).patch_entry_list.at(j) << std::endl;
                        }

                        patchEntryCount++;
                    }

                    if (patchEntryCount != main_data->rpkg_data.at(i).patch_entry_count)
                    {
                        std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).rpkg_file_name << " is corrupt." << std::endl;
                        std::exit(0);
                    }
                }
            }

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(j), sizeof(uint64_t));

                for (uint64_t j = 0; j < sizeof(uint64_t); j++)
                {
                    rpkg_meta_data.push_back(char8[j]);
                }

                std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(j), sizeof(uint64_t));

                for (uint64_t j = 0; j < sizeof(uint64_t); j++)
                {
                    rpkg_meta_data.push_back(char8[j]);
                }

                std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(j), sizeof(uint32_t));

                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                {
                    rpkg_meta_data.push_back(char4[j]);
                }
            }

            if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
            {
                rpkg_meta_data_file.write(rpkg_meta_data.data(), rpkg_meta_data.size());

                rpkg_meta_data_file.close();
            }

            start_time = std::chrono::high_resolution_clock::now();
            int stringstream_length = 80;

            std::vector<bool> extracted;

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                extracted.push_back(false);
            }

            uint64_t search_count = 0;

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                if (((j * (uint64_t)100000) / main_data->rpkg_data.at(i).hash.size()) % (uint64_t)100 == 0 && j > 0)
                {
                    stringstream_length = update_console(message, main_data->rpkg_data.at(i).hash.size(), j, start_time, stringstream_length);
                }

                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                bool found = false;

                uint64_t input_filter_index = 0;

                for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
                {
                    std::size_t found_position = hash_file_name.find(main_data->input_filter.at(z));

                    if (found_position != std::string::npos && main_data->input_filter.at(z) != "")
                    {
                        found = true;

                        input_filter_index = z;

                        break;
                    }
                }

                if (found || !main_data->mode_filter)
                {
                    if (main_data->input_filter.size() > 0)
                    {
                        extracted.at(input_filter_index) = true;
                    }

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::vector<char> input_data(hash_size, 0);
                    std::ifstream file = std::ifstream(main_data->input_rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.data(), hash_size);

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.data(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
                    {
                        hash_file_path = main_data->input_output_path + rpkg_base_name;

                        if (!path_exists(hash_file_path))
                        {
                            final_path = replace_slashes(hash_file_path);
                            if (!std::filesystem::create_directories(final_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << hash_file_path << std::endl;
                                std::exit(0);
                            }
                        }

                        hash_file_path.append("/" + main_data->rpkg_data.at(i).hash_resource_type.at(j));

                        if (!path_exists(hash_file_path))
                        {
                            final_path = replace_slashes(hash_file_path);
                            if (!std::filesystem::create_directories(final_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << hash_file_path << std::endl;
                                std::exit(0);
                            }
                        }
                    }

                    std::vector<char>* search_data;
                    uint64_t search_size;

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    std::vector<char> output_data(decompressed_size, 0);

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }

                        hash_file_path.append("/" + hash_file_name);
                        final_path = replace_slashes(hash_file_path);

                        if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search))
                        {
                            if (main_data->mode_extract_all_ores)
                            {
                                final_path = main_data->input_ores_path;
                            }

                            std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: Hash file " << final_path << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            output_file.write(output_data.data(), decompressed_size);

                            output_file.close();
                        }

                        search_data = &output_data;
                        search_size = decompressed_size;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "Extracted " << main_data->rpkg_data.at(i).hash_string.at(j) << " to " << hash_file_path << std::endl;
                        }
                    }
                    else
                    {
                        hash_file_path.append("/" + hash_file_name);
                        final_path = replace_slashes(hash_file_path);

                        if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search))
                        {
                            if (main_data->mode_extract_all_ores)
                            {
                                final_path = main_data->input_ores_path;
                            }

                            std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: Hash file " << final_path << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            output_file.write(input_data.data(), hash_size);

                            output_file.close();
                        }

                        search_data = &input_data;
                        search_size = hash_size;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "Extracted " << main_data->rpkg_data.at(i).hash_string.at(j) << " to " << hash_file_path << std::endl;
                        }
                    }

                    if (main_data->mode_text_search)
                    {
                        uint64_t position = 0;

                        bool done_searching = false;

                        while (!done_searching)
                        {
                            if ((position + main_data->input_text_search.length()) > search_size)
                            {
                                done_searching = true;
                                break;
                            }

                            for (uint64_t k = 0; k < main_data->input_text_search.length(); k++)
                            {
                                if (std::tolower(search_data->data()[position + k]) == std::tolower(main_data->input_text_search[k]))
                                {
                                    if (k == (main_data->input_text_search.length() - 1))
                                    {
                                        search_count++;

                                        std::cout << main_data->console_prefix << "Found text string \"" << main_data->input_text_search << "\" in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(j) << " at offset 0x" << std::hex << std::uppercase << (position - 1) << std::endl;

                                        bool done_searching_start = false;

                                        uint64_t position_start = position;

                                        while (!done_searching_start)
                                        {
                                            if (position_start == 0)
                                            {
                                                done_searching_start = true;
                                                break;
                                            }

                                            if (search_data->data()[position_start] < 0x20 || search_data->data()[position_start] > 0x7E)
                                            {
                                                done_searching_start = true;
                                                break;
                                            }

                                            position_start--;
                                        }

                                        std::cout << main_data->console_prefix << "Text string \"" << main_data->input_text_search << "\" is contained in string: " << std::string(search_data->data() + position_start + 1) << std::endl;

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
                    else if (main_data->mode_hex_search)
                    {
                        uint64_t position = 0;

                        bool done_searching = false;

                        std::vector<char> hex_search;

                        if (main_data->input_hex_search.length() % 2 != 0)
                        {
                            main_data->input_hex_search = "0" + main_data->input_hex_search;
                        }

                        for (uint64_t i = 0; i < (main_data->input_hex_search.length() / 2); i++)
                        {
                            hex_search.push_back((char)strtoul(main_data->input_hex_search.substr(i * 2, 2).c_str(), nullptr, 16));
                        }

                        while (!done_searching && hash_size > 0)
                        {
                            if ((position + hex_search.size()) > search_size)
                            {
                                done_searching = true;
                                break;
                            }

                            for (uint64_t k = 0; k < hex_search.size(); k++)
                            {
                                if (search_data->data()[position + k] == hex_search.at(k))
                                {
                                    if (k == (hex_search.size() - 1))
                                    {
                                        search_count++;

                                        std::cout << main_data->console_prefix << "Found hex string \"";

                                        for (uint64_t k = 0; k < hex_search.size(); k++)
                                        {
                                            std::cout << main_data->console_prefix << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)(unsigned char)hex_search.at(k);
                                        }

                                        std::cout << main_data->console_prefix << "\" in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(j) << " at offset 0x" << std::hex << std::uppercase << (position - 1) << std::endl;

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
                    else if (main_data->mode_regex_search)
                    {
                        std::smatch m;
                        std::regex re(main_data->input_regex_search);
                        std::string data_string;
                        data_string.reserve((uint64_t)search_size * (uint64_t)6);

                        for (uint64_t k = 0; k < search_size; k++)
                        {
                            if (search_data->data()[k] >= 0x20 && search_data->data()[k] <= 0x7E)
                            {
                                data_string += search_data->data()[k];
                            }
                            else
                            {
                                char value[5];
                                sprintf_s(value, "\\x%02X", (int)(unsigned char)search_data->data()[k]);
                                data_string += value;
                            }
                        }

                        //std::cout << main_data->console_prefix << data_string << std::endl;

                        uint64_t position = 0;

                        while (std::regex_search(data_string, m, re))
                        {
                            position += m.position();

                            std::cout << main_data->console_prefix << "Regex search with regex \"" << main_data->input_regex_search << "\" returned result in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(j) << std::endl;// << " at offset 0x" << std::hex << std::uppercase << position << std::endl;

                            for (size_t k = 0; k < m.size(); k++)
                            {
                                std::cout << main_data->console_prefix << "Match[" << k << "]: " << m[k].str() << std::endl;
                            }

                            data_string = m.suffix().str();
                        }
                    }

                    std::vector<char> meta_data;

                    std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(j), sizeof(uint64_t));
                    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                    {
                        meta_data.push_back(char8[k]);
                    }

                    std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(j), sizeof(uint64_t));
                    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                    {
                        meta_data.push_back(char8[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_resource_type.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_size.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_dummy.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_final.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_memory.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_video_memory.at(j), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    if (main_data->rpkg_data.at(i).hash_reference_table_size.at(j) > 0)
                    {
                        uint32_t hash_reference_table_size_count = 0;
                        uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count & 0x3FFFFFFF;

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count, sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }
                        hash_reference_table_size_count += 4;

                        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                        {
                            std::memcpy(&char1, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_type.at(k), sizeof(uint8_t));
                            for (uint64_t l = 0; l < sizeof(uint8_t); l++)
                            {
                                meta_data.push_back(char1[l]);
                            }
                            hash_reference_table_size_count += 1;
                        }

                        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                        {
                            std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference.at(k), sizeof(uint64_t));
                            for (uint64_t l = 0; l < sizeof(uint64_t); l++)
                            {
                                meta_data.push_back(char8[l]);
                            }
                            hash_reference_table_size_count += 8;
                        }

                        if (hash_reference_table_size_count != main_data->rpkg_data.at(i).hash_reference_table_size.at(j))
                        {
                            std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).hash_string.at(j) << " is corrupt." << std::endl;
                            std::exit(0);
                        }
                    }

                    if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
                    {
                        final_path += ".meta";
                        std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

                        if (!output_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: Meta data file " << final_path << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        output_file.write(meta_data.data(), meta_data.size());
                    }
                }
            }

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                if (!extracted.at(z))
                {
                    std::cout << std::endl << main_data->console_prefix << "Error: " << main_data->input_filter.at(z) << " is not in " << main_data->input_rpkg_file_path << std::endl;
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        std::stringstream ss;

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ');
    }
}

void extract_from_rpkg_with_map(main_variables* main_data)
{
    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

    if (!main_data->suppress_console_output)
    {
        std::cout << main_data->console_prefix << "Extract from RPKG file: " << main_data->input_rpkg_file_name << std::endl;
    }

    std::string message = main_data->console_prefix + "Extracting from RPKG: ";

    if (!main_data->suppress_console_output)
    {
        if (main_data->mode_filter)
        {
            std::cout << main_data->console_prefix << "Extract: All hash files with filter \"" << main_data->input_filter_string << "\"" << std::endl;
        }
        else
        {
            std::cout << main_data->console_prefix << "Extract: All hash files" << std::endl;
        }
    }

    import_rpkg_file_if_not_already(main_data, main_data->input_rpkg_file_path, main_data->input_rpkg_file_name, true);

    std::string rpkg_base_name = "";

    size_t pos = to_uppercase(main_data->input_rpkg_file_name).find(".RPKG");

    if (pos != std::string::npos)
    {
        rpkg_base_name = main_data->input_rpkg_file_name.substr(0, pos);
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: RPKG file name " << main_data->input_rpkg_file_name << " does not end in .rpkg" << std::endl;
        std::exit(0);
    }

    for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
    {
        if (main_data->rpkg_data.at(i).rpkg_file_name == main_data->input_rpkg_file_name)
        {
            std::string hash_file_path = main_data->input_output_path + rpkg_base_name;
            std::string final_path;

            std::ofstream rpkg_meta_data_file;

            if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
            {
                if (!path_exists(hash_file_path))
                {
                    final_path = replace_slashes(hash_file_path);

                    if (!std::filesystem::create_directories(final_path))
                    {
                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << hash_file_path << std::endl;
                        std::exit(0);
                    }
                }

                hash_file_path.append("/" + rpkg_base_name + ".meta");
                final_path = replace_slashes(hash_file_path);
                rpkg_meta_data_file = std::ofstream(final_path, std::ifstream::binary);

                if (!rpkg_meta_data_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: Meta data file " << final_path << " could not be created." << std::endl;
                    std::exit(0);
                }
            }

            std::vector<char> rpkg_meta_data;
            char char1[1];
            char char4[4];
            char char8[8];

            if (main_data->rpkg_data.at(i).rpkg_file_version == 1)
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

            if (main_data->rpkg_data.at(i).rpkg_file_version == 2)
            {
                rpkg_meta_data.push_back(0x01);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back((char)main_data->rpkg_data.at(i).rpkgv2_chunk_number);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x00);
                rpkg_meta_data.push_back(0x78);
                rpkg_meta_data.push_back(0x78);
            }

            std::memcpy(&char4, &main_data->rpkg_data.at(i).rpkg_file_count, sizeof(uint32_t));

            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
            {
                rpkg_meta_data.push_back(char4[j]);
            }

            std::memcpy(&char4, &main_data->rpkg_data.at(i).rpkg_table_offset, sizeof(uint32_t));

            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
            {
                rpkg_meta_data.push_back(char4[j]);
            }

            std::memcpy(&char4, &main_data->rpkg_data.at(i).rpkg_table_size, sizeof(uint32_t));

            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
            {
                rpkg_meta_data.push_back(char4[j]);
            }

            if (main_data->debug)
            {
                std::cout << main_data->console_prefix << "main_data->rpkg_data.at(i).fileCount: " << main_data->rpkg_data.at(i).rpkg_file_count << std::endl;
                std::cout << main_data->console_prefix << "main_data->rpkg_data.at(i).tableOffset: " << main_data->rpkg_data.at(i).rpkg_table_offset << std::endl;
                std::cout << main_data->console_prefix << "main_data->rpkg_data.at(i).tableSize: " << main_data->rpkg_data.at(i).rpkg_table_size << std::endl;
            }

            if (main_data->rpkg_data.at(i).is_patch_file)
            {
                std::memcpy(&char4, &main_data->rpkg_data.at(i).patch_entry_count, sizeof(uint32_t));

                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                {
                    rpkg_meta_data.push_back(char4[j]);
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << "main_data->rpkg_data.at(" << i << ").patch_entry_count: " << main_data->rpkg_data.at(i).patch_entry_count << std::endl;
                }

                if (main_data->rpkg_data.at(i).patch_entry_count > 0)
                {
                    uint32_t patchEntryCount = 0;

                    for (uint64_t j = 0; j < main_data->rpkg_data.at(i).patch_entry_count; j++)
                    {
                        std::memcpy(&char8, &main_data->rpkg_data.at(i).patch_entry_list.at(j), sizeof(uint64_t));
                        for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                        {
                            rpkg_meta_data.push_back(char8[k]);
                        }

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "main_data->rpkg_data.at(" << i << ").patch_entry_list.at(" << j << "): " << main_data->rpkg_data.at(i).patch_entry_list.at(j) << std::endl;
                        }

                        patchEntryCount++;
                    }

                    if (patchEntryCount != main_data->rpkg_data.at(i).patch_entry_count)
                    {
                        std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).rpkg_file_name << " is corrupt." << std::endl;
                        std::exit(0);
                    }
                }
            }

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(j), sizeof(uint64_t));

                for (uint64_t j = 0; j < sizeof(uint64_t); j++)
                {
                    rpkg_meta_data.push_back(char8[j]);
                }

                std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(j), sizeof(uint64_t));

                for (uint64_t j = 0; j < sizeof(uint64_t); j++)
                {
                    rpkg_meta_data.push_back(char8[j]);
                }

                std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(j), sizeof(uint32_t));

                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                {
                    rpkg_meta_data.push_back(char4[j]);
                }
            }

            if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
            {
                rpkg_meta_data_file.write(rpkg_meta_data.data(), rpkg_meta_data.size());

                rpkg_meta_data_file.close();
            }

            bool extracted = false;

            uint64_t search_count = 0;

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                uint64_t hash = std::strtoull(main_data->input_filter.at(z).c_str(), nullptr, 16);

                std::map<uint64_t, uint64_t>::iterator it = main_data->rpkg_data.at(i).hash_map.find(hash);

                if (it != main_data->rpkg_data.at(i).hash_map.end())
                {
                    std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(it->second) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(it->second);

                    std::size_t found_position = hash_file_name.find(main_data->input_filter.at(z));

                    extracted = true;

                    if (main_data->mode_extract_all_ores)
                    {
                        std::ofstream ores_meta_hash_file = std::ofstream(main_data->input_ores_path + "_" + hash_file_name, std::ifstream::binary);

                        if (!ores_meta_hash_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: meta file " << main_data->input_ores_path + "-" + hash_file_name + "/" + "meta" << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        ores_meta_hash_file.close();
                    }

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(it->second) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(it->second);

                        if (main_data->rpkg_data.at(i).is_xored.at(it->second) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(it->second);
                    }

                    std::vector<char> input_data(hash_size, 0);
                    std::ifstream file = std::ifstream(main_data->input_rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(it->second), file.beg);
                    file.read(input_data.data(), hash_size);

                    if (main_data->rpkg_data.at(i).is_xored.at(it->second) == 1)
                    {
                        xor_data(input_data.data(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
                    {
                        hash_file_path = main_data->input_output_path + rpkg_base_name;

                        if (!path_exists(hash_file_path))
                        {
                            final_path = replace_slashes(hash_file_path);
                            if (!std::filesystem::create_directories(final_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << hash_file_path << std::endl;
                                std::exit(0);
                            }
                        }

                        hash_file_path.append("/" + main_data->rpkg_data.at(i).hash_resource_type.at(it->second));

                        if (!path_exists(hash_file_path))
                        {
                            final_path = replace_slashes(hash_file_path);
                            if (!std::filesystem::create_directories(final_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << hash_file_path << std::endl;
                                std::exit(0);
                            }
                        }
                    }

                    std::vector<char>* search_data;
                    uint64_t search_size;

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(it->second);
                    std::vector<char> output_data(decompressed_size, 0);

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(it->second))
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }

                        hash_file_path.append("/" + hash_file_name);
                        final_path = replace_slashes(hash_file_path);

                        if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search))
                        {
                            if (main_data->mode_extract_all_ores)
                            {
                                final_path = main_data->input_ores_path;
                            }

                            std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: Hash file " << final_path << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            output_file.write(output_data.data(), decompressed_size);

                            output_file.close();
                        }

                        search_data = &output_data;
                        search_size = decompressed_size;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "Extracted " << main_data->rpkg_data.at(i).hash_string.at(it->second) << " to " << hash_file_path << std::endl;
                        }
                    }
                    else
                    {
                        hash_file_path.append("/" + hash_file_name);
                        final_path = replace_slashes(hash_file_path);

                        if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search))
                        {
                            if (main_data->mode_extract_all_ores)
                            {
                                final_path = main_data->input_ores_path;
                            }

                            std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: Hash file " << final_path << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            output_file.write(input_data.data(), hash_size);

                            output_file.close();
                        }

                        search_data = &input_data;
                        search_size = hash_size;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "Extracted " << main_data->rpkg_data.at(i).hash_string.at(it->second) << " to " << hash_file_path << std::endl;
                        }
                    }

                    if (main_data->mode_text_search)
                    {
                        uint64_t position = 0;

                        bool done_searching = false;

                        while (!done_searching)
                        {
                            if ((position + main_data->input_text_search.length()) > search_size)
                            {
                                done_searching = true;
                                break;
                            }

                            for (uint64_t k = 0; k < main_data->input_text_search.length(); k++)
                            {
                                if (std::tolower(search_data->data()[position + k]) == std::tolower(main_data->input_text_search[k]))
                                {
                                    if (k == (main_data->input_text_search.length() - 1))
                                    {
                                        search_count++;

                                        std::cout << main_data->console_prefix << "Found text string \"" << main_data->input_text_search << "\" in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(it->second) << " at offset 0x" << std::hex << std::uppercase << (position - 1) << std::endl;

                                        bool done_searching_start = false;

                                        uint64_t position_start = position;

                                        while (!done_searching_start)
                                        {
                                            if (position_start == 0)
                                            {
                                                done_searching_start = true;
                                                break;
                                            }

                                            if (search_data->data()[position_start] < 0x20 || search_data->data()[position_start] > 0x7E)
                                            {
                                                done_searching_start = true;
                                                break;
                                            }

                                            position_start--;
                                        }

                                        std::cout << main_data->console_prefix << "Text string \"" << main_data->input_text_search << "\" is contained in string: " << std::string(search_data->data() + position_start + 1) << std::endl;

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
                    else if (main_data->mode_hex_search)
                    {
                        uint64_t position = 0;

                        bool done_searching = false;

                        std::vector<char> hex_search;

                        if (main_data->input_hex_search.length() % 2 != 0)
                        {
                            main_data->input_hex_search = "0" + main_data->input_hex_search;
                        }

                        for (uint64_t i = 0; i < (main_data->input_hex_search.length() / 2); i++)
                        {
                            hex_search.push_back((char)strtoul(main_data->input_hex_search.substr(i * 2, 2).c_str(), nullptr, 16));
                        }

                        while (!done_searching && hash_size > 0)
                        {
                            if ((position + hex_search.size()) > search_size)
                            {
                                done_searching = true;
                                break;
                            }

                            for (uint64_t k = 0; k < hex_search.size(); k++)
                            {
                                if (search_data->data()[position + k] == hex_search.at(k))
                                {
                                    if (k == (hex_search.size() - 1))
                                    {
                                        search_count++;

                                        std::cout << main_data->console_prefix << "Found hex string \"";

                                        for (uint64_t k = 0; k < hex_search.size(); k++)
                                        {
                                            std::cout << main_data->console_prefix << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)(unsigned char)hex_search.at(k);
                                        }

                                        std::cout << main_data->console_prefix << "\" in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(it->second) << " at offset 0x" << std::hex << std::uppercase << (position - 1) << std::endl;

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
                    else if (main_data->mode_regex_search)
                    {
                        std::smatch m;
                        std::regex re(main_data->input_regex_search);
                        std::string data_string;
                        data_string.reserve((uint64_t)search_size * (uint64_t)6);

                        for (uint64_t k = 0; k < search_size; k++)
                        {
                            if (search_data->data()[k] >= 0x20 && search_data->data()[k] <= 0x7E)
                            {
                                data_string += search_data->data()[k];
                            }
                            else
                            {
                                char value[5];
                                sprintf_s(value, "\\x%02X", (int)(unsigned char)search_data->data()[k]);
                                data_string += value;
                            }
                        }

                        //std::cout << main_data->console_prefix << data_string << std::endl;

                        uint64_t position = 0;

                        while (std::regex_search(data_string, m, re))
                        {
                            position += m.position();

                            std::cout << main_data->console_prefix << "Regex search with regex \"" << main_data->input_regex_search << "\" returned result in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(it->second) << std::endl;// << " at offset 0x" << std::hex << std::uppercase << position << std::endl;

                            for (size_t k = 0; k < m.size(); k++)
                            {
                                std::cout << main_data->console_prefix << "Match[" << k << "]: " << m[k].str() << std::endl;
                            }

                            data_string = m.suffix().str();
                        }
                    }

                    std::vector<char> meta_data;

                    std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(it->second), sizeof(uint64_t));
                    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                    {
                        meta_data.push_back(char8[k]);
                    }

                    std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(it->second), sizeof(uint64_t));
                    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                    {
                        meta_data.push_back(char8[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_resource_type.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_size.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_dummy.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_final.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_memory.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_video_memory.at(it->second), sizeof(uint32_t));
                    for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                    {
                        meta_data.push_back(char4[k]);
                    }

                    if (main_data->rpkg_data.at(i).hash_reference_table_size.at(it->second) > 0)
                    {
                        uint32_t hash_reference_table_size_count = 0;
                        uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(it->second).hash_reference_count & 0x3FFFFFFF;

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_data.at(it->second).hash_reference_count, sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }
                        hash_reference_table_size_count += 4;

                        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                        {
                            std::memcpy(&char1, &main_data->rpkg_data.at(i).hash_reference_data.at(it->second).hash_reference_type.at(k), sizeof(uint8_t));
                            for (uint64_t l = 0; l < sizeof(uint8_t); l++)
                            {
                                meta_data.push_back(char1[l]);
                            }
                            hash_reference_table_size_count += 1;
                        }

                        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                        {
                            std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_reference_data.at(it->second).hash_reference.at(k), sizeof(uint64_t));
                            for (uint64_t l = 0; l < sizeof(uint64_t); l++)
                            {
                                meta_data.push_back(char8[l]);
                            }
                            hash_reference_table_size_count += 8;
                        }

                        if (hash_reference_table_size_count != main_data->rpkg_data.at(i).hash_reference_table_size.at(it->second))
                        {
                            std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).hash_string.at(it->second) << " is corrupt." << std::endl;
                            std::exit(0);
                        }
                    }

                    if (!(main_data->mode_text_search || main_data->mode_hex_search || main_data->mode_regex_search || main_data->mode_extract_all_ores))
                    {
                        final_path += ".meta";
                        std::ofstream output_file = std::ofstream(final_path, std::ifstream::binary);

                        if (!output_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: Meta data file " << final_path << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        output_file.write(meta_data.data(), meta_data.size());
                    }
                }

                if (!extracted)
                {
                    std::cout << main_data->console_prefix << "Error: " << main_data->input_filter_string << " is not in " << main_data->input_rpkg_file_path << std::endl;
                }

                if (!main_data->suppress_console_output)
                {
                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    std::stringstream ss;

                    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

                    std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ');
                }
            }
        }
    }
}

void extract_all_ores(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_file_name = entry.path().string();
                }

                if (to_uppercase(rpkg_file_name.substr((rpkg_file_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_file_name);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid RPKG file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid RPKG file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            //if (rpkg_file_names.at(i) == "chunk0.rpkg")
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        if (!path_exists(main_data->input_output_path + "ORES"))
        {
            if (!std::filesystem::create_directories(main_data->input_output_path + "ORES"))
            {
                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << main_data->input_output_path + "ORES" << std::endl;
                std::exit(0);
            }
        }

        bool found_ores = false;

        std::vector<std::string> found_in;
        std::vector<std::string> not_found_in;

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            found_in.push_back("");

            not_found_in.push_back("");
        }

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            std::vector<bool> extracted;

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                extracted.push_back(false);
            }

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (hash_file_name == "00858D45F5F9E3CA.ORES") //main_data->rpkg_data.at(i).hash_resource_type.at(j) == "ORES")
                {
                    found_ores = true;

                    std::cout << main_data->console_prefix << "ORES resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* ores_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        ores_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
                    }
                    else
                    {
                        ores_data = &input_data;
                    }

                    std::vector<uint64_t> ores_hash_resource;
                    std::vector<std::string> ores_hash_resource_file_path;
                    std::vector<std::vector<std::string>> ores_hash_resource_file_path_elements;

                    uint32_t ores_hash_resource_file_count = 0;

                    uint32_t position = 0x10;

                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    std::memcpy(&bytes4, (&ores_data->get()[0] + position), sizeof(bytes4));
                    position = bytes4 + 0xC;

                    std::memcpy(&ores_hash_resource_file_count, (&ores_data->get()[0] + position), sizeof(bytes4));
                    position += 0x4;

                    std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
                    int stringstream_length = 80;

                    std::string message = main_data->console_prefix + "Extracting ORES linked files: ";

                    for (uint64_t k = 0; k < ores_hash_resource_file_count; k++)
                    {
                        if (((k * (uint64_t)100000) / (uint64_t)ores_hash_resource_file_count) % (uint64_t)100 == 0 && k > 0)
                        {
                            stringstream_length = update_console(message, ores_hash_resource_file_count, k, start_time, stringstream_length);
                        }

                        uint32_t string_offset = 0;
                        uint8_t string_length = 0;
                        uint64_t hash_lsb = 0;
                        uint64_t hash_msb = 0;
                        uint64_t hash = 0;

                        std::memcpy(&string_length, (&ores_data->get()[0] + position), sizeof(bytes1));
                        position += 0x8;

                        std::memcpy(&string_offset, (&ores_data->get()[0] + position), sizeof(bytes4));
                        string_offset += 0x10;
                        position += 0x8;

                        std::memcpy(&hash_lsb, (&ores_data->get()[0] + position), sizeof(bytes4));
                        position += 0x4;

                        std::memcpy(&hash_msb, (&ores_data->get()[0] + position), sizeof(bytes4));
                        position += 0x4;

                        hash = (hash_lsb << 32) | hash_msb;

                        //std::cout << main_data->console_prefix << "String length: " << string_length << std::endl;
                        //std::cout << main_data->console_prefix << "String offset: " << string_offset << std::endl;
                        //std::cout << main_data->console_prefix << "Hash: " << uint64_t_to_hex_string(hash) << std::endl;
                        //std::cout << main_data->console_prefix << "Hash linked ORES file path: " << std::string((&ores_data->get()[0] + string_offset)) << std::endl;

                        ores_hash_resource.push_back(hash);

                        ores_hash_resource_file_path.push_back(std::string((&ores_data->get()[0] + string_offset)));

                        replace_slashes(ores_hash_resource_file_path.back());

                        bool found = false;

                        uint64_t input_filter_index = 0;

                        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
                        {
                            std::size_t found_position_hash = hash_file_name.find(main_data->input_filter.at(z));

                            std::size_t found_position_ores = to_uppercase(ores_hash_resource_file_path.back()).find(to_uppercase(main_data->input_filter.at(z)));

                            if ((found_position_hash != std::string::npos && main_data->input_filter.at(z) != "") || (found_position_ores != std::string::npos && main_data->input_filter.at(z) != ""))
                            {
                                found = true;

                                input_filter_index = z;

                                break;
                            }
                        }

                        if (found || !main_data->mode_filter)
                        {
                            if (main_data->input_filter.size() > 0)
                            {
                                extracted.at(input_filter_index) = true;
                            }

                            std::vector<std::string> temp_ores_file_path_elements;

                            std::smatch m;
                            std::regex re("^([^\\/]+)\\/");
                            std::regex_search(ores_hash_resource_file_path.back(), m, re);

                            if (m.size() > 0)
                            {
                                temp_ores_file_path_elements.push_back(m[1].str());

                                std::smatch m2;
                                re.assign("\\/([^\\/]+)");
                                std::string ores_path = ores_hash_resource_file_path.back();

                                while (std::regex_search(ores_path, m2, re))
                                {
                                    temp_ores_file_path_elements.push_back(m2[1].str());

                                    ores_path = m2.suffix().str();
                                }
                            }
                            else
                            {
                                temp_ores_file_path_elements.push_back(ores_hash_resource_file_path.back());
                            }

                            std::string path = main_data->input_output_path + "ORES/";

                            for (uint64_t l = 0; l < temp_ores_file_path_elements.size(); l++)
                            {
                                if (l == (temp_ores_file_path_elements.size() - 1))
                                {
                                    bool ores_file_already_found = false;

                                    for (uint64_t x = 0; x < main_data->rpkg_data.size(); x++)
                                    {
                                        if (!ores_file_already_found)
                                        {
                                            std::map<uint64_t, uint64_t>::iterator it = main_data->rpkg_data.at(x).hash_map.find(hash);

                                            if (it != main_data->rpkg_data.at(x).hash_map.end())
                                            {
                                                ores_file_already_found = true;

                                                //std::cout << main_data->console_prefix << "Extracting " << ores_hash_resource_file_path.back() << "(" << uint64_t_to_hex_string(hash) << ") from RPKG file " << main_data->rpkg_data.at(x).rpkg_file_name << std::endl;

                                                path.append(temp_ores_file_path_elements.at(l));

                                                std::vector<std::string> current_input_filters = main_data->input_filter;

                                                main_data->input_filter.clear();

                                                main_data->input_filter.push_back(uint64_t_to_hex_string(hash));
                                                main_data->mode_filter = true;
                                                main_data->input_rpkg_file_name = main_data->rpkg_data.at(x).rpkg_file_name;
                                                main_data->input_rpkg_file_path = main_data->rpkg_data.at(x).rpkg_file_path;
                                                main_data->input_ores_path = path;
                                                main_data->suppress_console_output = true;

                                                extract_from_rpkg_with_map(main_data);

                                                main_data->mode_filter = false;

                                                main_data->input_filter.clear();

                                                main_data->input_filter = current_input_filters;

                                                //std::cout << std::endl;
                                            }
                                        }
                                    }

                                    //std::cout << main_data->console_prefix << "file name: " << temp_ores_file_path_elements.at(l) << std::endl;
                                }
                                else
                                {
                                    path.append(temp_ores_file_path_elements.at(l));

                                    if (!path_exists(path))
                                    {
                                        if (!std::filesystem::create_directories(path))
                                        {
                                            std::cout << main_data->console_prefix << "Error: Couldn't create directory " << path << std::endl;
                                            std::exit(0);
                                        }
                                    }

                                    path.append("/");

                                    //std::cout << main_data->console_prefix << "directory: " << path << std::endl;
                                }
                            }
                        }
                    }

                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    std::stringstream ss;

                    ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

                    std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
                }
            }

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                if (extracted.at(z))
                {
                    if (found_in.at(z) == "")
                    {
                        found_in.at(z).append(main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                    else
                    {
                        found_in.at(z).append(", " + main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                }
                else
                {
                    if (not_found_in.at(z) == "")
                    {
                        not_found_in.at(z).append(main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                    else
                    {
                        not_found_in.at(z).append(", " + main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting ORES linked files: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (!found_ores)
        {
            std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " does not contain a RPKG file that contains 00858D45F5F9E3CA.ORES" << std::endl;
            std::exit(0);
        }

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            std::cout << std::endl << main_data->console_prefix << "\"" << main_data->input_filter.at(z) << "\" was found in and extracted from: " << found_in.at(z) << std::endl;

            std::cout << std::endl << main_data->console_prefix << "\"" << main_data->input_filter.at(z) << "\" was not found in RPKG file(s): " << not_found_in.at(z) << std::endl;
        }
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void extract_all_wwev(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_file_name = entry.path().string();
                }

                if (to_uppercase(rpkg_file_name.substr((rpkg_file_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_file_name);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid RPKG file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid RPKG file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            //if (rpkg_file_names.at(i) == "chunk0.rpkg")
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        if (!path_exists(main_data->input_output_path + "WWEV"))
        {
            if (!std::filesystem::create_directories(main_data->input_output_path + "WWEV"))
            {
                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << main_data->input_output_path + "WWEV" << std::endl;
                std::exit(0);
            }
        }

        uint64_t wwev_count = 0;
        uint64_t wwev_hash_size_total = 0;

        start_time = std::chrono::high_resolution_clock::now();
        period_count = 1;

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "WWEV")
                {
                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                    if (time_in_seconds_from_start_time > console_update_rate)
                    {
                        start_time = end_time;

                        if (period_count > 3)
                        {
                            period_count = 0;
                        }

                        std::stringstream ss;

                        ss << "Scanning RPKGs for WWEV files" << std::string(period_count, '.');

                        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                        period_count++;
                    }

                    wwev_hash_size_total += main_data->rpkg_data.at(i).hash_size_final.at(j);

                    wwev_count++;
                }
            }
        }

        ss.str(std::string());

        ss << "Scanning RPKGs for WWEV files: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        std::cout << main_data->console_prefix << "Extracting WWEV to *.wem/*.ogg files to path: " << main_data->input_output_path + "WWEV" << std::endl;

        std::string message = main_data->console_prefix + "Extracting WWEV files: ";

        if (main_data->mode_filter)
        {
            std::cout << main_data->console_prefix << "Extracting WWEV to *.wem/*.ogg files with filter \"" << main_data->input_filter_string << "\"" << std::endl;
        }

        uint64_t wwev_count_current = 0;
        uint64_t wwev_hash_size_current = 0;

        std::vector<std::string> found_in;
        std::vector<std::string> not_found_in;

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            found_in.push_back("");

            not_found_in.push_back("");
        }

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            bool archive_folder_created = false;

            std::vector<bool> extracted;

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                extracted.push_back(false);
            }

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "WWEV")
                {
                    if (((wwev_count_current * (uint64_t)100000) / (uint64_t)wwev_count) % (uint64_t)10 == 0 && wwev_count_current > 0)
                    {
                        stringstream_length = update_console(message, wwev_hash_size_total, wwev_hash_size_current, start_time, stringstream_length);
                    }

                    wwev_hash_size_current += main_data->rpkg_data.at(i).hash_size_final.at(j);

                    wwev_count_current++;

                    std::string current_path = main_data->input_output_path + "WWEV/" + main_data->rpkg_data.at(i).rpkg_file_name;

                    //std::cout << main_data->console_prefix << "WWEV resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* wwev_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        wwev_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
                    }
                    else
                    {
                        wwev_data = &input_data;
                    }

                    uint32_t wwev_file_name_length = 0;
                    uint32_t wwev_file_count = 0;
                    uint32_t wwev_file_count_test = 0;

                    uint32_t position = 0;

                    char input[1024];
                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    std::memcpy(&wwev_file_name_length, (&wwev_data->get()[0] + position), sizeof(bytes4));

                    std::vector<char> wwev_meta_data;

                    char hash[8];

                    std::memcpy(&hash, &main_data->rpkg_data.at(i).hash.at(j), 0x8);

                    for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                    {
                        wwev_meta_data.push_back(hash[k]);
                    }

                    std::memcpy(&input, (&wwev_data->get()[0] + position), (wwev_file_name_length + (uint64_t)0xC));
                    for (uint64_t k = 0; k < (wwev_file_name_length + (uint64_t)0xC); k++)
                    {
                        wwev_meta_data.push_back(input[k]);
                    }

                    position += 0x4;

                    std::unique_ptr<char[]> wwev_file_name;
                    wwev_file_name = std::make_unique<char[]>((uint64_t)wwev_file_name_length + (uint64_t)1);
                    wwev_file_name[wwev_file_name_length] = 0;

                    std::memcpy(wwev_file_name.get(), (&wwev_data->get()[0] + position), wwev_file_name_length);
                    position += wwev_file_name_length;
                    position += 0x4;

                    std::memcpy(&wwev_file_count, (&wwev_data->get()[0] + position), sizeof(bytes4));
                    position += 0x4;

                    std::memcpy(&wwev_file_count_test, (&wwev_data->get()[0] + position), sizeof(bytes4));

                    //std::cout << main_data->console_prefix << "WWEV resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    bool found = false;

                    uint64_t input_filter_index = 0;

                    for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
                    {
                        std::size_t found_position_hash = hash_file_name.find(main_data->input_filter.at(z));

                        std::size_t found_position_wwev = to_uppercase(std::string(wwev_file_name.get())).find(to_uppercase(main_data->input_filter.at(z)));

                        if ((found_position_hash != std::string::npos && main_data->input_filter.at(z) != "") || (found_position_wwev != std::string::npos && main_data->input_filter.at(z) != ""))
                        {
                            found = true;

                            input_filter_index = z;

                            break;
                        }
                    }

                    if (found || !main_data->mode_filter)
                    {
                        if (main_data->input_filter.size() > 0)
                        {
                            extracted.at(input_filter_index) = true;
                        }

                        if (!archive_folder_created)
                        {
                            if (!path_exists(current_path))
                            {
                                archive_folder_created = true;

                                if (!std::filesystem::create_directories(current_path))
                                {
                                    std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                                    std::exit(0);
                                }
                            }
                        }

                        current_path.append("/");
                        current_path.append(std::string(wwev_file_name.get()));

                        if (!path_exists(current_path))
                        {
                            if (!std::filesystem::create_directories(current_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                                std::exit(0);
                            }
                        }

                        std::string wem_path = current_path + "/wem";

                        if (!path_exists(wem_path))
                        {
                            if (!std::filesystem::create_directories(wem_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << wem_path << std::endl;
                                std::exit(0);
                            }
                        }

                        std::string ogg_path = current_path + "/ogg";

                        if (!path_exists(ogg_path))
                        {
                            if (!std::filesystem::create_directories(ogg_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << ogg_path << std::endl;
                                std::exit(0);
                            }
                        }

                        std::ofstream wwev_meta_data_file = std::ofstream(current_path + "/" + "meta", std::ifstream::binary);

                        if (!wwev_meta_data_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: meta file " << current_path + "/" + "meta" << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        if (wwev_file_count > 0)
                        {
                            for (uint64_t k = 0; k < wwev_file_count; k++)
                            {
                                std::memcpy(&input, (&wwev_data->get()[0] + position), 0x8);
                                for (uint64_t l = 0; l < 0x8; l++)
                                {
                                    wwev_meta_data.push_back(input[l]);
                                }

                                position += 0x4;

                                uint32_t wem_size;

                                std::memcpy(&wem_size, (&wwev_data->get()[0] + position), sizeof(bytes4));
                                position += 0x4;

                                std::unique_ptr<char[]> wwev_file_data;
                                wwev_file_data = std::make_unique<char[]>(wem_size);

                                std::memcpy(wwev_file_data.get(), (&wwev_data->get()[0] + position), wem_size);
                                position += wem_size;

                                std::string wem_file = wem_path + "/" + std::to_string(k) + ".wem";

                                std::ofstream output_file = std::ofstream(wem_file, std::ifstream::binary);

                                if (!output_file.good())
                                {
                                    std::cout << main_data->console_prefix << "Error: wem file " << wem_file << " could not be created." << std::endl;
                                    std::exit(0);
                                }

                                output_file.write(wwev_file_data.get(), wem_size);

                                output_file.close();

                                //std::cout << main_data->console_prefix << "wem file created: " << wem_file << std::endl;

                                if (main_data->mode_extract_all_wwev_to_ogg)
                                {
                                    if (!path_exists("packed_codebooks_aoTuV_603.bin"))
                                    {
                                        std::cout << main_data->console_prefix << "Error: Missing packed_codebooks_aoTuV_603.bin file." << std::endl;
                                        std::cout << main_data->console_prefix << "       Attempting to create the packed_codebooks_aoTuV_603.bin file." << std::endl;

                                        output_file = std::ofstream("packed_codebooks_aoTuV_603.bin", std::ifstream::binary);

                                        if (!output_file.good())
                                        {
                                            std::cout << main_data->console_prefix << "Error: packed_codebooks_aoTuV_603.bin file packed_codebooks_aoTuV_603.bin could not be created." << std::endl;
                                            std::cout << main_data->console_prefix << "       A packed_codebooks_aoTuV_603.bin must be in the same directory as rpkg.exe" << std::endl;
                                            std::exit(0);
                                        }

                                        output_file.write((const char*)codebook, sizeof(codebook));
                                    }

                                    std::string ogg_file = ogg_path + "/" + std::to_string(k) + ".ogg";

                                    output_file = std::ofstream(ogg_file, std::ifstream::binary);

                                    if (!output_file.good())
                                    {
                                        std::cout << main_data->console_prefix << "Error: ogg file " << ogg_file << " could not be created." << std::endl;
                                        std::exit(0);
                                    }

                                    try
                                    {
                                        Wwise_RIFF_Vorbis ww(wem_file, "packed_codebooks_aoTuV_603.bin", false, false, kNoForcePacketFormat);

                                        ww.generate_ogg(output_file);
                                    }
                                    catch (const Parse_error& pe)
                                    {
                                        std::cout << main_data->console_prefix << "WWEV resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;
                                        std::cout << main_data->console_prefix << "Error parsing ogg file " << wem_file << " could not be created." << std::endl;
                                        std::cout << main_data->console_prefix << "Error: " << pe << std::endl;
                                    }

                                    output_file.close();

                                    //std::cout << main_data->console_prefix << "ogg file created: " << ogg_file << std::endl;
                                }
                            }
                        }
                        else
                        {
                            std::memcpy(&bytes4, (&wwev_data->get()[0] + position), 0x4);

                            if (bytes4 == 0)
                            {
                                std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                for (uint64_t k = 0; k < 0x4; k++)
                                {
                                    wwev_meta_data.push_back(input[k]);
                                }
                            }
                            else
                            {
                                std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                for (uint64_t k = 0; k < 0x4; k++)
                                {
                                    wwev_meta_data.push_back(input[k]);
                                }

                                std::memcpy(&bytes4, (&wwev_data->get()[0] + position), 0x4);

                                position += 0x4;

                                for (uint64_t k = 0; k < bytes4; k++)
                                {
                                    std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    for (uint64_t l = 0; l < 0x4; l++)
                                    {
                                        wwev_meta_data.push_back(input[l]);
                                    }

                                    position += 0x4;

                                    std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    for (uint64_t l = 0; l < 0x4; l++)
                                    {
                                        wwev_meta_data.push_back(input[l]);
                                    }

                                    position += 0x4;

                                    uint32_t wem_size;

                                    std::memcpy(&wem_size, (&wwev_data->get()[0] + position), sizeof(bytes4));

                                    std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    for (uint64_t l = 0; l < 0x4; l++)
                                    {
                                        wwev_meta_data.push_back(input[l]);
                                    }

                                    position += 0x4;

                                    //std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    //for (uint64_t l = 0; l < wem_size; l++)
                                    //{
                                        //wwev_meta_data.push_back(wwev_data->get()[position + l]);
                                    //}

                                    position += wem_size;
                                }
                            }
                        }

                        if ((position + 0x4) <= decompressed_size)
                        {
                            std::memcpy(&bytes4, (&wwev_data->get()[0] + position), 0x4);

                            if (bytes4 == 0)
                            {
                                std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                for (uint64_t k = 0; k < 0x4; k++)
                                {
                                    wwev_meta_data.push_back(input[k]);
                                }
                            }
                            else
                            {
                                std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                for (uint64_t k = 0; k < 0x4; k++)
                                {
                                    wwev_meta_data.push_back(input[k]);
                                }

                                std::memcpy(&bytes4, (&wwev_data->get()[0] + position), 0x4);

                                position += 0x4;

                                for (uint64_t k = 0; k < bytes4; k++)
                                {
                                    std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    for (uint64_t l = 0; l < 0x4; l++)
                                    {
                                        wwev_meta_data.push_back(input[l]);
                                    }

                                    position += 0x4;

                                    std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    for (uint64_t l = 0; l < 0x4; l++)
                                    {
                                        wwev_meta_data.push_back(input[l]);
                                    }

                                    position += 0x4;

                                    uint32_t wem_size;

                                    std::memcpy(&wem_size, (&wwev_data->get()[0] + position), sizeof(bytes4));

                                    std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    for (uint64_t l = 0; l < 0x4; l++)
                                    {
                                        wwev_meta_data.push_back(input[l]);
                                    }

                                    position += 0x4;

                                    //std::memcpy(&input, (&wwev_data->get()[0] + position), 0x4);
                                    //for (uint64_t l = 0; l < wem_size; l++)
                                    //{
                                        //wwev_meta_data.push_back(wwev_data->get()[position + l]);
                                    //}

                                    position += wem_size;
                                }
                            }
                        }
                        else if (position == decompressed_size)
                        {
                            //std::cout << main_data->console_prefix << hash_file_name << "'s size is odd: " << ((position + 0x4) - decompressed_size) << std::endl;
                        }
                        else
                        {
                            std::cout << main_data->console_prefix << (position + 0x4) << "," << decompressed_size << std::endl;

                            std::cout << main_data->console_prefix << hash_file_name << "'s size is odd: " << ((position + 0x4) - decompressed_size) << std::endl;
                        }

                        wwev_meta_data_file.write(wwev_meta_data.data(), wwev_meta_data.size());

                        wwev_meta_data_file.close();

                        std::vector<char> meta_data;
                        char char1[1];
                        char char4[4];
                        char char8[8];

                        std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(j), sizeof(uint64_t));
                        for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                        {
                            meta_data.push_back(char8[k]);
                        }

                        std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(j), sizeof(uint64_t));
                        for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                        {
                            meta_data.push_back(char8[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_resource_type.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_size.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_dummy.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_final.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_memory.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_video_memory.at(j), sizeof(uint32_t));
                        for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                        {
                            meta_data.push_back(char4[k]);
                        }

                        if (main_data->rpkg_data.at(i).hash_reference_table_size.at(j) > 0)
                        {
                            uint32_t hash_reference_table_size_count = 0;
                            uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count & 0x3FFFFFFF;

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count, sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }
                            hash_reference_table_size_count += 4;

                            for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                            {
                                std::memcpy(&char1, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_type.at(k), sizeof(uint8_t));
                                for (uint64_t l = 0; l < sizeof(uint8_t); l++)
                                {
                                    meta_data.push_back(char1[l]);
                                }
                                hash_reference_table_size_count += 1;
                            }

                            for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                            {
                                std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference.at(k), sizeof(uint64_t));
                                for (uint64_t l = 0; l < sizeof(uint64_t); l++)
                                {
                                    meta_data.push_back(char8[l]);
                                }
                                hash_reference_table_size_count += 8;
                            }

                            if (hash_reference_table_size_count != main_data->rpkg_data.at(i).hash_reference_table_size.at(j))
                            {
                                std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).hash_string.at(j) << " is corrupt." << std::endl;
                                std::exit(0);
                            }
                        }

                        std::ofstream output_file = std::ofstream(current_path + "/" + main_data->rpkg_data.at(i).hash_file_name.at(j) + "." + "meta", std::ifstream::binary);

                        if (!output_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: Meta data file " << current_path + "/" + main_data->rpkg_data.at(i).hash_file_name.at(j) + "." + "meta" << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        output_file.write(meta_data.data(), meta_data.size());

                        output_file.close();

                        /*uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count & 0x3FFFFFFF;

                        std::cout << main_data->console_prefix << main_data->rpkg_data.at(i).hash_file_name.at(j) << " has " << temp_hash_reference_count << " dependencies in " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                        if (temp_hash_reference_count > 0)
                        {
                            for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                            {
                                std::cout << main_data->console_prefix << main_data->rpkg_data.at(i).hash_file_name.at(j) << " is linked to " << main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_string.at(k) << std::endl;
                            }
                        }*/
                    }
                }
            }

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                if (extracted.at(z))
                {
                    if (found_in.at(z) == "")
                    {
                        found_in.at(z).append(main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                    else
                    {
                        found_in.at(z).append(", " + main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                }
                else
                {
                    if (not_found_in.at(z) == "")
                    {
                        not_found_in.at(z).append(main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                    else
                    {
                        not_found_in.at(z).append(", " + main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting WWEV to *.wem/*.ogg files: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            std::cout << std::endl << main_data->console_prefix << "\"" << main_data->input_filter.at(z) << "\" was found in and extracted from: " << found_in.at(z) << std::endl;

            std::cout << std::endl << main_data->console_prefix << "\"" << main_data->input_filter.at(z) << "\" was not found in RPKG file(s): " << not_found_in.at(z) << std::endl;
        }
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void rebuild_all_wwev(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        std::vector<std::string> wwev_folders;

        std::string input_folder = replace_slashes(main_data->input_rpkg_folder_path);

        //std::cout << main_data->console_prefix << input_folder << std::endl;

        if (input_folder.substr(input_folder.length() - 1, 1) == "/")
        {
            input_folder = input_folder.substr(0, input_folder.length() - 1);
        }

        //std::cout << main_data->console_prefix << input_folder << std::endl;

        if (to_uppercase(input_folder.substr((input_folder.length() - 4), 4)) != "/OGG" && to_uppercase(input_folder.substr((input_folder.length() - 4), 4)) != "/WEM" && path_exists(input_folder + "/meta") && path_exists(input_folder + "/wem"))
        {
            //std::cout << main_data->console_prefix << "WWEV folder found: " << input_folder << std::endl;

            bool wwev_folder_found = false;

            for (uint64_t i = 0; i < wwev_folders.size(); i++)
            {
                if (input_folder == wwev_folders.at(i))
                {
                    wwev_folder_found = true;
                }
            }

            if (!wwev_folder_found)
            {
                wwev_folders.push_back(input_folder);
            }
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_directory(entry.path().string()))
            {
                input_folder = replace_slashes(entry.path().string());

                //std::cout << main_data->console_prefix << input_folder << std::endl;

                if (input_folder.substr(input_folder.length() - 1, 1) == "/")
                {
                    input_folder = input_folder.substr(0, input_folder.length() - 1);
                }

                //std::cout << main_data->console_prefix << input_folder << std::endl;

                if (to_uppercase(input_folder.substr((input_folder.length() - 4), 4)) != "/OGG" && to_uppercase(input_folder.substr((input_folder.length() - 4), 4)) != "/WEM" && path_exists(input_folder + "/meta") && path_exists(input_folder + "/wem"))
                {
                    //std::cout << main_data->console_prefix << "WWEV folder found: " << input_folder << std::endl;

                    bool wwev_folder_found = false;

                    for (uint64_t i = 0; i < wwev_folders.size(); i++)
                    {
                        if (input_folder == wwev_folders.at(i))
                        {
                            wwev_folder_found = true;
                        }
                    }

                    if (!wwev_folder_found)
                    {
                        wwev_folders.push_back(input_folder);
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        std::string message = main_data->console_prefix + "Rebuilding WWEV files from: ";

        for (uint64_t i = 0; i < wwev_folders.size(); i++)
        {
            if (((i * (uint64_t)100000) / (uint64_t)wwev_folders.size()) % (uint64_t)10 == 0 && i > 0)
            {
                stringstream_length = update_console(message, wwev_folders.size(), i, start_time, stringstream_length);
            }

            //std::cout << main_data->console_prefix << wwev_folders.at(i) << std::endl;

            std::vector<std::string> wem_file_names;
            std::vector<std::string> wem_file_paths;
            std::vector<std::string> wem_file_root;

            for (const auto& entry : std::filesystem::directory_iterator(wwev_folders.at(i) + "/wem"))
            {
                if (std::filesystem::is_regular_file(entry.path().string()))
                {
                    std::size_t pos = entry.path().string().find_last_of("\\/");

                    std::string wem_file_name = "";
                    std::string hash = "";
                    std::string hash_resource_type = "";

                    if (pos != std::string::npos)
                    {
                        wem_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    }
                    else
                    {
                        wem_file_name = entry.path().string();
                    }

                    std::smatch m;
                    std::regex re("[0-9]+.wem$");

                    std::regex_search(wem_file_name, m, re);

                    if (m.size() > 0)
                    {
                        wem_file_paths.push_back(entry.path().string());
                        wem_file_names.push_back(wem_file_name);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << entry.path().string() << " is a valid wem file." << std::endl;
                        }
                    }
                    else
                    {
                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << entry.path().string() << " is not a valid wem file." << std::endl;
                        }
                    }
                }
            }

            bool all_wems_found = true;

            for (uint64_t j = 0; j < wem_file_paths.size(); j++)
            {
                if (path_exists(wwev_folders.at(i) + "/wem/" + std::to_string(j) + ".wem"))
                {
                    //std::cout << main_data->console_prefix << std::to_string(j) + ".wem" << " found." << std::endl;
                }
                else
                {
                    all_wems_found = false;

                    std::cout << main_data->console_prefix << std::to_string(j) + ".wem" << " is missing." << std::endl;
                }
            }

            if (all_wems_found)
            {
                std::ifstream meta_file = std::ifstream(wwev_folders.at(i) + "/meta", std::ifstream::binary);

                if (!meta_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: meta file " << wwev_folders.at(i) + "/meta" << " could not be opened." << std::endl;
                    std::exit(0);
                }

                meta_file.seekg(0, meta_file.end);

                uint64_t meta_file_size = meta_file.tellg();

                meta_file.seekg(0, meta_file.beg);

                std::unique_ptr<char[]> meta_data;
                meta_data = std::make_unique<char[]>(meta_file_size);

                uint64_t wwev_hash = 0;
                uint32_t wwev_file_name_length = 0;
                uint32_t wwev_file_count = 0;
                uint32_t wwev_file_count_test = 0;
                uint32_t wwev_file_header_id = 0;
                std::string wwev_file_name;

                char input[1024];
                uint8_t bytes1 = 0;
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;
                uint64_t position = 0;

                meta_file.read(meta_data.get(), meta_file_size);

                meta_file.close();

                std::memcpy(&wwev_hash, &meta_data.get()[position], sizeof(uint64_t));
                position += sizeof(uint64_t);

                std::memcpy(&wwev_file_name_length, &meta_data.get()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                std::memcpy(&input, &meta_data.get()[position], wwev_file_name_length);
                position += wwev_file_name_length;

                for (uint64_t j = 0; j < wwev_file_name_length; j++)
                {
                    wwev_file_name.push_back(input[j]);
                }

                std::memcpy(&wwev_file_header_id, &meta_data.get()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                std::memcpy(&wwev_file_count, &meta_data.get()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                if (wwev_file_count != wem_file_paths.size())
                {
                    wwev_file_count = (uint32_t)wem_file_paths.size();

                    std::cout << main_data->console_prefix << "Error: Mismatch between meta WWEV file count and number of wem files in directory: " << wwev_folders.at(i) << std::endl;
                }

                std::vector<uint32_t> wem_hashes;
                std::vector<uint32_t> wem_sizes;

                for (uint64_t j = 0; j < wwev_file_count; j++)
                {
                    uint32_t wem_hash = 0;
                    uint32_t wem_size = 0;

                    std::memcpy(&wem_hash, &meta_data.get()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    std::memcpy(&wem_size, &meta_data.get()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    wem_hashes.push_back(wem_hash);
                    wem_sizes.push_back(wem_size);
                }

                std::string wwev_hash_file_name = wwev_folders.at(i) + "/" + uint64_t_to_hex_string(wwev_hash) + ".WWEV";

                std::ofstream wwev_file = std::ofstream(wwev_hash_file_name, std::ifstream::binary);

                if (!wwev_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: WWEV file " << wwev_hash_file_name << " could not be created." << std::endl;
                    std::exit(0);
                }

                std::memcpy(&input, &wwev_file_name_length, sizeof(uint32_t));
                wwev_file.write(input, sizeof(uint32_t));

                wwev_file.write(wwev_file_name.c_str(), wwev_file_name_length);

                std::memcpy(&input, &wwev_file_header_id, sizeof(uint32_t));
                wwev_file.write(input, sizeof(uint32_t));

                std::memcpy(&input, &wwev_file_count, sizeof(uint32_t));
                wwev_file.write(input, sizeof(uint32_t));

                for (uint64_t j = 0; j < wem_file_paths.size(); j++)
                {
                    std::string wem_file_name = wwev_folders.at(i) + "/wem/" + std::to_string(j) + ".wem";

                    std::ifstream wem_file = std::ifstream(wem_file_name, std::ifstream::binary);

                    if (!wem_file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: wem file " << wem_file_name << " could not be opened." << std::endl;
                        std::exit(0);
                    }

                    wem_file.seekg(0, wem_file.end);

                    uint64_t wem_file_size = wem_file.tellg();

                    wem_file.seekg(0, wem_file.beg);

                    std::unique_ptr<char[]> wem_data;
                    wem_data = std::make_unique<char[]>(wem_file_size);

                    wem_file.read(wem_data.get(), wem_file_size);

                    wem_file.close();

                    std::memcpy(&input, &wem_hashes.at(j), sizeof(uint32_t));
                    wwev_file.write(input, sizeof(uint32_t));

                    std::memcpy(&input, &wem_file_size, sizeof(uint32_t));
                    wwev_file.write(input, sizeof(uint32_t));

                    wwev_file.write(wem_data.get(), wem_file_size);
                }

                if (meta_file_size > position)
                {
                    wwev_file.write(&meta_data.get()[position], (meta_file_size - position));
                }

                wwev_file.close();
            }
            else
            {
                std::cout << main_data->console_prefix << "Can't create WWEV from path: " << wwev_folders.at(i) << std::endl;
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to rebuild the WWEVs does not exist." << std::endl;
        std::exit(0);
    }
}

void extract_all_wwes(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_file_name = entry.path().string();
                }

                if (to_uppercase(rpkg_file_name.substr((rpkg_file_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_file_name);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid RPKG file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid RPKG file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            //if (rpkg_file_names.at(i) == "chunk0.rpkg")
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        if (!path_exists(main_data->input_output_path + "WWES"))
        {
            if (!std::filesystem::create_directories(main_data->input_output_path + "WWES"))
            {
                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << main_data->input_output_path + "WWES" << std::endl;
                std::exit(0);
            }
        }

        std::map<std::string, uint64_t> fxas_to_wwes_ioi_path_map;
        std::vector<std::string> fxas_wwes_ioi_path;
        std::vector<std::string> fxas_file_name;
        std::vector<uint64_t> fxas_hash;

        uint64_t total_wwes_fxas_linked = 0;
        uint64_t total_wwes_fxas_not_linked = 0;

        uint64_t wwes_count = 0;

        start_time = std::chrono::high_resolution_clock::now();
        period_count = 1;

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "WWES")
                {
                    wwes_count++;
                }

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "FXAS")
                {
                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                    if (time_in_seconds_from_start_time > console_update_rate)
                    {
                        start_time = end_time;

                        if (period_count > 3)
                        {
                            period_count = 0;
                        }

                        std::stringstream ss;

                        ss << "Importing IOI paths from FXAS files/resources" << std::string(period_count, '.');

                        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                        period_count++;
                    }

                    //std::cout << main_data->console_prefix << "FXAS resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* fxas_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        fxas_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
                    }
                    else
                    {
                        fxas_data = &input_data;
                    }

                    uint64_t position = 0;

                    bool done_searching = false;

                    main_data->input_text_search = ".wav";

                    std::string fxas_string;

                    while (!done_searching)
                    {
                        if ((position + main_data->input_text_search.length()) > decompressed_size)
                        {
                            done_searching = true;
                            break;
                        }

                        for (uint64_t k = 0; k < main_data->input_text_search.length(); k++)
                        {
                            if (std::tolower(fxas_data->get()[position + k]) == std::tolower(main_data->input_text_search[k]))
                            {
                                if (k == (main_data->input_text_search.length() - 1))
                                {
                                    //std::cout << main_data->console_prefix << "Found text string \"" << main_data->input_text_search << "\" in hash file/resouce " << main_data->rpkg_data.at(i).hash_file_name.at(j) << " at offset 0x" << std::hex << std::uppercase << position << std::endl;

                                    bool done_searching_start = false;

                                    uint64_t position_start = position;

                                    while (!done_searching_start)
                                    {
                                        if (position_start == 0)
                                        {
                                            done_searching_start = true;
                                            break;
                                        }

                                        if (fxas_data->get()[position_start] < 0x20 || fxas_data->get()[position_start] > 0x7E)
                                        {
                                            done_searching_start = true;
                                            break;
                                        }

                                        position_start--;
                                    }

                                    fxas_string = std::string(fxas_data->get() + position_start + 1);

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

                                            fxas_wwes_ioi_path.push_back(m[1].str());

                                            fxas_file_name.push_back(hash_file_name);

                                            fxas_hash.push_back(main_data->rpkg_data.at(i).hash.at(j));

                                            fxas_to_wwes_ioi_path_map[lowercase] = (fxas_wwes_ioi_path.size() - 1);

                                            //std::cout << main_data->console_prefix << "Text string \"" << main_data->input_text_search << "\" is contained in string: " << fxas_string << std::endl;
                                        }
                                        else
                                        {
                                            std::cout << main_data->console_prefix << "Error: Could not parse FXAS IOI path name." << std::endl;
                                            std::exit(0);
                                        }
                                    }
                                    else
                                    {
                                        std::cout << main_data->console_prefix << "Error: Could not find FXAS IOI path name." << std::endl;
                                        std::exit(0);
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

        ss.str(std::string());

        ss << "Importing IOI paths from FXAS files/resources: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        start_time = std::chrono::high_resolution_clock::now();
        int stringstream_length = 80;

        uint64_t wwes_count_current = 0;

        std::string message = main_data->console_prefix + "Extracting WWES to IOI Paths: ";

        if (main_data->mode_filter)
        {
            std::cout << main_data->console_prefix << "Extracting WWES to *.wem/*.ogg files with filter \"" << main_data->input_filter_string << "\"" << std::endl;
        }

        std::vector<std::string> found_in;
        std::vector<std::string> not_found_in;

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            found_in.push_back("");

            not_found_in.push_back("");
        }

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            bool archive_folder_created = false;

            std::vector<bool> extracted;

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                extracted.push_back(false);
            }

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "WWES")
                {
                    if (((wwes_count_current * (uint64_t)100000) / (uint64_t)wwes_count) % (uint64_t)10 == 0 && wwes_count_current > 0)
                    {
                        stringstream_length = update_console(message, wwes_count, wwes_count_current, start_time, stringstream_length);
                    }

                    wwes_count_current++;

                    //std::cout << main_data->console_prefix << "WWES resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* wwes_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        wwes_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
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
                            if (wwes_data->get()[position + k] != adtllabl[k])
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

                    position += (uint32_t)adtllabl.length();

                    std::memcpy(&wwes_file_name_length, (&wwes_data->get()[0] + position), sizeof(bytes4));
                    position += 0x8;

                    wwes_file_name_length -= 0x4;

                    std::vector<char> wwes_file_name;

                    std::memcpy(&input, (&wwes_data->get()[0] + position), wwes_file_name_length);
                    for (uint64_t k = 0; k < wwes_file_name_length; k++)
                    {
                        wwes_file_name.push_back(input[k]);
                    }

                    std::string wwes_string = std::string(wwes_file_name.begin(), wwes_file_name.end());

                    std::string lowercase;

                    for (int z = 0; z < wwes_string.length(); z++)
                    {
                        lowercase.push_back(std::tolower(wwes_string[z]));
                    }

                    //std::cout << main_data->console_prefix << "WWES string: " << lowercase << std::endl;

                    std::map<std::string, uint64_t>::iterator it = fxas_to_wwes_ioi_path_map.find(lowercase);

                    if (it != fxas_to_wwes_ioi_path_map.end())
                    {
                        bool found = false;

                        uint64_t input_filter_index = 0;

                        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
                        {
                            std::size_t found_position_hash = hash_file_name.find(main_data->input_filter.at(z));

                            std::size_t found_position_wwes = to_uppercase(lowercase).find(to_uppercase(main_data->input_filter.at(z)));

                            if ((found_position_hash != std::string::npos && main_data->input_filter.at(z) != "") || (found_position_wwes != std::string::npos && main_data->input_filter.at(z) != ""))
                            {
                                found = true;

                                input_filter_index = z;

                                break;
                            }
                        }

                        if (found || !main_data->mode_filter)
                        {
                            if (main_data->input_filter.size() > 0)
                            {
                                extracted.at(input_filter_index) = true;
                            }

                            //std::cout << main_data->console_prefix << lowercase << " was found in " << fxas_wwes_ioi_path.at(it->second) << std::endl;
                            //std::cout << main_data->console_prefix << "Located in FXAS file: " << fxas_file_name.at(it->second) << std::endl;

                            total_wwes_fxas_linked++;

                            std::size_t pos = fxas_wwes_ioi_path.at(it->second).find_last_of("\\/");

                            std::string directory = main_data->input_output_path + "WWES" + "/";

                            if (pos != std::string::npos)
                            {
                                directory.append(fxas_wwes_ioi_path.at(it->second).substr(0, pos));
                            }
                            else
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't parse directory from FXAS IOI path." << std::endl;
                                std::exit(0);
                            }

                            if (!path_exists(directory))
                            {
                                if (!std::filesystem::create_directories(directory))
                                {
                                    std::cout << main_data->console_prefix << "Error: Couldn't create directory " << directory << std::endl;
                                    std::exit(0);
                                }
                            }

                            std::string wem_file_name = main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second) + ".wem";

                            std::ofstream wem_file = std::ofstream(wem_file_name, std::ifstream::binary);

                            if (!wem_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: wem file " << wem_file_name << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            wem_file.write(wwes_data->get(), decompressed_size);

                            wem_file.close();

                            std::ofstream wwes_meta_data_file = std::ofstream(main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second) + "_" + hash_file_name, std::ifstream::binary);

                            if (!wwes_meta_data_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: meta file " << main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second) + "_" + hash_file_name << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            /*std::ofstream wwes_meta_data_file = std::ofstream(main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second) + ".meta", std::ifstream::binary);

                            if (!wwes_meta_data_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: meta file " << main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second) + ".meta" << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            std::vector<char> wwes_meta_data;

                            char hash[8];

                            std::memcpy(&hash, &main_data->rpkg_data.at(i).hash.at(j), 0x8);

                            for (int z = 0; z < 0x8; z++)
                            {
                                wwes_meta_data.push_back(hash[z]);
                            }

                            std::memcpy(&hash, &fxas_hash.at(it->second), 0x8);

                            for (int z = 0; z < 0x8; z++)
                            {
                                wwes_meta_data.push_back(hash[z]);
                            }

                            wwes_meta_data_file.write(wwes_meta_data.data(), wwes_meta_data.size());*/

                            wwes_meta_data_file.close();

                            //std::cout << main_data->console_prefix << "wem file created: " << wem_file << std::endl;

                            if (!path_exists("packed_codebooks_aoTuV_603.bin"))
                            {
                                std::cout << main_data->console_prefix << "Error: Missing packed_codebooks_aoTuV_603.bin file." << std::endl;
                                std::cout << main_data->console_prefix << "       Attempting to create the packed_codebooks_aoTuV_603.bin file." << std::endl;

                                std::ofstream output_file = std::ofstream("packed_codebooks_aoTuV_603.bin", std::ifstream::binary);

                                if (!output_file.good())
                                {
                                    std::cout << main_data->console_prefix << "Error: packed_codebooks_aoTuV_603.bin file packed_codebooks_aoTuV_603.bin could not be created." << std::endl;
                                    std::cout << main_data->console_prefix << "       A packed_codebooks_aoTuV_603.bin must be in the same directory as rpkg.exe" << std::endl;
                                    std::exit(0);
                                }

                                output_file.write((const char*)codebook, sizeof(codebook));
                            }

                            std::string ogg_file = main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second) + ".ogg";

                            std::ofstream output_file = std::ofstream(ogg_file, std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: ogg file " << ogg_file << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            try
                            {
                                Wwise_RIFF_Vorbis ww(wem_file_name, "packed_codebooks_aoTuV_603.bin", false, false, kNoForcePacketFormat);

                                ww.generate_ogg(output_file);
                            }
                            catch (const Parse_error& pe)
                            {
                                std::cout << main_data->console_prefix << "WWEV resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;
                                std::cout << main_data->console_prefix << "Error parsing ogg file " << wem_file_name << " could not be created." << std::endl;
                                std::cout << main_data->console_prefix << "Error: " << pe << std::endl;
                            }

                            output_file.close();

                            //std::cout << main_data->console_prefix << "ogg file created: " << ogg_file << std::endl;

                            std::vector<char> meta_data;
                            char char1[1];
                            char char4[4];
                            char char8[8];

                            std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(j), sizeof(uint64_t));
                            for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                            {
                                meta_data.push_back(char8[k]);
                            }

                            std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(j), sizeof(uint64_t));
                            for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                            {
                                meta_data.push_back(char8[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_resource_type.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_size.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_dummy.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_final.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_memory.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_video_memory.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            if (main_data->rpkg_data.at(i).hash_reference_table_size.at(j) > 0)
                            {
                                uint32_t hash_reference_table_size_count = 0;
                                uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count & 0x3FFFFFFF;

                                std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count, sizeof(uint32_t));
                                for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                {
                                    meta_data.push_back(char4[k]);
                                }
                                hash_reference_table_size_count += 4;

                                for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                                {
                                    std::memcpy(&char1, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_type.at(k), sizeof(uint8_t));
                                    for (uint64_t l = 0; l < sizeof(uint8_t); l++)
                                    {
                                        meta_data.push_back(char1[l]);
                                    }
                                    hash_reference_table_size_count += 1;
                                }

                                for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                                {
                                    std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference.at(k), sizeof(uint64_t));
                                    for (uint64_t l = 0; l < sizeof(uint64_t); l++)
                                    {
                                        meta_data.push_back(char8[l]);
                                    }
                                    hash_reference_table_size_count += 8;
                                }

                                if (hash_reference_table_size_count != main_data->rpkg_data.at(i).hash_reference_table_size.at(j))
                                {
                                    std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).hash_string.at(j) << " is corrupt." << std::endl;
                                    std::exit(0);
                                }
                            }

                            std::string output_meta_file_path = "";

                            output_meta_file_path = remove_all_string_from_string(replace_slashes(main_data->input_output_path + "WWES" + "/" + fxas_wwes_ioi_path.at(it->second)), "\"");
                            output_meta_file_path = remove_all_string_from_string(output_meta_file_path, "\'");

                            if (output_meta_file_path.substr(output_meta_file_path.length() - 1, 1) == "/")
                            {
                                output_meta_file_path = output_meta_file_path.substr(0, output_meta_file_path.length() - 1);
                            }

                            pos = output_meta_file_path.find_last_of("\\/");

                            if (pos != std::string::npos)
                            {
                                std::string metas_directory = output_meta_file_path.substr(0, pos + 1) + "/metas";

                                if (!path_exists(metas_directory))
                                {
                                    if (!std::filesystem::create_directories(metas_directory))
                                    {
                                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << directory << std::endl;
                                        std::exit(0);
                                    }
                                }

                                output_file = std::ofstream(metas_directory + "/" + main_data->rpkg_data.at(i).hash_file_name.at(j) + "." + "meta", std::ifstream::binary);

                                if (!output_file.good())
                                {
                                    std::cout << main_data->console_prefix << "Error: Meta data file " << metas_directory + "/" + main_data->rpkg_data.at(i).hash_file_name.at(j) + "/" + "meta" << " could not be created." << std::endl;
                                    std::exit(0);
                                }

                                output_file.write(meta_data.data(), meta_data.size());

                                output_file.close();
                            }
                            else
                            {
                                std::cout << main_data->console_prefix << "Error: Meta data file " << main_data->rpkg_data.at(i).hash_file_name.at(j) + "." + "meta" << " could not be created." << std::endl;
                            }
                        }
                    }
                    else
                    {
                        bool found = false;

                        uint64_t input_filter_index = 0;

                        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
                        {
                            std::size_t found_position_hash = hash_file_name.find(main_data->input_filter.at(z));

                            std::size_t found_position_wwev = to_uppercase(wwes_string).find(to_uppercase(main_data->input_filter.at(z)));

                            if ((found_position_hash != std::string::npos && main_data->input_filter.at(z) != "") || (found_position_wwev != std::string::npos && main_data->input_filter.at(z) != ""))
                            {
                                found = true;

                                input_filter_index = z;

                                break;
                            }
                        }

                        if (found || !main_data->mode_filter)
                        {
                            if (main_data->input_filter.size() > 0)
                            {
                                extracted.at(input_filter_index) = true;
                            }

                            std::cout << main_data->console_prefix << "Error: Could not match WWES string to any FXAS IOI path names." << std::endl;

                            total_wwes_fxas_not_linked++;

                            std::string directory = main_data->input_output_path + "WWES" + "/" + "Assembly\\Sound\\Wwise\\Originals\\Voices\\English(US)\\_UNKNOWN";

                            if (!path_exists(directory))
                            {
                                if (!std::filesystem::create_directories(directory))
                                {
                                    std::cout << main_data->console_prefix << "Error: Couldn't create directory " << directory << std::endl;
                                    std::exit(0);
                                }
                            }

                            std::string wem_file_name = directory + "/" + wwes_string + ".wem";

                            std::ofstream wem_file = std::ofstream(wem_file_name, std::ifstream::binary);

                            if (!wem_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: wem file " << wem_file_name << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            wem_file.write(wwes_data->get(), decompressed_size);

                            wem_file.close();

                            std::ofstream wwes_meta_data_file = std::ofstream(directory + "/" + wwes_string + ".meta", std::ifstream::binary);

                            if (!wwes_meta_data_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: meta file " << directory + "/" + wwes_string + ".meta" << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            std::vector<char> wwes_meta_data;

                            char hash[8];

                            std::memcpy(&hash, &main_data->rpkg_data.at(i).hash.at(j), 0x8);

                            for (int z = 0; z < 0x8; z++)
                            {
                                wwes_meta_data.push_back(hash[z]);
                            }

                            wwes_meta_data_file.write(wwes_meta_data.data(), wwes_meta_data.size());

                            wwes_meta_data_file.close();

                            //std::cout << main_data->console_prefix << "wem file created: " << wem_file << std::endl;

                            if (!path_exists("packed_codebooks_aoTuV_603.bin"))
                            {
                                std::cout << main_data->console_prefix << "Error: Missing packed_codebooks_aoTuV_603.bin file." << std::endl;
                                std::cout << main_data->console_prefix << "       Attempting to create the packed_codebooks_aoTuV_603.bin file." << std::endl;

                                std::ofstream output_file = std::ofstream("packed_codebooks_aoTuV_603.bin", std::ifstream::binary);

                                if (!output_file.good())
                                {
                                    std::cout << main_data->console_prefix << "Error: packed_codebooks_aoTuV_603.bin file packed_codebooks_aoTuV_603.bin could not be created." << std::endl;
                                    std::cout << main_data->console_prefix << "       A packed_codebooks_aoTuV_603.bin must be in the same directory as rpkg.exe" << std::endl;
                                    std::exit(0);
                                }

                                output_file.write((const char*)codebook, sizeof(codebook));
                            }

                            std::string ogg_file = directory + "/" + wwes_string + ".ogg";

                            std::ofstream output_file = std::ofstream(ogg_file, std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: ogg file " << ogg_file << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            try
                            {
                                Wwise_RIFF_Vorbis ww(wem_file_name, "packed_codebooks_aoTuV_603.bin", false, false, kNoForcePacketFormat);

                                ww.generate_ogg(output_file);
                            }
                            catch (const Parse_error& pe)
                            {
                                std::cout << main_data->console_prefix << "WWEV resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;
                                std::cout << main_data->console_prefix << "Error parsing ogg file " << wem_file_name << " could not be created." << std::endl;
                                std::cout << main_data->console_prefix << "Error: " << pe << std::endl;
                            }

                            output_file.close();

                            //std::cout << main_data->console_prefix << "ogg file created: " << ogg_file << std::endl;

                            std::vector<char> meta_data;
                            char char1[1];
                            char char4[4];
                            char char8[8];

                            std::memcpy(&char8, &main_data->rpkg_data.at(i).hash.at(j), sizeof(uint64_t));
                            for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                            {
                                meta_data.push_back(char8[k]);
                            }

                            std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_offset.at(j), sizeof(uint64_t));
                            for (uint64_t k = 0; k < sizeof(uint64_t); k++)
                            {
                                meta_data.push_back(char8[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_resource_type.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_size.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_table_dummy.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_final.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_memory.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_size_in_video_memory.at(j), sizeof(uint32_t));
                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                meta_data.push_back(char4[k]);
                            }

                            if (main_data->rpkg_data.at(i).hash_reference_table_size.at(j) > 0)
                            {
                                uint32_t hash_reference_table_size_count = 0;
                                uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count & 0x3FFFFFFF;

                                std::memcpy(&char4, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count, sizeof(uint32_t));
                                for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                {
                                    meta_data.push_back(char4[k]);
                                }
                                hash_reference_table_size_count += 4;

                                for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                                {
                                    std::memcpy(&char1, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_type.at(k), sizeof(uint8_t));
                                    for (uint64_t l = 0; l < sizeof(uint8_t); l++)
                                    {
                                        meta_data.push_back(char1[l]);
                                    }
                                    hash_reference_table_size_count += 1;
                                }

                                for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                                {
                                    std::memcpy(&char8, &main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference.at(k), sizeof(uint64_t));
                                    for (uint64_t l = 0; l < sizeof(uint64_t); l++)
                                    {
                                        meta_data.push_back(char8[l]);
                                    }
                                    hash_reference_table_size_count += 8;
                                }

                                if (hash_reference_table_size_count != main_data->rpkg_data.at(i).hash_reference_table_size.at(j))
                                {
                                    std::cout << main_data->console_prefix << "Error: Hash meta data for " << main_data->rpkg_data.at(i).hash_string.at(j) << " is corrupt." << std::endl;
                                    std::exit(0);
                                }
                            }

                            std::string metas_directory = directory + "/metas";

                            if (!path_exists(metas_directory))
                            {
                                if (!std::filesystem::create_directories(metas_directory))
                                {
                                    std::cout << main_data->console_prefix << "Error: Couldn't create directory " << directory << std::endl;
                                    std::exit(0);
                                }
                            }

                            output_file = std::ofstream(metas_directory + "/" + main_data->rpkg_data.at(i).hash_file_name.at(j) + "." + "meta", std::ifstream::binary);

                            if (!output_file.good())
                            {
                                std::cout << main_data->console_prefix << "Error: Meta data file " << metas_directory + "/" + main_data->rpkg_data.at(i).hash_file_name.at(j) + "." + "meta" << " could not be created." << std::endl;
                                std::exit(0);
                            }

                            output_file.write(meta_data.data(), meta_data.size());

                            output_file.close();
                        }
                    }
                }
            }

            for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
            {
                if (extracted.at(z))
                {
                    if (found_in.at(z) == "")
                    {
                        found_in.at(z).append(main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                    else
                    {
                        found_in.at(z).append(", " + main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                }
                else
                {
                    if (not_found_in.at(z) == "")
                    {
                        not_found_in.at(z).append(main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                    else
                    {
                        not_found_in.at(z).append(", " + main_data->rpkg_data.at(i).rpkg_file_name);
                    }
                }
            }
        }

        std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

        ss.str(std::string());

        ss << message << "100% Done in " << (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) << "s";

        std::cout << "\r" << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            std::cout << std::endl << main_data->console_prefix << "\"" << main_data->input_filter.at(z) << "\" was found in and extracted from: " << found_in.at(z) << std::endl;

            std::cout << std::endl << main_data->console_prefix << "\"" << main_data->input_filter.at(z) << "\" was not found in RPKG file(s): " << not_found_in.at(z) << std::endl;
        }
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void extract_all_locr(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_file_name = entry.path().string();
                }

                if (to_uppercase(rpkg_file_name.substr((rpkg_file_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_file_name);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid RPKG file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid RPKG file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            //if (rpkg_file_names.at(i) == "chunk0.rpkg")
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        if (!path_exists(main_data->input_output_path + "LOCR"))
        {
            if (!std::filesystem::create_directories(main_data->input_output_path + "LOCR"))
            {
                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << main_data->input_output_path + "LOCR" << std::endl;
                std::exit(0);
            }
        }

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            bool archive_folder_created = false;

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "LOCR")
                {
                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                    if (time_in_seconds_from_start_time > console_update_rate)
                    {
                        start_time = end_time;

                        if (period_count > 3)
                        {
                            period_count = 0;
                        }

                        ss.str(std::string());

                        ss << "Extracting LOCR as JSON" << std::string(period_count, '.');

                        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                        period_count++;
                    }

                    std::string current_path = main_data->input_output_path + "LOCR/" + main_data->rpkg_data.at(i).rpkg_file_name;

                    if (!archive_folder_created)
                    {
                        if (!path_exists(current_path))
                        {
                            archive_folder_created = true;

                            if (!std::filesystem::create_directories(current_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                                std::exit(0);
                            }
                        }
                    }

                    //std::cout << main_data->console_prefix << "LOCR resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* locr_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        locr_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
                    }
                    else
                    {
                        locr_data = &input_data;
                    }

                    uint32_t number_of_languages = 0;
                    std::vector<uint32_t> language_offsets;
                    std::vector<uint32_t> language_string_count;
                    std::vector<std::vector<uint32_t>> language_string_hash;
                    std::vector<std::vector<uint32_t>> language_string_length;
                    std::vector<std::vector<std::string>> language_string;
                    std::vector<std::set<std::pair<uint32_t, std::string>>> language_string_set;
                    std::vector<std::string> languages;
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");
                    languages.push_back("cn");
                    languages.push_back("jp");
                    languages.push_back("tc");

                    std::vector<char> json_meta_data;

                    json json_object;

                    uint32_t position = 0;

                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    position += 1;

                    std::memcpy(&number_of_languages, (&locr_data->get()[0] + position), sizeof(bytes4));

                    number_of_languages = (number_of_languages - 1) / 4;

                    for (uint64_t k = 0; k < number_of_languages; k++)
                    {
                        std::memcpy(&bytes4, (&locr_data->get()[0] + position), sizeof(bytes4));
                        position += 4;

                        language_offsets.push_back(bytes4);
                    }

                    for (uint64_t k = 0; k < (number_of_languages * (uint64_t)0x4 + (uint64_t)0x1); k++)
                    {
                        json_meta_data.push_back(locr_data->get()[k]);
                    }

                    for (uint64_t k = 0; k < number_of_languages; k++)
                    {
                        if (language_offsets.at(k) != 0xFFFFFFFF)
                        {
                            std::memcpy(&bytes4, (&locr_data->get()[0] + position), sizeof(bytes4));
                            position += 4;

                            language_string_count.push_back(bytes4);

                            std::vector<uint32_t> temp_language_string_hash;
                            std::vector<uint32_t> temp_language_string_length;
                            std::vector<std::string> temp_language_string;
                            std::set<std::pair<uint32_t, std::string>> temp_language_string_set;
                            json temp_language_json_object;

                            json temp_json_object1;

                            temp_json_object1["Language"] = languages.at(k);

                            temp_language_json_object.push_back(temp_json_object1);

                            for (uint64_t l = 0; l < language_string_count.back(); l++)
                            {
                                std::vector<char> temp_string;

                                std::memcpy(&bytes4, (&locr_data->get()[0] + position), sizeof(bytes4));
                                position += 4;

                                temp_language_string_hash.push_back(bytes4);

                                std::memcpy(&bytes4, (&locr_data->get()[0] + position), sizeof(bytes4));
                                position += 4;

                                temp_language_string_length.push_back(bytes4);

                                for (uint64_t m = 0; m < temp_language_string_length.back(); m++)
                                {
                                    temp_string.push_back(locr_data->get()[position]);
                                    position += 1;
                                }

                                position += 1;

                                if (temp_language_string_length.back() % 8 != 0)
                                {
                                    std::cout << main_data->console_prefix << "Error: LOCR file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                                    std::exit(0);
                                }

                                for (uint32_t m = 0; m < temp_language_string_length.back() / 8; m++)
                                {
                                    uint32_t data[2];
                                    std::memcpy(data, &temp_string[(uint64_t)m * (uint64_t)8], sizeof(uint32_t));
                                    std::memcpy(data + 1, &temp_string[(uint64_t)m * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                    xtea_decrypt_locr(data);

                                    std::memcpy(&temp_string[(uint64_t)m * (uint64_t)8], data, sizeof(uint32_t));
                                    std::memcpy(&temp_string[(uint64_t)m * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                                }

                                uint32_t last_zero_position = (uint32_t)temp_string.size();

                                if (temp_string.size() > 0)
                                {
                                    uint32_t m = (uint32_t)(temp_string.size() - 1);

                                    while (m >= 0)
                                    {
                                        if (temp_string.at(m) != 0)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            last_zero_position--;
                                        }

                                        m--;
                                    }
                                }

                                std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(), temp_string.end()).substr(0, last_zero_position);

                                temp_language_string.push_back(temp_string_with_zero_pad_removed);

                                std::pair<uint32_t, std::string> temp_pair;

                                temp_pair.first = temp_language_string_hash.back();
                                temp_pair.second = temp_language_string.back();

                                temp_language_string_set.insert(temp_pair);

                                json temp_json_object2;

                                temp_json_object2["StringHash"] = temp_language_string_hash.back();
                                temp_json_object2["String"] = temp_language_string.back();

                                //std::cout << main_data->console_prefix << temp_language_string_hash.back() << std::endl;
                                //std::cout << main_data->console_prefix << temp_language_string.back() << std::endl;

                                temp_language_json_object.push_back(temp_json_object2);
                            }

                            json_object.push_back(temp_language_json_object);
                        }
                    }

                    std::string json_path = current_path + "/" + hash_file_name + ".JSON";

                    std::ofstream json_file = std::ofstream(json_path, std::ifstream::binary);

                    if (!json_file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: JSON file " << json_path << " could not be created." << std::endl;
                        std::exit(0);
                    }

                    json_file << std::setw(4) << json_object << std::endl;

                    json_file.close();

                    std::string json_meta_path = current_path + "/" + hash_file_name + ".JSON.meta";

                    std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ifstream::binary);

                    if (!json_meta_file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: JSON meta file " << json_meta_path << " could not be created." << std::endl;
                        std::exit(0);
                    }

                    json_meta_file.write(json_meta_data.data(), json_meta_data.size());

                    json_meta_file.close();
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting LOCR as JSON: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void rebuild_all_locr(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> json_file_names;
        std::vector<std::string> json_file_paths;
        std::vector<std::string> json_file_base_paths;
        std::vector<uint64_t> locr_hashes;
        std::vector<std::string> locr_hash_strings;
        std::vector<std::string> locr_file_names;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string json_file_name = "";
                std::string json_file_base_path = "";
                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos)
                {
                    json_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    json_file_base_path = entry.path().string().substr(0, pos);
                }
                else
                {
                    json_file_name = entry.path().string();
                }

                if (to_uppercase(json_file_name.substr((json_file_name.length() - 5), 5)) == ".JSON")
                {
                    hash_file_name = to_uppercase(json_file_name.substr(0, (json_file_name.length() - 5)));
                }

                pos = hash_file_name.find_last_of(".");

                if (pos != std::string::npos)
                {
                    hash_string = hash_file_name.substr(0, pos);
                    resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << entry.path().string() << "," << hash_file_name << "," << hash_string << "," << resource_type << std::endl;
                }

                bool is_locr_hash_file = true;

                if (hash_string.length() != 16)
                {
                    is_locr_hash_file = false;
                }

                if (resource_type != "LOCR")
                {
                    is_locr_hash_file = false;
                }

                if (is_locr_hash_file)
                {
                    json_file_paths.push_back(entry.path().string());
                    json_file_base_paths.push_back(json_file_base_path);
                    json_file_names.push_back(json_file_name);
                    locr_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                    locr_file_names.push_back(to_uppercase(hash_file_name));
                    locr_hash_strings.push_back(to_uppercase(hash_string));

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid JSON file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a not valid JSON file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < json_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found JSON file: " << json_file_paths.at(i) << std::endl;
            }
        }

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t p = 0; p < json_file_paths.size(); p++)
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                ss.str(std::string());

                ss << "Rebuilding JSON as LOCR" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (path_exists(json_file_paths.at(p) + ".meta"))
            {
                std::ifstream input_json_meta_file = std::ifstream(json_file_paths.at(p) + ".meta", std::ifstream::binary);

                if (!input_json_meta_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be read." << std::endl;
                    std::exit(0);
                }

                input_json_meta_file.seekg(0, input_json_meta_file.end);

                uint64_t input_json_meta_file_size = input_json_meta_file.tellg();

                input_json_meta_file.seekg(0, input_json_meta_file.beg);

                if ((input_json_meta_file_size - 1) % 4 != 0)
                {
                    std::cout << main_data->console_prefix << "Error: JSON meta file " << main_data->input_packagedefinitions_thumbs_file_path + ".meta" << " is corrupt." << std::endl;
                    std::exit(0);
                }

                std::vector<char> input_json_meta_header(input_json_meta_file_size, 0);

                input_json_meta_file.read(input_json_meta_header.data(), input_json_meta_file_size);

                input_json_meta_file.close();

                std::ifstream input_json_file(json_file_paths.at(p));

                if (!input_json_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: JSON file " << json_file_paths.at(p) << " could not be read." << std::endl;
                    std::exit(0);
                }
                json input_json;

                input_json_file >> input_json;

                input_json_file.close();

                int language_count = 0;

                for (const auto& it : input_json.items())
                {
                    //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

                    bool language_found = false;

                    for (const auto& it2 : it.value().items())
                    {
                        if (it2.value().contains("Language"))
                        {
                            language_found = true;

                            language_count++;

                            //std::cout << main_data->console_prefix << it2.key() << ", " << it2.value() << std::endl << std::endl;
                        }
                    }

                    if (!language_found)
                    {
                        std::cout << main_data->console_prefix << "Error: JSON file " << json_file_paths.at(p) << " is malformed and can not be rebuilt into a LOCR file/resource." << std::endl;
                        std::exit(0);
                    }
                }

                std::vector<char> locr_data;

                if (input_json_meta_file_size == 0x19)
                {
                    //std::cout << main_data->console_prefix << locr_file_names.at(p) << " is a Hitman 3 type LOCR." << std::endl;

                    std::vector<std::string> languages;
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");

                    std::vector<bool> language_in_locr;

                    for (uint64_t i = 0; i < languages.size(); i++)
                    {
                        uint32_t offset = 0;

                        std::memcpy(&offset, &input_json_meta_header.data()[i * 0x4 + 0x1], sizeof(uint32_t));

                        if (offset != 0xFFFFFFFF)
                        {
                            language_in_locr.push_back(true);
                        }
                        else
                        {
                            language_in_locr.push_back(false);
                        }
                    }

                    locr_data.push_back(input_json_meta_header.data()[0]);

                    uint32_t offset = language_count * 0x4 + 0x1;
                    char char4[4];

                    std::memcpy(&char4, &offset, sizeof(uint32_t));

                    for (uint64_t i = 0; i < sizeof(uint32_t); i++)
                    {
                        locr_data.push_back(char4[i]);
                    }

                    std::vector<std::vector<uint32_t>> locr_language_section_string_hashes;
                    std::vector<std::vector<std::string>> locr_language_section_strings;
                    std::vector<std::vector<uint32_t>> locr_language_section_string_lengths;

                    for (uint64_t i = 0; i < languages.size(); i++)
                    {
                        if (language_in_locr.at(i))
                        {
                            for (const auto& it : input_json.items())
                            {
                                for (const auto& it2 : it.value().items())
                                {
                                    if (it2.value().contains("Language"))
                                    {
                                        if (it2.value()["Language"] == languages.at(i) && language_in_locr.at(i))
                                        {
                                            std::vector<uint32_t> temp_locr_language_section_string_hashes;
                                            std::vector<std::string> temp_locr_language_section_strings;
                                            std::vector<uint32_t> temp_locr_language_section_string_lengths;

                                            //std::cout << main_data->console_prefix << it2.key() << ", " << it2.value() << std::endl << std::endl;
                                            //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;
                                            //std::cout << main_data->console_prefix << "LOCR string count: " << it.value().size() << std::endl << std::endl;

                                            uint32_t locr_section_size = 0x4;

                                            for (const auto& it3 : it.value().items())
                                            {
                                                if (!it3.value().contains("Language"))
                                                {
                                                    //std::cout << main_data->console_prefix << "LOCR string: " << std::hex << it3.value()["String"] << std::endl;
                                                    //std::cout << main_data->console_prefix << "LOCR string hash: " << std::hex << it3.value()["StringHash"] << std::endl;

                                                    temp_locr_language_section_string_hashes.push_back((uint32_t)it3.value()["StringHash"]);

                                                    std::string temp_string = it3.value()["String"];

                                                    uint32_t string_length = (uint32_t)temp_string.length();

                                                    while (string_length % 8 != 0)
                                                    {
                                                        string_length++;

                                                        temp_string.push_back(0x0);
                                                    }

                                                    temp_locr_language_section_strings.push_back(temp_string);
                                                    temp_locr_language_section_string_lengths.push_back((uint32_t)temp_string.size());

                                                    locr_section_size += (uint32_t)temp_string.size() + (uint32_t)0x9;

                                                    //std::cout << main_data->console_prefix << "LOCR section size: " << std::hex << locr_section_size << std::endl;
                                                }
                                            }

                                            if (i != (languages.size() - 1))
                                            {
                                                offset += locr_section_size;

                                                std::memcpy(&char4, &offset, sizeof(uint32_t));

                                                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                                                {
                                                    locr_data.push_back(char4[j]);
                                                }
                                            }

                                            //std::cout << main_data->console_prefix << "LOCR offset: " << std::hex << offset << std::endl;

                                            locr_language_section_string_hashes.push_back(temp_locr_language_section_string_hashes);
                                            locr_language_section_strings.push_back(temp_locr_language_section_strings);
                                            locr_language_section_string_lengths.push_back(temp_locr_language_section_string_lengths);
                                        }
                                        else if (it2.value()["Language"] == languages.at(i) && !language_in_locr.at(i))
                                        {
                                            std::cout << main_data->console_prefix << "Error: LOCR has language in JSON but not in the meta file." << std::endl;
                                            std::exit(0);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::vector<uint32_t> temp_locr_language_section_string_hashes;
                            std::vector<std::string> temp_locr_language_section_strings;
                            std::vector<uint32_t> temp_locr_language_section_string_lengths;

                            locr_language_section_string_hashes.push_back(temp_locr_language_section_string_hashes);
                            locr_language_section_strings.push_back(temp_locr_language_section_strings);
                            locr_language_section_string_lengths.push_back(temp_locr_language_section_string_lengths);
                        }
                    }

                    for (uint64_t i = 0; i < languages.size(); i++)
                    {
                        for (const auto& it : input_json.items())
                        {
                            for (const auto& it2 : it.value().items())
                            {
                                if (it2.value().contains("Language"))
                                {
                                    if (it2.value()["Language"] == languages.at(i))
                                    {
                                        uint32_t section_string_count = (uint32_t)locr_language_section_string_hashes.at(i).size();

                                        std::memcpy(&char4, &section_string_count, sizeof(uint32_t));

                                        for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                                        {
                                            locr_data.push_back(char4[j]);
                                        }

                                        for (uint64_t j = 0; j < locr_language_section_strings.at(i).size(); j++)
                                        {
                                            std::memcpy(&char4, &locr_language_section_string_hashes.at(i).at(j), sizeof(uint32_t));

                                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                            {
                                                locr_data.push_back(char4[k]);
                                            }

                                            std::memcpy(&char4, &locr_language_section_string_lengths.at(i).at(j), sizeof(uint32_t));

                                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                            {
                                                locr_data.push_back(char4[k]);
                                            }

                                            for (uint32_t k = 0; k < locr_language_section_string_lengths.at(i).at(j) / 8; k++)
                                            {
                                                uint32_t data[2];
                                                std::memcpy(data, &locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                                std::memcpy(data + 1, &locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                                xtea_encrypt_locr(data);

                                                std::memcpy(&locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                                std::memcpy(&locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                                            }

                                            for (uint64_t k = 0; k < locr_language_section_string_lengths.at(i).at(j); k++)
                                            {
                                                locr_data.push_back(locr_language_section_strings.at(i).at(j)[k]);
                                            }

                                            locr_data.push_back(0x0);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    std::string current_path = json_file_base_paths.at(p) + "/LOCR.rebuilt";

                    if (!path_exists(current_path))
                    {
                        if (!std::filesystem::create_directories(current_path))
                        {
                            std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                            std::exit(0);
                        }
                    }

                    std::ofstream output_file = std::ofstream(current_path + "/" + locr_file_names.at(p), std::ifstream::binary);

                    if (!output_file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: Rebuilt LOCR file " << locr_file_names.at(p) << " could not be created." << std::endl;
                        std::exit(0);
                    }

                    output_file.write(locr_data.data(), locr_data.size());

                    //std::cout << main_data->console_prefix << "Successfully rebuilt LOCR " << current_path + "/" + locr_file_names.at(p) << " from " << json_file_paths.at(p) << std::endl;
                }
                else if (input_json_meta_file_size == 0x35)
                {
                    //std::cout << main_data->console_prefix << locr_file_names.at(p) << " is a Hitman 2 type LOCR." << std::endl;

                    std::vector<std::string> languages;
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");
                    languages.push_back("cn");
                    languages.push_back("jp");
                    languages.push_back("tc");

                    std::vector<bool> language_in_locr;

                    for (uint64_t i = 0; i < languages.size(); i++)
                    {
                        uint32_t offset = 0;

                        std::memcpy(&offset, &input_json_meta_header.data()[i * 0x4 + 0x1], sizeof(uint32_t));

                        if (offset != 0xFFFFFFFF)
                        {
                            language_in_locr.push_back(true);
                        }
                        else
                        {
                            language_in_locr.push_back(false);
                        }
                    }

                    locr_data.push_back(input_json_meta_header.data()[0]);

                    uint32_t offset = language_count * 0x4 + 0x1;
                    char char4[4];

                    std::memcpy(&char4, &offset, sizeof(uint32_t));

                    for (uint64_t i = 0; i < sizeof(uint32_t); i++)
                    {
                        locr_data.push_back(char4[i]);
                    }

                    std::vector<std::vector<uint32_t>> locr_language_section_string_hashes;
                    std::vector<std::vector<std::string>> locr_language_section_strings;
                    std::vector<std::vector<uint32_t>> locr_language_section_string_lengths;

                    for (uint64_t i = 0; i < languages.size(); i++)
                    {
                        if (language_in_locr.at(i))
                        {
                            for (const auto& it : input_json.items())
                            {
                                for (const auto& it2 : it.value().items())
                                {
                                    if (it2.value().contains("Language"))
                                    {
                                        if (it2.value()["Language"] == languages.at(i) && language_in_locr.at(i))
                                        {
                                            std::vector<uint32_t> temp_locr_language_section_string_hashes;
                                            std::vector<std::string> temp_locr_language_section_strings;
                                            std::vector<uint32_t> temp_locr_language_section_string_lengths;

                                            //std::cout << main_data->console_prefix << it2.key() << ", " << it2.value() << std::endl << std::endl;
                                            //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;
                                            //std::cout << main_data->console_prefix << "LOCR string count: " << it.value().size() << std::endl << std::endl;

                                            uint32_t locr_section_size = 0x4;

                                            for (const auto& it3 : it.value().items())
                                            {
                                                if (!it3.value().contains("Language"))
                                                {
                                                    //std::cout << main_data->console_prefix << "LOCR string: " << std::hex << it3.value()["String"] << std::endl;
                                                    //std::cout << main_data->console_prefix << "LOCR string hash: " << std::hex << it3.value()["StringHash"] << std::endl;

                                                    temp_locr_language_section_string_hashes.push_back((uint32_t)it3.value()["StringHash"]);

                                                    std::string temp_string = it3.value()["String"];

                                                    uint32_t string_length = (uint32_t)temp_string.length();

                                                    while (string_length % 8 != 0)
                                                    {
                                                        string_length++;

                                                        temp_string.push_back(0x0);
                                                    }

                                                    temp_locr_language_section_strings.push_back(temp_string);
                                                    temp_locr_language_section_string_lengths.push_back((uint32_t)temp_string.size());

                                                    locr_section_size += (uint32_t)temp_string.size() + (uint32_t)0x9;

                                                    //std::cout << main_data->console_prefix << "LOCR section size: " << std::hex << locr_section_size << std::endl;
                                                }
                                            }

                                            if (i != (languages.size() - 1))
                                            {
                                                offset += locr_section_size;

                                                std::memcpy(&char4, &offset, sizeof(uint32_t));

                                                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                                                {
                                                    locr_data.push_back(char4[j]);
                                                }
                                            }

                                            //std::cout << main_data->console_prefix << "LOCR offset: " << std::hex << offset << std::endl;

                                            locr_language_section_string_hashes.push_back(temp_locr_language_section_string_hashes);
                                            locr_language_section_strings.push_back(temp_locr_language_section_strings);
                                            locr_language_section_string_lengths.push_back(temp_locr_language_section_string_lengths);
                                        }
                                        else if (it2.value()["Language"] == languages.at(i) && !language_in_locr.at(i))
                                        {
                                            std::cout << main_data->console_prefix << "Error: LOCR has language in JSON but not in the meta file." << std::endl;
                                            std::exit(0);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::vector<uint32_t> temp_locr_language_section_string_hashes;
                            std::vector<std::string> temp_locr_language_section_strings;
                            std::vector<uint32_t> temp_locr_language_section_string_lengths;

                            locr_language_section_string_hashes.push_back(temp_locr_language_section_string_hashes);
                            locr_language_section_strings.push_back(temp_locr_language_section_strings);
                            locr_language_section_string_lengths.push_back(temp_locr_language_section_string_lengths);
                        }
                    }

                    for (uint64_t i = 0; i < languages.size(); i++)
                    {
                        for (const auto& it : input_json.items())
                        {
                            for (const auto& it2 : it.value().items())
                            {
                                if (it2.value().contains("Language"))
                                {
                                    if (it2.value()["Language"] == languages.at(i))
                                    {
                                        uint32_t section_string_count = (uint32_t)locr_language_section_string_hashes.at(i).size();

                                        std::memcpy(&char4, &section_string_count, sizeof(uint32_t));

                                        for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                                        {
                                            locr_data.push_back(char4[j]);
                                        }

                                        for (uint64_t j = 0; j < locr_language_section_strings.at(i).size(); j++)
                                        {
                                            std::memcpy(&char4, &locr_language_section_string_hashes.at(i).at(j), sizeof(uint32_t));

                                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                            {
                                                locr_data.push_back(char4[k]);
                                            }

                                            std::memcpy(&char4, &locr_language_section_string_lengths.at(i).at(j), sizeof(uint32_t));

                                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                                            {
                                                locr_data.push_back(char4[k]);
                                            }

                                            for (uint32_t k = 0; k < locr_language_section_string_lengths.at(i).at(j) / 8; k++)
                                            {
                                                uint32_t data[2];
                                                std::memcpy(data, &locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                                std::memcpy(data + 1, &locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                                xtea_encrypt_locr(data);

                                                std::memcpy(&locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                                std::memcpy(&locr_language_section_strings.at(i).at(j)[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                                            }

                                            for (uint64_t k = 0; k < locr_language_section_string_lengths.at(i).at(j); k++)
                                            {
                                                locr_data.push_back(locr_language_section_strings.at(i).at(j)[k]);
                                            }

                                            locr_data.push_back(0x0);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    std::string current_path = json_file_base_paths.at(p) + "/LOCR.rebuilt";

                    if (!path_exists(current_path))
                    {
                        if (!std::filesystem::create_directories(current_path))
                        {
                            std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                            std::exit(0);
                        }
                    }

                    std::ofstream output_file = std::ofstream(current_path + "/" + locr_file_names.at(p), std::ifstream::binary);

                    if (!output_file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: Rebuilt LOCR file " << locr_file_names.at(p) << " could not be created." << std::endl;
                        std::exit(0);
                    }

                    output_file.write(locr_data.data(), locr_data.size());

                    //std::cout << main_data->console_prefix << "Successfully rebuilt LOCR " << current_path + "/" + locr_file_names.at(p) << " from " << json_file_paths.at(p) << std::endl;
                }
                else
                {
                    std::cout << main_data->console_prefix << locr_file_names.at(p) << " is not a known LOCR type." << std::endl;
                    std::cout << main_data->console_prefix << "Can not rebuild " << locr_file_names.at(p) << " from JSON file " << json_file_paths.at(p) << std::endl;
                }

                //std::cout << main_data->console_prefix << "Languages found: " << language_count << std::endl;
            }
            else
            {
                std::cout << main_data->console_prefix << "Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be found." << std::endl;
                std::cout << main_data->console_prefix << "       Can not rebuild " << locr_file_names.at(p) << " from JSON file " << json_file_paths.at(p) << std::endl;
            }
        }

        ss.str(std::string());

        ss << "Rebuild JSON as LOCR: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to rebuild the LOCR files from does not exist." << std::endl;
        std::exit(0);
    }
}

void extract_all_rtlv(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_file_name = entry.path().string();
                }

                if (to_uppercase(rpkg_file_name.substr((rpkg_file_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_file_name);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid RPKG file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid RPKG file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            //if (rpkg_file_names.at(i) == "chunk0.rpkg")
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        if (!path_exists(main_data->input_output_path + "RTLV"))
        {
            if (!std::filesystem::create_directories(main_data->input_output_path + "RTLV"))
            {
                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << main_data->input_output_path + "RTLV" << std::endl;
                std::exit(0);
            }
        }

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            bool archive_folder_created = false;

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "RTLV")
                {
                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                    if (time_in_seconds_from_start_time > console_update_rate)
                    {
                        start_time = end_time;

                        if (period_count > 3)
                        {
                            period_count = 0;
                        }

                        ss.str(std::string());

                        ss << "Extracting RTLV as JSON" << std::string(period_count, '.');

                        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                        period_count++;
                    }

                    std::string current_path = main_data->input_output_path + "RTLV/" + main_data->rpkg_data.at(i).rpkg_file_name;

                    if (!archive_folder_created)
                    {
                        if (!path_exists(current_path))
                        {
                            archive_folder_created = true;

                            if (!std::filesystem::create_directories(current_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                                std::exit(0);
                            }
                        }
                    }

                    //std::cout << main_data->console_prefix << "RTLV resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* rtlv_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        rtlv_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
                    }
                    else
                    {
                        rtlv_data = &input_data;
                    }

                    uint32_t rtlv_data_size = 0;
                    uint64_t rtlv_header_value = 0;
                    uint32_t rtlv_identifier = 0;
                    std::vector<char> rtlv_footer;
                    uint32_t rtlv_header_data_size = 0;
                    std::vector<char> rtlv_header;
                    uint32_t languages_starting_offset = 0;
                    uint32_t number_of_languages = 0;
                    std::vector<uint32_t> language_offsets;
                    std::vector<std::vector<char>> language_data;
                    std::vector<uint16_t> language_data_sizes;
                    std::vector<uint16_t> language_unknowns;
                    std::vector<std::vector<std::string>> language_string;
                    std::vector<std::set<std::pair<uint32_t, std::string>>> language_string_set;
                    std::vector<std::string> languages;
                    languages.push_back("xx");
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");
                    languages.push_back("cn");
                    languages.push_back("jp");
                    languages.push_back("tc");

                    std::vector<char> json_meta_data;

                    json json_object;

                    uint32_t position = 0;

                    char char4[4] = "";
                    uint8_t bytes1 = 0;
                    uint16_t bytes2 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    position = 0x58;
                    std::memcpy(&languages_starting_offset, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                    
                    position = 0;
                    std::memcpy(&rtlv_header_value, (&rtlv_data->get()[0] + position), sizeof(bytes8));

                    for (uint64_t k = 0; k < sizeof(bytes8); k++)
                    {
                        json_meta_data.push_back(rtlv_data->get()[position + k]);
                    }

                    position += sizeof(bytes8);

                    std::memcpy(&bytes4, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                    position += sizeof(bytes4);

                    rtlv_data_size = ((bytes4 & 0x000000FF) << 0x18) + ((bytes4 & 0x0000FF00) << 0x8) + ((bytes4 & 0x00FF0000) >> 0x8) + ((bytes4 & 0xFF000000) >> 0x18);

                    if (languages_starting_offset < rtlv_data_size)
                    {
                        languages_starting_offset += 0xC;

                        rtlv_header_data_size = languages_starting_offset - position;

                        std::memcpy(&char4, &rtlv_header_data_size, sizeof(bytes4));

                        for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        {
                            json_meta_data.push_back(char4[k]);
                        }

                        for (uint64_t k = 0; k < rtlv_header_data_size; k++)
                        {
                            rtlv_header.push_back(rtlv_data->get()[position]);

                            json_meta_data.push_back(rtlv_data->get()[position]);

                            position += 1;
                        }

                        std::memcpy(&number_of_languages, (&rtlv_data->get()[0] + position), sizeof(bytes4));

                        for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        {
                            json_meta_data.push_back(rtlv_data->get()[position + k]);
                        }

                        position += sizeof(bytes4);

                        for (uint64_t k = 0; k < number_of_languages; k++)
                        {
                            uint16_t section_length = 0;

                            std::memcpy(&section_length, (&rtlv_data->get()[0] + position), sizeof(bytes2));
                            position += sizeof(bytes2);

                            std::memcpy(&bytes2, (&rtlv_data->get()[0] + position), sizeof(bytes2));
                            position += sizeof(bytes2);

                            language_unknowns.push_back(bytes2);

                            std::memcpy(&bytes4, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                            position += sizeof(bytes4);

                            std::memcpy(&bytes4, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                            position += sizeof(bytes4);

                            language_offsets.push_back(bytes4);

                            std::memcpy(&bytes4, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                            position += sizeof(bytes4);
                        }

                        for (uint64_t k = 0; k < number_of_languages; k++)
                        {
                            json temp_language_json_object;

                            temp_language_json_object["Language"] = languages.at(k);

                            std::memcpy(&bytes4, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                            position += sizeof(bytes4);

                            language_data_sizes.push_back(bytes4);

                            std::vector<char> temp_string;

                            if (language_data_sizes.back() == 0)
                            {
                                std::memcpy(&bytes4, (&rtlv_data->get()[0] + position), sizeof(bytes4));
                                position += sizeof(bytes4);

                                temp_language_json_object["String"] = "";
                            }
                            else
                            {
                                for (uint64_t l = 0; l < language_data_sizes.back(); l++)
                                {
                                    temp_string.push_back(rtlv_data->get()[position]);
                                    position += 1;
                                }

                                if (language_data_sizes.back() % 8 != 0)
                                {
                                    std::cout << main_data->console_prefix << "Error: RTLV file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                                    std::exit(0);
                                }

                                for (uint32_t m = 0; m < language_data_sizes.back() / 8; m++)
                                {
                                    uint32_t data[2];
                                    std::memcpy(data, &temp_string[(uint64_t)m * (uint64_t)8], sizeof(uint32_t));
                                    std::memcpy(data + 1, &temp_string[(uint64_t)m * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                    xtea_decrypt_rtlv(data);

                                    std::memcpy(&temp_string[(uint64_t)m * (uint64_t)8], data, sizeof(uint32_t));
                                    std::memcpy(&temp_string[(uint64_t)m * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                                }

                                uint32_t last_zero_position = (uint32_t)temp_string.size();

                                if (temp_string.size() > 0)
                                {
                                    uint32_t m = (uint32_t)(temp_string.size() - 1);

                                    while (m >= 0)
                                    {
                                        if (temp_string.at(m) != 0)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            last_zero_position--;
                                        }

                                        m--;
                                    }
                                }

                                std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(), temp_string.end()).substr(0, last_zero_position);

                                temp_language_json_object["String"] = temp_string_with_zero_pad_removed;
                            }

                            json_object.push_back(temp_language_json_object);
                        }

                        if ((decompressed_size - position) > 0)
                        {
                            for (uint64_t k = 0; k < (decompressed_size - position); k++)
                            {
                                json_meta_data.push_back(rtlv_data->get()[position + k]);
                            }
                        }

                        std::string json_path = current_path + "/" + hash_file_name + ".JSON";

                        std::ofstream json_file = std::ofstream(json_path, std::ifstream::binary);

                        if (!json_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: JSON file " << json_path << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        json_file << std::setw(4) << json_object << std::endl;

                        json_file.close();

                        std::string json_meta_path = current_path + "/" + hash_file_name + ".JSON.meta";

                        std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ifstream::binary);

                        if (!json_meta_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: JSON meta file " << json_meta_path << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        json_meta_file.write(json_meta_data.data(), json_meta_data.size());

                        json_meta_file.close();
                    }
                    else
                    {
                        //std::cout << main_data->console_prefix << "Error: RTLV file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " has no data to extract." << std::endl;
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting RTLV as JSON: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void rebuild_all_rtlv(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> json_file_names;
        std::vector<std::string> json_file_paths;
        std::vector<std::string> json_file_base_paths;
        std::vector<uint64_t> rtlv_hashes;
        std::vector<std::string> rtlv_hash_strings;
        std::vector<std::string> rtlv_file_names;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string json_file_name = "";
                std::string json_file_base_path = "";
                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos)
                {
                    json_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    json_file_base_path = entry.path().string().substr(0, pos);
                }
                else
                {
                    json_file_name = entry.path().string();
                }

                if (to_uppercase(json_file_name.substr((json_file_name.length() - 5), 5)) == ".JSON")
                {
                    hash_file_name = to_uppercase(json_file_name.substr(0, (json_file_name.length() - 5)));
                }

                pos = hash_file_name.find_last_of(".");

                if (pos != std::string::npos)
                {
                    hash_string = hash_file_name.substr(0, pos);
                    resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << entry.path().string() << "," << hash_file_name << "," << hash_string << "," << resource_type << std::endl;
                }

                bool is_rtlv_hash_file = true;

                if (hash_string.length() != 16)
                {
                    is_rtlv_hash_file = false;
                }

                if (resource_type != "RTLV")
                {
                    is_rtlv_hash_file = false;
                }

                if (is_rtlv_hash_file)
                {
                    json_file_paths.push_back(entry.path().string());
                    json_file_base_paths.push_back(json_file_base_path);
                    json_file_names.push_back(json_file_name);
                    rtlv_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                    rtlv_file_names.push_back(to_uppercase(hash_file_name));
                    rtlv_hash_strings.push_back(to_uppercase(hash_string));

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid JSON file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a not valid JSON file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < json_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found JSON file: " << json_file_paths.at(i) << std::endl;
            }
        }

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t p = 0; p < json_file_paths.size(); p++)
        {
            if (path_exists(json_file_paths.at(p) + ".meta"))
            {
                std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                if (time_in_seconds_from_start_time > console_update_rate)
                {
                    start_time = end_time;

                    if (period_count > 3)
                    {
                        period_count = 0;
                    }

                    ss.str(std::string());

                    ss << "Rebuilding JSON as RTLV" << std::string(period_count, '.');

                    std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                    period_count++;
                }

                std::ifstream input_json_meta_file = std::ifstream(json_file_paths.at(p) + ".meta", std::ifstream::binary);

                if (!input_json_meta_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be read." << std::endl;
                    std::exit(0);
                }

                input_json_meta_file.seekg(0, input_json_meta_file.end);

                uint64_t input_json_meta_file_size = input_json_meta_file.tellg();

                input_json_meta_file.seekg(0, input_json_meta_file.beg);

                std::vector<char> input_json_meta(input_json_meta_file_size, 0);

                input_json_meta_file.read(input_json_meta.data(), input_json_meta_file_size);

                input_json_meta_file.close();

                std::ifstream input_json_file(json_file_paths.at(p));

                if (!input_json_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: JSON file " << json_file_paths.at(p) << " could not be read." << std::endl;
                    std::exit(0);
                }

                json input_json;

                input_json_file >> input_json;

                input_json_file.close();

                int language_count = 0;

                for (const auto& it : input_json.items())
                {
                    //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

                    bool language_found = false;

                    if (it.value().contains("Language"))
                    {
                        language_found = true;

                        language_count++;

                        //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;
                    }

                    if (!language_found)
                    {
                        std::cout << main_data->console_prefix << "Error: JSON file " << json_file_paths.at(p) << " is malformed and can not be rebuilt into a RTLV file/resource." << std::endl;
                        std::exit(0);
                    }
                }

                std::vector<char> rtlv_data;

                char char2[2] = "";
                char char4[4] = "";
                char char8[8] = "";
                uint32_t bytes4 = 0;
                uint64_t bytes8 = 0;

                uint32_t position = 0;

                std::memcpy(&char8, &input_json_meta.data()[position], sizeof(uint64_t));
                position += sizeof(uint64_t);

                for (uint64_t j = 0; j < sizeof(uint64_t); j++)
                {
                    rtlv_data.push_back(char8[j]);
                }

                rtlv_data.push_back(0x0);
                rtlv_data.push_back(0x0);
                rtlv_data.push_back(0x0);
                rtlv_data.push_back(0x0);

                uint32_t rtlv_header_data_size = 0;

                std::memcpy(&rtlv_header_data_size, &input_json_meta.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                for (uint64_t j = 0; j < rtlv_header_data_size; j++)
                {
                    rtlv_data.push_back(input_json_meta.data()[position]);
                    position += 0x1;
                }

                uint32_t number_of_languages = 0;

                std::memcpy(&number_of_languages, &input_json_meta.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                if (language_count != number_of_languages)
                {
                    std::cout << main_data->console_prefix << "Error: Number of language in the input RTLV JSON file do not match the number of languages in the meta file." << std::endl;
                    std::exit(0);
                }

                std::memcpy(&char4, &number_of_languages, sizeof(uint32_t));

                for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                {
                    rtlv_data.push_back(char4[j]);
                }

                std::vector<std::string> language_strings;

                uint32_t offset = (uint32_t)rtlv_data.size() + (uint32_t)number_of_languages * (uint32_t)0x10 - (uint32_t)0xC;

                for (uint64_t i = 0; i < number_of_languages; i++)
                {
                    uint32_t key = 0;

                    for (const auto& it : input_json.items())
                    {
                        if (key == i)
                        {
                            //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

                            std::string temp_string = it.value()["String"];

                            //std::cout << it.value()["String"] << std::endl << std::endl;

                            uint32_t string_length = (uint32_t)temp_string.length();

                            while (string_length % 8 != 0)
                            {
                                string_length++;

                                temp_string.push_back(0x0);
                            }

                            uint32_t string_length_value = string_length | 0x40000000;

                            std::memcpy(&char4, &string_length_value, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                rtlv_data.push_back(char4[k]);
                            }

                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);

                            std::memcpy(&char4, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                rtlv_data.push_back(char4[k]);
                            }

                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);

                            if (string_length == 0x0)
                            {
                                offset += (uint32_t)0x4 + (uint32_t)string_length + (uint32_t)0x4;
                            }
                            else
                            {
                                offset += (uint32_t)0x4 + (uint32_t)string_length;
                            }

                            /*for (uint32_t k = 0; k < string_length / 8; k++)
                            {
                                uint32_t data[2];
                                std::memcpy(data, &temp_string[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                std::memcpy(data + 1, &temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                xtea_encrypt_rtlv(data);

                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                            }

                            for (uint64_t k = 0; k < string_length; k++)
                            {
                                rtlv_data.push_back(temp_string[k]);
                            }*/
                        }

                        key++;
                    }
                }

                for (uint64_t i = 0; i < number_of_languages; i++)
                {
                    uint32_t key = 0;

                    for (const auto& it : input_json.items())
                    {
                        if (key == i)
                        {
                            //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

                            std::string temp_string = it.value()["String"];

                            //std::cout << it.value()["String"] << std::endl << std::endl;

                            uint32_t string_length = (uint32_t)temp_string.length();

                            while (string_length % 8 != 0)
                            {
                                string_length++;

                                temp_string.push_back(0x0);
                            }

                            std::memcpy(&char4, &string_length, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                rtlv_data.push_back(char4[k]);
                            }

                            if (string_length == 0x0)
                            {
                                rtlv_data.push_back(0x0);
                                rtlv_data.push_back(0x0);
                                rtlv_data.push_back(0x0);
                                rtlv_data.push_back(0x0);
                            }

                            /*rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x40);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);

                            std::memcpy(&char4, &offset, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                rtlv_data.push_back(char4[k]);
                            }

                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);
                            rtlv_data.push_back(0x0);

                            offset += (uint32_t)0x4 + (uint32_t)string_length;*/

                            for (uint32_t k = 0; k < string_length / 8; k++)
                            {
                                uint32_t data[2];
                                std::memcpy(data, &temp_string[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                std::memcpy(data + 1, &temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                xtea_encrypt_rtlv(data);

                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                            }

                            for (uint64_t k = 0; k < string_length; k++)
                            {
                                rtlv_data.push_back(temp_string[k]);
                            }
                        }

                        key++;
                    }
                }

                //std::cout << json_file_paths.at(p) << std::endl;

                //std::cout << input_json_meta_file_size << ", " << position << std::endl << std::endl;

                uint32_t file_size = (uint32_t)rtlv_data.size() - (uint32_t)0x10;

                std::memcpy(&char4, &file_size, sizeof(uint32_t));

                rtlv_data.at(0x8) = char4[3];
                rtlv_data.at(0x9) = char4[2];
                rtlv_data.at(0xA) = char4[1];
                rtlv_data.at(0xB) = char4[0];

                for (uint64_t k = 0; k < (input_json_meta_file_size - position); k++)
                {
                    rtlv_data.push_back(input_json_meta.data()[position + k]);
                }

                std::string current_path = json_file_base_paths.at(p) + "/RTLV.rebuilt";

                if (!path_exists(current_path))
                {
                    if (!std::filesystem::create_directories(current_path))
                    {
                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                        std::exit(0);
                    }
                }

                std::ofstream output_file = std::ofstream(current_path + "/" + rtlv_file_names.at(p), std::ifstream::binary);

                if (!output_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: Rebuilt RTLV file " << rtlv_file_names.at(p) << " could not be created." << std::endl;
                    std::exit(0);
                }

                output_file.write(rtlv_data.data(), rtlv_data.size());

                //std::cout << main_data->console_prefix << "Successfully rebuilt RTLV " << current_path + "/" + rtlv_file_names.at(p) << " from " << json_file_paths.at(p) << std::endl;

                //std::cout << main_data->console_prefix << "Languages found: " << language_count << std::endl;
            }
            else
            {
                std::cout << main_data->console_prefix << "Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be found." << std::endl;
                std::cout << main_data->console_prefix << "       Can not rebuild " << rtlv_file_names.at(p) << " from JSON file " << json_file_paths.at(p) << std::endl;
            }
        }

        ss.str(std::string());

        ss << "Rebuilding JSON as RTLV: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to rebuild the RTLV files from does not exist." << std::endl;
        std::exit(0);
    }
}

void extract_all_dlge(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_file_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_file_name = entry.path().string();
                }

                if (to_uppercase(rpkg_file_name.substr((rpkg_file_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_file_name);

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid RPKG file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is not a valid RPKG file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            //if (rpkg_file_names.at(i) == "chunk0.rpkg")
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        if (!path_exists(main_data->input_output_path + "DLGE"))
        {
            if (!std::filesystem::create_directories(main_data->input_output_path + "DLGE"))
            {
                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << main_data->input_output_path + "DLGE" << std::endl;
                std::exit(0);
            }
        }

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
        {
            bool archive_folder_created = false;

            for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
            {
                std::string hash_file_name = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                if (main_data->rpkg_data.at(i).hash_resource_type.at(j) == "DLGE")
                {
                    std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                    double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                    if (time_in_seconds_from_start_time > console_update_rate)
                    {
                        start_time = end_time;

                        if (period_count > 3)
                        {
                            period_count = 0;
                        }

                        ss.str(std::string());

                        ss << "Extracting DLGE as JSON" << std::string(period_count, '.');

                        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                        period_count++;
                    }

                    std::string current_path = main_data->input_output_path + "DLGE/" + main_data->rpkg_data.at(i).rpkg_file_name;

                    if (!archive_folder_created)
                    {
                        if (!path_exists(current_path))
                        {
                            archive_folder_created = true;

                            if (!std::filesystem::create_directories(current_path))
                            {
                                std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                                std::exit(0);
                            }
                        }
                    }

                    //std::cout << main_data->console_prefix << "DLGE resource found: " << hash_file_name << " in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    uint64_t hash_size;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j) == 1)
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size.at(j);

                        if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = main_data->rpkg_data.at(i).hash_size_final.at(j);
                    }

                    std::unique_ptr<char[]> input_data;
                    input_data = std::make_unique<char[]>(hash_size);

                    std::ifstream file = std::ifstream(main_data->rpkg_data.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        std::cout << main_data->console_prefix << "Error: RPKG file " << main_data->input_rpkg_file_path << " could not be read." << std::endl;
                        std::exit(0);
                    }

                    file.seekg(main_data->rpkg_data.at(i).hash_offset.at(j), file.beg);
                    file.read(input_data.get(), hash_size);
                    file.close();

                    if (main_data->rpkg_data.at(i).is_xored.at(j) == 1)
                    {
                        xor_data(input_data.get(), (uint32_t)hash_size);

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "XORing input_file_data with a hashSize of " << hash_size << std::endl;
                        }
                    }

                    uint32_t decompressed_size = main_data->rpkg_data.at(i).hash_size_final.at(j);

                    std::unique_ptr<char[]> output_data;
                    output_data = std::make_unique<char[]>(decompressed_size);

                    std::unique_ptr<char[]>* dlge_data;

                    if (main_data->rpkg_data.at(i).is_lz4ed.at(j))
                    {
                        LZ4_decompress_safe(input_data.get(), output_data.get(), (int)hash_size, decompressed_size);

                        dlge_data = &output_data;

                        if (main_data->debug)
                        {
                            std::cout << main_data->console_prefix << "LZ4 decompression complete." << std::endl;
                        }
                    }
                    else
                    {
                        dlge_data = &input_data;
                    }

                    uint32_t dlge_data_size = 0;
                    uint32_t number_of_dlge_categories = 0;
                    std::vector<uint32_t> dlge_categories;
                    std::vector<uint32_t> dlge_identifiers;
                    std::vector<uint64_t> dlge_section_metas;
                    std::vector<char> dlge_footer;
                    uint32_t number_of_languages = 0;
                    std::vector<std::vector<std::string>> language_string;
                    std::vector<std::vector<uint32_t>> language_string_sizes;
                    std::vector<std::vector<uint64_t>> language_string_metas;
                    std::vector<std::string> languages;
                    languages.push_back("en");
                    languages.push_back("fr");
                    languages.push_back("it");
                    languages.push_back("de");
                    languages.push_back("es");
                    languages.push_back("ru");
                    languages.push_back("mx");
                    languages.push_back("br");
                    languages.push_back("pl");
                    languages.push_back("cn");
                    languages.push_back("jp");
                    languages.push_back("tc");

                    std::vector<char> json_meta_data;

                    json json_object;

                    uint32_t position = 0;

                    uint8_t bytes1 = 0;
                    uint16_t bytes2 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));
                    
                    //for (uint64_t k = 0; k < sizeof(bytes4); k++)
                    //{
                        //json_meta_data.push_back(dlge_data->get()[position + k]);
                    //}

                    position += sizeof(bytes4);

                    if (bytes4 != 0)
                    {
                        std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                        break;
                    }

                    std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));
                    
                    //for (uint64_t k = 0; k < sizeof(bytes4); k++)
                    //{
                        //json_meta_data.push_back(dlge_data->get()[position + k]);
                    //}

                    position += sizeof(bytes4);

                    if (bytes4 != 1)
                    {
                        std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                        break;
                    }

                    uint8_t text_available = 0;

                    std::memcpy(&bytes1, (&dlge_data->get()[0] + position), sizeof(bytes1));
                    
                    //for (uint64_t k = 0; k < sizeof(bytes1); k++)
                    //{
                        //json_meta_data.push_back(dlge_data->get()[position + k]);
                    //}

                    position += sizeof(bytes1);

                    if (bytes1 == 1)
                    {
                        text_available = 1;
                    }

                    uint32_t text_count = 0;

                    bool dlge_has_text = false;

                    bool do_not_break = true;

                    while (text_available)
                    {
                        number_of_dlge_categories++;

                        std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));

                        for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        {
                            json_meta_data.push_back(dlge_data->get()[position + k]);
                        }

                        position += sizeof(bytes4);

                        dlge_categories.push_back(bytes4);

                        std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));

                        for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        {
                            json_meta_data.push_back(dlge_data->get()[position + k]);
                        }

                        position += sizeof(bytes4);

                        dlge_identifiers.push_back(bytes4);

                        std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));

                        //for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        //{
                            //json_meta_data.push_back(dlge_data->get()[position + k]);
                        //}

                        position += sizeof(bytes4);

                        if (bytes4 != 0)
                        {
                            std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                            do_not_break = false;
                            break;
                        }

                        std::memcpy(&bytes8, (&dlge_data->get()[0] + position), sizeof(bytes8));

                        //for (uint64_t k = 0; k < sizeof(bytes8); k++)
                        //{
                            //json_meta_data.push_back(dlge_data->get()[position + k]);
                        //}

                        position += sizeof(bytes8);

                        if (bytes8 != 0xFFFFFFFFFFFFFFFF)
                        {
                            std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                            do_not_break = false;
                            break;
                        }

                        std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));

                        //for (uint64_t k = 0; k < sizeof(bytes4); k++)
                        //{
                            //json_meta_data.push_back(dlge_data->get()[position + k]);
                        //}

                        position += sizeof(bytes4);

                        if (bytes4 != 0)
                        {
                            std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                            do_not_break = false;
                            break;
                        }

                        uint32_t check_variable_1 = 0;
                        uint32_t check_variable_2 = 0;

                        std::memcpy(&check_variable_1, (&dlge_data->get()[0] + position), sizeof(bytes4));

                        position += sizeof(bytes4);

                        std::memcpy(&check_variable_2, (&dlge_data->get()[0] + position), sizeof(bytes4));

                        position += sizeof(bytes4);

                        position -= sizeof(bytes8);

                        if ((check_variable_1 == 0xFFFFFFFF && check_variable_2 == 0xFFFFFFFF) || ((check_variable_1 + 1) == check_variable_2) || check_variable_2 == 0xFFFFFFFF)
                        {
                            std::memcpy(&bytes8, (&dlge_data->get()[0] + position), sizeof(bytes8));

                            for (uint64_t k = 0; k < sizeof(bytes8); k++)
                            {
                                json_meta_data.push_back(dlge_data->get()[position + k]);
                            }

                            position += sizeof(bytes8);

                            dlge_section_metas.push_back(bytes8);
                        }
                        else
                        {
                            std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                            do_not_break = false;
                            break;
                        }

                        bool done_scanning_languages = false;

                        std::vector<uint32_t> temp_language_string_sizes;

                        std::vector<uint64_t> temp_language_string_metas;

                        number_of_languages = 0;

                        while (!done_scanning_languages)
                        {
                            json temp_language_json_object;

                            temp_language_json_object["Language"] = languages.at(number_of_languages);

                            std::memcpy(&bytes4, (&dlge_data->get()[0] + position), sizeof(bytes4));
                            position += sizeof(bytes4);

                            temp_language_string_sizes.push_back(bytes4);

                            std::vector<char> temp_string;

                            for (uint64_t l = 0; l < temp_language_string_sizes.back(); l++)
                            {
                                temp_string.push_back(dlge_data->get()[position]);
                                position += 1;
                            }

                            if (temp_language_string_sizes.back() % 8 != 0)
                            {
                                std::cout << main_data->console_prefix << "Error: DLGE file " << hash_file_name << " in " << main_data->rpkg_data.at(i).rpkg_file_name << " is malformed." << std::endl;
                                do_not_break = false;
                                break;
                            }

                            for (uint32_t m = 0; m < temp_language_string_sizes.back() / 8; m++)
                            {
                                uint32_t data[2];
                                std::memcpy(data, &temp_string[(uint64_t)m * (uint64_t)8], sizeof(uint32_t));
                                std::memcpy(data + 1, &temp_string[(uint64_t)m * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                xtea_decrypt_dlge(data);

                                std::memcpy(&temp_string[(uint64_t)m * (uint64_t)8], data, sizeof(uint32_t));
                                std::memcpy(&temp_string[(uint64_t)m * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));

                                dlge_has_text = true;
                            }

                            uint32_t last_zero_position = (uint32_t)temp_string.size();

                            if (temp_string.size() > 0)
                            {
                                uint32_t m = (uint32_t)(temp_string.size() - 1);

                                while (m >= 0)
                                {
                                    if (temp_string.at(m) != 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        last_zero_position--;
                                    }

                                    m--;
                                }
                            }

                            if (temp_string.size() > 0)
                            {
                                std::string temp_string_with_zero_pad_removed = std::string(temp_string.begin(), temp_string.end()).substr(0, last_zero_position);

                                temp_language_json_object["String"] = temp_string_with_zero_pad_removed;
                            }
                            else
                            {
                                temp_language_json_object["String"] = "";
                            }

                            json_object.push_back(temp_language_json_object);

                            if ((position + 0x8) <= decompressed_size)
                            {
                                std::memcpy(&check_variable_1, (&dlge_data->get()[0] + position), sizeof(bytes4));
                                position += sizeof(bytes4);

                                std::memcpy(&check_variable_2, (&dlge_data->get()[0] + position), sizeof(bytes4));
                                position += sizeof(bytes4);

                                position -= sizeof(bytes8);

                                if ((check_variable_1 == 0xFFFFFFFF && check_variable_2 == 0xFFFFFFFF) || ((check_variable_1 + 1) == check_variable_2) || check_variable_2 == 0xFFFFFFFF)
                                {
                                    std::memcpy(&bytes8, (&dlge_data->get()[0] + position), sizeof(bytes8));

                                    for (uint64_t k = 0; k < sizeof(bytes8); k++)
                                    {
                                        json_meta_data.push_back(dlge_data->get()[position + k]);
                                    }

                                    position += sizeof(bytes8);

                                    temp_language_string_metas.push_back(bytes8);
                                }
                                else
                                {
                                    done_scanning_languages = true;
                                }
                            }
                            else
                            {
                                done_scanning_languages = true;
                            }

                            number_of_languages++;
                        }

                        //std::cout << number_of_dlge_categories << "," << number_of_languages << std::endl;

                        if ((position + 0x1) <= decompressed_size)
                        {
                            std::memcpy(&bytes1, (&dlge_data->get()[0] + position), sizeof(bytes1));

                            //for (uint64_t k = 0; k < sizeof(bytes1); k++)
                            //{
                                //json_meta_data.push_back(dlge_data->get()[position + k]);
                            //}

                            if (bytes1 == 1)
                            {
                                text_available = 1;

                                position += sizeof(bytes1);
                            }
                            else
                            {
                                text_available = 0;
                            }
                        }
                        else
                        {
                            text_available = 0;
                        }
                        
                        text_count++;
                    }

                    if ((decompressed_size - position) > 0)
                    {
                        for (uint64_t k = 0; k < (decompressed_size - position); k++)
                        {
                            json_meta_data.push_back(dlge_data->get()[position + k]);
                        }
                    }

                    if (dlge_has_text && do_not_break)
                    {
                        std::string json_path = current_path + "/" + hash_file_name + ".JSON";

                        std::ofstream json_file = std::ofstream(json_path, std::ifstream::binary);

                        if (!json_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: JSON file " << json_path << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        json_file << std::setw(4) << json_object << std::endl;

                        json_file.close();

                        std::string json_meta_path = current_path + "/" + hash_file_name + ".JSON.meta";

                        std::ofstream json_meta_file = std::ofstream(json_meta_path, std::ifstream::binary);

                        if (!json_meta_file.good())
                        {
                            std::cout << main_data->console_prefix << "Error: JSON meta file " << json_meta_path << " could not be created." << std::endl;
                            std::exit(0);
                        }

                        char char4[4];

                        std::memcpy(&char4, &number_of_dlge_categories, sizeof(bytes4));
                        json_meta_file.write(char4, sizeof(bytes4));

                        std::memcpy(&char4, &number_of_languages, sizeof(bytes4));
                        json_meta_file.write(char4, sizeof(bytes4));

                        json_meta_file.write(json_meta_data.data(), json_meta_data.size());

                        json_meta_file.close();

                        std::vector<hash_depends_variables> hash_depends_data;

                        uint64_t hash = main_data->rpkg_data.at(i).hash.at(j);

                        for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
                        {
                            std::map<uint64_t, uint64_t>::iterator it2 = main_data->rpkg_data.at(i).hash_map.find(hash);

                            hash_depends_variables temp_hash_depends_data;

                            temp_hash_depends_data.rpkg_file_name = main_data->rpkg_data.at(i).rpkg_file_name;

                            if (it2 != main_data->rpkg_data.at(i).hash_map.end())
                            {
                                temp_hash_depends_data.hash = hash;

                                temp_hash_depends_data.hash_string = hash;

                                uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference_count & 0x3FFFFFFF;

                                //std::cout << main_data->console_prefix << main_data->input_filter.at(z) << " has " << temp_hash_reference_count << " dependencies in " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                                if (temp_hash_reference_count > 0)
                                {
                                    for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                                    {
                                        std::vector<std::string> dependency_in_rpkg_file;

                                        bool found = false;

                                        for (uint64_t j = 0; j < main_data->rpkg_data.size(); j++)
                                        {
                                            std::map<uint64_t, uint64_t>::iterator it3 = main_data->rpkg_data.at(j).hash_map.find(main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference.at(k));

                                            if (it3 != main_data->rpkg_data.at(j).hash_map.end())
                                            {
                                                if (!found)
                                                {
                                                    temp_hash_depends_data.hash_dependency_file_name.push_back(main_data->rpkg_data.at(j).hash_file_name.at(it3->second));

                                                    std::string hash_link_file_name = current_path + "/" + hash_file_name + "_" + main_data->rpkg_data.at(j).hash_file_name.at(it3->second);

                                                    if (main_data->rpkg_data.at(j).hash_resource_type.at(it3->second) == "WWES")
                                                    {
                                                        if (!path_exists(hash_link_file_name))
                                                        {
                                                            std::ofstream json_meta_hash_link_file = std::ofstream(hash_link_file_name, std::ifstream::binary);

                                                            if (!json_meta_hash_link_file.good())
                                                            {
                                                                std::cout << main_data->console_prefix << "Error: JSON meta file " << hash_link_file_name << " could not be created." << std::endl;
                                                                std::exit(0);
                                                            }

                                                            json_meta_hash_link_file.close();
                                                        }
                                                    }
                                                }

                                                found = true;

                                                dependency_in_rpkg_file.push_back(main_data->rpkg_data.at(j).rpkg_file_name);
                                            }
                                        }

                                        if (!found)
                                        {
                                            temp_hash_depends_data.hash_dependency_file_name.push_back(main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference_string.at(k));
                                        }

                                        temp_hash_depends_data.hash_dependency_map[hash] = temp_hash_depends_data.hash_dependency_map.size();

                                        temp_hash_depends_data.hash_dependency.push_back(uint64_t_to_hex_string(hash));

                                        //tmpHashDependencyData.hashDependencyPatchList.push_back(hashPatchDeletionSearch(main_data->rpkg_data, hash));

                                        temp_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
                                    }
                                }

                                hash_depends_data.push_back(temp_hash_depends_data);
                            }
                        }
                    }
                    else
                    {
                        //std::cout << hash_file_name << "," << position << std::endl;
                    }
                }
            }
        }

        ss.str(std::string());

        ss << "Extracting DLGE as JSON: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to generate the RPKG file does not exist." << std::endl;
        std::exit(0);
    }
}

void rebuild_all_dlge(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    main_data->input_rpkg_folder_path.append("/");

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        rpkg_variables temp_rpkg_data;

        std::vector<std::string> json_file_names;
        std::vector<std::string> json_file_paths;
        std::vector<std::string> json_file_base_paths;
        std::vector<uint64_t> dlge_hashes;
        std::vector<std::string> dlge_hash_strings;
        std::vector<std::string> dlge_file_names;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string json_file_name = "";
                std::string json_file_base_path = "";
                std::string hash_file_name = "";
                std::string hash_string = "";
                std::string resource_type = "";

                if (pos != std::string::npos)
                {
                    json_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                    json_file_base_path = entry.path().string().substr(0, pos);
                }
                else
                {
                    json_file_name = entry.path().string();
                }

                if (to_uppercase(json_file_name.substr((json_file_name.length() - 5), 5)) == ".JSON")
                {
                    hash_file_name = to_uppercase(json_file_name.substr(0, (json_file_name.length() - 5)));
                }

                pos = hash_file_name.find_last_of(".");

                if (pos != std::string::npos)
                {
                    hash_string = hash_file_name.substr(0, pos);
                    resource_type = hash_file_name.substr(pos + 1, hash_file_name.length() - (pos + 1));
                }

                if (main_data->debug)
                {
                    std::cout << main_data->console_prefix << entry.path().string() << "," << hash_file_name << "," << hash_string << "," << resource_type << std::endl;
                }

                bool is_dlge_hash_file = true;

                if (hash_string.length() != 16)
                {
                    is_dlge_hash_file = false;
                }

                if (resource_type != "DLGE")
                {
                    is_dlge_hash_file = false;
                }

                if (is_dlge_hash_file)
                {
                    json_file_paths.push_back(entry.path().string());
                    json_file_base_paths.push_back(json_file_base_path);
                    json_file_names.push_back(json_file_name);
                    dlge_hashes.push_back(std::strtoll(hash_string.c_str(), nullptr, 16));
                    dlge_file_names.push_back(to_uppercase(hash_file_name));
                    dlge_hash_strings.push_back(to_uppercase(hash_string));

                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a valid JSON file." << std::endl;
                    }
                }
                else
                {
                    if (main_data->debug)
                    {
                        std::cout << main_data->console_prefix << entry.path().string() << " is a not valid JSON file." << std::endl;
                    }
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < json_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found JSON file: " << json_file_paths.at(i) << std::endl;
            }
        }

        start_time = std::chrono::high_resolution_clock::now();

        console_update_rate = 1.0 / 2.0;
        period_count = 1;

        for (uint64_t p = 0; p < json_file_paths.size(); p++)
        {
            if (path_exists(json_file_paths.at(p) + ".meta"))
            {
                std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

                double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

                if (time_in_seconds_from_start_time > console_update_rate)
                {
                    start_time = end_time;

                    if (period_count > 3)
                    {
                        period_count = 0;
                    }

                    ss.str(std::string());

                    ss << "Rebuilding JSON as DLGE" << std::string(period_count, '.');

                    std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                    period_count++;
                }

                std::ifstream input_json_meta_file = std::ifstream(json_file_paths.at(p) + ".meta", std::ifstream::binary);

                if (!input_json_meta_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be read." << std::endl;
                    std::exit(0);
                }

                input_json_meta_file.seekg(0, input_json_meta_file.end);

                uint64_t input_json_meta_file_size = input_json_meta_file.tellg();

                input_json_meta_file.seekg(0, input_json_meta_file.beg);

                std::vector<char> input_json_meta(input_json_meta_file_size, 0);

                input_json_meta_file.read(input_json_meta.data(), input_json_meta_file_size);

                input_json_meta_file.close();

                std::ifstream input_json_file(json_file_paths.at(p));

                if (!input_json_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: JSON file " << json_file_paths.at(p) << " could not be read." << std::endl;
                    std::exit(0);
                }

                json input_json;

                input_json_file >> input_json;

                input_json_file.close();

                int category_language_count = 0;

                for (const auto& it : input_json.items())
                {
                    //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

                    bool language_found = false;

                    if (it.value().contains("Language"))
                    {
                        language_found = true;

                        category_language_count++;

                        //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;
                    }

                    if (!language_found)
                    {
                        std::cout << main_data->console_prefix << "Error: JSON file " << json_file_paths.at(p) << " is malformed and can not be rebuilt into a DLGE file/resource." << std::endl;
                        std::exit(0);
                    }
                }

                std::vector<char> dlge_data;

                char char4[4] = "";
                uint32_t bytes4 = 0;

                uint32_t position = 0;

                uint32_t number_of_dlge_categories = 0;

                std::memcpy(&number_of_dlge_categories, &input_json_meta.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                uint32_t number_of_languages = 0;

                std::memcpy(&number_of_languages, &input_json_meta.data()[position], sizeof(uint32_t));
                position += sizeof(uint32_t);

                if (category_language_count != (number_of_dlge_categories * number_of_languages))
                {
                    std::cout << main_data->console_prefix << "Error: Number of strings in the input DLGE JSON file do not match the number of strings in the meta file." << std::endl;
                    std::exit(0);
                }

                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x1);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);
                dlge_data.push_back(0x0);

                for (uint64_t i = 0; i < number_of_dlge_categories; i++)
                {
                    dlge_data.push_back(0x1);

                    std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                    {
                        dlge_data.push_back(char4[j]);
                    }

                    std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                    position += sizeof(uint32_t);

                    for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                    {
                        dlge_data.push_back(char4[j]);
                    }

                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0xFF);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);
                    dlge_data.push_back(0x0);

                    uint32_t key_range_min = (i * number_of_languages);
                    uint32_t key_range_max = key_range_min + number_of_languages;
                    uint32_t key_current = 0;

                    for (const auto& it : input_json.items())
                    {
                        if (key_current >= key_range_min && key_current < key_range_max)
                        {
                            //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

                            std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                            position += sizeof(uint32_t);

                            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                            {
                                dlge_data.push_back(char4[j]);
                            }

                            std::memcpy(&char4, &input_json_meta.data()[position], sizeof(uint32_t));
                            position += sizeof(uint32_t);

                            for (uint64_t j = 0; j < sizeof(uint32_t); j++)
                            {
                                dlge_data.push_back(char4[j]);
                            }

                            std::string temp_string = it.value()["String"];

                            //std::cout << it.value()["String"] << std::endl << std::endl;

                            uint32_t string_length = (uint32_t)temp_string.length();

                            while (string_length % 8 != 0)
                            {
                                string_length++;

                                temp_string.push_back(0x0);
                            }

                            std::memcpy(&char4, &string_length, sizeof(uint32_t));

                            for (uint64_t k = 0; k < sizeof(uint32_t); k++)
                            {
                                dlge_data.push_back(char4[k]);
                            }

                            for (uint32_t k = 0; k < string_length / 8; k++)
                            {
                                uint32_t data[2];
                                std::memcpy(data, &temp_string[(uint64_t)k * (uint64_t)8], sizeof(uint32_t));
                                std::memcpy(data + 1, &temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                                xtea_encrypt_dlge(data);

                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8], data, sizeof(uint32_t));
                                std::memcpy(&temp_string[(uint64_t)k * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
                            }

                            for (uint64_t k = 0; k < string_length; k++)
                            {
                                dlge_data.push_back(temp_string[k]);
                            }
                        }

                        key_current++;
                    }
                }

                //std::cout << json_file_paths.at(p) << std::endl;

                //std::cout << input_json_meta_file_size << ", " << position << std::endl << std::endl;

                for (uint64_t k = 0; k < (input_json_meta_file_size - position); k++)
                {
                    dlge_data.push_back(input_json_meta.data()[position + k]);
                }

                std::string current_path = json_file_base_paths.at(p) + "/DLGE.rebuilt";

                if (!path_exists(current_path))
                {
                    if (!std::filesystem::create_directories(current_path))
                    {
                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << current_path << std::endl;
                        std::exit(0);
                    }
                }

                std::ofstream output_file = std::ofstream(current_path + "/" + dlge_file_names.at(p), std::ifstream::binary);

                if (!output_file.good())
                {
                    std::cout << main_data->console_prefix << "Error: Rebuilt DLGE file " << dlge_file_names.at(p) << " could not be created." << std::endl;
                    std::exit(0);
                }

                output_file.write(dlge_data.data(), dlge_data.size());

                //std::cout << main_data->console_prefix << "Successfully rebuilt DLGE " << current_path + "/" + dlge_file_names.at(p) << " from " << json_file_paths.at(p) << std::endl;
                
                //std::cout << main_data->console_prefix << "Languages found: " << language_count << std::endl;
            }
            else
            {
                std::cout << main_data->console_prefix << "Error: JSON meta file " << json_file_paths.at(p) + ".meta" << " could not be found." << std::endl;
                std::cout << main_data->console_prefix << "       Can not rebuild " << dlge_file_names.at(p) << " from JSON file " << json_file_paths.at(p) << std::endl;
            }
        }

        ss.str(std::string());

        ss << "Rebuilding JSON as DLGE: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to rebuild the DLGE files from does not exist." << std::endl;
        std::exit(0);
    }
}

void decrypt_packagedefinition_thumbs(main_variables* main_data)
{
    std::ifstream file = std::ifstream(main_data->input_packagedefinitions_thumbs_file_path, std::ifstream::binary);

    if (!file.good())
    {
        std::cout << main_data->console_prefix << "Error: packagedefinitions.txt / thumbs.dat file " << main_data->input_packagedefinitions_thumbs_file_path << " could not be read." << std::endl;
        std::exit(0);
    }

    file.seekg(0, file.end);

    uint64_t packagedefinitions_thumbs_file_size = file.tellg();

    int packagedefinitions_thumbs_header_size = 20;

    packagedefinitions_thumbs_file_size -= packagedefinitions_thumbs_header_size;

    file.seekg(0, file.beg);

    std::vector<char> packagedefinitions_thumbs_header(packagedefinitions_thumbs_header_size, 0);

    file.read(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

    std::vector<char> input_data(packagedefinitions_thumbs_file_size, 0);

    file.read(input_data.data(), packagedefinitions_thumbs_file_size);

    file.close();

    while (input_data.size() % 8 != 0)
    {
        input_data.push_back(0x0);
    }

    uint32_t zero_pad_length = (uint32_t)(packagedefinitions_thumbs_file_size - input_data.size());

    for (uint64_t i = 0; i < input_data.size() / 8; i++)
    {
        uint32_t data[2];
        std::memcpy(data, &input_data[(uint64_t)i * (uint64_t)8], sizeof(uint32_t));
        std::memcpy(data + 1, &input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

        xtea_decrypt_pd(data);

        std::memcpy(&input_data[(uint64_t)i * (uint64_t)8], data, sizeof(uint32_t));
        std::memcpy(&input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
    }

    uint64_t last_zero_position = input_data.size();

    if (input_data.size() > 0)
    {
        for (uint64_t i = (input_data.size() - 1); i > 0; i--)
        {
            if (input_data.at(i) != 0)
            {
                break;
            }
            else
            {
                last_zero_position--;
            }
        }
    }

    std::string output_file_base_name = main_data->input_output_path + "/" + main_data->input_packagedefinitions_thumbs_file_name;

    std::ofstream output_file = std::ofstream(output_file_base_name + ".decrypted", std::ifstream::binary);

    if (!output_file.good())
    {
        std::cout << main_data->console_prefix << "Error: Output (packagedefinitions.txt / thumbs.dat).decrypted file " << output_file_base_name + ".decrypted" << " could not be created." << std::endl;
        std::exit(0);
    }

    output_file.write(input_data.data(), last_zero_position);

    std::cout << main_data->console_prefix << "Successfully decrypted " << output_file_base_name << " and saved to " << output_file_base_name + ".decrypted" << std::endl;

    std::ofstream output_file_meta = std::ofstream(output_file_base_name + ".decrypted.meta", std::ifstream::binary);

    if (!output_file_meta.good())
    {
        std::cout << main_data->console_prefix << "Error: Output (packagedefinitions.txt / thumbs.dat).decrypted.meta file " << output_file_base_name + ".decrypted.meta" << " could not be created." << std::endl;
        std::exit(0);
    }

    output_file_meta.write(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

    std::cout << main_data->console_prefix << "Successfully created decrypted meta file " << output_file_base_name + ".decrypted.meta" << std::endl;
    std::cout << main_data->console_prefix << "The meta file is used when re-encrypting back to " << output_file_base_name << std::endl;
}

void encrypt_packagedefinition_thumbs(main_variables* main_data)
{
    if (path_exists(main_data->input_packagedefinitions_thumbs_file_path + ".meta"))
    {
        std::ifstream meta_file = std::ifstream(main_data->input_packagedefinitions_thumbs_file_path + ".meta", std::ifstream::binary);

        if (!meta_file.good())
        {
            std::cout << main_data->console_prefix << "Error: packagedefinitions.txt / thumbs.dat meta file " << main_data->input_packagedefinitions_thumbs_file_path + ".meta" << " could not be read." << std::endl;
            std::exit(0);
        }

        meta_file.seekg(0, meta_file.end);

        uint64_t packagedefinitions_thumbs_meta_file_size = meta_file.tellg();

        meta_file.seekg(0, meta_file.beg);

        if (packagedefinitions_thumbs_meta_file_size != 20)
        {
            std::cout << main_data->console_prefix << "Error: packagedefinitions.txt / thumbs.dat meta file " << main_data->input_packagedefinitions_thumbs_file_path + ".meta" << " is corrupt." << std::endl;
            std::exit(0);
        }

        int packagedefinitions_thumbs_header_size = 20;

        std::vector<char> packagedefinitions_thumbs_header(packagedefinitions_thumbs_header_size, 0);

        meta_file.read(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

        meta_file.close();

        std::ifstream file = std::ifstream(main_data->input_packagedefinitions_thumbs_file_path, std::ifstream::binary);

        if (!file.good())
        {
            std::cout << main_data->console_prefix << "Error: packagedefinitions.txt / thumbs.dat file " << main_data->input_packagedefinitions_thumbs_file_path << " could not be read." << std::endl;
            std::exit(0);
        }

        file.seekg(0, file.end);

        uint64_t packagedefinitions_thumbs_file_size = file.tellg();

        file.seekg(0, file.beg);

        std::vector<char> input_data(packagedefinitions_thumbs_file_size, 0);

        file.read(input_data.data(), packagedefinitions_thumbs_file_size);

        file.close();

        while (input_data.size() % 8 != 0)
        {
            input_data.push_back(0x0);
        }

        uint32_t zero_pad_length = (uint32_t)(packagedefinitions_thumbs_file_size - input_data.size());

        for (uint64_t i = 0; i < input_data.size() / 8; i++)
        {
            uint32_t data[2];
            std::memcpy(data, &input_data[(uint64_t)i * (uint64_t)8], sizeof(uint32_t));
            std::memcpy(data + 1, &input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

            xtea_encrypt_pd(data);

            std::memcpy(&input_data[(uint64_t)i * (uint64_t)8], data, sizeof(uint32_t));
            std::memcpy(&input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
        }

        std::string output_file_base_name = main_data->input_output_path + "/" + main_data->input_packagedefinitions_thumbs_file_name;

        std::ofstream output_file = std::ofstream(output_file_base_name + ".encrypted", std::ifstream::binary);

        if (!output_file.good())
        {
            std::cout << main_data->console_prefix << "Error: Output (packagedefinitions.txt / thumbs.dat).encrypted file " << output_file_base_name + ".encrypted" << " could not be created." << std::endl;
            std::exit(0);
        }

        output_file.write(packagedefinitions_thumbs_header.data(), packagedefinitions_thumbs_header_size);

        output_file.write(input_data.data(), input_data.size());

        std::cout << main_data->console_prefix << "Successfully encrypted " << output_file_base_name << " and saved to " << output_file_base_name + ".encrypted" << std::endl;
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: packagedefinitions.txt / thumbs.dat meta file " << main_data->input_packagedefinitions_thumbs_file_path + ".meta" << " could not be found." << std::endl;
        std::cout << main_data->console_prefix << "       packagedefinitions.txt / thumbs.dat meta file is required to recreate the 20 byte packagedefinitions.txt / thumbs.dat headers." << std::endl;
        std::exit(0);
    }
}

void find_hash_depends(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    std::cout << main_data->console_prefix << main_data->input_rpkg_folder_path << std::endl;

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_files_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_files_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_files_name = entry.path().string();
                }

                if (to_uppercase(rpkg_files_name.substr((rpkg_files_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_files_name);
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            std::vector<hash_depends_variables> hash_depends_data;

            uint64_t hash = std::strtoull(main_data->input_filter.at(z).c_str(), nullptr, 16);

            for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
            {
                std::map<uint64_t, uint64_t>::iterator it2 = main_data->rpkg_data.at(i).hash_map.find(hash);

                hash_depends_variables temp_hash_depends_data;

                temp_hash_depends_data.rpkg_file_name = main_data->rpkg_data.at(i).rpkg_file_name;

                if (it2 != main_data->rpkg_data.at(i).hash_map.end())
                {
                    temp_hash_depends_data.hash = hash;

                    temp_hash_depends_data.hash_string = main_data->input_filter.at(z);

                    uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference_count & 0x3FFFFFFF;

                    std::cout << main_data->console_prefix << main_data->input_filter.at(z) << " has " << temp_hash_reference_count << " dependencies in " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;

                    if (temp_hash_reference_count > 0)
                    {
                        for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                        {
                            std::vector<std::string> dependency_in_rpkg_file;

                            bool found = false;

                            for (uint64_t j = 0; j < main_data->rpkg_data.size(); j++)
                            {
                                std::map<uint64_t, uint64_t>::iterator it3 = main_data->rpkg_data.at(j).hash_map.find(main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference.at(k));

                                if (it3 != main_data->rpkg_data.at(j).hash_map.end())
                                {
                                    if (!found)
                                    {
                                        temp_hash_depends_data.hash_dependency_file_name.push_back(main_data->rpkg_data.at(j).hash_file_name.at(it3->second));
                                    }

                                    found = true;

                                    dependency_in_rpkg_file.push_back(main_data->rpkg_data.at(j).rpkg_file_name);
                                }
                            }

                            if (!found)
                            {
                                temp_hash_depends_data.hash_dependency_file_name.push_back(main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference_string.at(k));
                            }

                            temp_hash_depends_data.hash_dependency_map[hash] = temp_hash_depends_data.hash_dependency_map.size();

                            temp_hash_depends_data.hash_dependency.push_back(uint64_t_to_hex_string(hash));

                            //tmpHashDependencyData.hashDependencyPatchList.push_back(hashPatchDeletionSearch(main_data->rpkg_data, hash));

                            temp_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
                        }
                    }

                    hash_depends_data.push_back(temp_hash_depends_data);
                }
            }

            int rpkg_dependency_count = 0;

            for (uint64_t i = 0; i < hash_depends_data.size(); i++)
            {
                if (hash_depends_data.at(i).hash_dependency.size() > 0)
                {
                    rpkg_dependency_count++;
                }
            }

            std::cout << main_data->console_prefix << main_data->input_filter.at(z) << " has dependencies in " << rpkg_dependency_count << " RPKG files:" << std::endl << std::endl;

            for (uint64_t i = 0; i < hash_depends_data.size(); i++)
            {
                if (hash_depends_data.at(i).hash_dependency.size() > 0)
                {
                    std::cout << main_data->console_prefix << main_data->input_filter.at(z) << " depends on " << hash_depends_data.at(i).hash_dependency_file_name.size() << " other hash files/resources in RPKG file: " << hash_depends_data.at(i).rpkg_file_name << std::endl;

                    if (hash_depends_data.at(i).hash_dependency_file_name.size() > 0)
                    {
                        std::cout << main_data->console_prefix << main_data->input_filter.at(z) << "'s dependencies:" << std::endl;

                        for (uint64_t j = 0; j < hash_depends_data.at(i).hash_dependency_file_name.size(); j++)
                        {
                            std::cout << main_data->console_prefix << "Hash file/resource: " << hash_depends_data.at(i).hash_dependency_file_name.at(j);

                            if (hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).size() > 0)
                            {
                                std::cout << main_data->console_prefix << ", Found in RPKG files: ";

                                for (uint64_t k = 0; k < hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).size(); k++)
                                {
                                    std::cout << main_data->console_prefix << hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).at(k);

                                    if (k < hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).size() - 1)
                                    {
                                        std::cout << main_data->console_prefix << ", ";

                                        for (uint64_t x = 0; x < main_data->rpkg_data.size(); x++)
                                        {
                                            if (main_data->rpkg_data.at(x).rpkg_file_name == hash_depends_data.at(i).hash_dependency_in_rpkg.at(j).at(k))
                                            {
                                                main_data->input_filter.clear();

                                                main_data->input_filter.push_back(hash_depends_data.at(i).hash_dependency_file_name.at(j));
                                                main_data->mode_filter = true;
                                                main_data->input_rpkg_file_name = main_data->rpkg_data.at(x).rpkg_file_name;
                                                main_data->input_rpkg_file_path = main_data->rpkg_data.at(x).rpkg_file_path;

                                                extract_from_rpkg(main_data);
                                            }
                                        }
                                    }
                                }

                                std::cout << std::endl;
                            }
                            else
                            {
                                std::cout << main_data->console_prefix << ", Found in RPKG files: None" << std::endl;
                            }
                        }
                    }

                    std::cout << std::endl;
                }
            }

            ss.str(std::string());

            uint32_t reverse_dependency_count = 0;

            std::vector<std::string> reverse_dependency;
            std::vector<std::vector<std::string>> reverse_dependency_in_rpkg_file;

            for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
            {
                for (uint64_t j = 0; j < main_data->rpkg_data.at(i).hash.size(); j++)
                {
                    uint32_t temp_hash_reference_count = main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference_count & 0x3FFFFFFF;

                    for (uint64_t k = 0; k < temp_hash_reference_count; k++)
                    {
                        if (main_data->rpkg_data.at(i).hash_reference_data.at(j).hash_reference.at(k) == hash)
                        {
                            reverse_dependency_count++;

                            std::string rd = main_data->rpkg_data.at(i).hash_string.at(j) + "." + main_data->rpkg_data.at(i).hash_resource_type.at(j);

                            if (reverse_dependency.size() > 0)
                            {
                                bool found = false;

                                for (uint64_t k = 0; k < reverse_dependency.size(); k++)
                                {
                                    if (reverse_dependency.at(k) == rd)
                                    {
                                        found = true;

                                        reverse_dependency_in_rpkg_file.at(k).push_back(main_data->rpkg_data.at(i).rpkg_file_name);
                                    }
                                }

                                if (!found)
                                {
                                    reverse_dependency.push_back(rd);

                                    std::vector<std::string> rpkgFiles;

                                    rpkgFiles.push_back(main_data->rpkg_data.at(i).rpkg_file_name);

                                    reverse_dependency_in_rpkg_file.push_back(rpkgFiles);
                                }
                            }
                            else
                            {
                                reverse_dependency.push_back(rd);

                                std::vector<std::string> rpkgFiles;

                                rpkgFiles.push_back(main_data->rpkg_data.at(i).rpkg_file_name);

                                reverse_dependency_in_rpkg_file.push_back(rpkgFiles);
                            }
                        }
                    }
                }
            }

            std::cout << main_data->console_prefix << main_data->input_filter.at(z) << " has reverse dependencies in " << reverse_dependency.size() << " RPKG files:" << std::endl << std::endl;

            if (reverse_dependency.size() > 0)
            {
                std::cout << main_data->console_prefix << main_data->input_filter.at(z) << "'s reverse dependencies:" << std::endl;

                for (uint64_t i = 0; i < reverse_dependency.size(); i++)
                {
                    std::cout << main_data->console_prefix << "Hash file/resource: " << reverse_dependency.at(i);

                    if (reverse_dependency_in_rpkg_file.at(i).size() > 0)
                    {
                        std::cout << main_data->console_prefix << ", Found in RPKG files: ";

                        for (uint64_t j = 0; j < reverse_dependency_in_rpkg_file.at(i).size(); j++)
                        {
                            std::cout << main_data->console_prefix << reverse_dependency_in_rpkg_file.at(i).at(j);

                            if (j < reverse_dependency_in_rpkg_file.at(i).size() - 1)
                            {
                                std::cout << main_data->console_prefix << ", ";
                            }
                        }

                        std::cout << std::endl;
                    }
                    else
                    {
                        std::cout << main_data->console_prefix << ", Found in RPKG files: None" << std::endl;
                    }
                }
            }
        }
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to search for RPKG files for hash depends mode does not exist." << std::endl;
        std::exit(0);
    }
}

void hash_probe(main_variables* main_data)
{
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\"");
    main_data->input_rpkg_folder_path = remove_all_string_from_string(main_data->input_rpkg_folder_path, "\'");

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "\\")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    if (main_data->input_rpkg_folder_path.substr(main_data->input_rpkg_folder_path.length() - 1, 1) == "/")
    {
        main_data->input_rpkg_folder_path = main_data->input_rpkg_folder_path.substr(0, main_data->input_rpkg_folder_path.length() - 1);
    }

    std::cout << main_data->console_prefix << main_data->input_rpkg_folder_path << std::endl;

    if (path_exists(main_data->input_rpkg_folder_path))
    {
        std::vector<std::string> rpkg_file_names;
        std::vector<std::string> rpkg_file_paths;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();

        double console_update_rate = 1.0 / 2.0;
        int period_count = 1;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(main_data->input_rpkg_folder_path))
        {
            std::chrono::time_point end_time = std::chrono::high_resolution_clock::now();

            double time_in_seconds_from_start_time = (0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());

            if (time_in_seconds_from_start_time > console_update_rate)
            {
                start_time = end_time;

                if (period_count > 3)
                {
                    period_count = 0;
                }

                std::stringstream ss;

                ss << "Scanning folder" << std::string(period_count, '.');

                std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ');

                period_count++;
            }

            if (std::filesystem::is_regular_file(entry.path().string()))
            {
                std::size_t pos = entry.path().string().find_last_of("\\/");

                std::string rpkg_files_name = "";
                std::string hash = "";
                std::string hash_resource_type = "";

                if (pos != std::string::npos)
                {
                    rpkg_files_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
                }
                else
                {
                    rpkg_files_name = entry.path().string();
                }

                if (to_uppercase(rpkg_files_name.substr((rpkg_files_name.length() - 5), 5)) == ".RPKG")
                {
                    rpkg_file_paths.push_back(entry.path().string());
                    rpkg_file_names.push_back(rpkg_files_name);
                }
            }
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        std::cout << "\r" << main_data->console_prefix << ss.str() << std::string((80 - ss.str().length()), ' ') << std::endl;

        if (main_data->debug)
        {
            for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
            {
                std::cout << main_data->console_prefix << "Found RPKG file: " << rpkg_file_paths.at(i) << std::endl;
            }
        }

        for (uint64_t i = 0; i < rpkg_file_paths.size(); i++)
        {
            import_rpkg_file_if_not_already(main_data, rpkg_file_paths.at(i), rpkg_file_names.at(i), true);
        }

        bool found = false;

        int found_count = 0;

        ss.str(std::string());

        for (uint64_t z = 0; z < main_data->input_filter.size(); z++)
        {
            uint64_t hash = std::strtoull(main_data->input_filter.at(z).c_str(), nullptr, 16);

            if (hash != 0)
            {
                std::cout << std::endl << main_data->console_prefix << "Searching RPKGs for input filter: " << main_data->input_filter.at(z) << std::endl;

                for (uint64_t i = 0; i < main_data->rpkg_data.size(); i++)
                {
                    std::map<uint64_t, uint64_t>::iterator it2 = main_data->rpkg_data.at(i).hash_map.find(hash);

                    if (it2 != main_data->rpkg_data.at(i).hash_map.end())
                    {
                        found = true;

                        found_count++;

                        ss << std::endl << main_data->console_prefix << main_data->input_filter.at(z) << " is in RPKG file: " << main_data->rpkg_data.at(i).rpkg_file_name << std::endl;
                        ss << main_data->console_prefix << "  - Data offset: " << main_data->rpkg_data.at(i).hash_offset.at(it2->second) << std::endl;
                        ss << main_data->console_prefix << "  - Data size: " << (main_data->rpkg_data.at(i).hash_size.at(it2->second) & 0x3FFFFFFF) << std::endl;

                        if (main_data->rpkg_data.at(i).is_lz4ed.at(it2->second))
                        {
                            ss << main_data->console_prefix << "  - LZ4: True" << std::endl;
                        }
                        else
                        {
                            ss << main_data->console_prefix << "  - LZ4: False" << std::endl;
                        }

                        if (main_data->rpkg_data.at(i).is_xored.at(it2->second))
                        {
                            ss << main_data->console_prefix << "  - XOR: True" << std::endl;
                        }
                        else
                        {
                            ss << main_data->console_prefix << "  - XOR: False" << std::endl;
                        }

                        ss << main_data->console_prefix << "  - Resource type: " << main_data->rpkg_data.at(i).hash_resource_type.at(it2->second) << std::endl;
                        ss << main_data->console_prefix << "  - Hash reference table size: " << main_data->rpkg_data.at(i).hash_reference_table_size.at(it2->second) << std::endl;
                        ss << main_data->console_prefix << "  - Forward hash depends: " << (main_data->rpkg_data.at(i).hash_reference_data.at(it2->second).hash_reference_count & 0x3FFFFFFF) << std::endl;
                        ss << main_data->console_prefix << "  - Final size: " << main_data->rpkg_data.at(i).hash_size_final.at(it2->second) << std::endl;
                        ss << main_data->console_prefix << "  - Size in memory: " << main_data->rpkg_data.at(i).hash_size_in_memory.at(it2->second) << std::endl;
                        ss << main_data->console_prefix << "  - Size in video memory: " << main_data->rpkg_data.at(i).hash_size_in_video_memory.at(it2->second) << std::endl << std::endl;

                    }
                }

                if (found)
                {
                    std::cout << main_data->console_prefix << "Input filter \"" << main_data->input_filter.at(z) << "\" was found in " << found_count << " RPKG files." << std::endl;

                    std::cout << main_data->console_prefix << ss.str() << std::endl;
                }
                else
                {
                    std::cout << main_data->console_prefix << "Input filter \"" << main_data->input_filter.at(z) << "\" was not found in any RPKG files." << std::endl;
                }
            }
            else
            {
                std::cout << main_data->console_prefix << "Unable to probe RPKG files for \"" << main_data->input_filter.at(z) << "\"." << std::endl;
                std::cout << main_data->console_prefix << "Input filter \"" << main_data->input_filter.at(z) << "\" is not a valid IOI hash identifier." << std::endl;
                std::cout << main_data->console_prefix << "IOI uses 64 bit hash identifiers for all it's hash files/resources/runtimeids." << std::endl;
            }
        }
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: The folder " << main_data->input_rpkg_folder_path << " to search for RPKG files for hash probe mode does not exist." << std::endl;
        std::exit(0);
    }
}

void compute_ioi_hash(main_variables* main_data)
{
    unsigned char signature[16];
    struct MD5Context md5c;

    std::string lowercase;

    for (int i = 0; i < main_data->input_to_ioi_hash.length(); i++)
    {
        lowercase.push_back(std::tolower(main_data->input_to_ioi_hash[i]));
    }

    //std::cout << main_data->console_prefix << "Input: " << main_data->input_to_ioi_hash << ", " << lowercase << std::endl;

    MD5Init(&md5c);
    MD5Update(&md5c, (unsigned char*)lowercase.c_str(), (unsigned int)lowercase.length());
    MD5Final(signature, &md5c);

    std::stringstream ss;

    for (uint64_t m = 0; m < 16; m++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)(unsigned char)signature[m];
    }

    std::cout << main_data->console_prefix << "Normal MD5 Hash: " << ss.str() << std::endl;

    ss.str(std::string());

    ss << "00";

    for (uint64_t m = 1; m < 8; m++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)(unsigned char)signature[m];
    }

    std::cout << main_data->console_prefix << "IOI Hash: " << ss.str() << std::endl;
}

void parse_input_filter(main_variables* main_data, std::string input_string)
{
    std::string input = std::string(input_string);

    std::smatch m;
    std::regex re("^([^, ]+)[, ]+");
    std::regex_search(input, m, re);

    if (m.size() > 0)
    {
        main_data->input_filter_string = input;

        //std::cout << main_data->console_prefix << m[1].str() << std::endl;

        main_data->input_filter.push_back(m[1].str());

        std::smatch m2;
        re.assign("[, ]+([^, ]+)");

        while (std::regex_search(input, m2, re))
        {
            main_data->input_filter.push_back(m2[1].str());

            input = m2.suffix().str();

            //std::cout << main_data->console_prefix << m2[1].str() << std::endl;
        }
    }
    else
    {
        main_data->input_filter_string = input;

        main_data->input_filter.push_back(input);
    }
}

void parse_input_output_path(main_variables* main_data, std::string input_string)
{
    main_data->input_output_path = input_string;

    if (main_data->input_output_path != "-" && main_data->input_output_path != "")
    {
        main_data->input_output_path = remove_all_string_from_string(main_data->input_output_path, "\"");
        main_data->input_output_path = remove_all_string_from_string(main_data->input_output_path, "\'");
        main_data->input_output_path = replace_slashes(main_data->input_output_path);

        if (main_data->input_output_path.substr(main_data->input_output_path.length() - 1, 1) == "\\")
        {
            main_data->input_output_path = main_data->input_output_path.substr(0, main_data->input_output_path.length() - 1);
        }

        if (main_data->input_output_path.substr(main_data->input_output_path.length() - 1, 1) == "/")
        {
            main_data->input_output_path = main_data->input_output_path.substr(0, main_data->input_output_path.length() - 1);
        }

        //std::cout << main_data->console_prefix << "input: " << main_data->input_output_path << std::endl;

        if (!path_exists(main_data->input_output_path))
        {
            std::vector<std::string> output_file_path_elements;

            //std::cout << main_data->console_prefix << "does not exist: " << main_data->input_output_path << std::endl;

            std::smatch m;
            std::regex re("^([^\\/]+)\\/");
            std::regex_search(main_data->input_output_path, m, re);

            if (m.size() > 0)
            {
                output_file_path_elements.push_back(m[1].str());

                std::smatch m2;
                re.assign("\\/([^\\/]+)");
                std::string output_path = main_data->input_output_path;

                while (std::regex_search(output_path, m2, re))
                {
                    output_file_path_elements.push_back(m2[1].str());

                    output_path = m2.suffix().str();
                }
            }
            else
            {
                output_file_path_elements.push_back(main_data->input_output_path);
            }

            std::string path = "";

            for (uint64_t l = 0; l < output_file_path_elements.size(); l++)
            {
                path.append(output_file_path_elements.at(l));

                if (!path_exists(path))
                {
                    if (!std::filesystem::create_directories(path))
                    {
                        std::cout << main_data->console_prefix << "Error: Couldn't create directory " << path << std::endl;
                        std::exit(0);
                    }
                }

                path.append("/");

                //std::cout << main_data->console_prefix << "directory: " << path << std::endl;
            }
        }

        main_data->input_output_path.append("/");
    }
    else
    {
        std::cout << main_data->console_prefix << "Error: Invalid RPKG file path." << std::endl;
        std::exit(0);
    }
}

void execute_command_json(main_variables* main_data)
{
    std::ifstream command_json_file(main_data->input_command_json_file_path);

    if (!command_json_file.good())
    {
        std::cout << main_data->console_prefix << "Error: JSON file " << main_data->input_command_json_file_path << " could not be read." << std::endl;
        std::exit(0);
    }

    json command_json;

    command_json_file >> command_json;

    command_json_file.close();

    std::cout << main_data->console_prefix << "Parsing input " << main_data->input_command_json_file_path << " command JSON" << std::endl;

    std::cout << main_data->console_prefix << "    Commands found: " << command_json.size() << std::endl;

    for (const auto& it : command_json.items())
    {
        //std::cout << main_data->console_prefix << it.key() << ", " << it.value() << std::endl << std::endl;

        if (it.value().contains("Command"))
        {
            main_variables main_data_backup;

            main_data_backup = *main_data;

            if (to_uppercase(it.value()["Command"]) == "COMPUTE_IOI_HASH")
            {
                main_data->console_prefix = "    ";

                main_data->mode_compute_ioi_hash = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("StringToHash"))
                {
                    main_data->input_to_ioi_hash = std::string(it.value()["StringToHash"]);

                    if (main_data->input_to_ioi_hash == "")
                    {
                        std::cout << "Error: No input string to hash specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: compute_ioi_hash" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  StringToHash: " << main_data->input_to_ioi_hash << std::endl;

                    compute_ioi_hash(main_data);
                }
                else
                {
                    std::cout << "Error: Command compute_ioi_hash requires a \"StringToHash\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "DECRYPT_PACKAGEDEFINITION_THUMBS")
            {
                main_data->console_prefix = "    ";

                main_data->mode_decrypt_packagedefinition_thumbs = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("FileToDecrypt"))
                {
                    parse_packagedefinitions_thumbs_file_input(std::string(it.value()["FileToDecrypt"]), main_data);

                    if (main_data->input_packagedefinitions_thumbs_file_path == "" || main_data->input_packagedefinitions_thumbs_file_name == "")
                    {
                        std::cout << "Error: Invalid packagedefinitions.txt / thumbs.dat file path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: decrypt_packagedefinition_thumbs" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  FileToDecrypt: " << main_data->input_packagedefinitions_thumbs_file_path << std::endl;

                    decrypt_packagedefinition_thumbs(main_data);
                }
                else
                {
                    std::cout << "Error: Command decrypt_packagedefinition_thumbs requires a \"FileToDecrypt\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "ENCRYPT_PACKAGEDEFINITION_THUMBS")
            {
                main_data->console_prefix = "    ";

                main_data->mode_encrypt_packagedefinition_thumbs = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("FileToEncrypt"))
                {
                    parse_packagedefinitions_thumbs_file_input(std::string(it.value()["FileToEncrypt"]), main_data);

                    if (main_data->input_packagedefinitions_thumbs_file_path == "" || main_data->input_packagedefinitions_thumbs_file_name == "")
                    {
                        std::cout << "Error: Invalid packagedefinitions.txt / thumbs.dat file path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: encrypt_packagedefinition_thumbs" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  FileToEncrypt: " << main_data->input_packagedefinitions_thumbs_file_path << std::endl;

                    encrypt_packagedefinition_thumbs(main_data);
                }
                else
                {
                    std::cout << "Error: Command encrypt_packagedefinition_thumbs requires a \"FileToEncrypt\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "EXTRACT_FROM_RPKG")
            {
                main_data->console_prefix = "    ";

                main_data->mode_extract_from_rpkg = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }

                if (it.value().contains("InputFilePath"))
                {
                    parse_rpkg_file_input(std::string(it.value()["InputFilePath"]), main_data);

                    if (main_data->input_rpkg_file_path == "" || main_data->input_rpkg_file_name == "")
                    {
                        std::cout << "Error: Invalid RPKG file path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: extract_from_rpkg" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    std::cout << "  InputFilePath: " << main_data->input_rpkg_file_path << std::endl;

                    extract_from_rpkg(main_data);
                }
                else
                {
                    std::cout << "Error: Command extract_from_rpkg requires a \"InputFilePath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "EXTRACT_ALL_ORES_FROM")
            {
                main_data->console_prefix = "    ";

                main_data->mode_extract_all_ores = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: extract_all_ores_from" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    extract_all_ores(main_data);
                }
                else
                {
                    std::cout << "Error: Command extract_all_ores_from requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "EXTRACT_ALL_WWES_TO_OGG_FROM")
            {
                main_data->console_prefix = "    ";

                main_data->mode_extract_all_wwes_to_ogg = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: extract_all_wwes_to_ogg_from" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    extract_all_wwes(main_data);
                }
                else
                {
                    std::cout << "Error: Command extract_all_wwes_to_ogg_from requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "EXTRACT_ALL_WWEV_TO_OGG_FROM")
            {
                main_data->console_prefix = "    ";

                main_data->mode_extract_all_wwev_to_ogg = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: extract_all_wwev_to_ogg_from" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    extract_all_wwev(main_data);
                }
                else
                {
                    std::cout << "Error: Command extract_all_wwev_to_ogg_from requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "EXTRACT_LOCR_TO_JSON_FROM")
            {
                main_data->console_prefix = "    ";

                main_data->mode_locr_to_json = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: extract_locr_to_json_from" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    extract_all_locr(main_data);
                }
                else
                {
                    std::cout << "Error: Command extract_locr_to_json_from requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "GENERATE_RPKG_FROM")
            {
                main_data->console_prefix = "    ";

                main_data->mode_generate_rpkg_file = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: generate_rpkg_from" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    generate_rpkg_file(main_data);
                }
                else
                {
                    std::cout << "Error: Command generate_rpkg_from requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "HASH_DEPENDS")
            {
                main_data->console_prefix = "    ";

                main_data->mode_hash_depends = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }
                else
                {
                    std::cout << "Error: Command hash_depends requires a \"Filter\" value in the input JSON." << std::endl;
                    std::exit(0);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: hash_depends" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    find_hash_depends(main_data);
                }
                else
                {
                    std::cout << "Error: Command hash_depends requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "HASH_PROBE")
            {
                main_data->console_prefix = "    ";

                main_data->mode_hash_probe = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }
                else
                {
                    std::cout << "Error: Command hash_probe requires a \"Filter\" value in the input JSON." << std::endl;
                    std::exit(0);
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: hash_probe" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    hash_probe(main_data);
                }
                else
                {
                    std::cout << "Error: Command hash_probe requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "REBUILD_LOCR_FROM_JSON_FROM")
            {
                main_data->console_prefix = "    ";

                main_data->mode_json_to_locr = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: rebuild_locr_from_json_from" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    rebuild_all_locr(main_data);
                }
                else
                {
                    std::cout << "Error: Command rebuild_locr_from_json_from requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "REBUILD_ALL_WWEV_IN")
            {
                main_data->console_prefix = "    ";

                main_data->mode_rebuild_all_wwev = true;

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("InputFolderPath"))
                {
                    main_data->input_rpkg_folder_path = std::string(it.value()["InputFolderPath"]);

                    if (main_data->input_rpkg_folder_path[0] == '-' || main_data->input_rpkg_folder_path == "")
                    {
                        std::cout << "Error: Invalid RPKG folder path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: rebuild_all_wwev_in" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    std::cout << "  InputFolderPath: " << main_data->input_rpkg_folder_path << std::endl;

                    rebuild_all_wwev(main_data);
                }
                else
                {
                    std::cout << "Error: Command rebuild_all_wwev_in requires a \"InputFolderPath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "SEARCH_RPKG")
            {
                main_data->console_prefix = "    ";

                if (it.value().contains("OutputPath"))
                {
                    main_data->mode_output_path = true;

                    parse_input_output_path(main_data, std::string(it.value()["OutputPath"]));
                }

                if (it.value().contains("Filter"))
                {
                    main_data->mode_filter = true;

                    parse_input_filter(main_data, std::string(it.value()["Filter"]));
                }

                if (it.value().contains("SearchType"))
                {
                    if (to_uppercase(it.value()["SearchType"]) == "TEXT")
                    {
                        main_data->mode_text_search = true;

                        if (it.value().contains("SearchString"))
                        {
                            main_data->input_text_search = std::string(it.value()["SearchString"]);
                        }
                        else
                        {
                            std::cout << "Error: Command search_rpkg requires a \"SearchString\" value in the input JSON." << std::endl;
                            std::exit(0);
                        }
                    }
                    else if (to_uppercase(it.value()["SearchType"]) == "HEX")
                    {
                        main_data->mode_hex_search = true;

                        if (it.value().contains("SearchString"))
                        {
                            main_data->input_hex_search = std::string(it.value()["SearchString"]);
                        }
                        else
                        {
                            std::cout << "Error: Command search_rpkg requires a \"SearchString\" value in the input JSON." << std::endl;
                            std::exit(0);
                        }
                    }
                    else if (to_uppercase(it.value()["SearchType"]) == "REGEX")
                    {
                        main_data->mode_regex_search = true;

                        if (it.value().contains("SearchString"))
                        {
                            main_data->input_regex_search = std::string(it.value()["SearchString"]);
                        }
                        else
                        {
                            std::cout << "Error: Command search_rpkg requires a \"SearchString\" value in the input JSON." << std::endl;
                            std::exit(0);
                        }
                    }
                    else
                    {
                        std::cout << main_data->console_prefix << "Error: \"SearchType\" value in the input JSON is not one of the only three valid options: text, hex, or regex" << std::endl;
                        std::exit(0);
                    }
                }
                else
                {
                    std::cout << "Error: Command search_rpkg requires a \"SearchType\" value in the input JSON." << std::endl;
                    std::exit(0);
                }

                if (it.value().contains("InputFilePath"))
                {
                    parse_rpkg_file_input(std::string(it.value()["InputFilePath"]), main_data);

                    if (main_data->input_rpkg_file_path == "" || main_data->input_rpkg_file_name == "")
                    {
                        std::cout << "Error: Invalid RPKG file path specified in the input JSON." << std::endl;
                        std::exit(0);
                    }

                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: search_rpkg" << std::endl;

                    if (main_data->mode_output_path)
                    {
                        std::cout << "  OutputPath: " << main_data->input_output_path << std::endl;
                    }
                    else
                    {
                        std::cout << "  OutputPath: None" << std::endl;
                    }

                    if (main_data->input_filter.size() > 0)
                    {
                        std::cout << "  Filter: " << main_data->input_filter_string << std::endl;
                    }
                    else
                    {
                        std::cout << "  Filter: None" << std::endl;
                    }

                    if (to_uppercase(it.value()["SearchType"]) == "TEXT")
                    {
                        std::cout << "  SearchType: Text" << std::endl;
                        std::cout << "  SearchString: " << it.value()["SearchString"] << std::endl;
                    }
                    else if (to_uppercase(it.value()["SearchType"]) == "HEX")
                    {
                        std::cout << "  SearchType: Hex" << std::endl;
                        std::cout << "  SearchString: " << it.value()["SearchString"] << std::endl;
                    }
                    else if (to_uppercase(it.value()["SearchType"]) == "REGEX")
                    {
                        std::cout << "  SearchType: Regex" << std::endl;
                        std::cout << "  SearchString: " << it.value()["SearchString"] << std::endl;
                    }

                    std::cout << "  InputFilePath: " << main_data->input_rpkg_file_path << std::endl;

                    extract_from_rpkg(main_data);
                }
                else
                {
                    std::cout << "Error: Command search_rpkg requires a \"InputFilePath\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }
            else if (to_uppercase(it.value()["Command"]) == "EXECUTE_SYSTEM_COMMAND")
            {
                main_data->console_prefix = "    ";

                if (it.value().contains("CommandLine"))
                {
                    std::cout << "Executing JSON command number " << it.key() << std::endl;

                    std::cout << "  Command: execute_system_command" << std::endl;

                    std::cout << "  CommandLine: " << it.value()["CommandLine"] << std::endl;

                    std::system(std::string(it.value()["CommandLine"]).c_str());
                }
                else
                {
                    std::cout << "Error: Command execute_system_command requires a \"CommandLine\" value in the input JSON." << std::endl;
                    std::exit(0);
                }
            }

            main_data = &main_data_backup;

            std::cout << std::endl;
        }
    }
}

void display_usage_info(main_variables* main_data)
{
    std::cout << main_data->console_prefix << "rpkg v1.3 - Works with RPKGv1 (GKPR) and RPKGv2 (2KPR) files." << std::endl;
    std::cout << main_data->console_prefix << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << main_data->console_prefix << "Note: All the information used to build this program was gleaned" << std::endl;
    std::cout << main_data->console_prefix << "      in a completely 'clean room' environment." << std::endl;
    std::cout << main_data->console_prefix << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << main_data->console_prefix << "Usage:" << std::endl << std::endl;
    std::cout << main_data->console_prefix << "  Drag And Drop Options:" << std::endl;
    std::cout << main_data->console_prefix << "    - RPKG files can be dragged and dropped directly onto the rpkg.exe file." << std::endl;
    std::cout << main_data->console_prefix << "        This causes rpkg.exe to automatically enter RPKG extraction mode and" << std::endl;
    std::cout << main_data->console_prefix << "        will fully extract all the hash files/resources from each RPKG dragged" << std::endl;
    std::cout << main_data->console_prefix << "        and dropped upon rpkg.exe. It mimics (-extract_from_rpkg) in behaviour." << std::endl;
    std::cout << main_data->console_prefix << "        One or more RPKG files can be dragged and dropped onto rpkg.exe at a time." << std::endl;
    std::cout << main_data->console_prefix << "    - rpkg.exe command JSON files can be dragged and dropped onto the rpkg.exe file." << std::endl;
    std::cout << main_data->console_prefix << "        This causes rpkg.exe to automatically enter rpkg.exe command JSON mode and" << std::endl;
    std::cout << main_data->console_prefix << "        will import, parse, and run all the commands listed in the input JSON file." << std::endl;
    std::cout << main_data->console_prefix << "        One or more JSON files can be dragged and dropped onto rpkg.exe at a time." << std::endl << std::endl;
    std::cout << main_data->console_prefix << "  Command Line Options:" << std::endl;
    std::cout << main_data->console_prefix << "    -compute_ioi_hash <string>" << std::endl;
    std::cout << main_data->console_prefix << "        Computes the IOI style truncated md5 hash/runtimeid from a string." << std::endl;
    std::cout << main_data->console_prefix << "    -decrypt_packagedefinition_thumbs <path to file>" << std::endl;
    std::cout << main_data->console_prefix << "        Decrypts packagedefinitions.txt / thumbs.dat files." << std::endl;
    std::cout << main_data->console_prefix << "    -encrypt_packagedefinition_thumbs <path to file>" << std::endl;
    std::cout << main_data->console_prefix << "        Encrypts packagedefinitions.txt / thumbs.dat files." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_from_rpkg <path to rpkg file>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources from an RPKG file." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_all_ores_from <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources located in the ORES files" << std::endl;
    std::cout << main_data->console_prefix << "        by their full IOI path names from all the RPKG files in a given directory." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_all_wwes_to_ogg_from <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources of type WWES to their" << std::endl;
    std::cout << main_data->console_prefix << "        full IOI internal Wwise file paths / names. It also converts" << std::endl;
    std::cout << main_data->console_prefix << "        the WWES (*.wem) files directly to *.ogg files for easy listening." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_all_wwev_to_ogg_from <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources of type WWEV to their" << std::endl;
    std::cout << main_data->console_prefix << "        partial IOI internal Wwise file paths / names. It also converts" << std::endl;
    std::cout << main_data->console_prefix << "        the WWEV (*.wem) files directly to *.ogg files for easy listening." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_dlge_to_json_from <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources of type DLGE from" << std::endl;
    std::cout << main_data->console_prefix << "        all the RPKG files in a given directory and then decrypts" << std::endl;
    std::cout << main_data->console_prefix << "        them in memory and outputs / formats them as JSON files." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_locr_to_json_from <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources of type LOCR from" << std::endl;
    std::cout << main_data->console_prefix << "        all the RPKG files in a given directory and then decrypts" << std::endl;
    std::cout << main_data->console_prefix << "        them in memory and outputs / formats them as JSON files." << std::endl;
    std::cout << main_data->console_prefix << "    -extract_rtlv_to_json_from <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Extracts all hash linked files/resources of type RTLV from" << std::endl;
    std::cout << main_data->console_prefix << "        all the RPKG files in a given directory and then decrypts" << std::endl;
    std::cout << main_data->console_prefix << "        them in memory and outputs / formats them as JSON files." << std::endl;
    std::cout << main_data->console_prefix << "    -filter <hash filter>" << std::endl;
    std::cout << main_data->console_prefix << "        Filters hash linked files/resources by string filter." << std::endl;
    std::cout << main_data->console_prefix << "        String filter can be a full hash value, partial hash value," << std::endl;
    std::cout << main_data->console_prefix << "        the hash resource type, partial hash resource type, WWES or WWEV" << std::endl;
    std::cout << main_data->console_prefix << "        partial of full file/resource name, etc..." << std::endl;
    std::cout << main_data->console_prefix << "        Can accept multiple filters: -filter <filter1>,<filter2>,..." << std::endl;
    std::cout << main_data->console_prefix << "    -generate_rpkg_from <path to folder to generate rpkg from>" << std::endl;
    std::cout << main_data->console_prefix << "        Generates a RPKG file from hash file(s) in a given folder and all subfolders." << std::endl;
    std::cout << main_data->console_prefix << "        The folder name is used for the filename of the generated RPKG file." << std::endl;
    std::cout << main_data->console_prefix << "    -hash_depends <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Lists the forward and reverse depends of a given hash file/resource." << std::endl;
    std::cout << main_data->console_prefix << "        Scans a directory, commonly Hitman's Runtime dir, and imports and" << std::endl;
    std::cout << main_data->console_prefix << "        scans all available RPKG files for dependency information and lists the results." << std::endl;
    std::cout << main_data->console_prefix << "    -hash_probe <path to folder containing RPKG files>" << std::endl;
    std::cout << main_data->console_prefix << "        Probes RPKG files for hash files/resources and displays key data points." << std::endl;
    std::cout << main_data->console_prefix << "        Scans a directory, commonly Hitman's Runtime dir, and imports and" << std::endl;
    std::cout << main_data->console_prefix << "        scans all available RPKG files for the existence of the hash file/resource." << std::endl;
    std::cout << main_data->console_prefix << "    -hex_search <hex string>" << std::endl;
    std::cout << main_data->console_prefix << "        Specifices the hex string to find within hash files/resources." << std::endl;
    std::cout << main_data->console_prefix << "    -output_path <path to output folder>" << std::endl;
    std::cout << main_data->console_prefix << "        Specifies output folder path to use instead of the current directory." << std::endl;
    std::cout << main_data->console_prefix << "    -rebuild_all_wwev_in <path to folders containing wem files>" << std::endl;
    std::cout << main_data->console_prefix << "        Rebuilds sets of individual Wwise .wem files that were previously" << std::endl;
    std::cout << main_data->console_prefix << "        extracted with (-extract_all_wwev_from). The folder specified by the" << std::endl;
    std::cout << main_data->console_prefix << "        argument can contain any number of sub folders, where each subfolder's" << std::endl;
    std::cout << main_data->console_prefix << "        name is linked to an individual WWEV that will be built from the *.wem" << std::endl;
    std::cout << main_data->console_prefix << "        files in that folder." << std::endl;
    std::cout << main_data->console_prefix << "    -rebuild_dlge_from_json_from <path to folder containing JSON (DLGE) files>" << std::endl;
    std::cout << main_data->console_prefix << "        Rebuilds DLGE files/resources from JSON (DLGE) files that were previously" << std::endl;
    std::cout << main_data->console_prefix << "        extracted with (-extract_dlge_to_json_from)." << std::endl;
    std::cout << main_data->console_prefix << "    -rebuild_locr_from_json_from <path to folder containing JSON (LOCR) files>" << std::endl;
    std::cout << main_data->console_prefix << "        Rebuilds LOCR files/resources from JSON (LOCR) files that were previously" << std::endl;
    std::cout << main_data->console_prefix << "        extracted with (-extract_locr_to_json_from)." << std::endl;
    std::cout << main_data->console_prefix << "    -rebuild_rtlv_from_json_from <path to folder containing JSON (RTLV) files>" << std::endl;
    std::cout << main_data->console_prefix << "        Rebuilds RTLV files/resources from JSON (RTLV) files that were previously" << std::endl;
    std::cout << main_data->console_prefix << "        extracted with (-extract_rtlv_to_json_from)." << std::endl;
    std::cout << main_data->console_prefix << "    -regex_search <regex>" << std::endl;
    std::cout << main_data->console_prefix << "        Specifies the regex which is used to find within hash files/resources." << std::endl;
    std::cout << main_data->console_prefix << "    -rpkg_command_json <path to rpkg command json file>" << std::endl;
    std::cout << main_data->console_prefix << "        Imports, parses, and runs all the commands listed in the input JSON file." << std::endl;
    std::cout << main_data->console_prefix << "        See included rpkg_command_reference.JSON for complete JSON command descriptions." << std::endl;
    std::cout << main_data->console_prefix << "        See included rpkg_command_JSON_examples\*.JSON for some command JSON examples." << std::endl;
    std::cout << main_data->console_prefix << "    -search_rpkg <path to rpkg file>" << std::endl;
    std::cout << main_data->console_prefix << "        Specifies RPKG file whose hash files/resources will to be searched through." << std::endl;
    std::cout << main_data->console_prefix << "    -text_search <text string>" << std::endl;
    std::cout << main_data->console_prefix << "        Specifices the text string to find within hash files/resources." << std::endl;
    std::cout << main_data->console_prefix << "  Examples:" << std::endl;
    std::cout << main_data->console_prefix << "    Computes the IOI style truncated md5 hash/runtimeid from a string:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -compute_ioi_hash \"[assembly:/_PRO/Scenes/Missions/CoastalTown/Mission01.entity].pc_entitytemplate\"" << std::endl;
    std::cout << main_data->console_prefix << "    Decrypts packagedefinitions.txt / thumbs.dat files:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -decrypt_packagedefinition_thumbs packagedefinition.txt" << std::endl;
    std::cout << main_data->console_prefix << "    Decrypts packagedefinitions.txt / thumbs.dat files:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -encrypt_packagedefinition_thumbs packagedefinition.txt.decrypted" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources from an RPKG file:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_from_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources from an RPKG file to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_from_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts one hash linked files/resources from an RPKG file by hash filter:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter 00123456789ABCDE -extract_from_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts multiple hash linked files/resources from an RPKG file by hash filter:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter 00123456789ABCDE,00123456789ABCDE -extract_from_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources from an RPKG file by hash resource type filter:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter ORES -extract_from_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources from an RPKG file by hash resource types ORES, REPO, and JSON:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter ORES,REPO,JSON -extract_from_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Generates a RPKG file from hash file(s) in a given folder and all subfolders:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -generate_rpkg_from chunk0patch1" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources located in the ORES files from all the RPKG files in a given directory:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_all_ores_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources located in the ORES files from all the RPKG files in a given directory to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_all_ores_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts and decrypts (to JSON) all hash linked files/resources of type DLGE from all the RPKG files in a given directory:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_dlge_to_json_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts and decrypts (to JSON) all hash linked files/resources of type DLGE from all the RPKG files in a given directory to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_dlge_to_json_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts and decrypts (to JSON) all hash linked files/resources of type LOCR from all the RPKG files in a given directory:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_locr_to_json_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts and decrypts (to JSON) all hash linked files/resources of type LOCR from all the RPKG files in a given directory to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_locr_to_json_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts and decrypts (to JSON) all hash linked files/resources of type RTLV from all the RPKG files in a given directory:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_rtlv_to_json_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts and decrypts (to JSON) all hash linked files/resources of type RTLV from all the RPKG files in a given directory to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_rtlv_to_json_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Rebuilds sets LOCR files from JSON files that were previously created with (-extract_locr_to_json_from):" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -rebuild_locr_from_json_from \"R:\\WWEV\"" << std::endl;
    std::cout << main_data->console_prefix << "    Rebuilds sets LOCR files from JSON files that were previously created with (-extract_locr_to_json_from) to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -rebuild_locr_from_json_from \"R:\\WWEV\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources of type WWES from all the RPKG files in a given directory:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_all_wwes_to_ogg_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources of type WWES from all the RPKG files in a given directory to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_all_wwes_to_ogg_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources of type WWEV from all the RPKG files in a given directory:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -extract_all_wwev_to_ogg_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Extracts all hash linked files/resources of type WWEV from all the RPKG files in a given directory to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -extract_all_wwev_to_ogg_from \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Rebuilds sets of individual Wwise .wem files that were previously extracted with (-extract_all_wwev_from):" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -rebuild_all_wwev_in \"R:\\WWEV\"" << std::endl;
    std::cout << main_data->console_prefix << "    Rebuilds sets of individual Wwise .wem files that were previously extracted with (-extract_all_wwev_from) to an output folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -output_path \"R:\\my\\output\\path\" -rebuild_all_wwev_in \"R:\\WWEV\"" << std::endl;
    std::cout << main_data->console_prefix << "    Lists the forward and reverse depends of a given hash file/resource:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter 00123456789ABCDE -hash_depends \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Lists the forward and reverse depends of two hash files/resources:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter 00123456789ABCDE,00123456789ABCDE -hash_depends \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Probes RPKG files for hash files/resources and displays key data points:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter 00123456789ABCDE -hash_probe \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Probes RPKG files for hash files/resources and displays key data points:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter 00123456789ABCDE,00123456789ABCDE -hash_probe \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\"" << std::endl;
    std::cout << main_data->console_prefix << "    Search a RPKG file's hash files/resources by hex string:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -hex_search 00112233445566 -search_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Search a RPKG file's hash files/resources by regex:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -regex_search \"assembly:[\\w/_]+\" -search_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Search a RPKG file's hash files/resources by text string:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -text_search assembly -search_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Search a RPKG file's hash files/resources by regex and filter by type ORES:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -filter ORES -regex_search \"assembly:[\\w/_.]+\" -search_rpkg \"C:\\Program Files\\Epic Games\\HITMAN3\\Runtime\\chunk0.rpkg\"" << std::endl;
    std::cout << main_data->console_prefix << "    Imports, parses, and runs all the commands listed in the input JSON file:" << std::endl;
    std::cout << main_data->console_prefix << "      The JSON input below batch extracts from all the RPKGs in the Hitman 3 Runtime folder:" << std::endl;
    std::cout << main_data->console_prefix << "        rpkg.exe -rpkg_command_json rpkg_command-batch_extract_chunk_rpkgs.JSON" << std::endl;
}

int main(int argc, char* argv[])
{
    main_variables main_data;

    if (argc > 1)
    {
        bool all_args_are_paths = true;

        for (uint32_t i = 1; i < argc; i++)
        {
            if (!path_exists(std::string(argv[i])))
            {
                all_args_are_paths = false;
            }
        }

        if (all_args_are_paths)
        {
            for (uint32_t i = 1; i < argc; i++)
            {
                if (to_uppercase(std::string(argv[i]).substr((std::string(argv[i]).length() - 5), 5)) == ".JSON")
                {
                    main_data.mode_command_json = true;

                    parse_command_json_file_input(std::string(argv[i]), &main_data);

                    execute_command_json(&main_data);
                }
                else if (to_uppercase(std::string(argv[i]).substr((std::string(argv[i]).length() - 5), 5)) == ".RPKG")
                {
                    main_data.mode_extract_from_rpkg = true;

                    parse_rpkg_file_input(std::string(argv[i]), &main_data);

                    if (main_data.input_rpkg_file_path == "" || main_data.input_rpkg_file_name == "")
                    {
                        std::cout << main_data.console_prefix << "Error: Invalid RPKG file path." << std::endl;
                        std::exit(0);
                    }

                    std::cout << main_data.console_prefix << "Operation Mode: Extract from RPKG file to folder" << std::endl;

                    extract_from_rpkg(&main_data);
                }
            }
        }
        else
        {
            process_command_line(argc, argv, &main_data);

            std::ifstream file = std::ifstream("R:\\rtlvtest", std::ifstream::binary);

            /*if (!file.good())
            {
                std::cout << main_data.console_prefix << "Error: packagedefinitions.txt / thumbs.dat file " << "R:\\rtlvtest" << " could not be read." << std::endl;
                std::exit(0);
            }

            file.seekg(0, file.end);

            uint64_t packagedefinitions_thumbs_file_size = file.tellg();

            file.seekg(0, file.beg);

            std::vector<char> input_data(packagedefinitions_thumbs_file_size, 0);

            file.read(input_data.data(), packagedefinitions_thumbs_file_size);

            file.close();

            while (input_data.size() % 8 != 0)
            {
                input_data.push_back(0x0);
            }

            uint32_t zero_pad_length = packagedefinitions_thumbs_file_size - input_data.size();

            for (uint64_t i = 0; i < input_data.size() / 8; i++)
            {
                uint32_t data[2];
                std::memcpy(data, &input_data[(uint64_t)i * (uint64_t)8], sizeof(uint32_t));
                std::memcpy(data + 1, &input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], sizeof(uint32_t));

                xtea_decrypt_locr(data);

                std::memcpy(&input_data[(uint64_t)i * (uint64_t)8], data, sizeof(uint32_t));
                std::memcpy(&input_data[(uint64_t)i * (uint64_t)8 + (uint64_t)4], data + 1, sizeof(uint32_t));
            }

            std::ofstream output_file = std::ofstream("R:\\rtlvtest.output", std::ifstream::binary);

            if (!output_file.good())
            {
                std::cout << main_data.console_prefix << "Error: Output (packagedefinitions.txt / thumbs.dat)-decrypted file " << "R:\\rtlvtest.output" << " could not be created." << std::endl;
                std::exit(0);
            }

            output_file.write(input_data.data(), input_data.size());

            std::exit(0);*/

            if (main_data.mode_command_json)
            {
                execute_command_json(&main_data);
            }
            else
            {
                if (main_data.mode_generate_rpkg_file)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Generate RPKG file from folder" << std::endl;

                    generate_rpkg_file(&main_data);
                }
                else if (main_data.mode_extract_from_rpkg || main_data.mode_text_search || main_data.mode_hex_search || main_data.mode_regex_search)
                {
                    if (main_data.mode_extract_from_rpkg)
                    {
                        std::cout << main_data.console_prefix << "Operation Mode: Extract from RPKG file to folder" << std::endl;
                    }
                    else if (main_data.mode_text_search)
                    {
                        std::cout << main_data.console_prefix << "Operation Mode: Search RPKG file for text" << std::endl;
                    }
                    else if (main_data.mode_hex_search)
                    {
                        std::cout << main_data.console_prefix << "Operation Mode: Search RPKG file for hex" << std::endl;
                    }
                    else if (main_data.mode_regex_search)
                    {
                        std::cout << main_data.console_prefix << "Operation Mode: Search RPKG file by regex" << std::endl;
                    }

                    extract_from_rpkg(&main_data);
                }
                else if (main_data.mode_extract_all_ores)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Extract all ORES data from RPKGs in folder" << std::endl;

                    extract_all_ores(&main_data);
                }
                else if (main_data.mode_extract_all_wwev_to_ogg)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Extract all WWEV data (to .ogg) from RPKGs in folder" << std::endl;

                    extract_all_wwev(&main_data);
                }
                else if (main_data.mode_rebuild_all_wwev)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Rebuild all WWEV in folder" << std::endl;

                    rebuild_all_wwev(&main_data);
                }
                else if (main_data.mode_extract_all_wwes_to_ogg)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Extract all WWES data (to .ogg) from RPKGs in folder" << std::endl;

                    extract_all_wwes(&main_data);
                }
                else if (main_data.mode_locr_to_json)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Extract all LOCR data (to JSON) from RPKGs in folder" << std::endl;

                    extract_all_locr(&main_data);
                }
                else if (main_data.mode_json_to_locr)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Rebuild all LOCR data from JSONs in folder" << std::endl;

                    rebuild_all_locr(&main_data);
                }
                else if (main_data.mode_rtlv_to_json)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Extract all RTLV data (to JSON) from RPKGs in folder" << std::endl;

                    extract_all_rtlv(&main_data);
                }
                else if (main_data.mode_json_to_rtlv)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Rebuild all RTLV data from JSONs in folder" << std::endl;

                    rebuild_all_rtlv(&main_data);
                }
                else if (main_data.mode_dlge_to_json)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Extract all DLGE data (to JSON) from RPKGs in folder" << std::endl;

                    extract_all_dlge(&main_data);
                }
                else if (main_data.mode_json_to_dlge)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Rebuild all DLGE data from JSONs in folder" << std::endl;

                    rebuild_all_dlge(&main_data);
                }
                else if (main_data.mode_hash_depends)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Find hash (file/resource)'s depends" << std::endl;

                    find_hash_depends(&main_data);
                }
                else if (main_data.mode_hash_probe)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Probe for hash file(s)/resource(s) in RPKG files" << std::endl;

                    hash_probe(&main_data);
                }
                else if (main_data.mode_compute_ioi_hash)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Compute the IOI hash/runtimeid" << std::endl;

                    compute_ioi_hash(&main_data);
                }
                else if (main_data.mode_decrypt_packagedefinition_thumbs)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Decrypt packagedefinitions.txt / thumbs.dat" << std::endl;

                    decrypt_packagedefinition_thumbs(&main_data);
                }
                else if (main_data.mode_encrypt_packagedefinition_thumbs)
                {
                    std::cout << main_data.console_prefix << "Operation Mode: Encrypt packagedefinitions.txt / thumbs.dat" << std::endl;

                    encrypt_packagedefinition_thumbs(&main_data);
                }
                else
                {
                    display_usage_info(&main_data);
                }
            }
        }
    }
    else
    {
        display_usage_info(&main_data);

        std::system("pause");
    }
}
