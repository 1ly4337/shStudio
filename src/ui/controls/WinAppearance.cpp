#include <windows.h>
#include <cstring>
#include "WinAppearance.h"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g5.h"

#pragma warning(disable : 4996)

bool isClassicAppearance()
{
    OSVERSIONINFOW version;
    memset(&version.dwMajorVersion, 0, 0x110);
    version.dwOSVersionInfoSize = 276;
    GetVersionExW(&version);
    if (version.dwMajorVersion < 5)
        return true;
    bool classic = false;
    HKEY key = 0;
    if (!RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Appearance", 0, KEY_READ, &key))
    {
        DWORD cbData = 128;
        unsigned short data[130];
        memset(data, 0, 0x80);
        if (!RegQueryValueExW(key, L"Current", 0, 0, (LPBYTE)data, &cbData))
        {
            String current((const wchar_t*)data, (unsigned)-1);
            classic = current.containsNoCase(L"standard");
        }
        RegCloseKey(key);
    }
    return classic;
}

BOOL isWindowsNT5()
{
    OSVERSIONINFOW version;
    memset(&version.dwMajorVersion, 0, 0x110);
    version.dwOSVersionInfoSize = 276;
    GetVersionExW(&version);
    return version.dwMajorVersion == 5;
}
