#include "ImportSQLEditor.h"
#include "String.hpp"
#include "gdi_cache.h"
#include "_imports_E.h"
#include <commctrl.h>

ImportSQLEditor::ImportSQLEditor(const wchar_t* table)
{
    m_table = table;
    m_importCaptions = false;
}

bool ImportSQLEditor::onCreate()
{
    String text;
    text << L"Use this dialog to import definitions from the " << m_table << L" table.";
    HWND label = CreateWindowExW(0, L"Static", text.m_buffer, WS_CHILD | WS_VISIBLE, 20, 15, 420, 18,
                                 m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    label = CreateWindowExW(0, L"Static",
                            L"All numerical values will be replaced by the values read from the database.",
                            WS_CHILD | WS_VISIBLE, 20, 35, 420, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    m_captions.m_hWnd = CreateWindowExW(0, L"Button", L"Also import items captions.", 0x50000002,
                                        20, 55, 160, 18, m_hWnd, (HMENU)'text', 0, 0);
    if (m_captions.m_hWnd)
        m_captions.setFontId(m_captions.m_fontId);
    else
        GetLastError();
    if (m_captions.m_hWnd)
        SetWindowSubclass(m_captions.m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_captions);

    label = CreateWindowExW(0, L"Static", L"Do you want to proceed?", WS_CHILD | WS_VISIBLE, 20, 87, 200, 18,
                            m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return Modal::onCreate();
}

bool ImportSQLEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (m_captions.m_hWnd != ctrl)
        return Modal::onCommand(code, ctrl, id);
    m_captions.setCheck(m_captions.isChecked() == 0);
    return true;
}

int ImportSQLEditor::mapCommandToResult(int id)
{
    switch (id)
    {
    case IDOK:
        return -1;
    case IDCANCEL:
        return IDNO;
    case IDYES:
        m_importCaptions = m_captions.isChecked() != 0;
        break;
    }
    return id;
}
