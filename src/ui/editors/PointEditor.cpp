#include "PointEditor.h"
#include "fPoint.h"
#include "editctl_C1.h"
#include "_imports_p06.h"

using namespace editctl_C1;

PointEditor::PointEditor(fPoint* point)
    : Modal()
    , m_point(point)
{
}

bool PointEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 310, 120);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Point Editor");
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

bool PointEditor::onCreate()
{
    label(m_hWnd, L"x:", 0x50000000, 25, 22, 25, 18, 4);
    floatEdit(m_x, m_hWnd, 0x785F5F5F , 45, 20, 60, 18, m_point->x);
    label(m_hWnd, L"y:", 0x50000000, 115, 22, 25, 18, 4);
    floatEdit(m_y, m_hWnd, 0x795F5F5F , 135, 20, 60, 18, m_point->y);
    label(m_hWnd, L"z:", 0x50000000, 205, 22, 25, 18, 4);
    floatEdit(m_z, m_hWnd, 0x7A5F5F5F , 225, 20, 60, 18, m_point->z);
    return Modal::onCreate();
}

int PointEditor::mapCommandToResult(int id)
{
    if (id == 1)
    {
        m_point->x = getFloat(m_x);
        m_point->y = getFloat(m_y);
        m_point->z = getFloat(m_z);
    }
    return id;
}
