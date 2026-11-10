#include "SkillDescEditor.h"
#include "Window.h"
#include "_imports_G.h"
#include <commctrl.h>
#include <cstdio>
#include <cstring>
#include <cwchar>

SkillDescEditor::SkillDescEditor(String* texts, int count)
{
    m_texts = texts;
    m_count = count;
    m_height = 25 * count + 120;
}

SkillDescEditor::~SkillDescEditor()
{
}

DWORD SkillDescEditor::getStyle()
{
    return WS_THICKFRAME;
}

bool SkillDescEditor::onCreate()
{
    createControls();
    loadValues();
    return Modal::onCreate();
}

bool SkillDescEditor::onGetMinMaxInfo(MINMAXINFO* mmi)
{
    mmi->ptMinTrackSize.x = 510;
    mmi->ptMinTrackSize.y = m_height;
    mmi->ptMaxTrackSize.x = 2048;
    mmi->ptMaxTrackSize.y = m_height;
    return true;
}

void SkillDescEditor::onSize(WPARAM type, int cx, int cy)
{
    for (int i = 0; i < m_count; ++i)
        m_edits[i].resize(cx - 80, 18);
    int width = m_btnWidth;
    int x = cx - m_btnMarginRight - width;
    int y = cy - m_btnMarginBottom;
    MoveWindow(m_btnCancel, x - 10, y, width, 24, FALSE);
    MoveWindow(m_btnOk, x - m_btnWidth - 30, y, m_btnWidth, 24, FALSE);
    RECT rc;
    SetRect(&rc, 0, y - 5, cx, y + 34);
    InvalidateRect(m_hWnd, &rc, TRUE);
}

int SkillDescEditor::mapCommandToResult(int id)
{
    if (id == IDOK)
        return saveValues();
    return id;
}

bool SkillDescEditor::createEditor(HWND parent)
{
    m_buttons = MB_OKCANCEL;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 840, m_height);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Skill Description Editor");
    if (!result)
        return result;
    HWND owner = Modal_getTopLevelWindow(parent);
    m_owner = owner;
    Window_centerOn(m_hWnd, owner);
    show(true);
    return true;
}

int SkillDescEditor::createControls()
{
    HWND label = CreateWindowExW(0, L"Static", L"Description of skill:", WS_CHILD | WS_VISIBLE,
                                 20, 10, 200, 14, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(1), 0);

    int i = 0;
    int y = 35;
    if (m_count > 0)
    {
        HMENU id = (HMENU)100;
        EditText* edit = m_edits;
        do
        {
            int next = i + 1;
            WCHAR number[4];
            memset(number, 0, sizeof(number));
            swprintf_s(number, 4, L"%d", i + 1);
            HWND numberLabel = CreateWindowExW(0, L"Static", number, WS_CHILD | WS_VISIBLE,
                                               20, y + 2, 20, 18, m_hWnd, 0, 0, 0);
            if (numberLabel)
                SendMessageW(numberLabel, WM_SETFONT, (WPARAM)getStdFont(1), 0);

            HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"",
                                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                                         50, y, 760, 18, m_hWnd, id, 0, 0);
            edit->m_hWnd = hEdit;
            if (hEdit)
            {
                edit->setFontId(edit->m_fontId);
                SetWindowSubclass(edit->m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)edit);
            }
            edit->setFont(getStdFont(6));
            SendMessageW(edit->m_hWnd, EM_LIMITTEXT, 512, 0);
            i = next;
            id = (HMENU)((char*)id + 2);
            y += 25;
            ++edit;
        } while (i < m_count);
    }
    return y;
}

int SkillDescEditor::loadValues()
{
    int result;
    for (int i = 0; i < m_count; ++i)
        result = (int)m_edits[i].setText(m_texts[i].m_buffer);
    return result;
}

int SkillDescEditor::saveValues()
{
    for (int i = 0; i < m_count; ++i)
    {
        String text;
        String* got = m_edits[i].getText(&text);
        String* dst = &m_texts[i];
        if (got != dst)
        {
            unsigned length = got->m_length;
            const wchar_t* src = got->m_buffer;
            dst->m_length = 0;
            dst->m_capacity = 0;
            if (dst->m_buffer)
            {
                delete[] dst->m_buffer;
                dst->m_buffer = 0;
            }
            if (length == (unsigned)-1)
            {
                if (!src)
                    continue;
                length = (unsigned)wcslen(src);
            }
            if (src && length && dst->reserve(length + dst->m_length))
            {
                memcpy(dst->m_buffer + dst->m_length, src, 2 * length);
                dst->m_length += length;
                dst->m_buffer[dst->m_length] = 0;
            }
        }
    }
    return 1;
}
