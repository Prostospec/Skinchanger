#include "memory_utils.h"
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
