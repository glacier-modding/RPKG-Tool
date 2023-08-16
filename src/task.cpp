#include "task.h"
#include "file.h"
#include "global.h"
#include "util.h"
#include "generic_function.h"
#include "rpkg_function.h"
#include "dev_function.h"
#include "map.h"
#include <string>
#include <vector>

void task::execute(std::string& command, std::string& input_path, std::string& filter, std::string& search,
                   std::string& search_type, std::string& output_path) {
    if (command == "-compute_ioi_hash") {
        LOG("IOI Hash: " + generic_function::compute_ioi_hash(filter));
    }
    if (command == "-compute_ioi_hash_from_file") {
        generic_function::compute_ioi_hash_from_file(input_path);
    } else if (command == "-decrypt_packagedefinition_thumbs") {
        generic_function::decrypt_packagedefinition_thumbs(input_path, output_path);
    } else if (command == "-encrypt_packagedefinition_thumbs") {
        generic_function::encrypt_packagedefinition_thumbs(input_path, output_path);
    } else if (command == "-extract_from_rpkg") {
        rpkg_extraction_vars rpkg_vars;

        rpkg_vars.input_path = input_path;
        rpkg_vars.filter = filter;
        rpkg_vars.output_path = output_path;

        rpkg_function::extract_from_rpkg(rpkg_vars);
    } else if (command == "-extract_from_rpkgs") {
        rpkg_extraction_vars rpkg_vars;

        rpkg_vars.input_path = input_path;
        rpkg_vars.filter = filter;
        rpkg_vars.output_path = output_path;

        rpkg_function::extract_from_rpkgs(rpkg_vars);
    } else if (command == "-extract_gfxf_from") {
        rpkg_function::extract_gfxf_from(input_path, filter, output_path);
    } else if (command == "-extract_ores_from") {
        rpkg_function::extract_ores_from(input_path, filter, output_path);
    } else if (command == "-extract_all_prim_model_from") {
        rpkg_function::extract_all_prim_model_from(input_path, filter, output_path);
    } else if (command == "-extract_prim_model_from") {
        rpkg_function::extract_prim_model_from(input_path, filter, output_path);
    } else if (command == "-extract_all_prim_model_of_temp_from") {
        rpkg_function::extract_all_prim_model_of_temp_from(input_path, filter, output_path);
    } else if (command == "-extract_all_prim_of_temp_from") {
        rpkg_function::extract_all_prim_of_temp_from(input_path, filter, output_path, GLB_SINGLE);
    } else if (command == "-extract_all_prim_to_glb_from") {
        rpkg_function::extract_all_prim_from(input_path, filter, output_path, GLB_SINGLE);
    } else if (command == "-extract_all_prim_to_gltf_from") {
        rpkg_function::extract_all_prim_from(input_path, filter, output_path, GLTF_SINGLE);
    } else if (command == "-extract_all_prim_to_obj_from") {
        rpkg_function::extract_all_prim_from(input_path, filter, output_path, OBJ_SINGLE);
    } else if (command == "-extract_prim_to_glb_from") {
        rpkg_function::extract_prim_from(input_path, filter, output_path, GLB_SINGLE, true);
    } else if (command == "-extract_prim_to_gltf_from") {
        rpkg_function::extract_prim_from(input_path, filter, output_path, GLTF_SINGLE, true);
    } else if (command == "-extract_prim_to_obj_from") {
        rpkg_function::extract_prim_from(input_path, filter, output_path, OBJ_SINGLE, true);
    } else if (command == "-extract_prim_textured_from") {
        rpkg_function::extract_prim_textured_from(input_path, filter, output_path);
    } else if (command == "-extract_all_text_from") {
        rpkg_function::extract_all_text_from(input_path, filter, output_path);
    } else if (command == "-extract_text_from") {
        rpkg_function::extract_text_from(input_path, filter, output_path);
    } else if (command == "-extract_wwem_to_ogg_from") {
        rpkg_function::extract_wwem_to_ogg_from(input_path, filter, output_path);
    } else if (command == "-extract_wwes_to_ogg_from") {
        rpkg_function::extract_wwes_to_ogg_from(input_path, filter, output_path);
    } else if (command == "-extract_wwev_to_ogg_from") {
        rpkg_function::extract_wwev_to_ogg_from(input_path, filter, output_path);
    } else if (command == "-extract_dlge_to_json_from") {
        rpkg_function::extract_dlge_to_json_from(input_path, filter, output_path, false);
    } else if (command == "-extract_locr_to_json_from") {
        rpkg_function::extract_locr_to_json_from(input_path, filter, output_path, false);
    } else if (command == "-extract_rtlv_to_json_from") {
        rpkg_function::extract_rtlv_to_json_from(input_path, filter, output_path, false);
    } else if (command == "-generate_rpkg_from") {
        rpkg_function::generate_rpkg_from(input_path, output_path, true);
    } else if (command == "-generate_rpkg_quickly_from") {
        rpkg_function::generate_rpkg_from(input_path, output_path, false);
    } else if (command == "-hash_depends") {
        rpkg_function::hash_depends(input_path, filter);
    } else if (command == "-extract_all_hash_depends_from") {
        rpkg_function::extract_all_hash_depends_from(input_path, filter, output_path, HashExtractionStrategy::ALL);
    } else if (command == "-extract_non_base_hash_depends_from") {
        rpkg_function::extract_all_hash_depends_from(input_path, filter, output_path,
                                                     HashExtractionStrategy::ALL_NON_BASE);
    } else if (command == "-extract_non_boot_hash_depends_from") {
        rpkg_function::extract_all_hash_depends_from(input_path, filter, output_path,
                                                     HashExtractionStrategy::ALL_NON_BOOT);
    } else if (command == "-extract_all_hash_depends_prim_models_from") {
        rpkg_function::extract_all_hash_depends_from(input_path, filter, output_path,
                                                     HashExtractionStrategy::PRIMS_ONLY);
    } else if (command == "-extract_direct_hash_depends_from") {
        rpkg_function::extract_direct_hash_depends_from(input_path, filter, output_path, false);
    } else if (command == "-extract_direct_hash_depends_prim_models_from") {
        rpkg_function::extract_direct_hash_depends_from(input_path, filter, output_path, true);
    } else if (command == "-hash_probe") {
        rpkg_function::hash_probe(input_path, filter);
    } else if (command == "-import_rpkg") {
        rpkg_function::import_rpkg(input_path, true);
    } else if (command == "-rebuild_prim_model_in") {
        rpkg_function::rebuild_prim_model_in(input_path, output_path);
    } else if (command == "-rebuild_prim_in") {
        rpkg_function::rebuild_prim_in(input_path, true);
    } else if (command == "-rebuild_text_in") {
        rpkg_function::rebuild_text_in(input_path, output_path, true);
    } else if (command == "-rebuild_gfxf_in") {
        rpkg_function::rebuild_gfxf_in(input_path);
    } else if (command == "-rebuild_wwev_in") {
        rpkg_function::rebuild_wwev_in(input_path);
    } else if (command == "-rebuild_dlge_from_json_from") {
        rpkg_function::rebuild_dlge_from_json_from(input_path, output_path);
    } else if (command == "-rebuild_locr_from_json_from") {
        rpkg_function::rebuild_locr_from_json_from(input_path);
    } else if (command == "-rebuild_rtlv_from_json_from") {
        rpkg_function::rebuild_rtlv_from_json_from(input_path);
    } else if (command == "-search_rpkg") {
        rpkg_extraction_vars rpkg_vars;

        rpkg_vars.input_path = input_path;
        rpkg_vars.filter = filter;
        rpkg_vars.search = search;
        rpkg_vars.search_type = search_type;
        rpkg_vars.output_path = output_path;
        rpkg_vars.search_mode = true;

        rpkg_function::extract_from_rpkg(rpkg_vars);
    } else if (command == "-dev_diff_rpkg_supermetas") {
        dev_function::dev_diff_rpkg_supermetas(input_path, filter, output_path);
    } else if (command == "-dev_extract_rpkg_supermetas") {
        dev_function::dev_extract_rpkg_supermetas(input_path, output_path);
    } else if (command == "-dev_extract_wwem_strings") {
        dev_function::dev_extract_wwem_strings(input_path, output_path);
    } else if (command == "-dev_extract_wwes_strings") {
        dev_function::dev_extract_wwes_strings(input_path, output_path);
    } else if (command == "-dev_extract_wwev_strings") {
        dev_function::dev_extract_wwev_strings(input_path, output_path);
    } else if (command == "-dev_extract_wwise_ids") {
        dev_function::dev_extract_wwise_ids(input_path, output_path);
    } else if (command == "-hash_list_percent_found") {
        rpkg_function::hash_list_percent_found();
    } else if (command == "-hash_meta_to_json") {
        rpkg_function::hash_meta_to_json(input_path);
    } else if (command == "-json_to_hash_meta") {
        rpkg_function::json_to_hash_meta(input_path);
    } else if (command == "-latest_hash") {
        rpkg_function::latest_hash(input_path, filter, output_path);
    } else if (command == "-export_map") {
        map::export_map(input_path, filter, search, output_path, false);
    } else if (command == "-export_map_textured") {
        map::export_map(input_path, filter, search, output_path, true);
    } else if (command == "-import_map") {
        map::import_map(input_path, filter, search, output_path);
    } else if (command == "-extract_entity_to_qn") {
        rpkg_function::extract_entity_to_qn(input_path, filter, output_path);
    } else if (command == "-extract_to_rt_json") {
        rpkg_function::extract_to_rt_json(input_path, filter, search, output_path);
    } else if (command == "-extract_mati_to_json") {
        rpkg_function::extract_mati_to_json(input_path, filter, output_path);
    } else if (command == "-json_to_mati") {
        rpkg_function::json_to_mati(input_path, output_path);
    } else if (command == "-mati_to_json") {
        rpkg_function::mati_to_json(input_path, filter, output_path);
    } else if (command == "-search_entities") {
        /*char* search_strings[1];
        search_strings[0] = "transform";
        search_strings[1] = "00";
        int search_types[2];
        search_types[0] = (int)entity::search_type::DEFAULT;
        search_types[1] = (int)entity::search_type::DEFAULT;
        int search_categories[2];
        search_categories[0] = (int)entity::search_category::ALL;
        search_categories[1] = (int)entity::search_category::TEMPHASH;
        rpkg_function::search_entities(input_path, search_strings, search_types, search_categories, 2, 10, true, true);*/
    } else if (command == "-search_localization") {
        rpkg_function::search_localization(input_path, search, output_path, true, true, true, 1000);
    } else if (command == "-get_line_string") {
        rpkg_function::get_line_string(input_path, filter, output_path);
    } else if (command == "-extract_sdef_to_json") {
        rpkg_function::extract_sdef_to_json(input_path, filter, output_path);
    } else if (command == "-json_to_sdef") {
        rpkg_function::json_to_sdef(input_path, output_path);
    } else if (command == "-sdef_to_json") {
        rpkg_function::sdef_to_json(input_path, output_path);
    } else if (command == "-dev_dlge_names") {
        dev_function::dev_dlge_names(input_path, output_path);
    } else if (command == "-search_repo") {
        rpkg_function::search_repo(input_path, search, 1000);
    } else if (command == "-extract_mrtr_to_json") {
        rpkg_function::extract_mrtr_to_json(input_path, filter, output_path);
    } else if (command == "-json_to_mrtr") {
        rpkg_function::json_to_mrtr(input_path, output_path);
    } else if (command == "-mrtr_to_json") {
        rpkg_function::mrtr_to_json(input_path, filter, output_path);
    } else if (command == "-extract_all_materials_to_json") {
        rpkg_function::extract_all_materials_to_json(input_path, output_path);
    } else if (command == "-extract_material_to_json") {
        rpkg_function::extract_material_to_json(input_path, filter, output_path);
    } else if (command == "-json_to_material") {
        rpkg_function::json_to_material(input_path, output_path);
    } else if (command == "-extract_asva_to_json") {
        rpkg_function::extract_asva_to_json(input_path, filter, output_path);
    } else if (command == "-json_to_asva") {
        rpkg_function::json_to_asva(input_path, output_path);
    } else if (command == "-extract_latest_hash") {
        rpkg_function::extract_latest_hash(input_path, filter, output_path);
    } else if (command == "-generate_ioi_treeview") {
        rpkg_function::generate_ioi_treeview(input_path, filter, output_path);
    } else if (command == "-dev_extract_all_entities_to_qn") {
        dev_function::dev_extract_all_entities_to_qn(input_path, output_path);
    }
}

void task::process_and_execute_files_draged_and_dropped(std::vector<std::string>& dragged_and_dropped_files) {
    for (std::string& dragged_and_dropped_file : dragged_and_dropped_files) {
        if (file::has_extension(dragged_and_dropped_file, ".rpkg")) {
            std::string command = "-extract_from_rpkg";
            std::string empty = "";

            task::execute(command, dragged_and_dropped_file, empty, empty, empty, empty);
        }
    }
}

void task::process_and_execute_command_line_args(std::vector<std::vector<std::string>>& commands) {
    std::string command = "";
    std::string input_path = "";
    std::string filter = "";
    std::string search = "";
    std::string search_type = "";
    std::string output_path = "";
    std::vector<std::string> commands_without_paths = {"-compute_ioi_hash"};
    std::vector<std::string> commands_with_paths = {"-compute_ioi_hash_from_file",
                                                    "-decrypt_packagedefinition_thumbs",
                                                    "-dev_diff_rpkg_supermetas",
                                                    "-dev_dlge_names",
                                                    "-dev_extract_all_entities_to_qn",
                                                    "-dev_extract_rpkg_supermetas",
                                                    "-dev_extract_wwem_strings",
                                                    "-dev_extract_wwes_strings",
                                                    "-dev_extract_wwev_strings",
                                                    "-dev_extract_wwise_ids",
                                                    "-hash_list_percent_found",
                                                    "-encrypt_packagedefinition_thumbs",
                                                    "-extract_from_rpkg",
                                                    "-extract_from_rpkgs",
                                                    "-extract_gfxf_from",
                                                    "-extract_ores_from",
                                                    "-extract_prel_refs",
                                                    "-extract_prim_model_from",
                                                    "-extract_all_prim_model_from",
                                                    "-extract_all_prim_model_of_temp_from",
                                                    "-extract_all_prim_of_temp_from",
                                                    "-extract_all_prim_to_glb_from",
                                                    "-extract_prim_to_glb_from",
                                                    "-extract_prim_to_obj_from",
                                                    "-extract_prim_textured_from",
                                                    "-extract_entity_to_qn",
                                                    "-extract_all_text_from",
                                                    "-extract_text_from",
                                                    "-extract_to_rt_json",
                                                    "-extract_wwem_to_ogg_from",
                                                    "-extract_wwes_to_ogg_from",
                                                    "-extract_wwev_to_ogg_from",
                                                    "-extract_dlge_to_json_from",
                                                    "-extract_locr_to_json_from",
                                                    "-extract_rtlv_to_json_from",
                                                    "-extract_mati_to_json",
                                                    "-extract_sdef_to_json",
                                                    "-extract_mrtr_to_json",
                                                    "-extract_all_materials_to_json",
                                                    "-extract_material_to_json",
                                                    "-extract_asva_to_json",
                                                    "-extract_latest_hash",
                                                    "-export_map",
                                                    "-export_map_textured",
                                                    "-import_map",
                                                    "-generate_ioi_treeview",
                                                    "-generate_rpkg_from",
                                                    "-generate_rpkg_quickly_from",
                                                    "-get_line_string",
                                                    "-hash_depends",
                                                    "-latest_hash",
                                                    "-hash_meta_to_json",
                                                    "-json_to_asva",
                                                    "-json_to_hash_meta",
                                                    "-json_to_material",
                                                    "-json_to_mati",
                                                    "-mati_to_json",
                                                    "-json_to_sdef",
                                                    "-sdef_to_json",
                                                    "-json_to_mrtr",
                                                    "-mrtr_to_json",
                                                    "-extract_all_hash_depends_from",
                                                    "-extract_non_base_hash_depends_from",
                                                    "-extract_non_boot_hash_depends_from",
                                                    "-extract_all_hash_depends_prim_models_from",
                                                    "-extract_direct_hash_depends_from",
                                                    "-extract_direct_hash_depends_prim_models_from",
                                                    "-hash_probe",
                                                    "-rebuild_prim_model_in",
                                                    "-rebuild_prim_in",
                                                    "-rebuild_text_in",
                                                    "-rebuild_gfxf_in",
                                                    "-rebuild_wwev_in",
                                                    "-rebuild_dlge_from_json_from",
                                                    "-rebuild_locr_from_json_from",
                                                    "-rebuild_rtlv_from_json_from",
                                                    "-search_entities",
                                                    "-search_localization",
                                                    "-search_repo",
                                                    "-search_rpkg"};

    int command_count = 0;

    for (auto& i : commands) {
        if (std::find(commands_with_paths.begin(), commands_with_paths.end(), i.at(0)) != commands_with_paths.end()) {
            command = i.at(0);
            input_path = i.at(1);
            command_count++;
        } else if (std::find(commands_without_paths.begin(), commands_without_paths.end(), i.at(0)) !=
                   commands_without_paths.end()) {
            command = i.at(0);
            filter = i.at(1);
            command_count++;
        } else if (util::to_lower_case(i.at(0)) == "-search") {
            search_type = "text";
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-text_search") {
            search_type = "text";
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-hex_search") {
            search_type = "hex";
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-regex_search") {
            search_type = "regex";
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-filter") {
            filter = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-map_path") {
            filter = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-map_filters") {
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-qn_format") {
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-version") {
            search = i.at(1);
        } else if (util::to_lower_case(i.at(0)) == "-output_path") {
            output_path = i.at(1);
        } else {
            LOG_AND_EXIT("Error: Invalid command in command line.");
        }
    }

    if (command_count > 1) {
        LOG_AND_EXIT("Error: Too many commands on the command line.");
    }

    task::execute(command, input_path, filter, search, search_type, output_path);
}