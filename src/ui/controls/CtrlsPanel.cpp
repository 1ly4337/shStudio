#include "CtrlsPanel.h"
#include "Doc.h"
#include "glViewer.h"
#include "fPoint.h"
#include "PaintDC.h"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <commctrl.h>

static inline HWND createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return hwnd;
}

bool CtrlsPanel::onCreate()
{
    HANDLE bitmap = LoadImageW(hModule, MAKEINTRESOURCEW(0xA9), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    createLabel(m_hWnd, L"Rotation:", 30, 0, 100, 16);
    int y = 18;
    int id = 200;
    for (int i = 0; i < 3; ++i) {
        m_rotReset[i].create(m_hWnd, (HMENU)(INT_PTR)id, 10, y, 18, 18, bitmap, id - 71);
        m_rotEdit[i].createNumber(m_hWnd, (HMENU)(INT_PTR)(id + 10), 30, y, 60, 18, 0, 0, 0);
        HWND buddy = m_rotEdit[i].m_hWnd;
        m_rotSpin[i].m_hWnd = CreateWindowExW(0, L"msctls_updown32", 0, 0x50000067, 30, y, 24, 18, m_hWnd,
                                              (HMENU)(INT_PTR)(id + 20), 0, 0);
        if (m_rotSpin[i].m_hWnd) {
            m_rotSpin[i].setFontId(m_rotSpin[i].m_fontId);
            SendMessageW(m_rotSpin[i].m_hWnd, UDM_SETBUDDY, (WPARAM)buddy, 0);
        } else {
            GetLastError();
        }
        SendMessageW(m_rotSpin[i].m_hWnd, UDM_SETRANGE32, 0, 360);
        y += 24;
        ++id;
    }
    createLabel(m_hWnd, L"Position:", 140, 0, 100, 16);
    y = 18;
    id = 230;
    for (int i = 0; i < 3; ++i) {
        m_posReset[i].create(m_hWnd, (HMENU)(INT_PTR)id, 120, y, 18, 18, bitmap, id - 97);
        m_posEdit[i].createNumber(m_hWnd, (HMENU)(INT_PTR)(id + 10), 140, y, 60, 18, 0, 0, 0);
        y += 24;
        ++id;
    }
    m_bottom = y + 6;
    DeleteObject(bitmap);
    return true;
}

LRESULT CtrlsPanel::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_PAINT)
        return Container::defWindowProc(msg, wParam, lParam);
    PaintDC dc(m_hWnd);
    RECT rc;
    getClientRect(&rc);
    HGDIOBJ old = SelectObject(dc.m_hdc, CreatePen(PS_SOLID, 1, 0xA19D9D));
    if (!dc.m_savedPen)
        dc.m_savedPen = old;
    else
        DeleteObject(old);
    int bottom = rc.bottom;
    MoveToEx(dc.m_hdc, 0, 0, 0);
    LineTo(dc.m_hdc, 0, bottom);
    return 0;
}

bool CtrlsPanel::onCommand(UINT code, HWND ctrl, UINT id)
{
    m_doc->setStatusText(0, 0);
    if (code == EN_UPDATE) {
        if (id - 210 <= 3) {
            if (m_updating)
                return true;
            fPoint rotation(m_viewer->m_rotation.x, m_viewer->m_rotation.y, m_viewer->m_rotation.z);
            float value = (&m_rotEdit[0] + (id - 210))->getFloat();
            rotation.at(id - 210) = value;
            m_viewer->vfunc_11C((int)(INT_PTR)&rotation);
            return true;
        }
        if (id - 240 > 3)
            return Container::onCommand(code, ctrl, id);
        if (m_updating)
            return true;
        fPoint position(m_viewer->m_position.x, m_viewer->m_position.y, m_viewer->m_position.z);
        float value = (&m_posEdit[0] + (id - 240))->getFloat();
        position.at(id - 240) = value;
        m_viewer->setPosition(&position);
        return true;
    }
    if (code == 0) {
        if (id - 200 <= 2) {
            m_rotEdit[id - 200].setInt(0);
            fPoint rotation(m_viewer->m_rotation.x, m_viewer->m_rotation.y, m_viewer->m_rotation.z);
            rotation.at(id - 200) = 0;
            m_viewer->vfunc_11C((int)(INT_PTR)&rotation);
            return true;
        }
        if (id - 230 <= 2) {
            m_posEdit[id - 230].setFloat(0.0f);
            fPoint position(m_viewer->m_position.x, m_viewer->m_position.y, m_viewer->m_position.z);
            position.at(id - 230) = 0;
            m_viewer->setPosition(&position);
            return true;
        }
    }
    return Container::onCommand(code, ctrl, id);
}

void CtrlsPanel::setRotation(fPoint* rotation)
{
    if (m_rotEdit[0].m_hWnd)
        m_rotEdit[0].setInt((int)rotation->x);
    if (m_rotEdit[1].m_hWnd)
        m_rotEdit[1].setInt((int)rotation->y);
    if (m_rotEdit[2].m_hWnd)
        m_rotEdit[2].setInt((int)rotation->z);
    m_viewer->vfunc_11C((int)(INT_PTR)rotation);
}

void CtrlsPanel::setPosition(fPoint* position)
{
    if (m_posEdit[0].m_hWnd)
        m_posEdit[0].setFloat(position->x);
    if (m_posEdit[1].m_hWnd)
        m_posEdit[1].setFloat(position->y);
    if (m_posEdit[2].m_hWnd)
        m_posEdit[2].setFloat(position->z);
    m_viewer->setPosition(position);
}

void CtrlsPanel::vslot2(const fPoint* rotation)
{
    m_updating = true;
    if (m_rotEdit[0].m_hWnd)
        m_rotEdit[0].setInt((int)rotation->x);
    if (m_rotEdit[1].m_hWnd)
        m_rotEdit[1].setInt((int)rotation->y);
    if (m_rotEdit[2].m_hWnd)
        m_rotEdit[2].setInt((int)rotation->z);
    m_updating = false;
}

void CtrlsPanel::vslot3(const fPoint* position)
{
    m_updating = true;
    if (m_posEdit[0].m_hWnd)
        m_posEdit[0].setFloat(position->x);
    if (m_posEdit[1].m_hWnd)
        m_posEdit[1].setFloat(position->y);
    if (m_posEdit[2].m_hWnd)
        m_posEdit[2].setFloat(position->z);
    m_updating = false;
}
