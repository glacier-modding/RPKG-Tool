#include "rpkg_function.h"
#include "file.h"
#include <filesystem>

void rpkg_function::import_rpkg_files_in_folder(const std::string& input_rpkg_folder_path) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_rpkg_folder_path)) {
        if (!std::filesystem::is_regular_file(entry.path().string()))
            continue;

        const std::size_t pos = entry.path().string().find_last_of("\\/");

        std::string rpkg_file_name;
        std::string hash;
        std::string hash_resource_type;

        if (pos != std::string::npos) {
            rpkg_file_name = entry.path().string().substr(pos + 1, entry.path().string().length() - (pos + 1));
        } else {
            rpkg_file_name = entry.path().string();
        }

        if (!file::has_extension(rpkg_file_name, ".rpkg"))
            continue;

        std::string rpkg_to_import = entry.path().string();

        rpkg_function::import_rpkg(rpkg_to_import, true);
    }
}