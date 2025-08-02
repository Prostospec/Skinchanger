#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>

bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, size_t size)
{
    SIZE_T bytesRead;
    return ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, &bytesRead) && bytesRead == size;
}

bool DataCompare(const BYTE* data, const BYTE* pattern, const char* mask)
{
    for (; *mask; ++mask, ++data, ++pattern)
    {
        if (*mask == 'x' && *data != *pattern)
            return false;
    }
    return true;
}

uintptr_t FindPattern(HANDLE hProcess, uintptr_t startAddress, uintptr_t endAddress, const char* pattern, const char* mask)
{
    SIZE_T size = endAddress - startAddress;
    std::vector<BYTE> buffer(size);

    if (!ReadMemory(hProcess, startAddress, buffer.data(), size))
        return 0;

    for (SIZE_T i = 0; i < size; ++i)
    {
        if (DataCompare(buffer.data() + i, (const BYTE*)pattern, mask))
            return startAddress + i;
    }
    return 0;
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

int main()
{
    const wchar_t* procName = L"cs2.exe";  // Имя процесса
    DWORD pid = GetProcessIdByName(procName);

    if (!pid)
    {
        std::cout << "Process not found\n";
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess)
    {
        std::cout << "Failed to open process\n";
        return 1;
    }

    uintptr_t baseAddress = GetModuleBaseAddress(pid, procName);
    if (!baseAddress)
    {
        std::cout << "Failed to get module base address\n";
        CloseHandle(hProcess);
        return 1;
    }

    // Для примера возьмем сканирование 32 MB после базового адреса
    uintptr_t endAddress = baseAddress + 0x2000000;

    // Пример паттерна (надо заменить на актуальный для CS2)
    const char* pattern = "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0";
    const char* mask = "xxx????xx";

    uintptr_t foundAddress = FindPattern(hProcess, baseAddress, endAddress, pattern, mask);

    if (foundAddress)
        std::cout << "Pattern found at: 0x" << std::hex << foundAddress << std::dec << std::endl;
    else
        std::cout << "Pattern not found\n";

    CloseHandle(hProcess);
    return 0;
}
