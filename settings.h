#pragma once
#include <string>
#include <vector>
#include "skin_manager.h"

struct WeaponSkinSetting
{
    int weaponId;
    SkinInfo skin;
};

bool SaveSettings(const std::string& filename, const std::vector<WeaponSkinSetting>& settings);
bool LoadSettings(const std::string& filename, std::vector<WeaponSkinSetting>& settings);
