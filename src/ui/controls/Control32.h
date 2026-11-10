#pragma once
#include <windows.h>

class String;

class Control32 {
public:
    Control32() : m_hWnd(NULL), m_fontId(0) {}
    virtual ~Control32() {}
    virtual int getFontId();
    virtual bool setFont(HFONT font);
    virtual bool setFontId(int id);
    virtual BOOL enable(bool on);
    virtual unsigned int isDisabled();
    virtual BOOL isEnabled();
    virtual BOOL invalidate();
    virtual BOOL move(int x, int y);
    virtual BOOL resize(int cx, int cy);
    virtual BOOL show(bool on);
    virtual LRESULT setText(const WCHAR* text);
    virtual LPRECT getClientRect(LPRECT rc);
    virtual LRESULT getTextLength();
    virtual String* getText(String* out);

    HWND m_hWnd;
    int  m_fontId;
};
static_assert(sizeof(Control32) == 0x0C, "Control32 size");
