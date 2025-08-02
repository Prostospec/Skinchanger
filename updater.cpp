// updater.cpp
#include "updater.h"
#include <windows.h>
#include <winhttp.h>
#include <string>

#pragma comment(lib, "winhttp.lib")

bool CheckForUpdates(const std::string& currentVersion, std::string& latestVersion, std::string& downloadUrl)
{
    bool updateAvailable = false;
    HINTERNET hSession = WinHttpOpen(L"CS2SkinChangerUpdater/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession)
        return false;

    HINTERNET hConnect = WinHttpConnect(hSession, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/repos/username/repo/releases/latest", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    BOOL bResults = WinHttpSendRequest(hRequest, L"User-Agent: CS2SkinChanger\r\n", -1L, NULL, 0, 0, 0);
    if (!bResults)
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults)
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    std::string response;
    DWORD dwSize = 0;
    do
    {
        DWORD dwDownloaded = 0;
        char buffer[1024];
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
            break;

        if (dwSize == 0)
            break;

        if (!WinHttpReadData(hRequest, buffer, min(dwSize, sizeof(buffer) - 1), &dwDownloaded))
            break;

        buffer[dwDownloaded] = 0;
        response += buffer;
    } while (dwSize > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    // Простой поиск версии в JSON
    size_t pos = response.find("\"tag_name\":\"");
    if (pos == std::string::npos)
        return false;

    size_t start = pos + 12;
    size_t end = response.find("\"", start);
    latestVersion = response.substr(start, end - start);

    if (latestVersion > currentVersion)
    {
        updateAvailable = true;

        pos = response.find("\"browser_download_url\":\"");
        if (pos != std::string::npos)
        {
            start = pos + 23;
            end = response.find("\"", start);
            downloadUrl = response.substr(start, end - start);
        }
    }

    return updateAvailable;
}
