#include "ZoneAreaEditor.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

ZoneAreaEditor::ZoneAreaEditor(DocView* view, kWorld::Sounds::ZoneArea* area)
    : WorldEditor(view, 0)
    , m_area(area)
{
}

ZoneAreaEditor::~ZoneAreaEditor()
{
}

bool ZoneAreaEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 190);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Zone Area Editor");
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

int ZoneAreaEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"Point 1", 0x50000000, 20, y + 12, 50, 18, 4);
    label(m_hWnd, L"x1:", 0x50000001, 70, y + 12, 25, 18, 4);
    floatEdit(m_point1[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_area, 0x14));
    label(m_hWnd, L"y1:", 0x50000001, 160, y + 12, 25, 18, 4);
    floatEdit(m_point1[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_area, 0x18));
    label(m_hWnd, L"z1:", 0x50000001, 250, y + 12, 25, 18, 4);
    floatEdit(m_point1[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_area, 0x1C));

    row = y + 38;
    label(m_hWnd, L"Point 2", 0x50000000, 20, y + 40, 50, 18, 4);
    label(m_hWnd, L"x2:", 0x50000001, 70, y + 40, 25, 18, 4);
    floatEdit(m_point2[0], m_hWnd, 0x78325F5F , 95, row, 60, 18, FIELD(float, m_area, 0x24));
    label(m_hWnd, L"y2:", 0x50000001, 160, y + 40, 25, 18, 4);
    floatEdit(m_point2[1], m_hWnd, 0x79325F5F , 185, row, 60, 18, FIELD(float, m_area, 0x28));
    label(m_hWnd, L"z2:", 0x50000001, 250, y + 40, 25, 18, 4);
    floatEdit(m_point2[2], m_hWnd, 0x7A325F5F , 275, row, 60, 18, FIELD(float, m_area, 0x2C));

    row = y + 66;
    label(m_hWnd, L"Params", 0x50000000, 20, y + 68, 50, 18, 4);
    label(m_hWnd, L"#1:", 0x50000001, 70, y + 68, 25, 18, 4);
    floatEdit(m_param1, m_hWnd, 0x75315F5F , 95, row, 60, 18, FIELD(float, m_area, 0x30));
    label(m_hWnd, L"#2:", 0x50000001, 160, y + 68, 25, 18, 4);
    m_param2.createNumber(m_hWnd, (HMENU)0x6B315F5F , 185, row, 60, 18, FIELD(int, m_area, 0x38), 0, 0);

    return y + 94;
}

void ZoneAreaEditor::save()
{
    FIELD(float, m_area, 0x14) = getFloat(m_point1[0]);
    FIELD(float, m_area, 0x18) = getFloat(m_point1[1]);
    FIELD(float, m_area, 0x1C) = getFloat(m_point1[2]);
    FIELD(float, m_area, 0x24) = getFloat(m_point2[0]);
    FIELD(float, m_area, 0x28) = getFloat(m_point2[1]);
    FIELD(float, m_area, 0x2C) = getFloat(m_point2[2]);
    FIELD(float, m_area, 0x30) = getFloat(m_param1);
    FIELD(int, m_area, 0x38) = m_param2.getInt(-1);
}
