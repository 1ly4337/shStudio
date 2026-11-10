#pragma once
#include <windows.h>
#include "Button.h"

class ToggleButton : public Button
{
public:
    ToggleButton() : m_bmpOff(0), m_bmpOn(0), m_checked(false) {}
    ~ToggleButton() override;

    LRESULT setCheck(bool checked) override;
    BOOL isChecked() override;

    bool create(HWND parent, HMENU id, int x, int y, int unused);
    bool createWindow(HWND parent, HMENU id, int x, int y, int cx, int cy);
    HDC draw(HDC hdc);
    static LRESULT CALLBACK subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
                                         DWORD_PTR refData);

public:
    HBITMAP m_bmpOff;
    HBITMAP m_bmpOn;
    bool    m_checked;
};
static_assert(sizeof(ToggleButton) == 0x18, "ToggleButton size");
