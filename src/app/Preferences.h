#pragma once
#include <windows.h>
#include "IniFile.h"

class Preferences : public IniFile
{
public:
    Preferences();
    ~Preferences() override;

    static void saveWindowPlacement(HWND hWnd);

public:
    int m_left;
    int m_top;
    int m_width;
    int m_height;
    int m_display;
};
static_assert(sizeof(Preferences) == 0x6C, "Preferences size");

extern Preferences g_preferences;
