// #include "dev_function.h"
// #include "rpkg_function.h"
// #include "file.h"
// #include "global.h"
// #include "crypto.h"
// #include "console.h"
// #include "util.h"
// #include "generic_function.h"
// #include <iostream>
// #include <unordered_map>
// #include <chrono>
// #include <sstream>
// #include <fstream>
// #include <regex>
// #include <filesystem>
/*
void dev_function::dev_extract_temp_strings(std::string& input_path, std::string& filter, std::string& output_path)
{
    initialize_property_map();

    initialize_enum_map();

    bool input_path_is_rpkg_file = false;

    if (std::filesystem::is_regular_file(input_path))
    {
        input_path_is_rpkg_file = true;
    }
    else
    {
        input_path = file::parse_input_folder_path(input_path);
    }

    if (file::path_exists(input_path))
    {
        if (!input_path_is_rpkg_file)
        {
            rpkg_function::import_rpkg_files_in_folder(input_path);
        }
        else
        {
            rpkg_function::import_rpkg(input_path);
        }

        std::stringstream ss;

        ss << "Scanning folder: Done";

        timing_string = ss.str();

        LOG("\r" + ss.str() + std::string((80 - ss.str().length()), ' '));

        LOG("Loading Hash List...");

        //generic_function::load_hash_list(true);

        LOG("Loading Hash List: Done");

        for (uint64_t i = 0; i < rpkgs.size(); i++)
        {
            for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
            {
                if (rpkgs.at(i).hash_resource_types.at(r) == "TEMP")
                {
                    for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                    {
                        uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                        temp temp_temp(i, hash_index);

                        std::set<uint32_t> logical_parents_set;

                        for (uint32_t e = 0; e < temp_temp.tblu_logicalParent.size(); e++)
                        {
                            if (temp_temp.tblu_logicalParent.at(e) >= temp_temp.tblu_logicalParent.size())
                            {
                                logical_parents_set.insert(temp_temp.tblu_logicalParent.at(e));
                            }
                        }

                        for (std::set<uint32_t>::iterator it = logical_parents_set.begin(); it != logical_parents_set.end(); it++)
                        {
                            for (uint32_t e = 0; e < temp_temp.tblu_logicalParent.size(); e++)
                            {
                                if (temp_temp.tblu_logicalParent.at(e) == (*it))
                                {
                                    std::cout << temp_temp.temp_file_name << "," << temp_temp.tblu_entityName.at(e) << std::endl;

                                    std::cout << temp_temp.tblu_depends_file_name.at(0) << "," << temp_temp.tblu_entityName.at(e) << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}*/