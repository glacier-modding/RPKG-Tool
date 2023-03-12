#pragma once

#include <string>

class dev_function {
public:
    static void dev_extract_all_entities_to_qn(std::string& input_path, const std::string& output_path);

    static void dev_dlge_names(std::string& input_path, const std::string& output_path);

    static void dev_map_editor(std::string& input_path, std::string& filter, std::string& output_path);

    static void dev_diff_rpkg_supermetas(std::string& input_path, std::string& filter, std::string& output_path);

    static void dev_extract_rpkg_supermetas(std::string& input_path, std::string& output_path);

    static void dev_extract_wwise_ids(std::string& input_path, std::string& output_path);

    static void dev_extract_wwem_strings(std::string& input_path, std::string& output_path);

    static void dev_extract_wwes_strings(std::string& input_path, std::string& output_path);

    static void dev_extract_wwev_strings(std::string& input_path, std::string& output_path);
};
