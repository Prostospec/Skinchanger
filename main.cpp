#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include "memory_utils.h"

HANDLE g_hProcess = NULL;

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

DWORD GetProcessIdByName(const wchar_t* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);
        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

int main()
{
    const wchar_t* procName = L"cs2.exe";  // имя процесса CS2 (пример)
    DWORD pid = GetProcessIdByName(procName);
    if (!pid)
    {
        std::cout << "Process not found\n";
        return 1;
    }

    g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!g_hProcess)
    {
        std::cout << "Failed to open process\n";
        return 1;
    }

    uintptr_t baseAddress = GetModuleBaseAddress(pid, procName);
    if (!baseAddress)
    {
        std::cout << "Failed to get module base address\n";
        CloseHandle(g_hProcess);
        return 1;
    }

    uintptr_t endAddress = baseAddress + 0x2000000; // пример - 32 Мб для сканирования

    // Пример паттерна — нужно заменить на актуальный паттерн из оффсетов
    const char* pattern = "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0";
    const char* mask = "xxx????xx";

    uintptr_t foundAddress = FindPattern(g_hProcess, baseAddress, endAddress, pattern, mask);

    if (foundAddress != 0)
    {
        std::cout << "Pattern found at: 0x" << std::hex << foundAddress << std::dec << std::endl;
    }
    else
    {
        std::cout << "Pattern not found\n";
    }

    CloseHandle(g_hProcess);
    return 0;
}
