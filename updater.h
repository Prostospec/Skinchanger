// updater.h
#pragma once
#include <string>

// Проверяет обновления, возвращает true, если есть новая версия
bool CheckForUpdates(const std::string& currentVersion, std::string& latestVersion, std::string& downloadUrl);
