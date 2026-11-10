#pragma once
#include <windows.h>
#include "Globals.h"

class iStream;
class oStream;

class Clipboard
{
public:
    Clipboard() : m_hOwner(0) {}
    virtual ~Clipboard() {}

    static bool setText(const wchar_t* text, bool empty);
    static bool setData(UINT format, oStream& data, bool empty);
    static bool getData(UINT format, iStream& data);

    HWND    m_hOwner;
    Globals m_globals;
};
static_assert(sizeof(Clipboard) == 0x1C, "Clipboard size");

extern Clipboard g_clipboard;
