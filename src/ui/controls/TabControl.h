#pragma once
#include <windows.h>
#include <commctrl.h>
#include "Control.hpp"

class TabControl : public Control
{
public:
    TabControl() : m_gradient(NULL), m_useGradient(false), m_currentPage(NULL) {}
    virtual ~TabControl();

    virtual LPRECT getClientRect(LPRECT rc);
    virtual LPCWSTR getClassName();
    virtual DWORD getStyle();
    virtual bool onCreate();
    bool onCommand(int code, LPARAM ctrl, int id) override;
    virtual void onFocus(bool focused);
    bool onInitMenu(WPARAM menu);
    bool onInitMenuPopup(HMENU menu) override { return onInitMenu((WPARAM)menu); }
    virtual void onSize(int type, int cx, int cy);
    virtual bool onRButton(int keys, POINT* pt, bool down);
    virtual bool onNotify(NMHDR* hdr, LRESULT* result);
    virtual LRESULT subclassProc(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual bool addPage(LPCWSTR text, HWND page, int index);
    virtual HWND getPage(int index);
    virtual HWND removePage(int index);
    virtual bool destroyPage(int index);
    virtual LPRECT getTabsRect(LPRECT rc);
    virtual DWORD getTabStyle() { return 0; }
    virtual bool createPages(RECT* rc) { return true; }
    virtual bool canLeavePage(int index) { return true; }
    virtual void onPageChanged(int index) {}
    virtual bool insertPage(LPCWSTR text, HWND page, LPARAM data, int index);
    virtual BOOL paint(HDC hdc);

    bool select(int index);

public:
    HBITMAP m_gradient;
    bool    m_useGradient;
    HWND    m_currentPage;
};
static_assert(sizeof(TabControl) == 0x24, "TabControl size");
