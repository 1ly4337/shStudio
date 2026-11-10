#pragma once
#include "Window.h"

class Popup : public Window
{
public:
    Popup()
    {
        m_iconId = 0;
        m_hMenu = 0;
        m_hStatusBar = 0;
        m_hProgress = 0;
        m_statusHeight = 0;
    }

    RECT* getClientRect(RECT* out) override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    DWORD getStyle() override;
    DWORD getExStyle() override;
    bool onCreate() override;
    void onResized() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onEnterSizeMove() override;
    void onFocus(bool focused) override;
    bool onInitMenu(HMENU menu) override;
    void onSize(WPARAM type, int cx, int cy) override;
    bool onTimer(UINT_PTR id) override;

    virtual bool createFrame(const RECT* rect, WORD menuId, LPCWSTR name);
    virtual void setStatusText(LPCWSTR fmt, INT_PTR arg);
    virtual LRESULT setProgress(WPARAM pos, LPARAM range);
    virtual bool postCommand(UINT id);
    virtual void updateMenu(HMENU menu);
    virtual void fillScriptMenu(HMENU scripts) {}
    virtual LPCWSTR getCommandHelp(UINT id) { return 0; }

    HMENU updateScriptMenu(HMENU menu);
    bool appendMenuItem(HMENU menu, UINT id);
    static Popup* findDocPopup(HWND hWnd);

public:
    HMENU m_hMenu;
    HWND  m_hStatusBar;
    HWND  m_hProgress;
    int   m_statusHeight;
    int   m_iconId;
};
static_assert(sizeof(Popup) == 0x48, "Popup size");
