#include "repo.h"
#include "global.h"
#include "crypto.h"
#include "util.h"
#include "thirdparty/lz4/lz4.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

repo::repo() = default;

repo::repo(uint64_t rpkgs_index, uint64_t hash_index)
{
    repo_rpkg_index = rpkgs_index;
    repo_hash_index = hash_index;

    uint64_t repo_hash_size;

    if (rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.lz4ed)
    {
        repo_hash_size = rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.header.data_size;

        if (rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.xored)
        {
            repo_hash_size &= 0x3FFFFFFF;
        }
    }
    else
    {
        repo_hash_size = rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.resource.size_final;
    }

    std::vector<char> repo_data;

    std::vector<char> repo_input_data = std::vector<char>(repo_hash_size, 0);

    std::ifstream file = std::ifstream(rpkgs.at(repo_rpkg_index).rpkg_file_path, std::ifstream::binary);

    if (!file.good())
    {
        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(repo_rpkg_index).rpkg_file_path + " could not be read.");
    }

    file.seekg(rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.header.data_offset, file.beg);
    file.read(repo_input_data.data(), repo_hash_size);
    file.close();

    if (rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.xored)
    {
        crypto::xor_data(repo_input_data.data(), (uint32_t)repo_hash_size);
    }

    uint32_t repo_decompressed_size = rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.resource.size_final;

    std::vector<char> repo_output_data = std::vector<char>(repo_decompressed_size, 0);

    if (rpkgs.at(repo_rpkg_index).hash.at(repo_hash_index).data.lz4ed)
    {
        LZ4_decompress_safe(repo_input_data.data(), repo_output_data.data(), (int)repo_hash_size, repo_decompressed_size);

        repo_data = repo_output_data;
    }
    else
    {
        repo_data = repo_input_data;
    }

    std::vector<char>().swap(repo_output_data);
    std::vector<char>().swap(repo_input_data);

    repo_response_data = "";

    try
    {
        nlohmann::json repo_json = nlohmann::json::parse(repo_data);

        repo_original = nlohmann::json::object();
        repo_modified = nlohmann::json::object();

        for (auto& repo_entry : repo_json)
        {
            repo_original[std::string(repo_entry["ID_"])] = repo_entry;
            repo_modified[std::string(repo_entry["ID_"])] = repo_entry;
        }
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;

        ss << "Error: Loading REPO" << "\n"
            << "Error message: " << e.what() << '\n'
            << "Error exception id: " << e.id << '\n'
            << "Error byte position of error: " << e.byte;

        repo_response_data = ss.str();
    }

    //repo_original = nlohmann::json::parse(repo_data);
    //repo_modified = nlohmann::json::parse(repo_data);

    std::vector<char>().swap(repo_data);
}

repo::repo(std::string repo_path)
{
    std::ifstream repo_file(repo_path, std::ifstream::binary);

    repo_file.seekg(0, repo_file.end);
    uint64_t repo_file_size = repo_file.tellg();
    repo_file.seekg(0, repo_file.beg);

    std::vector<char> repo_data = std::vector<char>(repo_file_size, 0);

    repo_file.read(repo_data.data(), repo_file_size);

    repo_file.close();

    repo_response_data = "";

    try
    {
        nlohmann::json repo_json = nlohmann::json::parse(repo_data);

        repo_original = nlohmann::json::object();
        repo_modified = nlohmann::json::object();

        for (auto& repo_entry : repo_json)
        {
            repo_original[std::string(repo_entry["ID_"])] = repo_entry;
            repo_modified[std::string(repo_entry["ID_"])] = repo_entry;
        }
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;

        ss << "Error: Loading REPO JSON" << "\n"
            << "Error message: " << e.what() << '\n'
            << "Error exception id: " << e.id << '\n'
            << "Error byte position of error: " << e.byte;

        repo_response_data = ss.str();
    }

    //repo_original = nlohmann::json::parse(repo_data);
    //repo_modified = nlohmann::json::parse(repo_data);

    std::vector<char>().swap(repo_data);
}

std::string repo::pretty_json(std::string input_json)
{
    yyjson_doc* doc = yyjson_read(input_json.c_str(), input_json.length(), 0);

    char* json = yyjson_write(doc, YYJSON_WRITE_PRETTY, NULL);

    std::string output_json = std::string(json);

    free(json);

    return output_json;
}

int repo::valid_json(std::string input_json)
{
    yyjson_doc* doc = yyjson_read(input_json.c_str(), input_json.length(), 0);
    
    if (doc)
    {
        yyjson_doc_free(doc);
        return 1;
    }

    yyjson_doc_free(doc);
    return 0;
}

void repo::check_json(std::string input_json)
{
    yyjson_read_err error;

    yyjson_read_flag flag = YYJSON_READ_ALLOW_INVALID_UNICODE;

    yyjson_doc* doc = yyjson_read_opts((char*)input_json.c_str(), input_json.length(), flag, NULL, &error);

    if (doc)
    {
        yyjson_doc_free(doc);
        check_json_response = "";
    }
    else
    {
        yyjson_doc_free(doc);

        int32_t line = 1;

        bool first_char_found = false;

        for (int32_t i = error.pos; i >= 0; i--)
        {
            if (input_json[i] != ' ')
            {
                if (!first_char_found)
                {
                    if (input_json[i] != ',')
                    {
                        line--;
                    }
                }

                if (input_json[i] == '\n')
                {
                    line++;
                }

                first_char_found = true;
            }            
        }

        check_json_response = error.msg;
        check_json_response += " on line ";
        check_json_response += util::uint32_t_to_string(line);
    }
}

uint32_t repo::search(std::string search_string, uint32_t results_count, uint32_t max_results)
{
    return results_count;
}

void repo::load_repo()
{
    /*std::ofstream output("r:\\ROFL\\output.txt", std::ofstream::binary);

    std::string out = "";

    uint32_t pointer = 0;

    out += "original\n";

    for (auto& repo_entry : repo_original)
    {
        repo_entries_original[repo_entry["ID_"]] = pointer;

        out += std::string(repo_entry["ID_"]) + ", " + std::to_string(pointer) + '\n';

        pointer++;
    }

    out += "modified\n";

    pointer = 0;

    for (auto& repo_entry : repo_modified)
    {
        repo_entries_modified[repo_entry["ID_"]] = pointer;

        out += std::string(repo_entry["ID_"]) + ", " + std::to_string(pointer) + '\n';

        pointer++;
    }

    output.write(out.data(), out.length());

    output.close();*/
}

void repo::get_child_entries(std::string id)
{
    
}

void repo::get_category(int category)
{
    repo_response_data = "";

    if (category == NPCS)
        get_npcs();
    else if (category == OUTFITS)
        get_outfits();
    else if (category == RUNTIME_QUESTITEMS)
        get_runtimes(RUNTIME_QUESTITEMS);
    else if (category == RUNTIME_MELEES)
        get_runtimes(RUNTIME_MELEES);
    else if (category == RUNTIME_KEYS)
        get_runtimes(RUNTIME_KEYS);
    else if (category == RUNTIME_EXPLOSIVES)
        get_runtimes(RUNTIME_EXPLOSIVES);
    else if (category == RUNTIME_TOOLS)
        get_runtimes(RUNTIME_TOOLS);
    else if (category == RUNTIME_PISTOLS)
        get_runtimes(RUNTIME_PISTOLS);
    else if (category == RUNTIME_INVALID_CATEGORY_ICONS)
        get_runtimes(RUNTIME_INVALID_CATEGORY_ICONS);
    else if (category == RUNTIME_SNIPERRIFLES)
        get_runtimes(RUNTIME_SNIPERRIFLES);
    else if (category == RUNTIME_ASSAULTRIFLES)
        get_runtimes(RUNTIME_ASSAULTRIFLES);
    else if (category == RUNTIME_REMOTES)
        get_runtimes(RUNTIME_REMOTES);
    else if (category == RUNTIME_SHOTGUNS)
        get_runtimes(RUNTIME_SHOTGUNS);
    else if (category == RUNTIME_SUITCASES)
        get_runtimes(RUNTIME_SUITCASES);
    else if (category == RUNTIME_SMGS)
        get_runtimes(RUNTIME_SMGS);
    else if (category == RUNTIME_DISTRACTIONS)
        get_runtimes(RUNTIME_DISTRACTIONS);
    else if (category == RUNTIME_POISONS)
        get_runtimes(RUNTIME_POISONS);
    else if (category == RUNTIME_CONTAINERS)
        get_runtimes(RUNTIME_CONTAINERS);
    else if (category == AREADISCOVEREDS)
        get_areadiscovereds();
    else if (category == LOCATIONS)
        get_locations();
    else if (category == MODIFIERS)
        get_modifiers();
    else if (category == DIFFICULTY_PARAMETERS)
        get_difficulty_parameters();
    else if (category == BEHAVIOURS)
        get_behaviours();
    else if (category == MAGAZINE_CONFIGS)
        get_magazine_configs();
    else if (category == AMMO_CONFIGS)
        get_ammo_configs();
}

void repo::get_npcs()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("Outfit"))
        {
            if (repo_entry.contains("Name"))
            {
                repo_response_data += repo_entry["Name"];
                repo_response_data += "|||| ||||";
                repo_response_data += repo_entry["ID_"];
                repo_response_data += "||||||";
            }
        }
    }
}

void repo::get_outfits()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("IsHitmanSuit"))
        {
            if (repo_entry.contains("CommonName"))
            {
                repo_response_data += repo_entry["CommonName"];
                repo_response_data += "|||| ||||";
                repo_response_data += repo_entry["ID_"];
                repo_response_data += "||||||";
            }
        }
    }
}

void repo::get_runtimes(int category)
{
    repo_response_data = "";

    std::unordered_map<std::string, yyjson_mut_val*> repo_runtimes;

    std::string inventoryCategoryIcon = "";

    if (category == RUNTIME_QUESTITEMS)
        inventoryCategoryIcon = "questitem";
    else if (category == RUNTIME_MELEES)
        inventoryCategoryIcon = "melee";
    else if (category == RUNTIME_KEYS)
        inventoryCategoryIcon = "key";
    else if (category == RUNTIME_EXPLOSIVES)
        inventoryCategoryIcon = "explosives";
    else if (category == RUNTIME_TOOLS)
        inventoryCategoryIcon = "tool";
    else if (category == RUNTIME_PISTOLS)
        inventoryCategoryIcon = "pistol";
    else if (category == RUNTIME_INVALID_CATEGORY_ICONS)
        inventoryCategoryIcon = "invalid_category_icon";
    else if (category == RUNTIME_SNIPERRIFLES)
        inventoryCategoryIcon = "sniperrifle";
    else if (category == RUNTIME_ASSAULTRIFLES)
        inventoryCategoryIcon = "assaultrifle";
    else if (category == RUNTIME_REMOTES)
        inventoryCategoryIcon = "remote";
    else if (category == RUNTIME_SHOTGUNS)
        inventoryCategoryIcon = "shotgun";
    else if (category == RUNTIME_SUITCASES)
        inventoryCategoryIcon = "suitcase";
    else if (category == RUNTIME_SMGS)
        inventoryCategoryIcon = "smg";
    else if (category == RUNTIME_DISTRACTIONS)
        inventoryCategoryIcon = "distraction";
    else if (category == RUNTIME_POISONS)
        inventoryCategoryIcon = "poison";
    else if (category == RUNTIME_CONTAINERS)
        inventoryCategoryIcon = "container";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("Runtime"))
        {
            if (repo_entry.contains("CommonName") && repo_entry.contains("InventoryCategoryIcon"))
            {
                std::string categoryIconName = util::to_lower_case(repo_entry["InventoryCategoryIcon"]);

                if (categoryIconName == inventoryCategoryIcon)
                {
                    repo_response_data += repo_entry["CommonName"];
                    repo_response_data += "||||";
                    repo_response_data += util::uint64_t_to_hex_string(std::strtoull(std::string(repo_entry["Runtime"]).c_str(), nullptr, 10));
                    repo_response_data += "||||";
                    repo_response_data += repo_entry["ID_"];
                    repo_response_data += "||||||";
                }
            }
        }
    }
}

void repo::get_areadiscovereds()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("PersistentBoolId"))
        {
            if (repo_entry.contains("Name"))
            {
                repo_response_data += repo_entry["Name"];
                repo_response_data += "|||| ||||";
                repo_response_data += repo_entry["ID_"];
                repo_response_data += "||||||";
            }
        }
    }
}

void repo::get_locations()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("Name_LOC"))
        {
            if (repo_entry.contains("Name"))
            {
                repo_response_data += repo_entry["Name"];
                repo_response_data += "|||| ||||";
                repo_response_data += repo_entry["ID_"];
                repo_response_data += "||||||";
            }
        }
    }
}

void repo::get_modifiers()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("ModifierType"))
        {
            repo_response_data += repo_entry["ModifierType"];
            repo_response_data += "|||| ||||";
            repo_response_data += repo_entry["ID_"];
            repo_response_data += "||||||";
        }
    }
}

void repo::get_difficulty_parameters()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("Parameter"))
        {
            repo_response_data += repo_entry["Parameter"];
            repo_response_data += "|||| ||||";
            repo_response_data += repo_entry["ID_"];
            repo_response_data += "||||||";
        }
    }
}

void repo::get_ammo_configs()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("AmmoImpactEffect"))
        {
            if (repo_entry.contains("Name"))
            {
                repo_response_data += repo_entry["Name"];
                repo_response_data += "|||| ||||";
                repo_response_data += repo_entry["ID_"];
                repo_response_data += "||||||";
            }
        }
    }
}

void repo::get_magazine_configs()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("AmmoConfig"))
        {
            repo_response_data += repo_entry["AmmoConfig"];
            repo_response_data += "|||| ||||";
            repo_response_data += repo_entry["ID_"];
            repo_response_data += "||||||";
        }
    }
}

void repo::get_behaviours()
{
    repo_response_data = "";

    for (auto& repo_entry : repo_modified)
    {
        if (repo_entry.contains("Type"))
        {
            if (repo_entry.contains("Name"))
            {
                repo_response_data += repo_entry["Name"];
                repo_response_data += "|||| ||||";
                repo_response_data += repo_entry["ID_"];
                repo_response_data += "||||||";
            }
        }
    }
}

void repo::get_image_hash(std::string id)
{
    repo_response_data = "";

    if (repo_modified.contains(id))
    {
        if (repo_modified[id].contains("Image"))
        {
            std::unordered_map<std::string, uint64_t>::iterator it2 = ores_object.ores_entries.find(std::string(repo_modified[id]["Image"]));

            if (it2 != ores_object.ores_entries.end())
            {
                //std::cout << "Image found: " << it2->first << " with hash " << util::uint64_t_to_hex_string(it2->second) << std::endl;

                repo_response_data = util::uint64_t_to_hex_string(it2->second);
            }
        }
    }
}

void repo::unload_repo()
{
    //std::unordered_map<std::string, uint32_t>().swap(repo_entries_original);
    //std::unordered_map<std::string, uint32_t>().swap(repo_entries_modified);
}

void repo::load_ores(uint64_t rpkgs_index, uint64_t hash_index)
{
    ores_object = ores(rpkgs_index, hash_index);
}

void repo::get_json(std::string id)
{
    repo_response_data = "";

    if (repo_modified.contains(id))
    {
        std::stringstream ss;

        ss << std::setw(4) << repo_modified[id];

        repo_response_data = ss.str();
    }
}

void repo::save_json(std::string id, std::string json)
{
    if (repo_modified.contains(id))
    {
        repo_response_data = "";

        try
        {
            repo_modified[id] = nlohmann::json::parse(json);

            std::string new_id = repo_modified[id]["ID_"];

            if (id != new_id)
            {
                repo_modified[new_id] = repo_modified[id];
                repo_modified.erase(id);

                repo_response_data = new_id;
            }
            else
            {
                repo_response_data = id;
            }
        }
        catch (json::parse_error& e)
        {
            std::stringstream ss;

            ss << "Error: Saving REPO JSON Entry" << "\n"
                << "Error message: " << e.what() << '\n'
                << "Error exception id: " << e.id << '\n'
                << "Error byte position of error: " << e.byte;

            repo_response_data = ss.str();
        }
    }
}

void repo::create_patch(std::string patch_path)
{
    nlohmann::json repo_patch;
    nlohmann::json smf_json_patch = nlohmann::json::object();

    repo_response_data = "";

    try
    {
        repo_patch = nlohmann::json::diff(repo_original, repo_modified);

        for (auto& entry : repo_patch)
        {
            if (entry["op"] == "add" || entry["op"] == "replace")
            {
                entry["op"] = "add";

                std::string path = entry["path"];

                size_t pos1 = path.find('/');
                size_t pos2 = path.substr(pos1 + 1).find('/');

                if (pos1 == 0 && pos2 == std::string::npos)
                {
                    std::string id = path.substr(pos1 + 1);

                    //entry["path"] = "/" + std::string(repo_modified[index]["ID_"]);

                    smf_json_patch[nlohmann::json::json_pointer(entry["path"])] = entry["value"];

                    //smf_json_patch[id] = repo_modified[id];
                }
                else if (pos1 == 0 && pos2 > 0)
                {
                    std::string id = path.substr(pos1 + 1, pos2);

                    //entry["path"] = "/" + std::string(repo_modified[index]["ID_"]) + path.substr(pos1 + 1 + pos2);

                    smf_json_patch[nlohmann::json::json_pointer(entry["path"])] = entry["value"];

                    //smf_json_patch[id] = repo_modified[id];
                }
            }
        }

        std::ofstream json_file(patch_path, std::ofstream::binary);

        json_file << std::setw(4) << smf_json_patch << std::endl;

        json_file.close();

        //std::ofstream json_file2(patch_path + "2", std::ofstream::binary);

        //json_file2 << std::setw(4) << repo_patch << std::endl;

        //json_file2.close();
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;

        ss << "Error: Creating SMF REPO Patch JSON" << "\n"
            << "Error message: " << e.what() << '\n'
            << "Error exception id: " << e.id << '\n'
            << "Error byte position of error: " << e.byte;

        repo_response_data = ss.str();
    }
}

void repo::import_patch(std::string patch_path)
{
    std::ifstream patch_file(patch_path, std::ifstream::binary);

    patch_file.seekg(0, patch_file.end);
    uint64_t patch_file_size = patch_file.tellg();
    patch_file.seekg(0, patch_file.beg);

    std::vector<char> patch_data = std::vector<char>(patch_file_size, 0);

    patch_file.read(patch_data.data(), patch_file_size);

    patch_file.close();

    repo_response_data = "";

    try
    {
        nlohmann::json patch_json = nlohmann::json::parse(patch_data);

        repo_original.merge_patch(patch_json);
        repo_modified.merge_patch(patch_json);
    }
    catch (json::parse_error& e)
    {
        std::stringstream ss;

        ss << "Error: Importing SMF REPO Patch JSON" << "\n"
            << "Error message: " << e.what() << '\n'
            << "Error exception id: " << e.id << '\n'
            << "Error byte position of error: " << e.byte;

        repo_response_data = ss.str();
    }
}

void repo::duplicate_entry(std::string id)
{
    std::string guid = util::generate_guid();

    nlohmann::json new_json = repo_modified[id];

    new_json["ID_"] = guid;

    repo_modified[guid] = new_json;

    get_entry(guid);
}

void repo::erase_entry(std::string id)
{
    repo_modified.erase(id);
}

void repo::get_entry(std::string id)
{
    repo_response_data = "";

    if (repo_modified[id].contains("Outfit") && repo_modified[id].contains("Name"))
    {
        repo_response_data = repo_modified[id]["Name"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("Runtime") && repo_modified[id].contains("CommonName"))
    {
        repo_response_data = repo_modified[id]["CommonName"];
        repo_response_data += "||||";
        repo_response_data += util::uint64_t_to_hex_string(std::strtoull(std::string(repo_modified[id]["Runtime"]).c_str(), nullptr, 10));
        repo_response_data += "||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("IsHitmanSuit") && repo_modified[id].contains("CommonName"))
    {
        repo_response_data = repo_modified[id]["CommonName"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("PersistentBoolId") && repo_modified[id].contains("Name"))
    {
        repo_response_data = repo_modified[id]["Name"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("Name_LOC") && repo_modified[id].contains("Name"))
    {
        repo_response_data = repo_modified[id]["Name"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("ModifierType"))
    {
        repo_response_data = repo_modified[id]["ModifierType"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("Parameter"))
    {
        repo_response_data = repo_modified[id]["Parameter"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("Type") && repo_modified[id].contains("Name"))
    {
        repo_response_data = repo_modified[id]["Name"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("AmmoConfig"))
    {
        repo_response_data = repo_modified[id]["AmmoConfig"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
    else if (repo_modified[id].contains("AmmoImpactEffect") && repo_modified[id].contains("Name"))
    {
        repo_response_data = repo_modified[id]["Name"];
        repo_response_data += "|||| ||||";
        repo_response_data += repo_modified[id]["ID_"];
        repo_response_data += "||||||";
    }
}

void repo::update_json_at_pointer(std::string id, std::string json_pointer, std::string value)
{
    if (repo_modified.contains(id))
    {
        repo_response_data = "";

        try
        {
            nlohmann::json json = repo_modified[id][nlohmann::json::json_pointer(json_pointer)];

            if (json.is_number())
            {
                json = std::strtod(value.c_str(), nullptr);

                /*if (json.is_number_float())
                {
                    json = std::strtod(value.c_str(), nullptr);
                }
                else if (json.is_number_integer())
                {
                    json = std::strtoll(value.c_str(), nullptr, 10);
                }
                else
                {
                    json = std::strtoull(value.c_str(), nullptr, 10);
                }*/
            }
            else if (json.is_boolean())
            {
                if (value == "true")
                {
                    json = true;
                }
                else
                {
                    json = false;
                }
            }
            else
            {
                json = value;
            }

            repo_modified[id][nlohmann::json::json_pointer(json_pointer)] = json;

            repo_response_data = id;

            if (json_pointer == "/ID_")
            {
                if (id != value)
                {
                    repo_modified[value] = repo_modified[id];
                    repo_modified.erase(id);

                    repo_response_data = value;
                }
            }
        }
        catch (json::parse_error& e)
        {
            std::stringstream ss;

            ss << "Error: Saving REPO JSON Entry" << "\n"
                << "Error message: " << e.what() << '\n'
                << "Error exception id: " << e.id << '\n'
                << "Error byte position of error: " << e.byte;

            repo_response_data = ss.str();
        }
    }
}