#include "AltEditor.h"
#include "CharAction.h"
#include "String.hpp"
#include <cstdlib>
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "_imports_p06.h"

namespace {

void createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy, int fontIndex)
{
    HWND label = CreateWindowExW(0, L"Static", text, 0x50000000, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(fontIndex), 0);
}

bool createEdit(EditText& edit, HWND parent, LPCWSTR text, int x, int y, int cx, int cy, int id,
                SUBCLASSPROC proc)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", text, 0x50010080, x, y, cx, cy, parent,
                                  (HMENU)(INT_PTR)id, 0, 0);
    if (!edit.m_hWnd)
        return false;
    edit.setFontId(edit.m_fontId);
    SetWindowSubclass(edit.m_hWnd, proc, 'edit', (DWORD_PTR)&edit);
    return true;
}

void createFloatEdit(EditText& edit, HWND parent, int x, int y, int id, float value)
{
    if (createEdit(edit, parent, L"", x, y, 50, 18, id, decimalEditSubclassProc))
        edit.setFloat(value);
}

float readFloat(EditText& edit)
{
    String text;
    edit.getText(&text);
    float value = 0.0f;
    if (text.length())
        value = (float)_wtof(text.c_str());
    return value;
}

}

AltEditor::AltEditor(CharAction* action)
    : Modal()
    , m_action(action)
{
}

AltEditor::~AltEditor()
{
}

bool AltEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 290, 220);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Action Editor"))
        return false;
    HWND top = Modal_getTopLevelWindow(parent);
    m_owner = top;
    Window_centerOn(m_hWnd, top);
    show(true);
    return true;
}

bool AltEditor::onCreate()
{
    createLabel(m_hWnd, L"ANI file:", 20, 18, 60, 16, 4);
    createEdit(m_aniFile, m_hWnd, m_action->m_name.c_str(), 80, 16, 170, 18, 100,
               tabSubclassProc);
    SendMessageW(m_aniFile.m_hWnd, EM_LIMITTEXT, 64, 0);

    createLabel(m_hWnd, L"Mode:", 20, 42, 40, 16, 4);
    m_mode.createNumber(m_hWnd, (HMENU)102, 80, 40, 50, 18, m_action->m_mode, 0, 0);
    m_unk1.createNumber(m_hWnd, (HMENU)103, 140, 40, 50, 18, m_action->m_unk1, 0, 0);

    createLabel(m_hWnd, L"Parameters:", 20, 66, 60, 16, 5);
    createLabel(m_hWnd, L"Bytes:", 20, 88, 60, 16, 4);
    m_byte1.createNumber(m_hWnd, (HMENU)110, 80, 86, 50, 18, m_action->m_byte1, 0, 0);
    m_byte2.createNumber(m_hWnd, (HMENU)111, 140, 86, 50, 18, m_action->m_byte2, 0, 0);
    m_byte3.createNumber(m_hWnd, (HMENU)112, 200, 86, 50, 18, m_action->m_byte3, 0, 0);

    createLabel(m_hWnd, L"Floats:", 20, 112, 60, 16, 4);
    createFloatEdit(m_flt1, m_hWnd, 80, 110, 120, m_action->m_floats[0]);
    createFloatEdit(m_flt2, m_hWnd, 140, 110, 121, m_action->m_floats[1]);
    createFloatEdit(m_flt3, m_hWnd, 200, 110, 122, m_action->m_floats[2]);
    return Modal::onCreate();
}

int AltEditor::mapCommandToResult(int id)
{
    if (id != IDOK)
        return id;
    {
        String text;
        String* got = m_aniFile.getText(&text);
        m_action->m_name = *got;
    }
    m_action->m_mode = (BYTE)m_mode.getInt(0);
    m_action->m_byte1 = (BYTE)m_byte1.getInt(0);
    m_action->m_byte2 = (BYTE)m_byte2.getInt(0);
    m_action->m_byte3 = (BYTE)m_byte3.getInt(0);
    m_action->m_unk1 = m_unk1.getInt(0);
    m_action->m_floats[0] = readFloat(m_flt1);
    m_action->m_floats[1] = readFloat(m_flt2);
    m_action->m_floats[2] = readFloat(m_flt3);
    return 1;
}
