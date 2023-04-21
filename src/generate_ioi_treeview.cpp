#include "rpkg_function.h"
#include "generic_function.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "ioi_treeview.h"
#include <iostream>
#include <unordered_map>
#include <sstream>

void rpkg_function::generate_ioi_treeview(std::string& input_path) {
    std::string input_rpkg_folder_path = file::parse_input_folder_path(input_path);

    if (file::path_exists(input_rpkg_folder_path)) {
        if (!hash_list_loaded) {
            LOG("Loading Hash List...");
            generic_function::load_hash_list(false);
            LOG("Loading Hash List: Done");
        }

        rpkg_function::import_rpkg_files_in_folder(input_rpkg_folder_path);

        if (!ioi_treeview.IsInitialized())
            ioi_treeview.InitializeTreeView();
        ioi_treeview.DisplayTreeView();

    } else {
        LOG_AND_EXIT("Error: The folder " + input_rpkg_folder_path +
                     " to search for RPKG files for latest hash mode does not exist.");
    }
}