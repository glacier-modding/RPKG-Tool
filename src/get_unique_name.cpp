#include "rpkg_function.h"
#include <unordered_map>

void rpkg_function::get_unique_name(std::unordered_map<std::string, uint32_t>& wwem_name_map, std::string& wem_base_name)
{
    if (wwem_name_map.empty())
    {
        wwem_name_map[wem_base_name] = wwem_name_map.size();
        return;
    }

    const auto it = wwem_name_map.find(wem_base_name);

    if (it == wwem_name_map.end())
    {
        wwem_name_map[wem_base_name] = wwem_name_map.size();
        return;
    }

    bool found_new_wem_name = false;

    uint32_t count = 0;

    while (!found_new_wem_name)
    {
        std::string test_name = wem_base_name + "-" + std::to_string(count);

        auto it2 = wwem_name_map.find(test_name);

        if (it2 == wwem_name_map.end())
        {
            wem_base_name = test_name;

            wwem_name_map[wem_base_name] = wwem_name_map.size();

            found_new_wem_name = true;
        }

        count++;
    }
}
