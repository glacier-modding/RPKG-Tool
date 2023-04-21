#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "text.h"
#include <sstream>
#include <filesystem>

void
rpkg_function::extract_text_from(std::string& input_path, const std::string& filter, const std::string& output_path) {
    task_single_status = TASK_EXECUTING;

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path)) {
        input_path_is_rpkg_file = true;
    } else {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (!file::path_exists(input_path)) {
        LOG_AND_EXIT("Error: The folder " + input_path + " to with the input RPKGs does not exist.");
    }

    if (!input_path_is_rpkg_file) {
        rpkg_function::import_rpkg_files_in_folder(input_path);
    } else {
        rpkg_function::import_rpkg(input_path);
    }

    std::stringstream ss;

    ss << "Scanning folder: Done";

    const std::vector<std::string> filters = util::parse_input_filter(filter);

    for (const auto& filter : filters) {
        uint64_t text_hash_value = std::strtoull(filter.c_str(), nullptr, 16);

        for (uint64_t i = 0; i < rpkgs.size(); i++) {
            uint64_t rpkg_index = i;

            if (rpkgs.at(i).rpkg_file_path != input_path)
                continue;

            auto it = rpkgs.at(rpkg_index).hash_map.find(text_hash_value);

            if (it == rpkgs.at(rpkg_index).hash_map.end())
                continue;

            if (gui_control == ABORT_CURRENT_TASK) {
                return;
            }

            text temp_text(rpkg_index, it->second);

            std::string text_output_dir = file::output_path_append("TEXT\\" + rpkgs.at(rpkg_index).rpkg_file_name,
                                                                   output_path);

            file::create_directories(text_output_dir);

            const std::string tga_path = text_output_dir + "\\" +
                                   util::uint64_t_to_hex_string(rpkgs.at(rpkg_index).hash.at(it->second).hash_value) +
                                   "." + rpkgs.at(rpkg_index).hash.at(it->second).hash_resource_type + ".tga";

            const std::string meta_path = text_output_dir + "\\" + "metas";

            file::create_directories(meta_path);

            if (temp_text.texd_found) {
                temp_text.save_texd_to_tga(tga_path);
            } else {
                temp_text.save_text_to_tga(tga_path);
            }

            temp_text.generate_tga_meta_files(meta_path);

            //temp_text.load_from_tga(tga_path);
        }
    }

    task_single_status = TASK_SUCCESSFUL;
}
