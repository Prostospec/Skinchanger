#include "skin_manager.h"
#include "memory_utils.h"
#include <iostream>

static HANDLE g_hProcess = NULL;

bool InitializeSkinManager(const std::wstring& processName)
{
    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0)
    {
        std::cerr << "Process not found\n";
        return false;
    }

    g_hProcess = OpenProcessHandle(pid);
    if (!g_hProcess)
    {
        std::cerr << "Failed to open process\n";
        return false;
    }

    return true;
}

bool ApplySkinToWeapon(int weaponId, const SkinInfo& skin)
{
    if (!g_hProcess)
        return false;

    // Здесь нужно указать адрес в памяти игры, который отвечает за скин оружия
    // Это пример, в реале адрес надо найти через анализ памяти или SDK
    uintptr_t skinAddress = 0x12345678 + weaponId * 0x100; // Заглушка

    // Пишем skinId в память
    return WriteMemory(g_hProcess, skinAddress, (void*)&skin.skinId, sizeof(skin.skinId));
}

void CleanupSkinManager()
{
    if (g_hProcess)
    {
        CloseHandle(g_hProcess);
        g_hProcess = NULL;
    }
}
