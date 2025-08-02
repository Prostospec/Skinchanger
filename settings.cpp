#include "settings.h"
#include <fstream>
#include "json.hpp"  // nlohmann json

using json = nlohmann::json;

bool SaveSettings(const std::string& filename, const std::vector<WeaponSkinSetting>& settings)
{
    json j;
    for (const auto& s : settings)
    {
        j.push_back({
            {"weaponId", s.weaponId},
            {"skinId", s.skin.skinId},
            {"skinName", s.skin.skinName}
        });
    }

    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    file << j.dump(4);
    return true;
}

bool LoadSettings(const std::string& filename, std::vector<WeaponSkinSetting>& settings)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    json j;
    file >> j;

    settings.clear();
    for (const auto& item : j)
    {
        WeaponSkinSetting s;
        s.weaponId = item["weaponId"].get<int>();
        s.skin.skinId = item["skinId"].get<int>();
        s.skin.skinName = item["skinName"].get<std::string>();
        settings.push_back(s);
    }

    return true;
}
