#include "VerticeEditor.h"
#include "kWorld_Vertice.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

VerticeEditor::VerticeEditor(DocView* view, kWorld::Vertice* vertice)
    : CoordsEditor(view, &m_localCoord)
    , m_localCoord(1)
    , m_vertice(vertice)
{
}

VerticeEditor::~VerticeEditor()
{
}

bool VerticeEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 200);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Effect Location Editor");
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

int VerticeEditor::createControls(int y)
{
    FIELD(DWORD, &m_localCoord, 0x20) = FIELD(DWORD, m_vertice, 0x14);
    FIELD(DWORD, &m_localCoord, 0x24) = FIELD(DWORD, m_vertice, 0x18);
    FIELD(DWORD, &m_localCoord, 0x28) = FIELD(DWORD, m_vertice, 0x1C);
    FIELD(DWORD, &m_localCoord, 0x30) = FIELD(DWORD, m_vertice, 0x24);
    FIELD(DWORD, &m_localCoord, 0x34) = FIELD(DWORD, m_vertice, 0x28);
    FIELD(DWORD, &m_localCoord, 0x38) = FIELD(DWORD, m_vertice, 0x2C);
    FIELD(DWORD, &m_localCoord, 0x40) = FIELD(DWORD, m_vertice, 0x34);
    FIELD(DWORD, &m_localCoord, 0x44) = FIELD(DWORD, m_vertice, 0x38);
    FIELD(DWORD, &m_localCoord, 0x48) = FIELD(DWORD, m_vertice, 0x3C);

    int row = CoordsEditor::createControls(y);
    label(m_hWnd, L"Effect:", 0x50000000, 20, row + 2, 65, 18, 4);
    m_effect.createNumber(m_hWnd, (HMENU)0x6B5F5F5F , 95, row, 60, 18, FIELD(int, m_vertice, 0x40), 0, 0);
    return row + 24;
}

void VerticeEditor::save()
{
    CoordsEditor::save();
    FIELD(DWORD, m_vertice, 0x14) = FIELD(DWORD, m_coord, 0x20);
    FIELD(DWORD, m_vertice, 0x18) = FIELD(DWORD, m_coord, 0x24);
    FIELD(DWORD, m_vertice, 0x1C) = FIELD(DWORD, m_coord, 0x28);
    FIELD(DWORD, m_vertice, 0x24) = FIELD(DWORD, m_coord, 0x30);
    FIELD(DWORD, m_vertice, 0x28) = FIELD(DWORD, m_coord, 0x34);
    FIELD(DWORD, m_vertice, 0x2C) = FIELD(DWORD, m_coord, 0x38);
    FIELD(DWORD, m_vertice, 0x34) = FIELD(DWORD, m_coord, 0x40);
    FIELD(DWORD, m_vertice, 0x38) = FIELD(DWORD, m_coord, 0x44);
    FIELD(DWORD, m_vertice, 0x3C) = FIELD(DWORD, m_coord, 0x48);
    FIELD(int, m_vertice, 0x40) = m_effect.getInt(-1);
}
