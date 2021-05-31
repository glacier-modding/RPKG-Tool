#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include "crypto.h"
#include "console.h"
#include "util.h"
#include "generic_function.h"
#include <iostream>
#include <map>

int rpkg_function::load_temp_tblu_hash_depends(uint64_t rpkg_index, uint64_t hash_index)
{
    /*hash_depends_variables temporary_temp_hash_depends_data;

    temporary_temp_hash_depends_data.rpkg_file_name = rpkgs.at(rpkg_index).rpkg_file_name;

    temporary_temp_hash_depends_data.hash_value = rpkgs.at(rpkg_index).hash.at(hash_index).hash_value;

    temporary_temp_hash_depends_data.hash_string = rpkgs.at(rpkg_index).hash.at(hash_index).hash_string;

    std::string hash_string = temporary_temp_hash_depends_data.hash_string;

    uint32_t temp_hash_reference_count = rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    LOG(hash_string + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(rpkg_index).rpkg_file_name);

    if (temp_hash_reference_count > 0)
    {
        for (uint64_t y = 0; y < temp_hash_reference_count; y++)
        {
            std::vector<std::string> dependency_in_rpkg_file;

            bool found = false;

            for (uint64_t z = 0; z < rpkgs.size(); z++)
            {
                std::map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(z).hash_map.find(rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference.at(y));

                if (it3 != rpkgs.at(z).hash_map.end())
                {
                    LOG(rpkgs.at(z).hash.at(it3->second).hash_resource_type);

                    if (!found)
                    {
                        temporary_temp_hash_depends_data.hash_dependency_file_name.push_back(rpkgs.at(z).hash.at(it3->second).hash_file_name);
                    }

                    found = true;

                    dependency_in_rpkg_file.push_back(rpkgs.at(z).rpkg_file_name);
                }
            }

            if (!found)
            {
                temporary_temp_hash_depends_data.hash_dependency_file_name.push_back(rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference_string.at(y));
            }

            std::string ioi_string = "";

            std::map<uint64_t, uint64_t>::iterator it2 = hash_list_hash_map.find(std::strtoull(temporary_temp_hash_depends_data.hash_dependency_file_name.back().c_str(), nullptr, 16));

            if (it2 != hash_list_hash_map.end())
            {
                temporary_temp_hash_depends_data.hash_dependency_file_name.back() = util::to_upper_case(hash_list_hash_file_names.at(it2->second));
                ioi_string = hash_list_hash_strings.at(it2->second);
            }

            temporary_temp_hash_depends_data.hash_dependency_ioi_string.push_back(ioi_string);

            temporary_temp_hash_depends_data.hash_dependency_map[temporary_temp_hash_depends_data.hash_value] = temporary_temp_hash_depends_data.hash_dependency_map.size();

            temporary_temp_hash_depends_data.hash_dependency.push_back(util::uint64_t_to_hex_string(temporary_temp_hash_depends_data.hash_value));

            temporary_temp_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
        }
    }

    temp_hash_depends_data.push_back(temporary_temp_hash_depends_data);

    int rpkg_dependency_count = 0;

    for (uint64_t x = 0; x < temp_hash_depends_data.size(); x++)
    {
        if (temp_hash_depends_data.at(x).hash_dependency.size() > 0)
        {
            rpkg_dependency_count++;
        }
    }

    LOG(hash_string + " has dependencies in " + util::uint32_t_to_string((uint32_t)rpkg_dependency_count) + " RPKG files:");

    std::string tblu_file_name = "";
    std::string tblu_in_rpkg = "";

    bool temp_tblu_depends_found = false;

    for (uint64_t x = 0; x < temp_hash_depends_data.size(); x++)
    {
        if (temp_hash_depends_data.at(x).hash_dependency.size() > 0)
        {
            LOG(hash_string + " depends on " + util::uint32_t_to_string((uint32_t)temp_hash_depends_data.at(x).hash_dependency_file_name.size()) + " other hash files/resources in RPKG file: " + temp_hash_depends_data.at(x).rpkg_file_name);

            if (temp_hash_depends_data.at(x).hash_dependency_file_name.size() > 0)
            {
                LOG(hash_string + "'s dependencies:");

                for (uint64_t y = 0; y < temp_hash_depends_data.at(x).hash_dependency_file_name.size(); y++)
                {
                    LOG("Hash file/resource: " + temp_hash_depends_data.at(x).hash_dependency_file_name.at(y));

                    LOG("  - IOI String: " + temp_hash_depends_data.at(x).hash_dependency_ioi_string.at(y));

                    if (temp_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size() > 0)
                    {
                        int temp_tblu_depends_count = 0;

                        if (temp_hash_depends_data.at(x).hash_dependency_file_name.at(y).substr((temp_hash_depends_data.at(x).hash_dependency_file_name.at(y).length() - 4), 4) == "TBLU")
                        {
                            if (temp_tblu_depends_count == 0)
                            {
                                temp_tblu_depends_found = true;

                                tblu_file_name = temp_hash_depends_data.at(x).hash_dependency_file_name.at(y);
                                tblu_in_rpkg = temp_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).at(temp_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size() - 1);

                                temp_tblu_depends_count++;
                            }
                            else
                            {
                                return TEMP_TBLU_TOO_MANY;
                            }
                        }

                        LOG_NO_ENDL("  - Found in RPKG files: ");

                        for (uint64_t z = 0; z < temp_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size(); z++)
                        {
                            LOG_NO_ENDL(temp_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).at(z));

                            if (z < temp_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size() - 1)
                            {
                                LOG_NO_ENDL(", ");
                            }
                        }

                        LOG_NO_ENDL(std::endl);
                    }
                    else
                    {
                        LOG("  - Found in RPKG files: None");
                    }
                }
            }

            LOG_NO_ENDL(std::endl);
        }
    }

    if (!temp_tblu_depends_found)
    {
        return TEMP_TBLU_NOT_FOUND_IN_DEPENDS;
    }

    bool tblu_found_in_rpkg = false;

    uint64_t tblu_hash_value = std::strtoull(tblu_file_name.c_str(), nullptr, 16);

    std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index).hash_map.find(tblu_hash_value);

    if (it2 != rpkgs.at(rpkg_index).hash_map.end())
    {
        tblu_found_in_rpkg = true;

        tblu_rpkg_index_1 = rpkg_index;
        tblu_rpkg_index_2 = it2->second;
    }

    if (!tblu_found_in_rpkg)
    {
        for (uint64_t x = 0; x < rpkgs.size(); x++)
        {
            if (!tblu_found_in_rpkg)
            {
                rpkg_index = (uint64_t)rpkgs.size() - (uint64_t)0x1 - (uint64_t)x;

                std::map<uint64_t, uint64_t>::iterator it2 = rpkgs.at(rpkg_index).hash_map.find(tblu_hash_value);

                if (it2 != rpkgs.at(rpkg_index).hash_map.end())
                {
                    tblu_found_in_rpkg = true;

                    hash_index = it2->second;

                    tblu_rpkg_index_1 = rpkg_index;
                    tblu_rpkg_index_2 = it2->second;
                }
            }
        }
    }

    if (!tblu_found_in_rpkg)
    {
        return TEMP_TBLU_NOT_FOUND_IN_RPKG;
    }

    tblu_found_in_rpkg = true;

    hash_depends_variables temporary_tblu_hash_depends_data;

    temporary_tblu_hash_depends_data.rpkg_file_name = rpkgs.at(rpkg_index).rpkg_file_name;

    temporary_tblu_hash_depends_data.hash_value = rpkgs.at(rpkg_index).hash.at(hash_index).hash_value;

    temporary_tblu_hash_depends_data.hash_string = rpkgs.at(rpkg_index).hash.at(hash_index).hash_string;

    temp_hash_reference_count = rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

    LOG(hash_string + " has " + util::uint32_t_to_string(temp_hash_reference_count) + " dependencies in " + rpkgs.at(rpkg_index).rpkg_file_name);

    if (temp_hash_reference_count > 0)
    {
        for (uint64_t y = 0; y < temp_hash_reference_count; y++)
        {
            std::vector<std::string> dependency_in_rpkg_file;

            bool found = false;

            for (uint64_t z = 0; z < rpkgs.size(); z++)
            {
                std::map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(z).hash_map.find(rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference.at(y));

                if (it3 != rpkgs.at(z).hash_map.end())
                {
                    LOG(rpkgs.at(z).hash.at(it3->second).hash_resource_type);

                    if (!found)// && rpkgs.at(z).hash.at(it3->second).hash_resource_type == "TBLU")
                    {
                        temporary_tblu_hash_depends_data.hash_dependency_file_name.push_back(rpkgs.at(z).hash.at(it3->second).hash_file_name);
                    }

                    found = true;

                    dependency_in_rpkg_file.push_back(rpkgs.at(z).rpkg_file_name);
                }
            }

            if (!found)
            {
                temporary_tblu_hash_depends_data.hash_dependency_file_name.push_back(rpkgs.at(rpkg_index).hash.at(hash_index).hash_reference_data.hash_reference_string.at(y));
            }

            std::string ioi_string = "";

            std::map<uint64_t, uint64_t>::iterator it4 = hash_list_hash_map.find(std::strtoull(temporary_tblu_hash_depends_data.hash_dependency_file_name.back().c_str(), nullptr, 16));

            if (it4 != hash_list_hash_map.end())
            {
                temporary_tblu_hash_depends_data.hash_dependency_file_name.back() = util::to_upper_case(hash_list_hash_file_names.at(it4->second));
                ioi_string = hash_list_hash_strings.at(it4->second);
            }

            temporary_tblu_hash_depends_data.hash_dependency_ioi_string.push_back(ioi_string);

            temporary_tblu_hash_depends_data.hash_dependency_map[temporary_tblu_hash_depends_data.hash_value] = temporary_tblu_hash_depends_data.hash_dependency_map.size();

            temporary_tblu_hash_depends_data.hash_dependency.push_back(util::uint64_t_to_hex_string(temporary_tblu_hash_depends_data.hash_value));

            temporary_tblu_hash_depends_data.hash_dependency_in_rpkg.push_back(dependency_in_rpkg_file);
        }
    }

    tblu_hash_depends_data.push_back(temporary_tblu_hash_depends_data);

    rpkg_dependency_count = 0;

    for (uint64_t x = 0; x < tblu_hash_depends_data.size(); x++)
    {
        if (tblu_hash_depends_data.at(x).hash_dependency.size() > 0)
        {
            rpkg_dependency_count++;
        }
    }

    LOG(tblu_file_name + " has dependencies in " + util::uint32_t_to_string((uint32_t)rpkg_dependency_count) + " RPKG files:");

    for (uint64_t x = 0; x < tblu_hash_depends_data.size(); x++)
    {
        if (tblu_hash_depends_data.at(x).hash_dependency.size() > 0)
        {
            LOG(tblu_file_name + " depends on " + util::uint32_t_to_string((uint32_t)tblu_hash_depends_data.at(x).hash_dependency_file_name.size()) + " other hash files/resources in RPKG file: " + tblu_hash_depends_data.at(x).rpkg_file_name);

            if (tblu_hash_depends_data.at(x).hash_dependency_file_name.size() > 0)
            {
                LOG(tblu_file_name + "'s dependencies:");

                for (uint64_t y = 0; y < tblu_hash_depends_data.at(x).hash_dependency_file_name.size(); y++)
                {
                    LOG("Hash file/resource: " + tblu_hash_depends_data.at(x).hash_dependency_file_name.at(y));

                    LOG("  - IOI String: " + tblu_hash_depends_data.at(x).hash_dependency_ioi_string.at(y));

                    if (tblu_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size() > 0)
                    {
                        LOG_NO_ENDL("  - Found in RPKG files: ");

                        for (uint64_t z = 0; z < tblu_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size(); z++)
                        {
                            LOG_NO_ENDL(tblu_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).at(z));

                            if (z < tblu_hash_depends_data.at(x).hash_dependency_in_rpkg.at(y).size() - 1)
                            {
                                LOG_NO_ENDL(", ");
                            }
                        }

                        LOG_NO_ENDL(std::endl);
                    }
                    else
                    {
                        LOG("  - Found in RPKG files: None");
                    }
                }
            }

            LOG_NO_ENDL(std::endl);
        }
    }

    return TEMP_TBLU_FOUND;*/

    return 0;
}