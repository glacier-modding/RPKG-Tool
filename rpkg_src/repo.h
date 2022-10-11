#pragma once

#include <string>
#include <unordered_map>
#include <set>
#include <stdint.h>
#include "thirdparty/json/json.hpp"
#include "ores.h"

enum REPOCategory
{
	NPCS,
	OUTFITS,
	RUNTIME_QUESTITEMS,
	RUNTIME_MELEES,
	RUNTIME_KEYS,
	RUNTIME_EXPLOSIVES,
	RUNTIME_TOOLS,
	RUNTIME_PISTOLS,
	RUNTIME_INVALID_CATEGORY_ICONS,
	RUNTIME_SNIPERRIFLES,
	RUNTIME_ASSAULTRIFLES,
	RUNTIME_REMOTES,
	RUNTIME_SHOTGUNS,
	RUNTIME_SUITCASES,
	RUNTIME_SMGS,
	RUNTIME_DISTRACTIONS,
	RUNTIME_POISONS,
	RUNTIME_CONTAINERS,
	AREADISCOVEREDS,
	LOCATIONS,
	MODIFIERS,
	MAGAZINE_CONFIGS,
	AMMO_CONFIGS,
	DIFFICULTY_PARAMETERS,
	BEHAVIOURS
};

class repo
{
public:
	repo();
	repo(uint64_t rpkgs_index, uint64_t hash_index);
	repo(std::string repo_path);

	void load_repo();
	void load_categories();
	void unload_repo();
	uint32_t search(std::string search_string, uint32_t results_count, uint32_t max_results);
	std::string pretty_json(std::string input_json);
	int valid_json(std::string input_json);
	void check_json(std::string input_json);
	void load_ores(uint64_t rpkgs_index, uint64_t hash_index);
	void get_child_entries(std::string id);
	void get_category(int category);
	void get_npcs();
	void get_outfits();
	void get_runtimes(int category);
	void get_areadiscovereds();
	void get_locations();
	void get_modifiers();
	void get_magazine_configs();
	void get_ammo_configs();
	void get_difficulty_parameters();
	void get_behaviours();
	void get_json(std::string id);
	void save_json(std::string id, std::string json);
	void get_image_hash(std::string id);
	void create_patch(std::string patch_path);
	void import_patch(std::string patch_path);
	void duplicate_entry(std::string id);
	void erase_entry(std::string id);
	void get_entry(std::string id);
	void update_json_at_pointer(std::string id, std::string json_pointer, std::string value);

	uint32_t repo_rpkg_index = 0;
	uint32_t repo_hash_index = 0;
	nlohmann::json repo_original;
	nlohmann::json repo_modified;
	std::unordered_map<std::string, uint32_t> repo_entries_original;
	std::unordered_map<std::string, uint32_t> repo_entries_modified;
	std::set<std::string> repo_runtime_categories;
	ores ores_object;
};