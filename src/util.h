#pragma once
#include <string>
#include <vector>

class util
{
public:
    static uint32_t uint32_t_byteswap(uint32_t input);
    static std::string generate_guid();
    static bool is_valid_hash(std::string hash);
    static std::string hash_type(uint64_t hash_value);
    static uint64_t ioi_string_to_hash(const std::string& ioi_string);
    static std::string hash_to_ioi_string(uint64_t hash_value, bool return_hash_if_not_found);
    static std::string hash_to_hash_list_string(uint64_t hash_value);
    static bool floats_equal(float value1, float value2);
    static std::string find_string_between_strings(std::string_view& input_string, const std::string& string_before,
                                                   const std::string& string_after);
    static void split_string(const std::string& temp_string, const std::string& split_string, std::vector<uint64_t>& split_positions);
    static void split_string_view(const std::string_view& temp_string_view, const std::string& split_string, std::vector<uint64_t>& split_positions);
    static std::string string_to_hex_string(std::string input_string);
    static std::vector<std::string> parse_input_filter(std::string input_string);
    static std::string to_upper_case(std::string s);
    static std::string to_lower_case(std::string s);
    static std::string remove_all_string_from_string(std::string input_string, const std::string& to_remove);
    static std::string uint64_t_to_hex_string_for_qn(uint64_t bytes8);
    static std::string uint64_t_to_hex_string(uint64_t bytes8);
    static std::string uint32_t_to_hex_string(uint32_t bytes4);
    static std::string uint16_t_to_hex_string(uint16_t bytes2);
    static std::string uint8_t_to_hex_string(uint8_t bytes1);
    static std::string uint64_t_to_string(uint64_t bytes8);
    static std::string uint32_t_to_string(uint32_t bytes4);
    static std::string uint16_t_to_string(uint16_t bytes2);
    static std::string uint8_t_to_string(uint8_t bytes1);
    static std::string int32_t_to_string(int32_t bytes4);
    static std::string float_to_string(float bytes4);
    static std::string short_to_string(short bytes2);
    static std::string bool_to_string(bool value);
    static std::wstring string_to_wstring(const std::string& input_string);
    static bool lz4_compress_hc(const char* source, std::vector<char>& destination, int source_size, int& compressed_size);
    static bool float_equal(float float_existing, float float_new, float tolerance);
    static void replace_all_string_in_string(std::string& input, const std::string& to_replace, const std::string& replace_with);
};