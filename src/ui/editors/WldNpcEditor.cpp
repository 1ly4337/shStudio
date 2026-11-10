#include "WldNpcEditor.h"
#include "PointEditor.h"
#include "kWorld_NPC.h"
#include "fPoint.h"
#include "ptrArray.h"
#include "editctl_C1.h"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <cstdio>
#include <cstring>

using namespace editctl_C1;

static inline ptrArray<fPoint*>& npcPositions(kWorld::NPC* npc)
{
    return FIELD(ptrArray<fPoint*>, npc, 0x2C);
}

WldNpcEditor::WldNpcEditor(DocView* view, kWorld::NPC* npc)
    : WorldEditor(view, 0)
    , m_npc(npc)
{
}

WldNpcEditor::~WldNpcEditor()
{
}

bool WldNpcEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 365, 320);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"NPC Editor");
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

int WldNpcEditor::createControls(int y)
{
    int row = y + 10;
    label(m_hWnd, L"NPC Type", 0x50000000, 20, row + 2, 60, 18, 4);
    m_type.createNumber(m_hWnd, (HMENU)0x78315F5F , 95, row, 60, 18, FIELD(int, m_npc, 0x10), 0, 0);
    label(m_hWnd, L"id:", 0x50000000, 165, row + 2, 25, 18, 4);
    m_id.createNumber(m_hWnd, (HMENU)0x79315F5F , 185, row, 60, 18, FIELD(int, m_npc, 0x14), 0, 0);

    row += 24;
    label(m_hWnd, L"Position", 0x50000000, 20, row + 2, 50, 18, 4);
    label(m_hWnd, L"x:", 0x50000000, 75, row + 2, 25, 18, 4);
    floatEdit(m_position[0], m_hWnd, 0x78315F5F , 95, row, 60, 18, FIELD(float, m_npc, 0x1C));
    label(m_hWnd, L"y:", 0x50000000, 165, row + 2, 25, 18, 4);
    floatEdit(m_position[1], m_hWnd, 0x79315F5F , 185, row, 60, 18, FIELD(float, m_npc, 0x20));
    label(m_hWnd, L"z:", 0x50000000, 255, row + 2, 25, 18, 4);
    floatEdit(m_position[2], m_hWnd, 0x7A315F5F , 275, row, 60, 18, FIELD(float, m_npc, 0x24));

    row += 24;
    label(m_hWnd, L"Orientation", 0x50000000, 20, row + 2, 65, 18, 4);
    floatEdit(m_orientation, m_hWnd, 0x6F725F5F , 95, row, 60, 18, FIELD(float, m_npc, 0x28));

    row += 32;
    label(m_hWnd, L"Additional Positions", 0x50000000, 20, row, 200, 18, 4);

    row += 18;
    m_positions.create(m_hWnd, (HMENU)0x616C7470 , 20, row, 205, 113, 8, 0x20000, 0);
    m_positions.addColumn(L"X", 60, LCF_FLOAT, 0x706F7358 );
    m_positions.addColumn(L"y", 60, LCF_FLOAT, 0x706F7359 );
    m_positions.addColumn(L"z", 60, LCF_FLOAT, 0x706F735A );
    button(m_add, m_hWnd, L"Add", 0x78, 250, row + 25, 60, 20);
    button(m_del, m_hWnd, L"Del", 0x79, 250, row + 50, 60, 20);

    ptrArray<fPoint*>& positions = npcPositions(m_npc);
    for (int i = 0; i < positions.size(); ++i)
    {
        fPoint* p = positions.get(i);
        wchar_t buffer[16] = { 0 };
        swprintf_s(buffer, 16, L"%.3f", (double)p->x);
        m_positions.insertTextItem(buffer, -1, -1);

        swprintf_s(buffer, 16, L"%.3f", (double)p->y);
        LVITEMW item;
        memset(&item.iItem, 0, 0x38);
        item.pszText = buffer;
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 1;
        SendMessageW(m_positions.m_hWnd, LVM_SETITEMTEXTW, i, (LPARAM)&item);

        swprintf_s(buffer, 16, L"%.3f", (double)p->z);
        memset(&item.iItem, 0, 0x38);
        item.pszText = buffer;
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 2;
        SendMessageW(m_positions.m_hWnd, LVM_SETITEMTEXTW, i, (LPARAM)&item);
    }
    return row + 116;
}

void WldNpcEditor::save()
{
    FIELD(int, m_npc, 0x10) = m_type.getInt(-1);
    FIELD(int, m_npc, 0x14) = m_id.getInt(-1);
    FIELD(float, m_npc, 0x1C) = getFloat(m_position[0]);
    FIELD(float, m_npc, 0x20) = getFloat(m_position[1]);
    FIELD(float, m_npc, 0x24) = getFloat(m_position[2]);
    FIELD(float, m_npc, 0x28) = getFloat(m_orientation);
}

bool WldNpcEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 0x78)
    {
        int index = m_positions.getNextSelected(0);
        fPoint point;
        PointEditor editor(&point);
        if (editor.create(m_hWnd) && editor.doModal() == 1)
        {
            wchar_t buffer[16] = { 0 };
            swprintf_s(buffer, 16, L"%.3f", (double)point.x);
            int row = m_positions.insertTextItem(buffer, -1, index);
            swprintf_s(buffer, 16, L"%.3f", (double)point.y);
            m_positions.ListCtrl::setCellText(row, 1, buffer);
            swprintf_s(buffer, 16, L"%.3f", (double)point.z);
            m_positions.ListCtrl::setCellText(row, 2, buffer);
            fPoint* copy = new fPoint(point);
            npcPositions(m_npc).insert(copy, index);
        }
        return true;
    }
    if (id != 0x79)
        return Modal::onCommand(code, ctrl, id);

    int index = m_positions.getNextSelected(0);
    if (index < 0 || !npcPositions(m_npc).remove(index))
        return true;
    m_positions.deleteItem(index);
    return true;
}

bool WldNpcEditor::onNotify(NMHDR* hdr, LRESULT* result)
{
    if (hdr->code != (UINT)LVN_ITEMACTIVATE)
        return false;
    int index = ((NMITEMACTIVATE*)hdr)->iItem;
    fPoint* p = npcPositions(m_npc).get(index);
    if (p)
    {
        PointEditor editor(p);
        if (editor.create(m_hWnd) && editor.doModal() == 1)
        {
            wchar_t buffer[16] = { 0 };
            swprintf_s(buffer, 16, L"%.3f", (double)p->x);
            m_positions.ListCtrl::setCellText(index, 0, buffer);
            swprintf_s(buffer, 16, L"%.3f", (double)p->y);
            m_positions.ListCtrl::setCellText(index, 1, buffer);
            swprintf_s(buffer, 16, L"%.3f", (double)p->z);
            m_positions.ListCtrl::setCellText(index, 2, buffer);
        }
    }
    return true;
}
