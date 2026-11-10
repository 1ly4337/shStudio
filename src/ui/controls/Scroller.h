#pragma once
#include "Window.h"

class Scroller : public Window
{
public:
    Scroller();

    bool onScroll(bool horizontal, int code, int pos) override;
    void onSize(WPARAM type, int cx, int cy) override;
    bool onMouseWheel(int delta, POINT* pt, UINT keys) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    virtual int getLineStep();
    virtual LONG updateScrollBars();
    virtual BOOL resetScroll();

public:
    int  m_contentWidth;
    int  m_contentHeight;
    int  m_scrollX;
    int  m_scrollY;
    int  m_field44;
    int  m_field48;
    bool m_updating;
};
static_assert(sizeof(Scroller) == 0x50, "Scroller size");
