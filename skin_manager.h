#pragma once
#include <string>
#include <windows.h>

struct SkinInfo
{
    int skinId;           // ID скина (пример)
    std::string skinName; // Имя скина
};

// Инициализация менеджера скинов (открытие процесса и т.п.)
bool InitializeSkinManager(const std::wstring& processName);

// Функция замены скина в игре (примитивная заглушка)
bool ApplySkinToWeapon(int weaponId, const SkinInfo& skin);

// Очистка ресурсов
void CleanupSkinManager();
