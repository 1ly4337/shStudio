#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ida_compat.h"
#include "EditText.h"
#include "Button.h"
#include "_imports_p07.h"

#pragma warning(disable : 4066 4498)

const long MON_FORMAT_MO2 = 'MO2 ';
const long MON_FORMAT_MO4 = 'MO4 ';

const wchar_t* monModelVersion(const wchar_t* fileName);

#define MM_FORMAT(m)    FIELD(long, (m), 0x0C)
#define MM_INDEX(m)     FIELD(int, (m), 0x10)
#define MM_NAME(m)      FIELD(String, (m), 0x14)
#define MM_UNKN(m)      FIELD(BYTE, (m), 0x34)
#define MM_ANIMS(m)     FIELD(Strings, (m), 0x38)
#define MM_VERTICES(m)  FIELD(Strings, (m), 0x64)
#define MM_TEXTURES(m)  FIELD(Strings, (m), 0x90)
#define MM_HEIGHT(m)    FIELD(float, (m), 0xBC)

static inline HWND monCreateLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy, int fontIndex)
{
    HWND label = CreateWindowExW(0, L"Static", text, 0x50000000, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(fontIndex), 0);
    return label;
}

static inline void monCreateEdit(EditText& edit, HWND parent, int id, const wchar_t* text, int x, int y, int cx,
                                 int cy, SUBCLASSPROC proc)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", text, 0x50010080, x, y, cx, cy, parent,
                                  (HMENU)(INT_PTR)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, proc, 0x65646974 , (DWORD_PTR)&edit);
    }
}

static inline void monCreateButton(Button& button, HWND parent, const wchar_t* text, int id, int x, int y, int cx,
                                   int cy)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, 0x50000000, x, y, cx, cy, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}
