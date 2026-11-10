#include "p05_common.h"
#include "DungeonCoordsViewer.h"
#include "CtrlHelpers.h"
#include "Clipboard.h"
#include "String.hpp"

DungeonCoordsViewer::DungeonCoordsViewer(fPoint p1, fPoint p2, fPoint p3)
    : Modal(), m_pt1(), m_pt2(), m_pt3(), m_edits()
{
    m_pt1.x = p1.x; m_pt1.y = p1.y; m_pt1.z = p1.z;
    m_pt2.x = p2.x; m_pt2.y = p2.y; m_pt2.z = p2.z;
    m_pt3.x = p3.x; m_pt3.y = p3.y; m_pt3.z = p3.z;
}

DungeonCoordsViewer::~DungeonCoordsViewer()
{
}

bool DungeonCoordsViewer::create(HWND parent)
{
    m_buttons = 0;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 290, 180);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Coordinates Viewer");
    if (result) {
        HWND owner = (HWND)Modal_getTopLevelWindow(parent);
        HWND hWnd = m_hWnd;
        m_owner = owner;
        Window_centerOn(hWnd, owner);
        show(true);
        return true;
    }
    return result;
}

bool DungeonCoordsViewer::onCreate()
{
    createControls(10);
    return Modal::onCreate() != 0;
}

int DungeonCoordsViewer::mapCommandToResult(int id)
{
    int result = id;
    if (id == 1) {
        String text;
        const fPoint* pts[3] = { &m_pt1, &m_pt2, &m_pt3 };
        for (int i = 0; i < 3; ++i) {
            text << L'(';
            String& s1 = text << -pts[i]->x;
            s1 << L',' << L'\t';
            String& s2 = s1 << pts[i]->y;
            s2 << L',' << L'\t';
            String& s3 = s2 << pts[i]->z;
            s3 << L')';
            s3 << L"\r\n";
        }
        Clipboard::setText(text.m_buffer, true);
        return 1;
    }
    return result;
}

int DungeonCoordsViewer::createControls(int y)
{
    HWND label;
    label = CreateWindowExW(0, L"Static", L"X", WS_CHILD | WS_VISIBLE, 100, y + 3, 60, 18, m_hWnd, 0, 0, 0);
    if (label) SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(1), 0);
    label = CreateWindowExW(0, L"Static", L"Y", WS_CHILD | WS_VISIBLE, 170, y + 3, 60, 18, m_hWnd, 0, 0, 0);
    if (label) SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(1), 0);
    label = CreateWindowExW(0, L"Static", L"Z", WS_CHILD | WS_VISIBLE, 240, y + 3, 60, 18, m_hWnd, 0, 0, 0);
    if (label) SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(1), 0);

    static const wchar_t* rowNames[3] = { L"Pt 1", L"Pt 2", L"Pt 3" };
    const fPoint* pts[3] = { &m_pt1, &m_pt2, &m_pt3 };
    for (int row = 0; row < 3; ++row) {
        int rowY = y + 24 * (row + 1);
        label = CreateWindowExW(0, L"Static", rowNames[row], WS_CHILD | WS_VISIBLE, 20, rowY + 3, 50, 18,
                                m_hWnd, 0, 0, 0);
        if (label) SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
        float values[3] = { -pts[row]->x, pts[row]->y, pts[row]->z };
        for (int col = 0; col < 3; ++col) {
            EditText& edit = m_edits[row * 3 + col];
            UINT_PTR id = 0x5F5F | ((UINT_PTR)('1' + row) << 16) | ((UINT_PTR)('x' + col) << 24);
            HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 70 + 70 * col, rowY,
                                         60, 18, m_hWnd, (HMENU)id, 0, 0);
            edit.m_hWnd = hEdit;
            if (hEdit) {
                edit.setFontId(edit.m_fontId);
                SetWindowSubclass(edit.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&edit);
                edit.setFloat((float)(double)values[col]);
            }
        }
    }
    return y + 96;
}
