#include "ItemEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"
#include "String.hpp"
#include "ptrArray.h"
#include "Node.h"
#include "primitiveVector.h"
#include "Preferences.h"
#include "BasicEditor.h"
#include "AccessoryEditor.h"
#include "CapeEditor.h"
#include "CustomEditor.h"
#include "GearEditor.h"
#include "GoodsEditor.h"
#include "WeaponEditor.h"
#include "LapisEditor.h"
#include "LapisiaEditor.h"
#include "MountEditor.h"
#include "PetEditor.h"
#include "WingEditor.h"

primitiveVector<unsigned char> g_weaponItemTypes;
primitiveVector<unsigned char> g_gearItemTypes;
primitiveVector<unsigned char> g_accessoryItemTypes;
primitiveVector<unsigned char> g_goodsItemTypes;

ItemEditor::ItemEditor(Window* owner, Data::Items* items, Data::Item* item)
    : Modal()
    , m_owner(owner)
    , m_items(items)
    , m_item(item)
{
    m_flag4C4 = true;
}

ItemEditor::~ItemEditor()
{
}

void ItemEditor::reserved73()
{
    if (m_icon.m_hWnd)
        m_icon.m_dropDown.show(false);
}

bool openItemEditor(Window* owner, Data::Items* items, Data::Item* item)
{
    if (!g_weaponItemTypes.m_count)
    {
        static const unsigned char weapons[] = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 19, 34, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
            55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 69, 75, 84, 90 };
        for (unsigned i = 0; i < sizeof(weapons); ++i)
            g_weaponItemTypes.add(weapons[i]);
    }
    if (!g_gearItemTypes.m_count)
    {
        static const unsigned char gears[] = {
            16, 17, 18, 20, 21, 31, 32, 33, 35, 36, 67, 68, 70, 71, 72, 73, 74, 76, 77, 82, 83, 85, 86, 87, 88,
            89, 91, 92 };
        for (unsigned i = 0; i < sizeof(gears); ++i)
            g_gearItemTypes.add(gears[i]);
    }
    if (!g_accessoryItemTypes.m_count)
    {
        static const unsigned char accessories[] = { 22, 23, 37, 40, 96, 97 };
        for (unsigned i = 0; i < sizeof(accessories); ++i)
            g_accessoryItemTypes.add(accessories[i]);
    }
    if (!g_goodsItemTypes.m_count)
    {
        static const unsigned char goods[] = {
            25, 27, 28, 29, 38, 41, 43, 44, 78, 79, 80, 94, 99, 100, 101, 102, 103 };
        for (unsigned i = 0; i < sizeof(goods); ++i)
            g_goodsItemTypes.add(goods[i]);
    }

    ItemEditor* editor = 0;
    bool show3D = false;
    unsigned char type = FIELD(unsigned char, item, 0x54);

    int i = 0;
    for (; i < g_weaponItemTypes.m_count; ++i)
        if (g_weaponItemTypes.m_items[i] == type)
            break;
    if (i < g_weaponItemTypes.m_count)
    {
        editor = new WeaponEditor(owner, items, item);
        show3D = g_preferences.getInt(L"3D View", L"show weapon", 1) == 1;
    }
    else
    {
        int j = 0;
        for (; j < g_gearItemTypes.m_count; ++j)
            if (g_gearItemTypes.m_items[j] == type)
                break;
        if (j < g_gearItemTypes.m_count)
        {
            editor = new GearEditor(owner, items, item);
            show3D = g_preferences.getInt(L"3D View", L"show gear", 1) == 1;
        }
        else if (type == 24 || type == 39)
        {
            editor = new CapeEditor(owner, items, item);
            show3D = g_preferences.getInt(L"3D View", L"show mantle", 1) == 1;
        }
        else if (g_accessoryItemTypes.contains(type))
        {
            editor = new AccessoryEditor(owner, items, item);
        }
        else if (g_goodsItemTypes.contains(type))
        {
            editor = new GoodsEditor(owner, items, item);
        }
        else
        {
            switch (type)
            {
            case 30:
            case 98:
                editor = new LapisEditor(owner, items, item);
                break;
            case 42:
                editor = new MountEditor(owner, items, item);
                break;
            case 95:
                editor = new LapisiaEditor(owner, items, item);
                break;
            case 120:
                editor = new PetEditor(owner, items, item);
                break;
            case 121:
                editor = new WingEditor(owner, items, item);
                break;
            case 150:
                editor = new CustomEditor(owner, items, item);
                break;
            default:
                break;
            }
        }
    }

    bool result = false;
    if (editor)
    {
        if (editor->createEditor())
        {
            if (show3D)
                SendMessageW(editor->m_hWnd, WM_COMMAND, 200, 0);
            result = editor->doModal() == 1;
        }
        delete editor;
    }
    else
    {
        BasicEditor basic(items, item, 0);
        if (basic.create(owner->m_hWnd))
            result = basic.doModal() == 1;
    }
    return result;
}

void ItemEditor::refreshModelsImpl()
{
    SendMessageW(m_model.m_hWnd, CB_RESETCONTENT, 0, 0);
    m_model.m_items.deleteAll();
    ptrArray<Node*>* models = reinterpret_cast<ptrArray<Node*>*>(
        g4_clientData()->getCharModels(m_classMask, FIELD(unsigned char, m_item, 0x54), true, 0));
    if (models)
    {
        int count = models->size();
        String text;
        for (int i = 0; i < count; ++i)
        {
            text.clear();
            Node* model = models->m_items[i];
            text << L' ' << (long)FIELD(long, model, 0x0C) << " - " << FIELD(String, model, 0x18);
            m_model.addItem(text.m_buffer, FIELD(long, model, 0x0C));
        }
        SetWindowPos(m_model.m_hWnd, 0, 0, 0, 180, 400, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
