#include <windows.h>
#include <commctrl.h>
#include <new>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "EditText.h"
#include "String.hpp"
#include "CtrlHelpers.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

static const UINT_PTR kEditSubclassId = 0x65646974;

bool EditText::create(HWND parent, HMENU id, int x, int y, int cx, int cy, const wchar_t* text, DWORD style,
                      int unused)
{
    DWORD editStyle = style;
    if ((style & WS_BORDER) != 0)
        editStyle = style & ~WS_BORDER;
    DWORD fullStyle;
    if ((editStyle & ES_MULTILINE) != 0)
        fullStyle = editStyle | WS_VSCROLL | ES_WANTRETURN | ES_AUTOVSCROLL;
    else
        fullStyle = editStyle | ES_AUTOHSCROLL;
    m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", text, fullStyle | WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y,
                             cx, cy, parent, id, 0, 0);
    if (!m_hWnd)
        return false;
    setFontId(m_fontId);
    return true;
}

HWND EditText::createNumber(HWND parent, HMENU id, int x, int y, int cx, int cy, UINT value, DWORD style,
                            int unused)
{
    if (!create(parent, id, x, y, cx, cy, L"", style | ES_NUMBER, unused))
        return 0;
    SendMessageW(m_hWnd, EM_LIMITTEXT, 0x10, 0);
    if ((int)(INT_PTR)id <= 0 || !SetDlgItemInt(parent, (int)(INT_PTR)id, value, TRUE))
    {
        wchar_t buffer[16];
        swprintf_s(buffer, 0x10, L"%ld", value);
        setText(buffer);
    }
    GetDlgItemInt(parent, (int)(INT_PTR)id, 0, TRUE);
    SetWindowSubclass(m_hWnd, integerEditSubclassProc, kEditSubclassId, (DWORD_PTR)this);
    GetDlgItemInt(parent, (int)(INT_PTR)id, 0, TRUE);
    return m_hWnd;
}

LRESULT EditText::setText(const WCHAR* text)
{
    return SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)text);
}

String* EditText::getText(String* out)
{
    WPARAM size = (SendMessageW(m_hWnd, WM_GETTEXTLENGTH, 0, 0) + 32) & 0xFFFFFFE0;
    wchar_t* buffer = new wchar_t[size];
    memset(buffer, 0, 2 * size);
    LRESULT copied = SendMessageW(m_hWnd, WM_GETTEXT, size, (LPARAM)buffer);
    new (out) String();
    out->append(buffer, (unsigned)copied);
    delete[] buffer;
    return out;
}

int EditText::getInt(int def)
{
    int value = def;
    LONG id = GetWindowLongW(m_hWnd, GWL_ID);
    if (id > 0)
    {
        BOOL translated = FALSE;
        UINT result = GetDlgItemInt(GetParent(m_hWnd), id, &translated, FALSE);
        if (!translated)
            return value;
        return (int)result;
    }
    wchar_t buffer[32];
    if (m_hWnd && SendMessageW(m_hWnd, WM_GETTEXT, 0x20, (LPARAM)buffer) > 0)
        swscanf_s(buffer, L"%ld", &value);
    return value;
}

float EditText::getFloat()
{
    alignas(String) unsigned char storage[sizeof(String)];
    String* text = getText((String*)storage);
    float value;
    if (text->m_length)
        value = (float)_wtof(text->m_buffer);
    else
        value = 0.0f;
    text->~String();
    return value;
}

void EditText::setInt(int value)
{
    String text;
    text << (long)value;
    SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)text.m_buffer);
}
