#pragma once
#include <windows.h>
#include "String.hpp"

class VersionInfo
{
public:
    explicit VersionInfo(HMODULE module);
    ~VersionInfo();

    void load(LPCWSTR fileName);
    static void queryString(LPCVOID block, const wchar_t* prefix, const wchar_t* name, String* out);
    String fileVersion() const;

public:
    String m_comments;
    String m_companyName;
    String m_fileDescription;
    String m_legalCopyright;
    String m_originalFilename;
    String m_productName;
    WORD   m_fileVersion[4];
    WORD   m_productVersion[4];
};
static_assert(sizeof(VersionInfo) == 0xD0, "VersionInfo size");

String getModuleFileName(HMODULE module);
