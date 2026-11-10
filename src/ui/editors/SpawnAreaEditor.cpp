#include "SpawnAreaEditor.h"
#include "kWorld_Spawn.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"

using namespace editctl_C1;

SpawnAreaEditor::SpawnAreaEditor(DocView* view, kWorld::Spawn* spawn)
    : WorldEditor(view, 0)
    , m_spawn(spawn)
{
}

SpawnAreaEditor::~SpawnAreaEditor()
{
}

bool SpawnAreaEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 370, 205);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Spawn Area Editor");
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

int SpawnAreaEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"Area", 0x50000000, 20, y + 12, 50, 18, 1);
    label(m_hWnd, L"x1:", 0x50000001, 70, y + 12, 25, 18, 4);
    floatEdit(m_point1[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_spawn, 0x18));
    label(m_hWnd, L"y1:", 0x50000001, 160, y + 12, 25, 18, 4);
    floatEdit(m_point1[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_spawn, 0x1C));
    label(m_hWnd, L"z1:", 0x50000001, 250, y + 12, 25, 18, 4);
    floatEdit(m_point1[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_spawn, 0x20));

    row += 24;
    label(m_hWnd, L"x2:", 0x50000001, 70, row + 2, 25, 18, 4);
    floatEdit(m_point2[0], m_hWnd, 0x78325F5F , 95, row, 60, 18, FIELD(float, m_spawn, 0x28));
    label(m_hWnd, L"y2:", 0x50000001, 160, row + 2, 25, 18, 4);
    floatEdit(m_point2[1], m_hWnd, 0x79325F5F , 185, row, 60, 18, FIELD(float, m_spawn, 0x2C));
    label(m_hWnd, L"z2:", 0x50000001, 250, row + 2, 25, 18, 4);
    floatEdit(m_point2[2], m_hWnd, 0x7A325F5F , 275, row, 60, 18, FIELD(float, m_spawn, 0x30));

    row += 24;
    label(m_hWnd, L"Faction", 0x50000000, 20, row + 3, 50, 18, 1);
    m_faction.createNumber(m_hWnd, (HMENU)0x636E7472 , 95, row, 60, 18, FIELD(int, m_spawn, 0x38), 0, 0);

    row += 24;
    label(m_hWnd, L"Unknown", 0x50000000, 20, row + 3, 50, 18, 4);
    label(m_hWnd, L"k1:", 0x50000001, 70, row + 2, 25, 18, 4);
    m_unknown[0].createNumber(m_hWnd, (HMENU)0x75315F5F , 95, row, 60, 18, FIELD(int, m_spawn, 0x10), 0, 0);
    label(m_hWnd, L"k2:", 0x50000001, 160, row + 2, 25, 18, 4);
    m_unknown[1].createNumber(m_hWnd, (HMENU)0x75325F5F , 185, row, 60, 18, FIELD(int, m_spawn, 0x34), 0, 0);
    label(m_hWnd, L"k3:", 0x50000001, 250, row + 2, 25, 18, 4);
    m_unknown[2].createNumber(m_hWnd, (HMENU)0x75335F5F , 275, row, 60, 18, FIELD(int, m_spawn, 0x3C), 0, 0);

    return row + 24;
}

int SpawnAreaEditor::mapCommandToResult(int id)
{
    if (id == 1)
    {
        int faction = m_faction.getInt(-1);
        if (faction != 1 && faction != 2)
        {
            AlertDial::alert(m_hWnd, L"The value of Faction must be 1 or 2", 0, 16);
            return -1;
        }
        save();
    }
    return id;
}

void SpawnAreaEditor::save()
{
    FIELD(float, m_spawn, 0x18) = getFloat(m_point1[0]);
    FIELD(float, m_spawn, 0x1C) = getFloat(m_point1[1]);
    FIELD(float, m_spawn, 0x20) = getFloat(m_point1[2]);
    FIELD(float, m_spawn, 0x28) = getFloat(m_point2[0]);
    FIELD(float, m_spawn, 0x2C) = getFloat(m_point2[1]);
    FIELD(float, m_spawn, 0x30) = getFloat(m_point2[2]);
    FIELD(int, m_spawn, 0x38) = m_faction.getInt(-1);
    FIELD(int, m_spawn, 0x10) = m_unknown[0].getInt(-1);
    FIELD(int, m_spawn, 0x34) = m_unknown[1].getInt(-1);
    FIELD(int, m_spawn, 0x3C) = m_unknown[2].getInt(-1);
}
