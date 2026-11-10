#pragma once
#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shellapi.h>

class String;

class Control
{
public:
    Control() : m_hWnd(0), m_id(-1), m_flags(0), m_theme(0), m_acceptFiles(false) {}

    virtual ~Control();
    virtual int getId() { return m_id; }
    virtual BOOL invalidate(bool erase);
    virtual BOOL enable(bool on);
    virtual BOOL isEnabled();
    virtual BOOL move(int x, int y);
    virtual BOOL resize(int cx, int cy);
    virtual LRESULT setRedraw(bool on);
    virtual BOOL show(bool on);
    virtual BOOL isVisible();
    virtual LRESULT setFont(HFONT font);
    virtual bool setText(const wchar_t* text);
    virtual bool getText(String& out);
    virtual int modifyStyle(DWORD add, DWORD remove);
    virtual int modifyStyleEx(DWORD add, DWORD remove);
    virtual BOOL modifyFlags(int add, int remove);
    virtual BOOL setFlags(int flags);
    virtual bool openTheme(LPCWSTR classList, DWORD flags);
    virtual HRESULT closeTheme();
    virtual LRESULT setWindowTheme(LPCWSTR theme);
    virtual bool onDropFile(int index, POINT* pt, const wchar_t* path) { return false; }
    virtual POINT* getPosition(POINT* out);
    virtual SIZE* getSize(SIZE* out);
    virtual LPRECT getClientRect(LPRECT rc);
    virtual LPRECT getWindowRect(LPRECT rc);
    virtual LPRECT getRectInParent(LPRECT rc);
    virtual const wchar_t* getClassName() = 0;
    virtual DWORD getStyle();
    virtual DWORD getWindowExStyle() { return 0; }
    virtual DWORD getCtrlStyle() { return 0; }
    virtual bool onCreate();
    virtual void onDestroy() {}
    virtual void onActivate(bool active) {}
    virtual bool onCommand(int code, LPARAM ctrl, int id) { return false; }
    virtual void onDropFiles(HDROP drop, POINT* pt);
    virtual void onFocus(bool gained) {}
    virtual bool onInitMenuPopup(HMENU menu) { return false; }
    virtual bool onKey(short vk, int flags, bool down);
    virtual bool onChar(WPARAM ch, LPARAM flags) { return false; }
    virtual void onSize(int type, int cx, int cy) {}
    virtual bool onSysCommand(WPARAM cmd, POINT* pt) { return false; }
    virtual bool onSysKey(WPARAM vk, LPARAM flags, bool down) { return false; }
    virtual bool onLButton(int keys, POINT* pt, bool down) { return false; }
    virtual bool onMButton(int keys, POINT* pt, bool down) { return false; }
    virtual bool onRButton(int keys, POINT* pt, bool down) { return false; }
    virtual bool onDoubleClick(int keys, POINT* pt, int button) { return false; }
    virtual bool onMouseMove(int keys, POINT* pt) { return false; }
    virtual bool onMouseWheel(short delta, POINT* pt, int keys) { return false; }
    virtual bool onMouseLeave() { return false; }
    virtual bool onNotify(NMHDR* hdr, LRESULT* result) { return false; }
    virtual int vslot50(int);
    virtual LRESULT onCustomDraw(NMLVCUSTOMDRAW* cd, int ctrlId) { return 0; }
    virtual LRESULT windowProc(UINT msg, WPARAM wParam, LPARAM lParam);

    bool create(HWND parent, HMENU id, int x, int y, int cx, int cy, DWORD style, DWORD exStyle,
                int unused);
    static Control* fromHandle(HWND hWnd);
    static LRESULT CALLBACK subclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                         UINT_PTR idSubclass, DWORD_PTR refData);

    HWND   m_hWnd;
    int    m_id;
    int    m_flags;
    HTHEME m_theme;
    bool   m_acceptFiles;
};
static_assert(sizeof(Control) == 0x18, "Control size");
