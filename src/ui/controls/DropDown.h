#pragma once
#include <windows.h>
#include "Window.h"

class GDC;

class DropDown : public Window
{
public:
    DropDown() : m_owner(0), m_cellW(0), m_cellH(0), m_skipShow(false), m_hit(false), m_selected(-1), m_hot(-1) {}

    virtual BOOL show(bool show);
    virtual void onRegisterClass(WNDCLASSEXW* wcx);
    virtual bool onKey(WPARAM vk, LPARAM flags, bool down);
    virtual bool onMouseMove(WPARAM keys, POINT* pt);
    virtual bool onLButton(WPARAM keys, POINT* pt, bool down);
    virtual bool onRButton(WPARAM keys, POINT* pt, bool down);
    virtual LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual int getItemCount() = 0;
    virtual void drawBackground(GDC* dc) {}
    virtual void drawItem(GDC* dc, int index, RECT* rc) = 0;

    int hitTest(POINT* pt);
    BOOL drawFrame(GDC* dc, bool excludeClient);
    void print(HDC hdc, DWORD flags);
    void paint(GDC* dc, RECT* rc);

public:
    HWND m_owner;
    int  m_cellW;
    int  m_cellH;
    int  m_columns;
    bool m_skipShow;
    bool m_hit;
    int  m_selected;
    int  m_hot;
};
static_assert(sizeof(DropDown) == 0x50, "DropDown size");
