#include "NamedAreaEditor.h"
#include "kWorld_NamedArea.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

NamedAreaEditor::NamedAreaEditor(DocView* view, kWorld::NamedArea* area)
    : WorldEditor(view, area)
    , m_area(area)
{
}

bool NamedAreaEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 330);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Named Area Editor");
    if (result)
    {
        HWND top = Modal_getTopLevelWindow(parent);
        m_owner = top;
        Window_centerOn(m_hWnd, top);
        show(true);
        return true;
    }
    return result;
}

int NamedAreaEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"Area", 0x50000000, 20, y + 12, 50, 18, 1);
    label(m_hWnd, L"x1:", 0x50000001, 70, y + 12, 25, 18, 4);
    floatEdit(m_point1[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_area, 0x54));
    label(m_hWnd, L"y1:", 0x50000001, 160, y + 12, 25, 18, 4);
    floatEdit(m_point1[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_area, 0x58));
    label(m_hWnd, L"z1:", 0x50000001, 250, y + 12, 25, 18, 4);
    floatEdit(m_point1[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_area, 0x5C));

    row = y + 34;
    label(m_hWnd, L"x2:", 0x50000001, 70, y + 36, 25, 18, 4);
    floatEdit(m_point2[0], m_hWnd, 0x78325F5F , 95, row, 60, 18, FIELD(float, m_area, 0x64));
    label(m_hWnd, L"y2:", 0x50000001, 160, y + 36, 25, 18, 4);
    floatEdit(m_point2[1], m_hWnd, 0x79325F5F , 185, row, 60, 18, FIELD(float, m_area, 0x68));
    label(m_hWnd, L"z2:", 0x50000001, 250, y + 36, 25, 18, 4);
    floatEdit(m_point2[2], m_hWnd, 0x7A325F5F , 275, row, 60, 18, FIELD(float, m_area, 0x6C));

    row = y + 58;
    label(m_hWnd, L"Attributes", 0x50000000, 20, y + 60, 50, 18, 4);
    label(m_hWnd, L"k1:", 0x50000001, 70, y + 60, 25, 18, 4);
    m_k1.createNumber(m_hWnd, (HMENU)0x6B315F5F , 95, row, 60, 18, FIELD(int, m_area, 0x70), 0, 0);
    label(m_hWnd, L"mode:", 0x50000001, 155, y + 60, 30, 18, 4);
    m_mode.createNumber(m_hWnd, (HMENU)0x6D6F6465 , 185, row, 60, 18, FIELD(int, m_area, 0x74), 0, 0);
    label(m_hWnd, L"k3:", 0x50000001, 250, y + 60, 25, 18, 4);
    m_k3.createNumber(m_hWnd, (HMENU)0x6B335F5F , 275, row, 60, 18, FIELD(int, m_area, 0x78), 0, 0);

    label(m_hWnd, L"Label 1 (displayed label if mode is 0, filename if mode is 2)", 0x50000000,
          20, y + 90, 300, 18, 4);
    textEdit(m_caption1Edit, m_hWnd, 0x73315F5F , 20, y + 108, 315, 50, 0x50211044, 0x100);
    label(m_hWnd, L"Label 2", 0x50000000, 20, y + 162, 200, 18, 4);
    textEdit(m_caption2Edit, m_hWnd, 0x73325F5F , 20, y + 180, 315, 50, 0x50211044, 0x100);

    return y + 234;
}

void NamedAreaEditor::save()
{
    FIELD(float, m_area, 0x54) = getFloat(m_point1[0]);
    FIELD(float, m_area, 0x58) = getFloat(m_point1[1]);
    FIELD(float, m_area, 0x5C) = getFloat(m_point1[2]);
    FIELD(float, m_area, 0x64) = getFloat(m_point2[0]);
    FIELD(float, m_area, 0x68) = getFloat(m_point2[1]);
    FIELD(float, m_area, 0x6C) = getFloat(m_point2[2]);
    FIELD(int, m_area, 0x70) = m_k1.getInt(-1);
    FIELD(int, m_area, 0x74) = m_mode.getInt(-1);
    FIELD(int, m_area, 0x78) = m_k3.getInt(-1);
    WorldEditor::save();
}
