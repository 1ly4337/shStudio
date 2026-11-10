#include <windows.h>
#include <shlwapi.h>
#include <cstring>
#include "IniFile.h"
#include "VersionInfo.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

IniFile::IniFile()
    : m_reg(HKEY_CURRENT_USER)
{
    m_flags = 1;
    HMODULE module = GetModuleHandleW(0);
    String modulePath = getModuleFileName(module);
    m_appName = PathFindFileNameW(modulePath.m_buffer);
    if (m_flags & 2)
    {
        String key(L"SOFTWARE\\", (unsigned)-1);
        VersionInfo version(module);
        if (version.m_companyName.m_length)
            key.append(version.m_companyName.m_buffer, version.m_companyName.m_length) << L'\\';
        if (version.m_productName.m_length)
            key.append(version.m_productName.m_buffer, version.m_productName.m_length);
        else
            key.append(m_appName.m_buffer, m_appName.m_length);
        m_reg.open(key.m_buffer, KEY_ALL_ACCESS);
    }
    if (m_flags & 1)
    {
        m_iniPath.clear();
        m_iniPath.append(modulePath.m_buffer, modulePath.m_length);
        m_iniPath.removeFileName();
        m_iniPath << L"settings";
        m_iniPath << L".ini";
    }
    else if (m_flags & 4)
    {
        m_iniPath = L"settings";
    }
    m_iniExists = m_iniPath.m_length && PathFileExistsW(m_iniPath.m_buffer);
}

IniFile::~IniFile()
{
    m_reg.close();
}

UINT IniFile::getInt(LPCWSTR section, LPCWSTR key, INT def)
{
    if (m_iniPath.m_length)
        return GetPrivateProfileIntW(section, key, def, m_iniPath.m_buffer);
    if (m_reg.m_open)
    {
        RegKey sectionKey(m_reg.m_key, section, KEY_READ);
        if (sectionKey.m_open)
            return sectionKey.getInt(key, def);
    }
    return def;
}

String IniFile::getString(LPCWSTR section, LPCWSTR key, LPCWSTR def)
{
    String result;
    if (m_iniPath.m_length)
    {
        WCHAR returned[640];
        memset(returned, 0, sizeof(returned));
        DWORD count = GetPrivateProfileStringW(section, key, def, returned, 640, m_iniPath.m_buffer);
        if (count)
        {
            result.assign(returned, count);
            return result;
        }
        result = def;
        return result;
    }
    if (!m_reg.m_open)
    {
        result = def;
        return result;
    }
    RegKey sectionKey(m_reg.m_key, section, KEY_READ);
    if (!sectionKey.m_open || !sectionKey.getString(key, &result))
        result = def;
    return result;
}

void IniFile::writeString(LPCWSTR section, LPCWSTR key, LPCWSTR value)
{
    if (m_flags & 1)
        WritePrivateProfileStringW(section, key, value, m_iniPath.m_buffer);
    if (m_flags & 2)
    {
        RegKey sectionKey(m_reg.m_key, section, KEY_ALL_ACCESS);
        if (sectionKey.m_open)
            sectionKey.setString(key, value);
    }
}

void IniFile::writeInt(LPCWSTR section, LPCWSTR key, int value)
{
    if (m_flags & 1)
    {
        String text;
        text << (long)value;
        WritePrivateProfileStringW(section, key, text.m_buffer, m_iniPath.m_buffer);
    }
    if (m_flags & 2)
    {
        RegKey sectionKey(m_reg.m_key, section, KEY_ALL_ACCESS);
        if (sectionKey.m_open)
        {
            DWORD data = (DWORD)value;
            RegSetValueExW(sectionKey.m_key, key, 0, REG_DWORD, (const BYTE*)&data, 4);
        }
    }
}
