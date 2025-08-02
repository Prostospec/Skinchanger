#pragma once
#include <windows.h>
#include <string>

DWORD GetProcessIdByName(const std::wstring& processName);
HANDLE OpenProcessHandle(DWORD processId);
bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size);
bool WriteMemory(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size);
