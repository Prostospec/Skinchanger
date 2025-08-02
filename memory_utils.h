#pragma once
#include <windows.h>
#include <cstdint>

bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, size_t size);
uintptr_t FindPattern(HANDLE hProcess, uintptr_t startAddress, uintptr_t endAddress, const char* pattern, const char* mask);
