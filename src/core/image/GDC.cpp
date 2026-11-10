#include "GDC.h"
#include "_imports_p06.h"

GDC::GDC(HDC hdc)
    : m_hdc(hdc), m_savedPen(0), m_saved0C(0), m_saved10(0), m_saved14(0)
{
}

GDC::~GDC()
{
    restore();
}

HGDIOBJ GDC::restore()
{
    if (m_savedPen)
    {
        DeleteObject(SelectObject(m_hdc, m_savedPen));
        m_savedPen = 0;
    }
    if (m_saved0C)
    {
        SelectObject(m_hdc, m_saved0C);
        m_saved0C = 0;
    }
    if (m_saved10)
    {
        SelectObject(m_hdc, m_saved10);
        m_saved10 = 0;
    }
    HGDIOBJ result = m_saved14;
    if (result)
    {
        result = SelectObject(m_hdc, m_saved14);
        m_saved14 = 0;
    }
    return result;
}

BOOL GDC::frameRect(const RECT* rc)
{
    MoveToEx(m_hdc, rc->left, rc->top, 0);
    LineTo(m_hdc, rc->right - 1, rc->top);
    LineTo(m_hdc, rc->right - 1, rc->bottom - 1);
    LineTo(m_hdc, rc->left, rc->bottom - 1);
    return LineTo(m_hdc, rc->left, rc->top);
}

BOOL GDC::frameRect(const RECT* rc, COLORREF color)
{
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ old = SelectObject(m_hdc, pen);
    if (m_savedPen)
        DeleteObject(old);
    else
        m_savedPen = old;
    MoveToEx(m_hdc, rc->left, rc->top, 0);
    LineTo(m_hdc, rc->right - 1, rc->top);
    LineTo(m_hdc, rc->right - 1, rc->bottom - 1);
    LineTo(m_hdc, rc->left, rc->bottom - 1);
    return LineTo(m_hdc, rc->left, rc->top);
}

BOOL GDC::fillRect(const RECT* rc, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    UnrealizeObject(brush);
    FillRect(m_hdc, rc, brush);
    return DeleteObject(brush);
}
