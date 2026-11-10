#pragma once
#include <windows.h>
#include "Button.h"

class FlatButton : public Button
{
public:
    FlatButton();
    ~FlatButton() override;

    bool create(HWND parent, HMENU id, int x, int y, int cx, int cy, HANDLE bitmap, int mode);
    bool createWindow(HWND parent, HMENU id, int x, int y, int cx, int cy);
    static LRESULT CALLBACK subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData);

public:
    HBITMAP m_bmpNormal;
    HBITMAP m_bmpHover;
    int     m_width;
    int     m_height;
    DWORD   m_state;
    bool    m_tracking;
};
static_assert(sizeof(FlatButton) == 0x24, "FlatButton size");
