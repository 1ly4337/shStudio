#include "MerchantEditor.h"
#include <commctrl.h>
#include "Merchant.h"
#include "StudioFrame.h"
#include "ClientData.h"
#include "Application.h"
#include "Data_Item.h"
#include "primitiveArray.h"
#include "String.hpp"
#include "_imports_D.h"

int g_merchantTypeIds[18] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 16, 100, 101, 117, 200};

static inline void setStaticFont(HWND label, unsigned int font)
{
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(font), 0);
}

bool MerchantEditor::fillItemTypes(ComboBox* combo, char merchantType, char faction)
{
    SendMessageW(combo->m_hWnd, CB_RESETCONTENT, 0, 0);
    combo->m_items.StringTable::deleteAll();
    switch (merchantType)
    {
    case 0:
        combo->ComboBox::addItem(L"Good Items 25 (type 25)", 25);
        combo->ComboBox::addItem(L"Good Items 44 (type 44)", 44);
        combo->ComboBox::addItem(L"Good Items 78 (type 78)", 78);
        combo->ComboBox::addItem(L"Good Items 79 (type 79)", 79);
        combo->ComboBox::addItem(L"Good Items 80 (type 80)", 80);
        combo->ComboBox::addItem(L"Good Items 100 (type 100)", 100);
        combo->ComboBox::addItem(L"Good Items 101 (type 101)", 101);
        combo->ComboBox::addItem(L"Good Items 102 (type 102)", 102);
        combo->ComboBox::addItem(L"Good Items 103 (type 103)", 103);
        combo->ComboBox::addItem(L"Quest Items 1 (type 27)", 27);
        combo->ComboBox::addItem(L"Quest Items 2 (type 28)", 28);
        combo->ComboBox::addItem(L"Quest Items 3 (type 29)", 29);
        combo->ComboBox::addItem(L"Quest Items 4 (type 99)", 99);
        combo->ComboBox::selectData(25);
        return true;
    case 3:
        combo->ComboBox::addItem(L"Jewelry", 0);
        combo->ComboBox::addItem(L"Amulets", 23);
        combo->ComboBox::addItem(L"Loops", 40);
        combo->ComboBox::addItem(L"Rings", 22);
        combo->ComboBox::addItem(L"Lapis", 30);
        combo->ComboBox::selectData(0);
        return true;
    case 1:
        if (faction)
        {
            if (faction == 1)
            {
                combo->ComboBox::addItem(L"1-Hand  Swords", 1);
                combo->ComboBox::addItem(L"2-Hands Swords", 2);
                combo->ComboBox::addItem(L"1-Hand  Blunts", 7);
                combo->ComboBox::addItem(L"2-Hands Blunts", 8);
                combo->ComboBox::addItem(L"Dual Swords", 5);
                combo->ComboBox::addItem(L"Spears", 6);
                combo->ComboBox::addItem(L"Reverse Swords", 9);
                combo->ComboBox::addItem(L"Daggers", 10);
                combo->ComboBox::addItem(L"Bows", 13);
                combo->ComboBox::addItem(L"Cross-Bows", 14);
                combo->ComboBox::addItem(L"Staffs", 12);
                combo->ComboBox::addItem(L"Knuckles", 15);
                combo->ComboBox::selectData(1);
                return true;
            }
            if (faction == 2)
            {
                combo->ComboBox::addItem(L"1-Hand  Axes", 3);
                combo->ComboBox::addItem(L"2-Hands Axes", 4);
                combo->ComboBox::addItem(L"1-Hand  Blunts", 7);
                combo->ComboBox::addItem(L"2-Hands Blunts", 8);
                combo->ComboBox::addItem(L"Dual Axes", 5);
                combo->ComboBox::addItem(L"Spears", 6);
                combo->ComboBox::addItem(L"Reverse Swords", 9);
                combo->ComboBox::addItem(L"Daggers", 10);
                combo->ComboBox::addItem(L"Bows", 13);
                combo->ComboBox::addItem(L"Javelins", 11);
                combo->ComboBox::addItem(L"Staffs", 12);
                combo->ComboBox::addItem(L"Knuckles", 15);
                combo->ComboBox::selectData(3);
                return true;
            }
            return true;
        }
        break;
    case 2:
        if (faction)
        {
            combo->ComboBox::addItem(L"All Gears", 0);
            if (faction == 1)
            {
                combo->ComboBox::addItem(L"Helmets", 16);
                combo->ComboBox::addItem(L"Armors", 17);
                combo->ComboBox::addItem(L"Pants", 18);
                combo->ComboBox::addItem(L"Boots", 21);
                combo->ComboBox::addItem(L"Gantlets", 20);
                combo->ComboBox::addItem(L"Shields", 19);
            }
            else if (faction == 2)
            {
                combo->ComboBox::addItem(L"Caps", 31);
                combo->ComboBox::addItem(L"Protectors", 32);
                combo->ComboBox::addItem(L"Legging", 33);
                combo->ComboBox::addItem(L"Walkers", 36);
                combo->ComboBox::addItem(L"Mufflers", 35);
                combo->ComboBox::addItem(L"Shields", 34);
            }
            combo->ComboBox::selectData(0);
            return true;
        }
        break;
    case 6:
        if (faction)
        {
            combo->ComboBox::addItem(L"All Mounts", 42);
            combo->ComboBox::selectData(42);
        }
        else
        {
            combo->ComboBox::addItem(L"Cannot sell to both factions", 0);
            combo->ComboBox::selectData(0);
        }
        return true;
    case 7:
        if (!faction)
            break;
        if (faction == 1)
        {
            combo->ComboBox::addItem(L"Capes", 24);
            combo->ComboBox::selectData(24);
        }
        else
        {
            combo->ComboBox::addItem(L"Mantles", 39);
            combo->ComboBox::selectData(39);
        }
        return true;
    default:
        combo->ComboBox::addItem(L"All", 0);
        combo->ComboBox::selectData(0);
        return true;
    }
    combo->ComboBox::addItem(L"Cannot sell to both factions", 0);
    combo->ComboBox::selectData(0);
    return false;
}

MerchantEditor::MerchantEditor(Window* owner, Merchant* merchant)
    : Modal(),
      m_merchant(merchant),
      m_icon(),
      m_name(),
      m_welcome(),
      m_sellTo(),
      m_type(),
      m_sold(owner, merchant),
      m_itemType(),
      m_itemClass(),
      m_stock(owner)
{
}

MerchantEditor::~MerchantEditor()
{
}

bool MerchantEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 720, 550);
    bool ok = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Merchant Editor");
    if (ok)
    {
        m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(m_hWnd, m_owner);
        show(true);
        return true;
    }
    return ok;
}

void MerchantEditor::reserved73()
{
    m_icon.m_dropDown.show(false);
}

bool MerchantEditor::onCreate()
{
    ClientData& clientData = g_studioFrame->m_client.m_clientData;
    ImageStrip* icons32 = clientData.getIcons(L"npc_face32");
    ImageStrip* icons64 = clientData.getIcons(L"npc_face64");
    if (icons32 && icons64)
    {
        m_icon.m_bitmaps = *icons64;
        m_icon.m_cells = *icons32;
        m_icon.create(m_hWnd, (HMENU)101, 0, 22, 60, 60, 64, 0);
        m_icon.select(m_merchant->m_shape);
    }

    setStaticFont(CreateWindowExW(0, L"Static", L"Name:", 0x50000000, 120, 22, 35, 16, m_hWnd, 0, 0, 0), 4);
    m_name.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 160, 20, 190, 18, m_hWnd,
                                    (HMENU)0x66, 0, 0);
    if (m_name.m_hWnd)
    {
        m_name.setFontId(m_name.m_fontId);
        SetWindowSubclass(m_name.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_name);
    }
    SendMessageW(m_name.m_hWnd, EM_LIMITTEXT, 50, 0);

    setStaticFont(CreateWindowExW(0, L"Static", L"Type:", 0x50000000, 120, 49, 35, 18, m_hWnd, 0, 0, 0), 4);
    m_type.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_type.m_dropLines & 3) | 0x50010000, 160, 44, 190, 300,
                                    m_hWnd, (HMENU)0x6A, 0, 0);
    if (m_type.m_hWnd)
    {
        m_type.setFontId(m_type.m_fontId);
        SendMessageW(m_type.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        m_type.m_flags = 1;
    }
    else
    {
        GetLastError();
    }
    for (int i = 0; i < 18; ++i)
        m_type.ComboBox::addItem(g_merchantTypeNames[i], g_merchantTypeIds[i]);
    m_type.ComboBox::selectData(m_merchant->m_prof);
    SetWindowPos(m_type.m_hWnd, 0, 0, 0, 190, 300, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

    setStaticFont(CreateWindowExW(0, L"Static", L"Sell to:", 0x50000000, 120, 76, 35, 18, m_hWnd, 0, 0, 0), 4);
    m_sellTo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_sellTo.m_dropLines & 3) | 0x50010000,
                                      160, 72, 80, 200, m_hWnd, (HMENU)0x67, 0, 0);
    if (m_sellTo.m_hWnd)
        m_sellTo.setFontId(m_sellTo.m_fontId);
    else
        GetLastError();
    if (SendMessageW(m_sellTo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Both") != -1)
        m_sellTo.m_items.StringTable::put((int)SendMessageW(m_sellTo.m_hWnd, CB_GETCOUNT, 0, 0), L"Both");
    if (SendMessageW(m_sellTo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Light") != -1)
        m_sellTo.m_items.StringTable::put((int)SendMessageW(m_sellTo.m_hWnd, CB_GETCOUNT, 0, 0), L"Light");
    if (SendMessageW(m_sellTo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Dark") != -1)
        m_sellTo.m_items.StringTable::put((int)SendMessageW(m_sellTo.m_hWnd, CB_GETCOUNT, 0, 0), L"Dark");
    SendMessageW(m_sellTo.m_hWnd, CB_SETCURSEL, m_merchant->m_country, 0);

    setStaticFont(CreateWindowExW(0, L"Static", L"Welcome Msg:", 0x50000000, 20, 100, 100, 16, m_hWnd, 0, 0, 0), 4);
    m_welcome.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50211044, 20, 116, 330, 66, m_hWnd,
                                       (HMENU)0x69, 0, 0);
    if (m_welcome.m_hWnd)
    {
        m_welcome.setFontId(m_welcome.m_fontId);
        SetWindowSubclass(m_welcome.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_welcome);
    }
    SendMessageW(m_welcome.m_hWnd, EM_LIMITTEXT, 0x200, 0);

    setStaticFont(CreateWindowExW(0, L"Static", L"Sold Items:", 0x50000000, 20, 191, 100, 14, m_hWnd, 0, 0, 0), 5);
    m_sold.Control::create(m_hWnd, (HMENU)0x6E, 20, 207, 330, 248, 0x10200000, 0, 0);
    m_sold.load(&m_merchant->m_items);
    HWND info = CreateWindowExW(0, L"Static", 0, 0x50000001, 20, 468, 330, 24, m_hWnd, (HMENU)0x6F, 0, 0);
    setStaticFont(info, 4);
    m_soldInfo = info;
    HWND button = CreateWindowExW(0, L"Button", L"<<", 0x50000000, 355, 308, 40, 24, m_hWnd, (HMENU)0x73, 0, 0);
    setStaticFont(button, 0);
    m_addButton = button;

    setStaticFont(CreateWindowExW(0, L"Static", L"Available Items", 0x50000000, 400, 18, 100, 14, m_hWnd, 0, 0, 0), 5);
    setStaticFont(CreateWindowExW(0, L"Static", L"Type", 0x50000000, 400, 46, 40, 14, m_hWnd, 0, 0, 0), 4);
    m_itemType.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_itemType.m_dropLines & 3) | 0x50010000, 440, 42, 200,
                                        300, m_hWnd, (HMENU)0x78, 0, 0);
    if (m_itemType.m_hWnd)
    {
        m_itemType.setFontId(m_itemType.m_fontId);
        SendMessageW(m_itemType.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        m_itemType.m_flags = 1;
    }
    else
    {
        GetLastError();
    }
    setStaticFont(CreateWindowExW(0, L"Static", L"Class", 0x50000000, 400, 76, 40, 14, m_hWnd, 0, 0, 0), 4);
    m_itemClass.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_itemClass.m_dropLines & 3) | 0x50010000,
                                         440, 72, 200, 300, m_hWnd, (HMENU)0x79, 0, 0);
    if (m_itemClass.m_hWnd)
        m_itemClass.setFontId(m_itemClass.m_fontId);
    else
        GetLastError();
    m_stock.Control::create(m_hWnd, (HMENU)0x7A, 400, 105, 290, 350, 0x10200000, 0, 0);
    info = CreateWindowExW(0, L"Static", 0, 0x50000001, 400, 465, 300, 24, m_hWnd, (HMENU)0x7B, 0, 0);
    setStaticFont(info, 4);
    m_stockInfo = info;

    updateStock(true);
    SetWindowPos(m_itemType.m_hWnd, 0, 0, 0, 200, 300, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    m_name.setText(m_merchant->m_name.c_str());
    m_welcome.setText(m_merchant->m_welcome.c_str());
    return Modal::onCreate();
}

bool MerchantEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == 0)
    {
        if (id == 115)
        {
            if (m_stock.getCount(1) < 1)
                return true;
            int count = m_stock.getCount(0);
            for (int i = 0; i < count; ++i)
            {
                if (SendMessageW(m_stock.m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED)
                    m_sold.m_items.add(static_cast<Data::Item*>(m_stock.getItemParam(i)));
            }
            m_sold.setItemCount(m_sold.m_items.size());
            return true;
        }
        return Modal::onCommand(code, ctrl, id);
    }
    if (code != 1)
        return Modal::onCommand(code, ctrl, id);
    if (id == 120 || id == 121)
    {
        updateStock(false);
    }
    else if (id == 103 || id == 106)
    {
        updateStock(true);
        return true;
    }
    return true;
}

bool MerchantEditor::updateStock(bool refillFilters)
{
    char merchantType;
    if (m_type.m_hWnd)
        merchantType = (char)SendMessageW(m_type.m_hWnd, CB_GETITEMDATA, SendMessageW(m_type.m_hWnd, CB_GETCURSEL, 0, 0), 0);
    else
        merchantType = 0;
    char faction = (char)SendMessageW(m_sellTo.m_hWnd, CB_GETCURSEL, 0, 0);
    bool ok = true;
    SendMessageW(m_stock.m_hWnd, LVM_DELETEALLITEMS, 0, 0);
    if (refillFilters)
    {
        ok = fillItemTypes(&m_itemType, merchantType, faction);
        SendMessageW(m_itemClass.m_hWnd, CB_RESETCONTENT, 0, 0);
        m_itemClass.m_items.StringTable::deleteAll();
        if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"All") != -1)
            m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"All");
        if (merchantType == 2 || merchantType == 1)
        {
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Regular") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Regular");
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Noble") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Noble");
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Worship") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Worship");
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Heroic") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Heroic");
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Dread") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Dread");
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Legendary") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Legendary");
            if (SendMessageW(m_itemClass.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Goddess") != -1)
                m_itemClass.m_items.StringTable::put((int)SendMessageW(m_itemClass.m_hWnd, CB_GETCOUNT, 0, 0), L"Goddess");
        }
        SendMessageW(m_itemClass.m_hWnd, CB_SETCURSEL, 0, 0);
    }

    int itemType;
    switch (merchantType)
    {
    case 0:   itemType = 10; break;
    case 1:   itemType = 1; break;
    case 2:   itemType = 2; break;
    case 3:   itemType = 3; break;
    case 4:   itemType = 4; break;
    case 6:   itemType = 5; break;
    case 7:   itemType = 6; break;
    case 10:  itemType = 7; break;
    case 11:  itemType = 9; break;
    case 101: itemType = 8; break;
    default:
        return false;
    }
    if (ok)
    {
        unsigned char subType;
        if (m_itemType.m_hWnd)
            subType = (unsigned char)SendMessageW(m_itemType.m_hWnd, CB_GETITEMDATA,
                                                  SendMessageW(m_itemType.m_hWnd, CB_GETCURSEL, 0, 0), 0);
        else
            subType = 0;
        LRESULT itemClass = SendMessageW(m_itemClass.m_hWnd, CB_GETCURSEL, 0, 0);
        if (!m_stock.load(itemType, subType, faction == 1, (int)itemClass - 1))
        {
            m_stock.enable(false);
            m_sold.enable(false);
            EnableWindow(m_addButton, FALSE);
        }
    }
    return true;
}

int MerchantEditor::mapCommandToResult(int id)
{
    if (id == 1)
    {
        m_merchant->m_type = 1;
        char merchantType;
        if (m_type.m_hWnd)
            merchantType = (char)SendMessageW(m_type.m_hWnd, CB_GETITEMDATA, SendMessageW(m_type.m_hWnd, CB_GETCURSEL, 0, 0), 0);
        else
            merchantType = 0;
        m_merchant->m_prof = (uint8_t)merchantType;
        if (m_icon.m_hWnd)
        {
            int index = m_icon.m_selected;
            int modelId;
            if (index < 0 || index >= FIELD(int, &m_icon.m_bitmaps, 0x10) )
                modelId = 0;
            else
                modelId = FIELD(int*, &m_icon.m_bitmaps, 0x08) [index];
            m_merchant->m_shape = modelId;
        }
        m_merchant->m_country = (int)SendMessageW(m_sellTo.m_hWnd, CB_GETCURSEL, 0, 0);
        {
            String text;
            m_merchant->m_name = *m_name.getText(&text);
        }
        {
            String text;
            m_merchant->m_welcome = *m_welcome.getText(&text);
        }
        if (m_sold.isEnabled())
            m_sold.save(&m_merchant->m_items);
    }
    return id;
}

bool MerchantEditor::edit(Window* owner, Merchant* merchant)
{
    if (!g_studioFrame)
        return false;
    SetCursor(hCursor);
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    if (GetCursorPos(&pt))
        SetCursorPos(pt.x + 1, pt.y + 1);
    ClientData& clientData = g_studioFrame->m_client.m_clientData;
    clientData.loadSData(&clientData.m_quests, L"Npc\\NpcQuest.sdata", 'file', 1);
    clientData.loadSData(&clientData.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
    SetCursor(g_arrowCursor);
    MerchantEditor editor(owner, merchant);
    bool ok;
    if (editor.create(owner->m_hWnd))
        ok = editor.doModal() == 1;
    else
        ok = false;
    return ok;
}
