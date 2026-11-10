#include <windows.h>
#include <shlwapi.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include "Library.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

Library g_shell32(L"shell32.dll");

Library::Library(LPCWSTR name)
    : m_module(0), m_refCount(0), m_error(0)
{
    load(name);
}

DWORD Library::load(LPCWSTR name)
{
    if (m_module)
    {
        FreeLibrary(m_module);
        m_module = 0;
    }
    m_error = ERROR_BAD_PATHNAME;
    memset(m_path, 0, sizeof(m_path));
    m_module = LoadLibraryW(name);
    if (m_module)
    {
        GetModuleFileNameW(m_module, m_path, MAX_PATH);
    }
    else if (!wcsstr(name, L"\\"))
    {
        WCHAR dir[MAX_PATH];
        if (GetModuleFileNameW(0, dir, MAX_PATH) && PathRemoveFileSpecW(dir) > 0)
        {
            swprintf_s(m_path, MAX_PATH, L"%s\\%s", dir, name);
            m_module = LoadLibraryW(m_path);
        }
    }
    if (m_module)
    {
        m_error = 0;
        return 0;
    }
    m_error = GetLastError();
    return m_error;
}

Library::~Library()
{
    release();
}

void Library::release()
{
    if (m_module)
    {
        if (m_refCount)
        {
            --*m_refCount;
            if (!*m_refCount)
            {
                operator delete(m_refCount);
                FreeLibrary(m_module);
            }
            m_refCount = 0;
            m_module = 0;
        }
        else
        {
            FreeLibrary(m_module);
            m_module = 0;
        }
    }
}
