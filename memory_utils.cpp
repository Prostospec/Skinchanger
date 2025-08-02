#include "memory_utils.h"
#include <tlhelp32.h>

DWORD GetProcessIdByName(const std::wstring& processName)
{
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry))
    {
        do
        {
            if (processName == entry.szExeFile)
            {
                processId = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return processId;
}

HANDLE OpenProcessHandle(DWORD processId)
{
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
}

bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size)
{
    SIZE_T bytesRead;
    return ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, &bytesRead) && bytesRead == size;
}

bool WriteMemory(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size)
{
    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, (LPVOID)address, buffer, size, &bytesWritten) && bytesWritten == size;
}
