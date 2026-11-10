#pragma once
#include <windows.h>
#include "Window.h"
#include "HTable.h"
#include "HWndVec.h"

class GDC;

class Container : public Window
{
public:
    Container()
        : m_textColors(32, 32),
          m_highlightColor(0), m_focus(0), m_transparent(false)
    {
    }
    virtual ~Container();

    virtual HWND destroy();
    virtual void onActivate(bool active);
    virtual LRESULT onCtlColor(UINT msg, HDC hdc, HWND ctrl);
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool onKey(WPARAM vk, LPARAM flags, bool down);
    virtual LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual bool createChild(HWND parent, HMENU id, const RECT* rect, DWORD style, DWORD exStyle,
                             LPCWSTR windowName);
    virtual bool createChildAt(HWND parent, HMENU id, int x, int y, int cx, int cy, DWORD style,
                               DWORD exStyle, LPCWSTR windowName);
    virtual void containerV67() {}
    virtual bool drawNcBorder(GDC* dc);
    virtual bool onScrollBar(int bar, int code, int pos) { return false; }
    virtual bool onDrawItem(UINT id, DRAWITEMSTRUCT* dis);

public:
    HTable<HWND, unsigned long> m_textColors;
    HWndVec  m_highlighted;
    COLORREF m_highlightColor;
    HWND     m_focus;
    bool     m_transparent;
};
static_assert(sizeof(Container) == 0x68, "Container size");

bool Container_toggleCheck(HWND button);
