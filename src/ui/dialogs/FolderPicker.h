#pragma once
#include <windows.h>
#include "Window.h"
#include "String.hpp"

class GDC;

class FolderPicker : public Window
{
public:
    FolderPicker();
    ~FolderPicker() override;

    bool onMouseMove(WPARAM keys, POINT* pt) override;
    bool onLButton(WPARAM keys, POINT* pt, bool down) override;
    bool onMouseLeave() override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    void paint(GDC* dc);

public:
    HBITMAP m_button;
    String  m_path;
    String  m_title;
    int     m_reserved;
    bool    m_browse;
    bool    m_tracking;
};
static_assert(sizeof(FolderPicker) == 0x80, "FolderPicker size");
