#include "WldPortalEditor.h"
#include "kWorld_Portal.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

WldPortalEditor::WldPortalEditor(DocView* view, kWorld::Portal* portal)
    : WorldEditor(view, portal)
    , m_portal(portal)
{
}

WldPortalEditor::~WldPortalEditor()
{
}

bool WldPortalEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 350);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Portal Editor");
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

int WldPortalEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"Portal", 0x50000000, 20, y + 12, 50, 18, 1);
    label(m_hWnd, L"x1:", 0x50000001, 70, y + 12, 25, 18, 4);
    floatEdit(m_point1[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_portal, 0x54));
    label(m_hWnd, L"y1:", 0x50000001, 160, y + 12, 25, 18, 4);
    floatEdit(m_point1[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_portal, 0x58));
    label(m_hWnd, L"z1:", 0x50000001, 250, y + 12, 25, 18, 4);
    floatEdit(m_point1[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_portal, 0x5C));

    row = y + 34;
    label(m_hWnd, L"x2:", 0x50000001, 70, y + 36, 25, 18, 4);
    floatEdit(m_point2[0], m_hWnd, 0x78325F5F , 95, row, 60, 18, FIELD(float, m_portal, 0x64));
    label(m_hWnd, L"y2:", 0x50000001, 160, y + 36, 25, 18, 4);
    floatEdit(m_point2[1], m_hWnd, 0x79325F5F , 185, row, 60, 18, FIELD(float, m_portal, 0x68));
    label(m_hWnd, L"z2:", 0x50000001, 250, y + 36, 25, 18, 4);
    floatEdit(m_point2[2], m_hWnd, 0x7A325F5F , 275, row, 60, 18, FIELD(float, m_portal, 0x6C));

    row = y + 58;
    label(m_hWnd, L"Target", 0x50000000, 20, y + 60, 50, 18, 1);
    label(m_hWnd, L"map:", 0x50000001, 70, y + 60, 25, 18, 4);
    m_map.createNumber(m_hWnd, (HMENU)0x6D61705F , 95, row, 60, 18, FIELD(unsigned char, m_portal, 0x74), 0, 0);
    label(m_hWnd, L"fac:", 0x50000001, 160, y + 60, 25, 18, 4);
    m_faction.createNumber(m_hWnd, (HMENU)0x6661635F , 185, row, 60, 18, FIELD(short, m_portal, 0x76), 0, 0);
    label(m_hWnd, L"unk:", 0x50000001, 250, y + 60, 25, 18, 4);
    m_unknown.createNumber(m_hWnd, (HMENU)0x756E6B5F , 275, row, 60, 18, FIELD(unsigned char, m_portal, 0x78), 0, 0);

    row = y + 82;
    label(m_hWnd, L"Location", 0x50000000, 20, y + 84, 50, 18, 4);
    label(m_hWnd, L"x:", 0x50000001, 70, y + 84, 25, 18, 4);
    floatEdit(m_location[0], m_hWnd, 0x78335F5F , 95, row, 60, 18, FIELD(float, m_portal, 0x80));
    label(m_hWnd, L"y:", 0x50000001, 160, y + 84, 25, 18, 4);
    floatEdit(m_location[1], m_hWnd, 0x79335F5F , 185, row, 60, 18, FIELD(float, m_portal, 0x84));
    label(m_hWnd, L"z:", 0x50000001, 250, y + 84, 25, 18, 4);
    floatEdit(m_location[2], m_hWnd, 0x7A335F5F , 275, row, 60, 18, FIELD(float, m_portal, 0x88));

    label(m_hWnd, L"Label 1", 0x50000000, 20, y + 110, 200, 18, 4);
    textEdit(m_caption1Edit, m_hWnd, 0x73315F5F , 20, y + 128, 315, 50, 0x50211044, 0x100);
    label(m_hWnd, L"Label 2", 0x50000000, 20, y + 182, 200, 18, 4);
    textEdit(m_caption2Edit, m_hWnd, 0x73325F5F , 20, y + 200, 315, 50, 0x50211044, 0x100);

    return y + 254;
}

void WldPortalEditor::save()
{
    FIELD(float, m_portal, 0x54) = getFloat(m_point1[0]);
    FIELD(float, m_portal, 0x64) = getFloat(m_point2[0]);
    FIELD(float, m_portal, 0x80) = getFloat(m_location[0]);
    FIELD(float, m_portal, 0x58) = getFloat(m_point1[1]);
    FIELD(float, m_portal, 0x68) = getFloat(m_point2[1]);
    FIELD(float, m_portal, 0x84) = getFloat(m_location[1]);
    FIELD(float, m_portal, 0x5C) = getFloat(m_point1[2]);
    FIELD(float, m_portal, 0x6C) = getFloat(m_point2[2]);
    FIELD(float, m_portal, 0x88) = getFloat(m_location[2]);
    FIELD(unsigned char, m_portal, 0x74) = (unsigned char)m_map.getInt(-1);
    FIELD(short, m_portal, 0x76) = (short)m_faction.getInt(-1);
    FIELD(unsigned char, m_portal, 0x78) = (unsigned char)m_unknown.getInt(-1);
    WorldEditor::save();
}
