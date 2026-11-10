#include "CoordsEditor2.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

CoordsEditor2::CoordsEditor2(DocView* view, kWorld::Element::Coord* coord)
    : CoordsEditor(view, coord)
{
}

bool CoordsEditor2::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 200);
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

int CoordsEditor2::createControls(int y)
{
    int row = CoordsEditor::createControls(y);
    label(m_hWnd, L"Index", 0x50000000, 20, row + 2, 50, 18, 4);
    m_index.createNumber(m_hWnd, (HMENU)0x6B315F5F , 95, row, 60, 18, FIELD(int, m_coord, 0x14), 0, 0);
    return row + 24;
}

void CoordsEditor2::save()
{
    CoordsEditor::save();
    FIELD(int, m_coord, 0x14) = m_index.getInt(-1);
}
