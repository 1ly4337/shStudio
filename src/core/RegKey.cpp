#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include "RegKey.h"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g5.h"

static WCHAR g_regKeyName[256];

bool RegKey::create(HKEY parent, LPCWSTR subKey, REGSAM sam)
{
    DWORD disposition;
    m_created = false;
    if (!RegOpenKeyExW(parent, subKey, 0, sam, &m_key))
        return true;
    if ((sam & 0x80000) == 0)
        return false;
    m_created = true;
    return RegCreateKeyExW(parent, subKey, 0, 0, 0, sam, 0, &m_key, &disposition) == 0;
}

RegKey::~RegKey()
{
    close();
    if (m_path)
        free(m_path);
}

bool RegKey::open(wchar_t* subKey, REGSAM sam)
{
    if (m_open)
    {
        HKEY old = m_key;
        m_open = create(old, subKey, sam);
        RegCloseKey(old);
    }
    else
    {
        m_open = create(m_key, subKey, sam);
    }
    if (m_path)
        free(m_path);
    m_path = _wcsdup(subKey);
    return m_open;
}

bool RegKey::enumKey(DWORD* index, String* name)
{
    DWORD cchName = 256;
    memset(g_regKeyName, 0, 0x100);
    bool ok = RegEnumKeyExW(m_key, *index, g_regKeyName, &cchName, 0, 0, 0, 0) == 0;
    name->clear();
    if (ok)
    {
        name->append(g_regKeyName, cchName);
        ++*index;
        return true;
    }
    return false;
}

bool RegKey::setString(LPCWSTR valueName, const wchar_t* value)
{
    return value
        && RegSetValueExW(m_key, valueName, 0, REG_SZ, (const BYTE*)value,
                          2 * (DWORD)wcslen(value) + 2) == 0;
}

int RegKey::getInt(LPCWSTR valueName, int def)
{
    int value = def;
    DWORD cbData = 4;
    DWORD type = REG_DWORD;
    if (RegQueryValueExW(m_key, valueName, 0, &type, (LPBYTE)&value, &cbData))
        return def;
    return value;
}

bool RegKey::getString(LPCWSTR valueName, String* out)
{
    DWORD cbData = 4;
    DWORD type = REG_SZ;
    out->clear();
    if (RegQueryValueExW(m_key, valueName, 0, &type, 0, &cbData) || type != REG_SZ)
        return false;
    wchar_t* data = new wchar_t[cbData];
    LSTATUS status = RegQueryValueExW(m_key, valueName, 0, &type, (LPBYTE)data, &cbData);
    if (!status)
        *out = data;
    delete[] data;
    return status == 0;
}
