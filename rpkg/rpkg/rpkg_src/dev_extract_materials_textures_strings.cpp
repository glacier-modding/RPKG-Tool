#include "dev_function.h"
#include "global.h"
#include "file.h"
#include "util.h"
#include "crypto.h"
#include "generic_function.h"
#include "thirdparty/lz4/lz4.h"
#include "thirdparty/lz4/lz4hc.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>

void dev_function::dev_extract_materials_textures_strings(std::string& input_path, std::string& filter, std::string& output_path)
{
    LOG("Loading Hash List...");

    generic_function::load_hash_list(true);

    LOG("Loading Hash List: Done");

    std::vector<std::string> mati_hash_files;
    std::vector<std::string> mati_hash_strings;

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "MATI")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    std::string hash_file_name = rpkgs.at(i).hash.at(hash_index).hash_file_name;

                    uint64_t hash_size;

                    if (rpkgs.at(i).hash.at(hash_index).is_lz4ed == 1)
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).hash_size;

                        if (rpkgs.at(i).hash.at(hash_index).is_xored == 1)
                        {
                            hash_size &= 0x3FFFFFFF;
                        }
                    }
                    else
                    {
                        hash_size = rpkgs.at(i).hash.at(hash_index).hash_size_final;
                    }

                    std::vector<char> input_data(hash_size, 0);

                    std::ifstream file = std::ifstream(rpkgs.at(i).rpkg_file_path, std::ifstream::binary);

                    if (!file.good())
                    {
                        LOG_AND_EXIT("Error: RPKG file " + rpkgs.at(i).rpkg_file_path + " could not be read.");
                    }

                    file.seekg(rpkgs.at(i).hash.at(hash_index).hash_offset, file.beg);
                    file.read(input_data.data(), hash_size);
                    file.close();

                    if (rpkgs.at(i).hash.at(hash_index).is_xored == 1)
                    {
                        crypto::xor_data(input_data.data(), (uint32_t)hash_size);
                    }

                    uint32_t decompressed_size = rpkgs.at(i).hash.at(hash_index).hash_size_final;

                    std::vector<char> output_data(decompressed_size, 0);

                    std::vector<char>* mati_data;

                    if (rpkgs.at(i).hash.at(hash_index).is_lz4ed)
                    {
                        LZ4_decompress_safe(input_data.data(), output_data.data(), (int)hash_size, decompressed_size);

                        mati_data = &output_data;
                    }
                    else
                    {
                        mati_data = &input_data;
                    }

                    uint64_t position = 0x10;

                    uint8_t bytes1 = 0;
                    uint32_t bytes4 = 0;
                    uint64_t bytes8 = 0;

                    std::string mati_string = std::string(&mati_data->data()[position]);

                    std::vector<std::string>::iterator it = std::find(mati_hash_files.begin(), mati_hash_files.end(), hash_file_name);

                    if (it == mati_hash_files.end())
                    {
                        mati_hash_files.push_back(hash_file_name);
                        mati_hash_strings.push_back(mati_string);
                    }
                }
            }
        }
    }

    std::vector<std::string> substrings;
    substrings.push_back("/materials/g2/");
    substrings.push_back("/_pro/effects/materials/liquid/");
    substrings.push_back("/_pro/environment/materials/generic/plastic/");
    substrings.push_back("/materials/characters/rigged_props/clothbundle/");
    substrings.push_back("/_pro/effects/materials/bulletholes/");
    substrings.push_back("/_pro/effects/materials/debris/");
    substrings.push_back("/_pro/effects/materials/fire/");
    substrings.push_back("/_pro/effects/materials/glow/");
    substrings.push_back("/_pro/items/materials/tools/");
    substrings.push_back("/_pro/items/materials/quest/");
    substrings.push_back("/_pro/effects/materials/smoke/");
    substrings.push_back("/_pro/items/materials/melee/");
    substrings.push_back("/_pro/items/materials/firearms/frames/pistol_silverballer/slides/");
    substrings.push_back("/_pro/items/materials/firearms/frames/pistol_silverballer/barrels/");
    substrings.push_back("/_pro/items/materials/firearms/frames/pistol_silverballer/grips/");
    substrings.push_back("/_pro/items/materials/firearms/frames/pistol_silverballer/magazines/");
    substrings.push_back("/_pro/items/materials/firearms/frames/pistol_silverballer/");
    substrings.push_back("/_pro/items/materials/firearms/frames/pistol_silverballer/silencers/");
    substrings.push_back("/_pro/items/materials/gadget/");
    substrings.push_back("/materials/weapons/");
    substrings.push_back("/_pro/items/materials/explosives/");
    substrings.push_back("/_pro/_test/marcinkd/materials/");
    substrings.push_back("/_pro/environment/materials/decals/mechanical/");
    substrings.push_back("/_pro/environment/materials/props/utensils/");
    substrings.push_back("/_pro/environment/materials/props/food/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/traps/");
    substrings.push_back("/materials/weapons/static/");
    substrings.push_back("/_pro/environment/materials/props/electronic/");
    substrings.push_back("/_pro/environment/materials/props/containers/");
    substrings.push_back("/_pro/environment/materials/props/decorative/");
    substrings.push_back("/_pro/items/materials/keys/");
    substrings.push_back("/_pro/environment/materials/decals/logos/");
    substrings.push_back("/_pro/items/materials/firearms/frames/sniper_stealth_a/");
    substrings.push_back("/_pro/items/materials/firearms/accessories/scopes/scope_sniper_a/");
    substrings.push_back("/_pro/items/materials/firearms/accessories/silencers/");
    substrings.push_back("/materials/props/inside/");
    substrings.push_back("/_pro/environment/materials/generic/metal/");
    substrings.push_back("/_pro/effects/materials/dust/");
    substrings.push_back("/_pro/effects/materials/water/");
    substrings.push_back("/_pro/environment/materials/props/accessories/");
    substrings.push_back("/_pro/environment/materials/generic/glass/");
    substrings.push_back("/materials/basicmaterials/effects/decalmaps/");
    substrings.push_back("/materials/basicmaterials/effects/gameplay/");
    substrings.push_back("/materials/basicmaterials/constants/");
    substrings.push_back("/_pro/characters/assets/_base/pro_male_regular/materials/");
    substrings.push_back("/materials/osd/");
    substrings.push_back("/_pro/effects/materials/gameplay/");
    substrings.push_back("/_pro/characters/prototypes/agent47/version01/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/fashionboss/materials/");
    substrings.push_back("/_pro/characters/assets/hero/agent47/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/heads/male/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/sophus/materials/");
    substrings.push_back("/_pro/characters/assets/workers/scientist_01/materials/");
    substrings.push_back("/_pro/characters/assets/_base/pro_fem_regular/materials/");
    substrings.push_back("/materials/characters/supportingcast/jack_hughes/");
    substrings.push_back("/materials/characters/_nude/");
    substrings.push_back("/materials/characters/supportingcast/weapon_buyer/");
    substrings.push_back("/materials/characters/faces/male/");
    substrings.push_back("/materials/characters/hitman/");
    substrings.push_back("/_pro/characters/assets/workers/kitchenstaff/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/male/casual_01/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/nemesis/materials/");
    substrings.push_back("/_pro/characters/assets/workers/chefs_01/materials/");
    substrings.push_back("/_pro/characters/prototypes/outsourcetest_sep2013/formerdictator/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/formerdictator/materials/");
    substrings.push_back("/_pro/characters/assets/workers/waiters_01/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/male/formal_01/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/max_decker/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/philip/materials/");
    substrings.push_back("/_pro/characters/assets/workers/eventscrew_01/materials/");
    substrings.push_back("/_pro/characters/assets/guards/undercovercops_01/materials/");
    substrings.push_back("/_pro/characters/assets/workers/stylists_01/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/sheikh/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/female/formal_01/materials/");
    substrings.push_back("/_pro/characters/assets/workers/mansion_cleaners/females/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/fashionbosspartner/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/fashionmodels/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/editorinchief/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/female/casual_01/materials/");
    substrings.push_back("/_pro/characters/assets/crowd/female/formal/materials/");
    substrings.push_back("/_pro/characters/assets/workers/utilityworkers_01/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/female/_hair/ponytail/materials/");
    substrings.push_back("/_pro/effects/materials/basic/");
    substrings.push_back("/materials/characters/supportingcast/lennys_gang/");
    substrings.push_back("/materials/characters/supportingcast/bodyguard/");
    substrings.push_back("/materials/characters/pro/greenlightvideo_2013march/boardmembers/");
    substrings.push_back("/_pro/characters/assets/workers/utilityworkers_02/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/vampiremagician/materials/");
    substrings.push_back("/_pro/characters/assets/crowd/male/formal/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/musecustomer_female/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/female/_hair/short_slickedback/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/fashiondesigner/materials/");
    substrings.push_back("/_pro/characters/assets/guards/globalsecurity_01/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/heads/female/materials/");
    substrings.push_back("/_pro/characters/assets/workers/mansionstaff_01/materials/");
    substrings.push_back("/_pro/characters/assets/guards/cicada_lab_guard/materials/");
    substrings.push_back("/_pro/characters/assets/workers/delivery_01/materials/");
    substrings.push_back("/_pro/characters/assets/workers/gardeners/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/italy/priest/materials/");
    substrings.push_back("/_pro/environment/materials/decals/dirt/");
    substrings.push_back("/_pro/environment/materials/decals/smudge/");
    substrings.push_back("/_pro/environment/materials/decals/ground/");
    substrings.push_back("/_pro/environment/materials/decals/dust/");
    substrings.push_back("/_pro/environment/materials/decals/");
    substrings.push_back("/_pro/environment/materials/props/wires/");
    substrings.push_back("/_pro/environment/materials/decals/street/");
    substrings.push_back("/_pro/environment/materials/decals/foliage/");
    substrings.push_back("/_pro/environment/materials/decals/road/");
    substrings.push_back("/_pro/environment/materials/decals/markings/");
    substrings.push_back("/_pro/vehicles/materials/logo_decals/shipping/");
    substrings.push_back("/_pro/environment/materials/architecture/railings/");
    substrings.push_back("/_pro/environment/materials/architecture/decor/");
    substrings.push_back("/_pro/environment/materials/props/furniture/");
    substrings.push_back("/_pro/environment/materials/architecture/trims/");
    substrings.push_back("/_pro/environment/materials/props/sculptures/");
    substrings.push_back("/_pro/effects/materials/frost/");
    substrings.push_back("/_pro/environment/materials/props/office_props/");
    substrings.push_back("/materials/furniture/");
    substrings.push_back("/_pro/environment/materials/props/toilets/");
    substrings.push_back("/_pro/environment/materials/backdrop/sky/");
    substrings.push_back("/_pro/vehicles/materials/logo_decals/catering/");
    substrings.push_back("/_pro/vehicles/materials/logo_decals/electrics/");
    substrings.push_back("/_pro/environment/materials/architecture/stairs/");
    substrings.push_back("/_pro/environment/materials/generic/ceiling/");
    substrings.push_back("/_pro/environment/materials/generic/floor/");
    substrings.push_back("/_pro/environment/materials/generic/trim/");
    substrings.push_back("/_pro/environment/materials/props/signs/");
    substrings.push_back("/_pro/environment/materials/props/street_props/");
    substrings.push_back("/_pro/environment/materials/props/industrial/");
    substrings.push_back("/_pro/environment/materials/props/equipment/");
    substrings.push_back("/_pro/environment/materials/props/paintings/");
    substrings.push_back("/_pro/environment/materials/props/pipes/");
    substrings.push_back("/_pro/environment/materials/generic/wood/");
    substrings.push_back("/_pro/environment/materials/decals/assembly/");
    substrings.push_back("/_pro/environment/materials/generic/wall/");
    substrings.push_back("/_pro/environment/materials/architecture/pillars/");
    substrings.push_back("/_pro/environment/materials/decals/edges/");
    substrings.push_back("/_pro/environment/materials/generic/stone/");
    substrings.push_back("/_pro/environment/materials/props/books/");
    substrings.push_back("/_pro/environment/materials/props/plants/");
    substrings.push_back("/_pro/environment/materials/decals/effects/");
    substrings.push_back("/_pro/environment/materials/props/trash/");
    substrings.push_back("/_pro/environment/materials/levels/paris_fashionshow/");
    substrings.push_back("/_pro/environment/materials/architecture/doors/");
    substrings.push_back("/_pro/environment/materials/props/lamps/");
    substrings.push_back("/_pro/environment/materials/generic/cloth/");
    substrings.push_back("/_pro/environment/materials/architecture/scaffolding/");
    substrings.push_back("/_pro/_test/oleksandr/materials/");
    substrings.push_back("/_pro/environment/materials/architecture/windows/");
    substrings.push_back("/_pro/environment/materials/decorative/");
    substrings.push_back("/_pro/environment/materials/props/clothing/");
    substrings.push_back("/_pro/environment/materials/foliage/");
    substrings.push_back("/_pro/environment/materials/props/fabric/");
    substrings.push_back("/_pro/environment/materials/generic/ice/");
    substrings.push_back("/_pro/_test/alanb/materials/");
    substrings.push_back("/_pro/environment/materials/decals/fabric/");
    substrings.push_back("/_pro/environment/materials/props/shop_props/");
    substrings.push_back("/_pro/environment/materials/generic/ground/");
    substrings.push_back("/_pro/_test/janusk/materials/");
    substrings.push_back("/_pro/environment/materials/props/harbour_props/");
    substrings.push_back("/_pro/environment/materials/decals/trims/");
    substrings.push_back("/_pro/environment/materials/backdrop/buildings/");
    substrings.push_back("/_pro/environment/materials/backdrop/props/");
    substrings.push_back("/_pro/environment/materials/architecture/railing/");
    substrings.push_back("/_pro/vehicles/materials/generic_interior_a/");
    substrings.push_back("/_pro/environment/materials/backdrop/vehicles/");
    substrings.push_back("/_pro/vehicles/materials/barge_a/");
    substrings.push_back("/_pro/vehicles/materials/semi_truck_a/");
    substrings.push_back("/_pro/vehicles/materials/tires/");
    substrings.push_back("/_pro/environment/materials/generic/roof/");
    substrings.push_back("/_pro/effects/materials/screens/");
    substrings.push_back("/_pro/environment/materials/decals/signs/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/surveillance/");
    substrings.push_back("/_pro/vehicles/materials/french_compact_a/");
    substrings.push_back("/_pro/vehicles/materials/english_suv_a/");
    substrings.push_back("/_pro/vehicles/materials/old_luxury_sedan_a/");
    substrings.push_back("/_pro/vehicles/materials/van_large_a/");
    substrings.push_back("/_pro/vehicles/materials/french_hatchback_a/");
    substrings.push_back("/_pro/vehicles/materials/french_sedan_a/");
    substrings.push_back("/_pro/vehicles/materials/french_wagon_a/");
    substrings.push_back("/_pro/vehicles/materials/new_asian_compact_a/");
    substrings.push_back("/_pro/vehicles/materials/old_japanese_sedan_a/");
    substrings.push_back("/_pro/environment/materials/architecture/facades/");
    substrings.push_back("/_pro/environment/materials/architecture/chimneys/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/containers/");
    substrings.push_back("/_pro/vehicles/materials/van_small_a/");
    substrings.push_back("/_pro/vehicles/materials/limousine_b/");
    substrings.push_back("/_pro/effects/materials/mechanical/");
    substrings.push_back("/_pro/vehicles/materials/luxury_helicopter_a/");
    substrings.push_back("/_pro/vehicles/materials/box_truck_a/");
    substrings.push_back("/materials/characters/rigged_props/bodybag/");
    substrings.push_back("/_pro/characters/assets/animals/pigeon/materials/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/distractions/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/activators/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/palaceownerswife/materials/");
    substrings.push_back("/_pro/effects/materials/insects/");
    substrings.push_back("/_pro/environment/materials/props/school/");
    substrings.push_back("/_pro/environment/materials/levels/new_zealand/");
    substrings.push_back("/_pro/environment/materials/architecture/beams/");
    substrings.push_back("/_pro/environment/materials/decals/wall/");
    substrings.push_back("/_pro/environment/materials/backdrop/terrain/");
    substrings.push_back("/_pro/environment/materials/props/utilities/");
    substrings.push_back("/_pro/environment/materials/levels/coastal_town_italy/");
    substrings.push_back("/_pro/environment/materials/props/gym/");
    substrings.push_back("/_pro/environment/materials/props/electronics/");
    substrings.push_back("/_pro/vehicles/materials/train_colonial_mumbai_a/");
    substrings.push_back("/_pro/environment/materials/props/garden_props/");
    substrings.push_back("/_pro/environment/materials/props/shops_props/");
    substrings.push_back("/_pro/environment/materials/decals/rust/");
    substrings.push_back("/_pro/vehicles/materials/bicycle_cafe_a/");
    substrings.push_back("/_pro/vehicles/materials/bicycle_race_a/bicycle_race_a/");
    substrings.push_back("/_pro/vehicles/materials/minibus_a/");
    substrings.push_back("/_pro/vehicles/materials/bus_mumbai_a/");
    substrings.push_back("/_pro/environment/materials/props/leisure/");
    substrings.push_back("/_pro/environment/materials/props/rocks/");
    substrings.push_back("/_pro/environment/materials/generic/weaved/");
    substrings.push_back("/_pro/environment/materials/props/flags/");
    substrings.push_back("/_pro/environment/materials/props/farm_props/");
    substrings.push_back("/_pro/environment/materials/props/papers/");
    substrings.push_back("/_pro/environment/materials/generic/rock/");
    substrings.push_back("/materials/props/inside_02/");
    substrings.push_back("/_pro/environment/materials/props/bedroom/");
    substrings.push_back("/_pro/environment/materials/generic/fabric/");
    substrings.push_back("/_pro/environment/materials/props/weaponry/");
    substrings.push_back("/_pro/environment/materials/levels/the_ark/");
    substrings.push_back("/materials/basicmaterials/effects/");
    substrings.push_back("/_pro/environment/materials/levels/seagull/");
    substrings.push_back("/_pro/environment/materials/generic/concrete/");
    substrings.push_back("/_pro/vehicles/materials/trains_mumbai/");
    substrings.push_back("/_pro/environment/materials/generic/colombiaterrainblends/");
    substrings.push_back("/_pro/environment/materials/architecture/walls/");
    substrings.push_back("/_pro/vehicles/materials/italian_sportscar_a/");
    substrings.push_back("/_pro/environment/materials/levels/hokkaido/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/letters/");
    substrings.push_back("/_pro/environment/materials/levels/raccoon/");
    substrings.push_back("/_pro/environment/materials/debug/");
    substrings.push_back("/_pro/environment/materials/props/expo/");
    substrings.push_back("/_pro/environment/materials/decals/decoration/");
    substrings.push_back("/_pro/environment/materials/levels/falcon/");
    substrings.push_back("/_pro/environment/materials/props/laboratory/");
    substrings.push_back("/_pro/environment/materials/architecture/fences/");
    substrings.push_back("/materials/basicmaterials/wood/");
    substrings.push_back("/_pro/vehicles/materials/old_japanese_hatchback_a/");
    substrings.push_back("/_pro/environment/materials/levels/miami/props/");
    substrings.push_back("/_pro/_test/lennio/");
    substrings.push_back("/_pro/environment/materials/levels/miami/");
    substrings.push_back("/_pro/vehicles/materials/tuk_tuk_a/");
    substrings.push_back("/_pro/environment/materials/architecture/floors/");
    substrings.push_back("/_pro/characters/assets/individuals/hokkaido/motorcyclist/materials/");
    substrings.push_back("/_pro/environment/materials/foliage/colombia_lva_materials/");
    substrings.push_back("/_pro/environment/materials/levels/maldives/");
    substrings.push_back("/_pro/environment/materials/props/fountains/");
    substrings.push_back("/_pro/vehicles/materials/longtail_boat_a/");
    substrings.push_back("/_pro/environment/materials/levels/stingray/");
    substrings.push_back("/_pro/environment/materials/props/hospital/");
    substrings.push_back("/_pro/characters/assets/_apparel/accessories/misc/materials/");
    substrings.push_back("/_pro/environment/materials/props/garage_props/");
    substrings.push_back("/_pro/environment/materials/props/curtains/");
    substrings.push_back("/_pro/items/materials/firearms/accessories/scopes/xhairs/");
    substrings.push_back("/_pro/items/materials/firearms/frames/sniper_chinese_dragon_a/");
    substrings.push_back("/_pro/environment/materials/generic/misc/");
    substrings.push_back("/_pro/vehicles/materials/tractor_old_a/");
    substrings.push_back("/_pro/environment/materials/levels/gecko/");
    substrings.push_back("/_glacier/materials/");
    substrings.push_back("/_pro/environment/materials/props/farmhouse_props/");
    substrings.push_back("/_pro/environment/materials/generic/leather/");
    substrings.push_back("/_pro/environment/materials/backdrop/sky/clouds/");
    substrings.push_back("/_pro/environment/materials/props/constructionsite/");
    substrings.push_back("/_pro/environment/materials/levels/marrakesh/signs/");
    substrings.push_back("/_pro/_licensed/quixel/materials/foliage/");
    substrings.push_back("/_pro/_licensed/quixel/materials/props/forest/");
    substrings.push_back("/_pro/_licensed/quixel/materials/props/rocks/");
    substrings.push_back("/_pro/environment/materials/props/bathroom/");
    substrings.push_back("/_pro/environment/materials/levels/marrakesh/");
    substrings.push_back("/_pro/effects/materials/water/bulldog/");
    substrings.push_back("/_pro/environment/materials/levels/bulldog/");
    substrings.push_back("/_pro/_test/hildes/materials/");
    substrings.push_back("/materials/_test/danielj/");
    substrings.push_back("/_pro/_licensed/quixel/materials/generic/ground/");
    substrings.push_back("/_pro/environment/materials/props/ruins/");
    substrings.push_back("/_pro/vehicles/materials/bus_bar_a/");
    substrings.push_back("/_pro/environment/materials/levels/fox/");
    substrings.push_back("/_pro/environment/materials/props/constructionsite_props/");
    substrings.push_back("/_pro/environment/materials/props/misc/");
    substrings.push_back("/_pro/environment/materials/levels/llama/");
    substrings.push_back("/_pro/_licensed/quixel/materials/generic/rock/");
    substrings.push_back("/_pro/environment/materials/foliage/grapevine_a/");
    substrings.push_back("/_pro/_licensed/quixel/materials/scatter/");
    substrings.push_back("/_pro/qa/materials/mockupblocks/");
    substrings.push_back("/_pro/_licensed/quixel/materials/generic/wall/");
    substrings.push_back("/_pro/environment/materials/levels/wet/generic/walls/");
    substrings.push_back("/_pro/environment/materials/levels/wet/decals/");
    substrings.push_back("/_pro/environment/materials/levels/wet/");
    substrings.push_back("/_pro/environment/materials/levels/wet/generic/glass/");
    substrings.push_back("/_pro/_test/stephann/materials/neon/");
    substrings.push_back("/_pro/effects/materials/custom/rat/");
    substrings.push_back("/_pro/characters/assets/individuals/flamingo/android/materials/");
    substrings.push_back("/_pro/environment/materials/levels/wet/generic/ground/");
    substrings.push_back("/_pro/environment/materials/architecture/roofs/");
    substrings.push_back("/_pro/environment/materials/architecture/corbels/");
    substrings.push_back("/_pro/environment/materials/levels/wet/generic/metal/");
    substrings.push_back("/_pro/_test/nigela/materials/");
    substrings.push_back("/_pro/environment/materials/levels/wet/props/");
    substrings.push_back("/_pro/_licensed/quixel/materials/props/trees/");
    substrings.push_back("/_pro/environment/materials/levels/wolverine/");
    substrings.push_back("/_pro/vehicles/materials/apc_a/");
    substrings.push_back("/_pro/environment/materials/decals/scratches/");
    substrings.push_back("/_pro/items/materials/firearms/frames/ar_stealth_a/");
    substrings.push_back("/_pro/items/materials/poison/");
    substrings.push_back("/_pro/items/materials/traps/");
    substrings.push_back("/_pro/characters/assets/guards/bodyguards_01/materials/");
    substrings.push_back("/_pro/characters/assets/civilians/italy/local_mens/materials/");
    substrings.push_back("/_pro/environment/materials/levels/the_facility/");
    substrings.push_back("/_pro/characters/assets/civilians/tourist_beach/materials/");
    substrings.push_back("/_pro/vehicles/materials/old_french_compact_a/");
    substrings.push_back("/_pro/characters/assets/guards/mansian_guard/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/italy/private_investigator/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/italy/plumbers/materials/");
    substrings.push_back("/_pro/items/materials/firearms/frames/shotgun_stealth_a/");
    substrings.push_back("/_pro/environment/materials/examples/");
    substrings.push_back("/_pro/environment/materials/examples/procedural/");
    substrings.push_back("/_pro/vehicles/materials/new_luxury_sedan_a/");
    substrings.push_back("/materials/companyhq/");
    substrings.push_back("/_pro/vehicles/materials/scooter_c/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/unique/");
    substrings.push_back("/_pro/environment/materials/props/observatory/");
    substrings.push_back("/_pro/characters/assets/civilians/italy/local_mens_old/materials/");
    substrings.push_back("/_pro/characters/assets/individuals/paris/palaceowner/materials/");
    substrings.push_back("/_pro/effects/materials/closequarterscombat/cuts/");
    substrings.push_back("/_pro/environment/materials/generic/panels/");
    substrings.push_back("/_pro/environment/materials/levels/hawk/");
    substrings.push_back("/_pro/environment/materials/levels/skunk/");
    substrings.push_back("/_pro/environment/materials/architecture/curbs/");
    substrings.push_back("/_pro/environment/materials/levels/mumbai/");
    substrings.push_back("/_pro/environment/materials/utensils/");
    substrings.push_back("/_pro/_test/julienh/materials/");
    substrings.push_back("/_pro/environment/materials/architecture/decorative/");
    substrings.push_back("/materials/props/outside/");
    substrings.push_back("/_pro/environment/materials/architecture/arches/");
    substrings.push_back("/_pro/characters/assets/workers/mansion_cleaners/males/materials/");
    substrings.push_back("/_pro/environment/materials/architecture/balustres/");
    substrings.push_back("/_pro/environment/materials/generic/walls/");
    substrings.push_back("/_pro/environment/materials/generic/paper/");
    substrings.push_back("/_pro/characters/assets/animals/rat/materials/");
    substrings.push_back("/_pro/vehicles/materials/old_japanese_wagon_a/");
    substrings.push_back("/_pro/characters/assets/individuals/italy/psychiatrist/materials/");
    substrings.push_back("/_pro/characters/assets/crowd/male/casual/materials/");
    substrings.push_back("/_pro/environment/materials/decals/logo/");
    substrings.push_back("/_pro/effects/materials/closequarterscombat/");
    substrings.push_back("/_pro/characters/assets/crowd/female/casual/materials/");
    substrings.push_back("/_pro/environment/materials/decals/facades/");
    substrings.push_back("/_pro/environment/materials/architecture/fireplaces/");
    substrings.push_back("/materials/basicmaterials/other/");
    substrings.push_back("/materials/props/images/");
    substrings.push_back("/materials/props/paintings/");
    substrings.push_back("/materials/basicmaterials/metal/");
    substrings.push_back("/_pro/environment/materials/props/gameplay/ladders/");
    substrings.push_back("/materials/props/items/");
    substrings.push_back("/materials/vehicles/");
    substrings.push_back("/materials/basicmaterials/glass/");
    substrings.push_back("/materials/architecture/floor/");
    substrings.push_back("/_pro/_test/stephann/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/eyes/materials/");
    substrings.push_back("/_pro/characters/assets/_genericparts/eyelashes/materials/");
    substrings.push_back("/materials/basicmaterials/effects/glows/");
    substrings.push_back("/_pro/_test/tracyh/textures/");
    substrings.push_back("/materials/plants/");
    substrings.push_back("/materials/basicmaterials/stone/");
    substrings.push_back("/materials/architecture/decor/");
    substrings.push_back("/materials/environment/backdrop/");
    substrings.push_back("/materials/artfoundation/janusk/");
    substrings.push_back("/materials/basicmaterials/paper/");
    substrings.push_back("/materials/artfoundation/");
    substrings.push_back("/_pro/ui/materials/");

    std::map<uint64_t, uint64_t> mati_hashes_map;
    std::vector<uint64_t> mati_hashes;
    std::vector<std::string> mati_strings;
    std::vector<std::string> mati_substrings;

    for (int i = 0; i < mati_hash_files.size(); i++)
    {
        std::cout << mati_hash_files.at(i) << "," << mati_hash_strings.at(i) << std::endl;

        for (int j = 0; j < substrings.size(); j++)
        {
            std::string test_string = "[assembly:" + substrings.at(j) + mati_hash_strings.at(i) + "].pc_mi";

            std::string ioi_hash = dev_function::dev_compute_ioi_hash(test_string);

            //std::cout << test_string << "," << ioi_hash << std::endl;

            uint64_t temp_mati_hash = std::strtoull(ioi_hash.c_str(), nullptr, 16);

            std::map<uint64_t, uint64_t>::iterator it = hash_list_hash_map.find(temp_mati_hash);

            if (it != hash_list_hash_map.end())
            {
                std::cout << mati_hash_files.at(i) << " matches " << ioi_hash << std::endl;
                std::cout << "FOUND IT:" << test_string << std::endl;

                mati_hashes.push_back(temp_mati_hash);
                mati_strings.push_back(test_string);

                std::string mati_for_tex_string = substrings.at(j) + mati_hash_strings.at(i);

                mati_for_tex_string = util::remove_all_string_from_string(mati_for_tex_string, ".mi");
                mati_for_tex_string = util::remove_all_string_from_string(mati_for_tex_string, "/_pro/environment/materials/");

                mati_substrings.push_back("/_pro/environment/textures/" + mati_for_tex_string);
                mati_hashes_map[temp_mati_hash] = mati_hashes.size() - 1;
            }
        }
    }

    for (uint64_t i = 0; i < rpkgs.size(); i++)
    {
        for (uint64_t r = 0; r < rpkgs.at(i).hash_resource_types.size(); r++)
        {
            if (rpkgs.at(i).hash_resource_types.at(r) == "MATT")
            {
                for (uint64_t j = 0; j < rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).size(); j++)
                {
                    uint64_t hash_index = rpkgs.at(i).hashes_indexes_based_on_resource_types.at(r).at(j);

                    if (gui_control == ABORT_CURRENT_TASK)
                    {
                        return;
                    }

                    std::cout << std::endl << "For " << rpkgs.at(i).hash.at(hash_index).hash_file_name << std::endl;

                    uint32_t hash_reference_count = rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference_count & 0x3FFFFFFF;

                    for (uint32_t k = 0; k < hash_reference_count; k++)
                    {
                        bool found_at_all = false;

                        if ((k + 1) < hash_reference_count)
                        {
                            bool already_found = false;

                            for (uint64_t x = 0; x < rpkgs.size(); x++)
                            {
                                if (!found_at_all)
                                {
                                    std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(x).hash_map.find(rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference.at(k));

                                    if (it != rpkgs.at(x).hash_map.end())
                                    {
                                        std::string hash_resource_type = rpkgs.at(x).hash.at(it->second).hash_resource_type;

                                        //std::cout << "Hash depend: " << rpkgs.at(x).hash.at(it->second).hash_file_name << "," << hash_resource_type << "," << rpkgs.at(x).rpkg_file_name << std::endl;

                                        if (hash_resource_type == "MATB")
                                        {
                                            for (uint64_t z = 0; z < rpkgs.size(); z++)
                                            {
                                                if (!found_at_all)
                                                {
                                                    std::map<uint64_t, uint64_t>::iterator it3 = rpkgs.at(z).hash_map.find(rpkgs.at(i).hash.at(hash_index).hash_reference_data.hash_reference.at(k + 1));

                                                    if (it3 != rpkgs.at(z).hash_map.end())
                                                    {
                                                        if (!found_at_all)
                                                        {
                                                            std::string hash_resource_type2 = rpkgs.at(z).hash.at(it3->second).hash_resource_type;

                                                            if (hash_resource_type2 == "MATI")
                                                            {
                                                                std::string matt_hash_file_name = rpkgs.at(i).hash.at(hash_index).hash_file_name;
                                                                std::string matb_hash_file_name = rpkgs.at(x).hash.at(it->second).hash_file_name;
                                                                std::string mati_hash_file_name = rpkgs.at(z).hash.at(it3->second).hash_file_name;

                                                                std::map<uint64_t, uint64_t>::iterator it6 = mati_hashes_map.find(rpkgs.at(z).hash.at(it3->second).hash_value);

                                                                if (it6 != mati_hashes_map.end())
                                                                {
                                                                    std::cout << mati_hash_file_name << "," << "[" << mati_strings.at(it6->second) << "].pc_mi" << std::endl;
                                                                    std::cout << matt_hash_file_name << "," << "[" << mati_strings.at(it6->second) << "].pc_entitytype" << std::endl;
                                                                    std::cout << matb_hash_file_name << "," << "[" << mati_strings.at(it6->second) << "].pc_entityblueprint" << std::endl;
                                                                }
                                                                else
                                                                {
                                                                    std::cout << "NOT FOUND INSIDE1" << std::endl;

                                                                    std::map<uint64_t, uint64_t>::iterator it4 = hash_list_hash_map.find(rpkgs.at(z).hash.at(it3->second).hash_value);

                                                                    if (it4 != hash_list_hash_map.end())
                                                                    {
                                                                        //std::cout << matt_hash_file_name << " is linked to:" << std::endl;
                                                                        //std::cout << "  - " << matb_hash_file_name << std::endl;
                                                                        //std::cout << "  - " << mati_hash_file_name << std::endl;
                                                                        std::cout << mati_hash_file_name << "," << "[assembly:/*/" << hash_list_hash_strings.at(it4->second) << "].pc_mi" << std::endl;
                                                                        std::cout << matt_hash_file_name << "," << "[assembly:/*/" << hash_list_hash_strings.at(it4->second) << "].pc_entitytype" << std::endl;
                                                                        std::cout << matb_hash_file_name << "," << "[assembly:/*/" << hash_list_hash_strings.at(it4->second) << "].pc_entityblueprint" << std::endl;
                                                                    }
                                                                    else
                                                                    {
                                                                        std::cout << matt_hash_file_name << " is linked to:" << std::endl;
                                                                        std::cout << "  - " << matb_hash_file_name << std::endl;
                                                                        std::cout << "  - " << mati_hash_file_name << std::endl;
                                                                        std::cout << "NOT FOUND INSIDE2" << std::endl;
                                                                    }
                                                                }

                                                                already_found = true;

                                                                found_at_all = true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (!found_at_all)
                        {
                            std::cout << "NOT FOUND AT ALL" << std::endl;
                        }
                    }
                }
            }
        }
    }

    std::vector<std::string> param1;
    param1.push_back("?/diffuse_60.tex]");
    param1.push_back("?/diffuse_a.tex]");
    param1.push_back("?/specular_a.tex]");
    param1.push_back("?/diffuse_128.tex]");
    param1.push_back("?/normal_a.tex]");
    param1.push_back("?/indexfire_a.tex]");
    param1.push_back("?/emissive_b.tex]");
    param1.push_back("?/detail_mask.tex]");
    param1.push_back("?/emissive_a.tex]");
    param1.push_back("?/catwalkwaterfresnel.tex]");
    param1.push_back("?/detail_a.tex]");
    param1.push_back("?/mask_a.tex]");
    param1.push_back("?/alpha_a.tex]");
    param1.push_back("?/gradient_a.tex]");
    param1.push_back("?/emissive_c.tex]");
    param1.push_back("?/color_mask.tex]");
    param1.push_back("?/diffuse_c.tex]");
    param1.push_back("?/height_a.tex]");
    param1.push_back("?/mask_b.tex]");
    param1.push_back("?/gloss_a.tex]");
    param1.push_back("?/diffuse_b.tex]");
    param1.push_back("?/diffuse_weddingguests.tex]");
    param1.push_back("?/diffuse_g.tex]");
    param1.push_back("?/trans_a.tex]");
    param1.push_back("?/wrinkle_mask_b.tex]");
    param1.push_back("?/detailmask.tex]");
    param1.push_back("?/hair_mask.tex]");
    param1.push_back("?/diffusespec.tex]");
    param1.push_back("?/carpaintburn.tex]");
    param1.push_back("?/mask_g.tex]");
    param1.push_back("?/diffuse_blue.tex]");
    param1.push_back("?/specular_inner.tex]");
    param1.push_back("?/specular_inner_hq.tex]");
    param1.push_back("?/diffuse_darkbrown.tex]");
    param1.push_back("?/diffuse_a_red.tex]");
    param1.push_back("?/diffuse_black_a.tex]");
    param1.push_back("?/diffuse_a_blue.tex]");
    param1.push_back("?/emmissive_a.tex]");
    param1.push_back("?/diffuse_a_yellow.tex]");
    param1.push_back("?/alpha.tex]");
    param1.push_back("?/detailmask_a.tex]");
    param1.push_back("?/diffuse.tex]");
    param1.push_back("?/normal_standard.tex]");
    param1.push_back("?/diffuse_standard.tex]");
    param1.push_back("?/outer_reflect.tex]");
    param1.push_back("?/spectrans_standard.tex]");
    param1.push_back("?/diffuse_e.tex]");
    param1.push_back("?/wrinkle_map.tex]");
    param1.push_back("?/spec_n_trans.tex]");
    param1.push_back("?/spec_n_trans_hq.tex]");
    param1.push_back("?/liquid_radius.tex]");
    param1.push_back("?/diffuse_black.tex]");
    param1.push_back("?/outer_highlight_oldeyes.tex]");
    param1.push_back("?/mask_e.tex]");
    param1.push_back("?/diffuse_d.tex]");
    param1.push_back("?/height_b.tex]");
    param1.push_back("?/mask_c.tex]");
    param1.push_back("?/diffuse_rough.tex]");
    param1.push_back("?/height_f.tex]");
    param1.push_back("?/diffuse_smooth.tex]");
    param1.push_back("?/diffuse_hazardelectric.tex]");
    param1.push_back("?/variance_a.tex]");
    param1.push_back("?/diffuse_party.tex]");
    param1.push_back("?/specular_b.tex]");
    param1.push_back("?/diffuse_fashion.tex]");
    param1.push_back("?/ao_a.tex]");
    param1.push_back("?/transmission.tex]");
    param1.push_back("?/diffuse_althero.tex]");
    param1.push_back("?/diffuse_47skin.tex]");
    param1.push_back("?/transmission_a.tex]");
    param1.push_back("?/diffuse_lightbrown.tex]");
    param1.push_back("?/hairmask.tex]");
    param1.push_back("?/diffuse_lightarabic.tex]");
    param1.push_back("?/diffuse_f.tex]");
    param1.push_back("?/diffuse_lime.tex]");
    param1.push_back("?/diffuse_white.tex]");
    param1.push_back("?/diffuse_movie.tex]");
    param1.push_back("?/diffuse_h.tex]");
    param1.push_back("?/diffuse_pattern.tex]");
    param1.push_back("?/diffuse_vest.tex]");
    param1.push_back("?/diffuse_effects.tex]");
    param1.push_back("?/diffuse_saints.tex]");
    param1.push_back("?/diffuse_weddingguest.tex]");
    param1.push_back("?/diffuse_white_logo.tex]");
    param1.push_back("?/diffuse_greatballs.tex]");
    param1.push_back("?/diffuse_agency.tex]");
    param1.push_back("?/diffuse_orange.tex]");
    param1.push_back("?/diffuse_grey.tex]");
    param1.push_back("?/blood_a.tex]");
    param1.push_back("?/fire_a.tex]");
    param1.push_back("?/back_fire.tex]");
    param1.push_back("?/normal_face.tex]");
    param1.push_back("?/normal_regular.tex]");
    param1.push_back("?/normal_lips.tex]");
    param1.push_back("?/wrinkle_mask_a.tex]");
    param1.push_back("?/translucence_a.tex]");
    param1.push_back("?/normal.tex]");
    param1.push_back("?/normal_inner.tex]");
    param1.push_back("?/normal_outer.tex]");
    param1.push_back("?/normal_b.tex]");
    param1.push_back("?/specular_a_red.tex]");
    param1.push_back("?/mask_d.tex]");
    param1.push_back("?/specular_a_blue.tex]");
    param1.push_back("?/normal_c.tex]");
    param1.push_back("?/specular_a_yellow.tex]");
    param1.push_back("?/detail.tex]");
    param1.push_back("?/flow_expanding.tex]");
    param1.push_back("?/rimlight_mask.tex]");
    param1.push_back("?/flow_directional.tex]");
    param1.push_back("?/specular_c.tex]");
    param1.push_back("?/flow_holeincenter.tex]");
    param1.push_back("?/flow_corner_right.tex]");
    param1.push_back("?/detail_b.tex]");
    param1.push_back("?/specular_w_hair.tex]");
    param1.push_back("?/normal_hq.tex]");
    param1.push_back("?/wrinkles_a.tex]");
    param1.push_back("?/wrinkles_b.tex]");
    param1.push_back("?/blood_normal_a.tex]");
    param1.push_back("?/translucency_a.tex]");
    param1.push_back("?/specular_d.tex]");
    param1.push_back("?/specular_f.tex]");
    param1.push_back("?/specular_e.tex]");
    param1.push_back("?/specular_movie.tex]");
    param1.push_back("?/specular_grey.tex]");
    param1.push_back("?/specular_basic.tex]");
    param1.push_back("?/specular_blonde.tex]");
    param1.push_back("?/specular_effects.tex]");
    param1.push_back("?/specular_black.tex]");
    param1.push_back("?/translucency.tex]");
    param1.push_back("?/normal_movie.tex]");
    param1.push_back("?/emissive.tex]");
    param1.push_back("?/normal_outer_hq.tex]");
    param1.push_back("?/hair.tex]");
    param1.push_back("?/foam_a.tex]");
    param1.push_back("?/height_d.tex]");
    param1.push_back("?/height_e.tex]");
    param1.push_back("?/side_fire.tex]");
    param1.push_back("?/side_smoke.tex]");
    param1.push_back("?/indexmuzzleflash_smg.tex]");
    param1.push_back("?/codecrying.tex]");
    param1.push_back("?/intexblood_a.tex]");
    param1.push_back("?/codeyellow.tex]");
    param1.push_back("?/codered.tex]");
    param1.push_back("?/indexfirework.tex]");
    param1.push_back("?/cartireburn.tex]");
    param1.push_back("?/brokenglassgradient.tex]");
    param1.push_back("?/diffuse_l.tex]");
    param1.push_back("?/diffuse_i.tex]");
    param1.push_back("?/diffuse_j.tex]");
    param1.push_back("?/ao.tex]");
    param1.push_back("?/colormask.tex]");
    param1.push_back("?/celldetail_a.tex]");
    param1.push_back("?/emission_a.tex]");
    param1.push_back("?/mask_h.tex]");
    param1.push_back("?/mask_f.tex]");
    param1.push_back("?/emission.tex]");
    param1.push_back("?/foam_b.tex]");
    param1.push_back("?/emissive_a_01.tex]");
    param1.push_back("?/stripes.tex]");
    param1.push_back("?/meltmask.tex]");
    param1.push_back("?/emmisive_a.tex]");
    param1.push_back("?/diffuse_red.tex]");
    param1.push_back("?/reflection_mask.tex]");
    param1.push_back("?/around_mask.tex]");
    param1.push_back("?/burnmask_a.tex]");
    param1.push_back("?/diffuse_green.tex]");
    param1.push_back("?/blue.tex]");
    param1.push_back("?/nosignal_a.tex]");
    param1.push_back("?/cornerright_mask.tex]");
    param1.push_back("?/edgereflect_mask.tex]");
    param1.push_back("?/sanguinelogo_mask.tex]");
    param1.push_back("?/emissive_e.tex]");
    param1.push_back("?/emissive_f.tex]");
    param1.push_back("?/diffuse_b_dmg.tex]");
    param1.push_back("?/height_c.tex]");
    param1.push_back("?/brokenscreen_height.tex]");
    param1.push_back("?/brokenscreen_emissive.tex]");
    param1.push_back("?/diffuse_k.tex]");
    param1.push_back("?/diffuse_m.tex]");
    param1.push_back("?/screen_garden.tex]");
    param1.push_back("?/screen_library.tex]");
    param1.push_back("?/screen_parking.tex]");
    param1.push_back("?/screen_corridor.tex]");
    param1.push_back("?/screen_museroom.tex]");
    param1.push_back("?/screen_crossroad.tex]");
    param1.push_back("?/screen_backstairs.tex]");
    param1.push_back("?/screen_backbalcony.tex]");
    param1.push_back("?/screen_backentrance.tex]");
    param1.push_back("?/screen_frontentrance.tex]");
    param1.push_back("?/translucency_mask.tex]");
    param1.push_back("?/diffuse_male.tex]");
    param1.push_back("?/hp_height.tex]");
    param1.push_back("?/mask_37to00.tex]");
    param1.push_back("?/diffuse_b_highcontrast.tex]");
    param1.push_back("?/normal_d.tex]");
    param1.push_back("?/normal_e.tex]");
    param1.push_back("?/flow_corner.tex]");
    param1.push_back("?/stripes_normal.tex]");
    param1.push_back("?/multifoam_a.tex]");
    param1.push_back("?/around_flow.tex]");
    param1.push_back("?/ashes_diff.tex]");
    param1.push_back("?/ashes_norm.tex]");
    param1.push_back("?/edgeright_flow.tex]");
    param1.push_back("?/edgesplit_flow.tex]");
    param1.push_back("?/cornerleft_flow.tex]");
    param1.push_back("?/edgereflect_flow.tex]");
    param1.push_back("?/cornerright_flow.tex]");
    param1.push_back("?/specular_scatter_a.tex]");
    param1.push_back("?/flowmap_a.tex]");
    param1.push_back("?/depthmap_a.tex]");
    param1.push_back("?/brokenscreen_normal.tex]");
    param1.push_back("?/direction_a.tex]");
    param1.push_back("?/emissive_a_02.tex]");
    param1.push_back("?/screen.tex]");
    param1.push_back("?/screen_b.tex]");

    std::vector<std::string> param2;
    param2.push_back("(ascolormap)");
    param2.push_back("(ascompoundnormal)");
    param2.push_back("(asnormalmap)");
    param2.push_back("(asheightmap)");
    param2.push_back("(ascubemap)");
    param2.push_back("(asbillboardmap)");

    std::vector<std::string> param3;
    param3.push_back(".pc_tex");
    param3.push_back(".pc_mipblock1");

    for (uint64_t j = 0; j < mati_substrings.size(); j++)
    {
        bool found = false;

        for (uint64_t x = 0; x < param1.size(); x++)
        {
            for (uint64_t y = 0; y < param2.size(); y++)
            {
                for (uint64_t z = 0; z < param3.size(); z++)
                {
                    std::string test_string = "[";

                    test_string.append(mati_substrings.at(j));
                    test_string.append(param1.at(x));
                    test_string.append(param2.at(y));
                    test_string.append(param3.at(z));

                    std::string ioi_hash = dev_function::dev_compute_ioi_hash(test_string);

                    //std::cout << "Testing: " << test_string << std::endl;

                    for (uint64_t i = 0; i < rpkgs.size(); i++)
                    {
                        std::map<uint64_t, uint64_t>::iterator it = rpkgs.at(i).hash_map.find(strtoull(ioi_hash.c_str(), nullptr, 16));

                        if (it != rpkgs.at(i).hash_map.end())
                        {
                            found = true;

                            std::cout << "Found: " << rpkgs.at(i).hash.at(it->second).hash_file_name << "," << test_string << std::endl;
                            //std::cout << "Hash: " << ioi_hash << std::endl;
                        }
                    }
                }
            }
        }

        if (!found)
        {
            std::cout << "Not found: " << mati_substrings.at(j) << std::endl;
        }
    }
}