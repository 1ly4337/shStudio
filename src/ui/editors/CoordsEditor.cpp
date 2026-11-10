#include "CoordsEditor.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

CoordsEditor::CoordsEditor(DocView* view, kWorld::Element::Coord* coord)
    : WorldEditor(view, 0)
    , m_coord(coord)
{
}

CoordsEditor::~CoordsEditor()
{
}

bool CoordsEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 180);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Coordinates Editor");
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

int CoordsEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"Location", 0x50000000, 20, y + 12, 50, 18, 4);
    label(m_hWnd, L"x:", 0x50000001, 70, y + 12, 25, 18, 4);
    floatEdit(m_location[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_coord, 0x20));
    label(m_hWnd, L"y:", 0x50000001, 160, y + 12, 25, 18, 4);
    floatEdit(m_location[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_coord, 0x24));
    label(m_hWnd, L"z:", 0x50000001, 250, y + 12, 25, 18, 4);
    floatEdit(m_location[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_coord, 0x28));

    row = y + 34;
    label(m_hWnd, L"Rotation", 0x50000000, 20, y + 36, 50, 18, 4);
    label(m_hWnd, L"x:", 0x50000001, 70, y + 36, 25, 18, 4);
    floatEdit(m_rotation[0], m_hWnd, 0x78325F5F , 95, row, 60, 18, FIELD(float, m_coord, 0x30));
    label(m_hWnd, L"y:", 0x50000001, 160, y + 36, 25, 18, 4);
    floatEdit(m_rotation[1], m_hWnd, 0x79325F5F , 185, row, 60, 18, FIELD(float, m_coord, 0x34));
    label(m_hWnd, L"z:", 0x50000001, 250, y + 36, 25, 18, 4);
    floatEdit(m_rotation[2], m_hWnd, 0x7A325F5F , 275, row, 60, 18, FIELD(float, m_coord, 0x38));

    row = y + 58;
    label(m_hWnd, L"Unknown", 0x50000000, 20, y + 60, 50, 18, 4);
    label(m_hWnd, L"1:", 0x50000001, 70, y + 60, 25, 18, 4);
    floatEdit(m_unknown[0], m_hWnd, 0x78335F5F , 95, row, 60, 18, FIELD(float, m_coord, 0x40));
    label(m_hWnd, L"2:", 0x50000001, 160, y + 60, 25, 18, 4);
    floatEdit(m_unknown[1], m_hWnd, 0x79335F5F , 185, row, 60, 18, FIELD(float, m_coord, 0x44));
    label(m_hWnd, L"3:", 0x50000001, 250, y + 60, 25, 18, 4);
    floatEdit(m_unknown[2], m_hWnd, 0x7A335F5F , 275, row, 60, 18, FIELD(float, m_coord, 0x48));

    return y + 82;
}

void CoordsEditor::save()
{
    FIELD(float, m_coord, 0x20) = getFloat(m_location[0]);
    FIELD(float, m_coord, 0x24) = getFloat(m_location[1]);
    FIELD(float, m_coord, 0x28) = getFloat(m_location[2]);
    FIELD(float, m_coord, 0x30) = getFloat(m_rotation[0]);
    FIELD(float, m_coord, 0x34) = getFloat(m_rotation[1]);
    FIELD(float, m_coord, 0x38) = getFloat(m_rotation[2]);
    FIELD(float, m_coord, 0x40) = getFloat(m_unknown[0]);
    FIELD(float, m_coord, 0x44) = getFloat(m_unknown[1]);
    FIELD(float, m_coord, 0x48) = getFloat(m_unknown[2]);
}
