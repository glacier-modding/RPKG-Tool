#include "temp.h"
#include "rpkg_function.h"
#include "file.h"
#include "global.h"
#include <unordered_map>

void rpkg_function::recursive_temp_loader(uint32_t rpkgs_index, const uint32_t hash_index, const uint32_t temp_version, std::unordered_map<uint32_t, uint32_t>& parents_map, std::vector<uint32_t> temps_indexes, const uint32_t level, const uint32_t level_max, const uint32_t temps_max)
{
	if (level > level_max && level_max != 0 || temps.size() > temps_max && temps_max != 0)
		return;

	const auto it = parents_map.find(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value);

	if (it != parents_map.end())
	{
		LOG("Recursive loop detected at level " + util::uint32_t_to_string(level) + " and the duplicate TEMP file is: " + util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." + rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type);

		const auto it2 = hash_list_hash_map.find(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value);

		if (it2 != hash_list_hash_map.end())
		{
			LOG("IOI String is: " + hash_list_hash_strings.at(it2->second));
		}

		for (uint64_t i = 0; i < temps_indexes.size(); i++)
		{
			LOG("Parent temps index(" + util::uint32_t_to_string(i) + "): " + util::uint32_t_to_string(temps_indexes.at(i)) + ", TEMP File Name: " + temps.at(temps_indexes.at(i)).temp_file_name);
		}
		return;
	}

	temps.emplace_back(temp(rpkgs_index, hash_index, temp_version));

	const uint64_t temps_index = temps.size() - 1;

	temps.at(temps_index).temp_temps_index = temps_index;

	temps_map[rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value] = temps_index;

	parents_map[rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value] = parents_map.size();

	temps.at(temps_index).parents = temps_indexes;

	if (temps.at(temps_index).parents.size() > 0)
	{
		temps.at(temps_indexes.back()).children.push_back(temps_index);
	}

	temps_indexes.push_back(temps_index);

	//for (uint64_t i = 0; i < temps.at(temps_index).parents.size(); i++)
	//{
	//temps.at(temps.at(temps_index).parents.at(i)).children.push_back(temps_index);
	//}

	//LOG(std::string(level * 2, '-') + "TEMP File Name: " + util::uint64_t_to_hex_string(rpkgs.at(rpkgs_index).hash.at(hash_index).hash_value) + "." + rpkgs.at(rpkgs_index).hash.at(hash_index).hash_resource_type);
	//LOG(std::string(level * 2, ' ') + "  - temps.back().temp_depends_file_name.size(): " + util::uint64_t_to_string(temps.at(temps_index).temp_depends_file_name.size()));
	//LOG(std::string(level * 2, ' ') + "  - temps.back().prim_depends_file_name.size(): " + util::uint64_t_to_string(temps.at(temps_index).prim_depends_file_name.size()));

	for (uint64_t i = 0; i < temps.at(temps_index).temp_depends_file_name.size(); i++)
	{
		//std::cout << temps.back().temp_depends_file_name.at(i) << std::endl;

		recursive_temp_loader(temps.at(temps_index).temp_depends_rpkg_index.at(i).at(temps.at(temps_index).temp_depends_rpkg_index_index.at(i)), temps.at(temps_index).temp_depends_hash_index.at(i).at(temps.at(temps_index).temp_depends_hash_index_index.at(i)), temp_version, parents_map, temps_indexes, level + 1, level_max, temps_max);

		//for (uint64_t j = 0; j < temps.at(temps_index).temp_depends_in_rpkgs.at(i).size(); j++)
		//{
		//uint64_t temp_temp_rpkg_index = temps.at(temps_index).temp_depends_rpkg_index.at(i).at(j);
		//uint64_t temp_temp_hash_index = temps.at(temps_index).temp_depends_hash_index.at(i).at(j);

		//recursive_temp_loader(temp_temp_rpkg_index, temp_temp_hash_index, parents_map, temps_indexes, level + 1, level_max, temps_max);
		//}
	}
}
