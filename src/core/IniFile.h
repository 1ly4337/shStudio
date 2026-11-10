#pragma once
#include <windows.h>
#include "String.hpp"
#include "RegKey.h"

class IniFile
{
public:
    IniFile();
    virtual ~IniFile();
    virtual UINT getInt(LPCWSTR section, LPCWSTR key, INT def);
    virtual String getString(LPCWSTR section, LPCWSTR key, LPCWSTR def);
    virtual void writeString(LPCWSTR section, LPCWSTR key, LPCWSTR value);
    virtual void writeInt(LPCWSTR section, LPCWSTR key, int value);

public:
    DWORD  m_flags;
    String m_appName;
    String m_iniPath;
    RegKey m_reg;
    bool   m_iniExists;
};
static_assert(sizeof(IniFile) == 0x58, "IniFile size");
