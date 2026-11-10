#include "ZoneEditor.h"
#include "TacticsZone.h"
#include "String.hpp"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <commctrl.h>
#include <cstdio>
#include <cstdlib>

static inline HWND createLabel(HWND parent, LPCWSTR text, DWORD style, int x, int y, int cx, int cy)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, style, x, y, cx, cy, parent, 0, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return hwnd;
}

static inline void createFloatEdit(EditText& edit, HWND parent, int x, int y, int id)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, x, y, 60, 18, parent,
                                  (HMENU)(INT_PTR)id, 0, 0);
    if (edit.m_hWnd) {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&edit);
        HWND hwnd = edit.m_hWnd;
        wchar_t text[32];
        swprintf_s(text, 32, L"%.4f", 0.0);
        SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)text);
    }
}

static inline void createIntEdit(EditText& edit, HWND parent, int x, int y, int id)
{
    HWND dlg = parent;
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50012080, x, y, 60, 18, dlg,
                                  (HMENU)(INT_PTR)id, 0, 0);
    if (edit.m_hWnd) {
        edit.setFontId(edit.m_fontId);
        SendMessageW(edit.m_hWnd, EM_LIMITTEXT, 16, 0);
        if (!SetDlgItemInt(dlg, id, 0, TRUE)) {
            wchar_t text[16];
            swprintf_s(text, 16, L"%ld", 0);
            edit.setText(text);
        }
        GetDlgItemInt(dlg, id, 0, TRUE);
        SetWindowSubclass(edit.m_hWnd, integerEditSubclassProc, 'edit', (DWORD_PTR)&edit);
        GetDlgItemInt(dlg, id, 0, TRUE);
    }
}

static inline void setFloatText(HWND hwnd, float value)
{
    wchar_t text[32];
    double v = value;
    if (v >= 1.0) {
        if (v >= 100.0) {
            const wchar_t* fmt = L"%.2f";
            if (v >= 1000.0)
                fmt = L"%.1f";
            swprintf_s(text, 32, fmt, v);
        } else {
            swprintf_s(text, 32, L"%.3f", v);
        }
    } else {
        swprintf_s(text, 32, L"%.4f", v);
    }
    SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)text);
}

static inline void setIntText(HWND hwnd, int value)
{
    String text;
    text << (long)value;
    SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)text.m_buffer);
}

static inline float readFloat(EditText& edit)
{
    String text;
    edit.getText(&text);
    float value = 0.0f;
    if (text.m_length)
        value = (float)_wtof(text.m_buffer);
    return value;
}

ZoneEditor::ZoneEditor(TacticsZone* zone)
    : Modal()
{
    m_zone = zone;
}

ZoneEditor::~ZoneEditor()
{
}

bool ZoneEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 365, 290);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Zone Editor"))
        return false;
    HWND owner = Modal_getTopLevelWindow(parent);
    m_owner = owner;
    Window_centerOn(m_hWnd, owner);
    show(true);
    return true;
}

bool ZoneEditor::onCreate()
{
    createLabel(m_hWnd, L"Points:", WS_CHILD | WS_VISIBLE, 20, 20, 40, 14);
    int x = 65;
    for (unsigned i = 0; (int)i < 4; ) {
        unsigned n = i + 1;
        wchar_t label[8] = {0};
        swprintf_s(label, 8, L"#%u", n);
        createLabel(m_hWnd, label, WS_CHILD | WS_VISIBLE | SS_CENTER, x, 20, 40, 16);
        i = n;
        x += 70;
    }
    createLabel(m_hWnd, L"X:", WS_CHILD | WS_VISIBLE, 35, 38, 20, 14);
    createLabel(m_hWnd, L"Z:", WS_CHILD | WS_VISIBLE, 35, 62, 20, 14);

    int ex = 60;
    int id = 120;
    for (int i = 0; i < 4; ++i) {
        createFloatEdit(m_x[i], m_hWnd, ex, 36, id - 10);
        createFloatEdit(m_z[i], m_hWnd, ex, 60, id);
        ex += 70;
        ++id;
    }

    createLabel(m_hWnd, L"Map:", WS_CHILD | WS_VISIBLE, 20, 90, 30, 14);
    createIntEdit(m_map, m_hWnd, 60, 88, 100);
    createLabel(m_hWnd, L"Params:", WS_CHILD | WS_VISIBLE, 130, 90, 50, 14);
    createIntEdit(m_params[0], m_hWnd, 200, 88, 0x82);
    createIntEdit(m_params[1], m_hWnd, 270, 88, 0x83);
    createLabel(m_hWnd, L"Description:", WS_CHILD | WS_VISIBLE, 20, 116, 150, 18);

    m_desc.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50211044, 20, 134, 310, 64, m_hWnd,
                                    (HMENU)0x84, 0, 0);
    if (m_desc.m_hWnd) {
        m_desc.setFontId(m_desc.m_fontId);
        SetWindowSubclass(m_desc.m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)&m_desc);
    }
    SendMessageW(m_desc.m_hWnd, EM_LIMITTEXT, 0x200, 0);
    m_desc.setFont(getStdFont(6));

    setIntText(m_map.m_hWnd, FIELD(BYTE, m_zone, 0x5C));
    setFloatText(m_x[0].m_hWnd, *FIELD(float*, m_zone, 0x24));
    setFloatText(m_z[0].m_hWnd, *FIELD(float*, m_zone, 0x2C));
    setFloatText(m_x[1].m_hWnd, FIELD(float, m_zone, 0x30));
    setFloatText(m_z[1].m_hWnd, FIELD(float, m_zone, 0x38));
    setFloatText(m_x[2].m_hWnd, FIELD(float, m_zone, 0x3C));
    setFloatText(m_z[2].m_hWnd, FIELD(float, m_zone, 0x44));
    setFloatText(m_x[3].m_hWnd, FIELD(float, m_zone, 0x48));
    setFloatText(m_z[3].m_hWnd, FIELD(float, m_zone, 0x50));
    setIntText(m_params[0].m_hWnd, FIELD(BYTE, m_zone, 0x21));
    setIntText(m_params[1].m_hWnd, FIELD(BYTE, m_zone, 0x22));
    m_desc.setText(FIELD(const wchar_t*, m_zone, 0x64));
    return Modal::onCreate();
}

int ZoneEditor::mapCommandToResult(int id)
{
    if (id == 1) {
        FIELD(BYTE, m_zone, 0x5C) = (BYTE)m_map.getInt(-1);
        *FIELD(float*, m_zone, 0x24) = readFloat(m_x[0]);
        *FIELD(float*, m_zone, 0x2C) = readFloat(m_z[0]);
        FIELD(float, m_zone, 0x30) = readFloat(m_x[1]);
        FIELD(float, m_zone, 0x38) = readFloat(m_z[1]);
        FIELD(float, m_zone, 0x3C) = readFloat(m_x[2]);
        FIELD(float, m_zone, 0x44) = readFloat(m_z[2]);
        FIELD(float, m_zone, 0x48) = readFloat(m_x[3]);
        FIELD(float, m_zone, 0x50) = readFloat(m_z[3]);
        FIELD(BYTE, m_zone, 0x21) = (BYTE)m_params[0].getInt(-1);
        FIELD(BYTE, m_zone, 0x22) = (BYTE)m_params[1].getInt(-1);

        String text;
        String* got = m_desc.getText(&text);
        String* desc = &FIELD(String, m_zone, 0x60);
        if (got != desc) {
            unsigned len = got->m_length;
            const wchar_t* src = got->m_buffer;
            desc->m_length = 0;
            desc->m_capacity = 0;
            if (desc->m_buffer) {
                delete[] desc->m_buffer;
                desc->m_buffer = 0;
            }
            desc->append(src, len);
        }
    }
    return id;
}
