#include "cash_g3.h"
#include "EditText.h"
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "_imports_p07.h"

const wchar_t* g_bagNames[6] = {
    L"EXP & More", L"Buffs", L"Character & Fun", L"Gear & Upgrades", L"Costumes", L"Pets"
};
const wchar_t* g_bagTabNames[6] = {
    L"EXP && More", L"Buffs", L"Character && Fun", L"Gear && Upgrades", L"Costumes", L"Pets"
};
wchar_t g_bagNameBuffer[64];
COMDLG_FILTERSPEC g_itemSetFileTypes = { L"Item Sets File", L"*.sdata" };
wchar_t g_setEditorNumBuffer[16];

HWND g3_createLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy, int fontId)
{
    HWND h = CreateWindowExW(0, L"Static", text, 0x50000000u, x, y, cx, cy, parent, 0, 0, 0);
    if (h)
        SendMessageW(h, WM_SETFONT, (WPARAM)getStdFont(fontId), 0);
    return h;
}

HWND g3_createEdit(EditText& edit, HWND parent, const wchar_t* text, DWORD style,
                   int x, int y, int cx, int cy, int id)
{
    edit.m_hWnd = CreateWindowExW(0x200u, L"Edit", text, style, x, y, cx, cy, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (edit.m_hWnd) {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 0x65646974u , (DWORD_PTR)&edit);
    }
    return edit.m_hWnd;
}
