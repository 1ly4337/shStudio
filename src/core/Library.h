#pragma once
#include <windows.h>

class Library
{
public:
    explicit Library(LPCWSTR name);
    virtual ~Library();

    DWORD load(LPCWSTR name);
    void release();

public:
    HMODULE m_module;
    WCHAR   m_path[MAX_PATH];
    int*    m_refCount;
    DWORD   m_error;
};
static_assert(sizeof(Library) == 0x218, "Library size");

extern Library g_shell32;
