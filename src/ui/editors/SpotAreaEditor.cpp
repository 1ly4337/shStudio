#include "SpotAreaEditor.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

SpotAreaEditor::SpotAreaEditor(DocView* view, kWorld::Sounds::SpotArea* area)
    : WorldEditor(view, 0)
    , m_area(area)
{
}

SpotAreaEditor::~SpotAreaEditor()
{
}

bool SpotAreaEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 160);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Spot Area Editor");
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

int SpotAreaEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"Center", 0x50000000, 20, y + 12, 50, 18, 4);
    label(m_hWnd, L"x:", 0x50000001, 70, y + 12, 25, 18, 4);
    floatEdit(m_center[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_area, 0x18));
    label(m_hWnd, L"y:", 0x50000001, 160, y + 12, 25, 18, 4);
    floatEdit(m_center[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_area, 0x1C));
    label(m_hWnd, L"z:", 0x50000001, 250, y + 12, 25, 18, 4);
    floatEdit(m_center[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_area, 0x20));

    label(m_hWnd, L"Radius", 0x50000000, 20, y + 36, 50, 18, 4);
    floatEdit(m_radius, m_hWnd, 0x78325F5F , 95, y + 34, 60, 18, FIELD(float, m_area, 0x24));

    return y + 58;
}

void SpotAreaEditor::save()
{
    FIELD(float, m_area, 0x18) = getFloat(m_center[0]);
    FIELD(float, m_area, 0x1C) = getFloat(m_center[1]);
    FIELD(float, m_area, 0x20) = getFloat(m_center[2]);
    FIELD(float, m_area, 0x24) = getFloat(m_radius);
}
