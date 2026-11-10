#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include "VersionInfo.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

#pragma comment(lib, "version.lib")

VersionInfo::VersionInfo(HMODULE module)
{
    memset(m_fileVersion, 0, sizeof(m_fileVersion));
    memset(m_productVersion, 0, sizeof(m_productVersion));
    WCHAR fileName[MAX_PATH];
    if (GetModuleFileNameW(module, fileName, MAX_PATH))
        load(fileName);
}

VersionInfo::~VersionInfo()
{
}

void VersionInfo::load(LPCWSTR fileName)
{
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(fileName, &handle);
    if (!size)
        return;
    BYTE* data = new BYTE[size];
    if (GetFileVersionInfoW(fileName, 0, size, data))
    {
        LPVOID buffer = 0;
        UINT length = 0;
        if (VerQueryValueW(data, L"\\", &buffer, &length) && length == 52)
        {
            const __int16* words = (const __int16*)buffer;
            m_fileVersion[0] = (WORD)sub_41D430(words[5]);
            m_fileVersion[1] = (WORD)sub_41D430(words[4]);
            m_fileVersion[2] = (WORD)sub_41D430(words[7]);
            m_fileVersion[3] = (WORD)sub_41D430(words[6]);
            m_productVersion[0] = (WORD)sub_41D430(words[9]);
            m_productVersion[1] = (WORD)sub_41D430(words[8]);
            m_productVersion[2] = (WORD)sub_41D430(words[11]);
            m_productVersion[3] = (WORD)sub_41D430(words[10]);
        }
        LPVOID translation;
        UINT translationLength = 0;
        if (VerQueryValueW(data, L"\\VarFileInfo\\Translation", &translation, &translationLength)
            && translationLength)
        {
            wchar_t prefix[256];
            swprintf_s(prefix, 256, L"\\StringFileInfo\\%04x%04x\\",
                       ((unsigned short*)translation)[0], ((unsigned short*)translation)[1]);
            queryString(data, prefix, L"Comments", &m_comments);
            queryString(data, prefix, L"CompanyName", &m_companyName);
            queryString(data, prefix, L"FileDescription", &m_fileDescription);
            queryString(data, prefix, L"LegalCopyright", &m_legalCopyright);
            queryString(data, prefix, L"OriginalFilename", &m_originalFilename);
            queryString(data, prefix, L"ProductName", &m_productName);
        }
    }
    delete[] data;
}

void VersionInfo::queryString(LPCVOID block, const wchar_t* prefix, const wchar_t* name, String* out)
{
    wchar_t path[256];
    memset(path, 0, sizeof(path));
    wcscat_s(path, 256, prefix);
    wcscat_s(path, 256, name);
    LPVOID buffer = 0;
    UINT length = 0;
    if (VerQueryValueW(block, path, &buffer, &length) && length)
        out->assign((const wchar_t*)buffer, length);
}

String VersionInfo::fileVersion() const
{
    String result;
    result << (short)m_fileVersion[0] << L'.';
    result << (short)m_fileVersion[1] << L'.';
    result << (short)m_fileVersion[2] << L'.';
    result << (short)m_fileVersion[3];
    return result;
}

String getModuleFileName(HMODULE module)
{
    WCHAR fileName[MAX_PATH];
    GetModuleFileNameW(module, fileName, MAX_PATH);
    return String(fileName, (unsigned)-1);
}
