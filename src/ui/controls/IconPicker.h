#pragma once
#include <windows.h>
#include "Window.h"
#include "ImageList.h"
#include "ImageStrip.h"
#include "DropDownIcon.h"

class GDC;

class IconPicker : public Window
{
public:
    IconPicker();
    ~IconPicker() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onFocus(bool focused) override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    bool onMouseMove(WPARAM keys, POINT* pt) override;
    bool onLButton(WPARAM keys, POINT* pt, bool down) override;
    bool onMouseLeave() override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    virtual BOOL showIcon(int index);

    bool create(HWND parent, HMENU id, int x, int y, int cx, int cy, int iconSize, int unused);
    int select(int id);
    BOOL drawBorder(GDC* dc, bool excludeClient);
    void print(HDC hdc, UINT flags);
    BOOL paint(GDC* dc, RECT* rc);

public:
    ImageList    m_arrows;
    ImageList    m_icon;
    ImageStrip  m_bitmaps;
    ImageStrip  m_cells;
    DropDownIcon m_dropDown;
    int          m_columns;
    int          m_rows;
    int          m_cellWidth;
    int          m_cellHeight;
    int          m_selected;
    bool         m_tracking;
};
static_assert(sizeof(IconPicker) == 0x148, "IconPicker size");
