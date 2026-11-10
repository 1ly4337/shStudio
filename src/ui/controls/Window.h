#pragma once
#include <windows.h>
#include <uxtheme.h>
#include "bitMask.h"

class Window
{
public:
    Window();
    virtual ~Window();

    virtual void activate(bool active);
    virtual BOOL enable(bool enable);
    virtual BOOL invalidateRect(const RECT* rect, bool erase);
    virtual BOOL invalidate(bool erase);
    virtual BOOL move(int x, int y);
    virtual LRESULT setRedraw(bool redraw);
    virtual BOOL resize(int cx, int cy);
    virtual BOOL show(bool show);
    virtual BOOL isVisible();
    virtual LRESULT setText(LPCWSTR text);
    virtual LRESULT setFont(HFONT font);
    virtual void modifyStyle(DWORD add, DWORD remove);
    virtual void modifyExStyle(DWORD add, DWORD remove);
    virtual bool openTheme(LPCWSTR classList, DWORD flags);
    virtual HRESULT closeTheme();
    virtual LRESULT setWindowTheme(LPCWSTR theme);
    virtual POINT* getPosition(POINT* out);
    virtual SIZE* getSize(SIZE* out);
    virtual RECT* getClientRect(RECT* out);
    virtual RECT* getRectInParent(RECT* out);
    virtual RECT* getWindowRect(RECT* out);
    virtual LPCWSTR getClassName() { return L"Window"; }
    virtual LPCWSTR getThemeClassList(int part) { return 0; }
    virtual void onRegisterClass(WNDCLASSEXW* wcx) {}
    virtual DWORD getStyle() { return 0; }
    virtual DWORD getExStyle() { return 0; }
    virtual bool create(HWND parent, HMENU menuOrId, const RECT* rect, DWORD style, DWORD exStyle,
                        LPCWSTR windowName);
    virtual HWND destroy();
    virtual bool onNcCreate(CREATESTRUCTW* cs) { return true; }
    virtual bool onCreate() { return true; }
    virtual void onDestroy() {}
    virtual void onNcDestroy() {}
    virtual void onActivate(bool active) {}
    virtual void onMoved() {}
    virtual void onResized() {}
    virtual bool onClose() { return true; }
    virtual LRESULT onCtlColor(UINT msg, HDC hdc, HWND ctrl);
    virtual bool onCommand(UINT code, HWND ctrl, UINT id) { return false; }
    virtual bool onDropFile(LPCWSTR path, HWND target, int targetId) { return false; }
    virtual void onEnterSizeMove() {}
    virtual void onEnable(bool enabled) {}
    virtual void onFocus(bool focused) {}
    virtual bool onInitMenu(HMENU menu) { return false; }
    virtual bool onGetMinMaxInfo(MINMAXINFO* mmi) { return false; }
    virtual bool onKey(WPARAM vk, LPARAM flags, bool down) { return false; }
    virtual bool onMaximize() { return false; }
    virtual bool onMouseMove(WPARAM keys, POINT* pt) { return false; }
    virtual void onMove(int x, int y) {}
    virtual bool onNotify(NMHDR* hdr, LRESULT* result) { return false; }
    virtual bool onScroll(bool horizontal, int code, int pos) { return false; }
    virtual void onSetFont(HFONT font, bool redraw) {}
    virtual void onSize(WPARAM type, int cx, int cy) {}
    virtual bool onSysCommand(WPARAM cmd, POINT* pt) { return false; }
    virtual bool onSysKey(WPARAM vk, LPARAM flags, bool down) { return false; }
    virtual void onThemeChanged();
    virtual bool onTimer(UINT_PTR id) { return false; }
    virtual bool onLButton(WPARAM keys, POINT* pt, bool down) { return false; }
    virtual bool onMButton(WPARAM keys, POINT* pt, bool down) { return false; }
    virtual bool onRButton(WPARAM keys, POINT* pt, bool down) { return false; }
    virtual bool onDblClick(WPARAM keys, POINT* pt, int button) { return false; }
    virtual bool onMouseWheel(int delta, POINT* pt, UINT keys) { return false; }
    virtual bool onMouseLeave() { return false; }
    virtual bool vfunc_FC(int arg) { return false; }
    virtual LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static Window* fromHwnd(HWND hWnd, bool searchParents);
    HWND createStatic(int x, int y, int width, int height, LPCWSTR text, DWORD style, unsigned int fontId);
    HWND createButton(HMENU id, int x, int y, int width, int height, LPCWSTR text, int unused1, int unused2);

    HWND getHwnd() const { return m_hWnd; }

public:
    POINT   m_sizeMovePos;
    SIZE    m_sizeMoveSize;
    HWND    m_hWnd;
    HWND    m_hNotify;
    HTHEME  m_hTheme;
    COLORREF m_bkColor;
    DWORD   m_state;
    HMENU   m_childId;
    bitMask<unsigned char> m_flags;
};
static_assert(sizeof(Window) == 0x34, "Window size");

extern int g_classicAppearance;
bool Window_isClassicAppearance();

BOOL Window_centerOnScreen(HWND hWnd);
BOOL Window_centerOn(HWND hWnd, HWND reference);
HWND Window_fromPoint(const POINT* pt);
