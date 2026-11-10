#include "ScaleEditor.h"
#include "Window.h"
#include "fPoint.h"
#include "String.hpp"
#include <cstdlib>
#include "gdi_cache.h"
#include "Modal.h"
#include "CtrlHelpers.h"
#include "_imports_p06.h"

ScaleEditor::ScaleEditor(fPoint* factors)
    : Modal(), m_factors(factors)
{
}

ScaleEditor::~ScaleEditor()
{
}

bool ScaleEditor::create(HWND parent)
{
    m_buttons = MB_OKCANCEL;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 300, 125);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Scaling factors");
    if (!result)
        return result;
    HWND owner = Modal_getTopLevelWindow(parent);
    HWND self = m_hWnd;
    m_owner = owner;
    Window_centerOn(self, owner);
    show(true);
    return true;
}

bool ScaleEditor::onCreate()
{
    HWND label = CreateWindowExW(0, L"Static", L"Scaling", 0x50000000, 20, 18, 50, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    double value = m_factors->x;
    m_editX.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 70, 15, 60, 18, m_hWnd,
                                     (HMENU)0x785F5F5F, 0, 0);
    if (m_editX.m_hWnd)
    {
        m_editX.setFontId(m_editX.m_fontId);
        SetWindowSubclass(m_editX.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&m_editX);
        m_editX.setFloat((float)value);
    }
    value = m_factors->y;
    m_editY.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 140, 15, 60, 18, m_hWnd,
                                     (HMENU)0x795F5F5F, 0, 0);
    if (m_editY.m_hWnd)
    {
        m_editY.setFontId(m_editY.m_fontId);
        SetWindowSubclass(m_editY.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&m_editY);
        m_editY.setFloat((float)value);
    }
    value = m_factors->z;
    m_editZ.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 210, 15, 60, 18, m_hWnd,
                                     (HMENU)0x7A5F5F5F, 0, 0);
    if (m_editZ.m_hWnd)
    {
        m_editZ.setFontId(m_editZ.m_fontId);
        SetWindowSubclass(m_editZ.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&m_editZ);
        m_editZ.setFloat((float)value);
    }
    return Modal::onCreate();
}

int ScaleEditor::mapCommandToResult(int id)
{
    if (id == 1)
    {
        float value;
        {
            String text;
            m_editX.getText(&text);
            value = text.m_length ? (float)_wtof(text.m_buffer) : 0.0f;
        }
        m_factors->x = value;
        {
            String text;
            m_editY.getText(&text);
            value = text.m_length ? (float)_wtof(text.m_buffer) : 0.0f;
        }
        m_factors->y = value;
        {
            String text;
            m_editZ.getText(&text);
            value = text.m_length ? (float)_wtof(text.m_buffer) : 0.0f;
        }
        m_factors->z = value;
    }
    return id;
}
