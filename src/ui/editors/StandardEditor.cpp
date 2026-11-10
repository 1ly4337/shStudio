#include "StandardEditor.h"
#include "MerchantEditor.h"
#include "GateKeeperEditor.h"
#include "StandardNPC.h"
#include "Merchant.h"
#include "GateKeeper.h"
#include "AlertDial.h"
#include "gdi_cache.h"
#include "Window.h"
#include "ida_compat.h"

StandardEditor::StandardEditor(StandardNPC* npc, bool canChangeFamily)
    : NpcEditor(npc)
{
    m_title = L"Standard NPC";
    m_canChangeFamily = canChangeFamily;
    m_tallWelcome = true;
    m_height = 330;
}

StandardEditor::~StandardEditor()
{
}

void StandardEditor::createExtraControls()
{
    HWND label = CreateWindowExW(0, L"Static", L"Family:", WS_CHILD | WS_VISIBLE, 220, 53, 40, 18,
                                 m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    m_family.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_family.m_dropLines & 3) | 0x50010000,
                                      260, 48, 110, 250, m_hWnd, (HMENU)0x78, 0, 0);
    if (m_family.m_hWnd)
    {
        m_family.setFontId(m_family.m_fontId);
        SendMessageW(m_family.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        m_family.m_flags = 1;
    }
    else
    {
        GetLastError();
    }
    m_family.addItem(L"Animal", 9);
    m_family.addItem(L"Apprentice", 10);
    m_family.addItem(L"Blacksmith", 3);
    m_family.addItem(L"Combat Commander", 13);
    m_family.addItem(L"Dead NPC", 12);
    m_family.addItem(L"Gambling House", 5);
    m_family.addItem(L"Guard", 8);
    m_family.addItem(L"Guild Master", 11);
    m_family.addItem(L"Normal NPC", 7);
    m_family.addItem(L"PvP Manager", 4);
    m_family.addItem(L"Warehouse", 6);
    SetWindowPos(m_family.m_hWnd, 0, 0, 0, 110, 250, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    m_family.selectData(FIELD(int, m_npc, 0x0C));
    if (!m_canChangeFamily)
        m_family.enable(false);
}

bool StandardEditor::applyExtra()
{
    LRESULT family;
    if (m_family.m_hWnd)
    {
        LRESULT selection = SendMessageW(m_family.m_hWnd, CB_GETCURSEL, 0, 0);
        family = SendMessageW(m_family.m_hWnd, CB_GETITEMDATA, selection, 0);
        if (family == -1)
        {
            AlertDial::alert(m_hWnd,
                             L"You must define the NPC's family.\nNote that the family cannot be changed once defined.",
                             0, MB_ICONERROR);
            return false;
        }
    }
    else
    {
        family = 0;
    }
    FIELD(int, m_npc, 0x0C) = (int)family;
    return true;
}

bool __fastcall editStandardNpc(Window* owner, StandardNPC* npc)
{
    int type = FIELD(int, npc, 0x0C);
    if (type == 1)
    {
        Merchant* merchant = dynamic_cast<Merchant*>(npc);
        if (merchant)
            return MerchantEditor::edit(owner, merchant);
    }
    else if (type == 2)
    {
        GateKeeper* gateKeeper = dynamic_cast<GateKeeper*>(npc);
        if (gateKeeper)
            return GateKeeperEditor::edit(owner->m_hWnd, gateKeeper);
    }
    else
    {
        StandardEditor editor(npc, false);
        if (editor.create(owner->m_hWnd))
            return editor.doModal() == IDOK;
    }
    return false;
}
