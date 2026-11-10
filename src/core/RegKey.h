#pragma once
#include <windows.h>
#include <cstdlib>

class String;

class RegKey
{
public:
    explicit RegKey(HKEY key) : m_key(key), m_created(false), m_open(false), m_path(0) {}
    RegKey(HKEY parent, LPCWSTR subKey, REGSAM sam) : m_key(0)
    {
        m_open = create(parent, subKey, sam);
        m_path = _wcsdup(subKey);
    }
    ~RegKey();

    void close()
    {
        if (m_key)
        {
            RegCloseKey(m_key);
            m_key = 0;
        }
        if (m_path)
        {
            free(m_path);
            m_path = 0;
        }
        m_open = false;
    }

    bool create(HKEY parent, LPCWSTR subKey, REGSAM sam);
    bool open(wchar_t* subKey, REGSAM sam);
    bool enumKey(DWORD* index, String* name);
    bool setString(LPCWSTR valueName, const wchar_t* value);
    int getInt(LPCWSTR valueName, int def);
    bool getString(LPCWSTR valueName, String* out);

public:
    HKEY     m_key;
    bool     m_created;
    bool     m_open;
    wchar_t* m_path;
};
static_assert(sizeof(RegKey) == 0xC, "RegKey size");
