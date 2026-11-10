#pragma once
#include <windows.h>
#include "Modal.h"
#include "IconPicker.h"
#include "ComboBox.h"
#include "EditText.h"
#include "Button.h"

namespace Data { class Item; class Items; }

class ItemEditor : public Modal
{
public:
    ItemEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~ItemEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual void reserved73();
    virtual int mapCommandToResult(int result);

    virtual bool createEditor() = 0;
    virtual void loadControls();
    virtual void saveControls();
    virtual void refreshModels() {}

    void createNameRow(int x , int y , int width);
    int  createDescriptionRow(int x , int y, int width, int height);
    int  createEquipRow(int x , int y, bool a4, bool a5, bool a6);
    int  createFactionRow(int x, int y, bool a4, bool a5);
    LRESULT createColorRow(int x, int y, int labelWidth, int comboWidth);
    int  createLevelSlotRow(int x , int y, Window* parent);
    int  createDefenseRow(int x, int y, Window* parent, WPARAM elementSel);
    int  createStatsRows(int x, int y, Window* parent);
    int  createHpSpMpRow(int x, int y, Window* parent);
    int  createStatCountRow(int x, int y, Window* parent, int a5);
    int  createDropStackRow(int x , int y, Window* parent);
    int  createPriceRow(int x, int y, Window* parent, int a5);

    void refreshModelsImpl();

public:
    Window*      m_owner;
    Data::Items* m_items;
    Data::Item*  m_item;
    IconPicker   m_icon;
    ComboBox     m_model;
    EditText     m_name;
    EditText     m_description;
    EditText     m_level;
    ComboBox     m_faction;
    Button       m_classes[6];
    ComboBox     m_mode;
    ComboBox     m_color;
    EditText     m_max;
    EditText     m_enchantItem;
    Button       m_button324;
    ComboBox     m_restriction;
    ComboBox     m_element;
    ComboBox     m_sex;
    ComboBox     m_slots;
    EditText     m_endurance;
    EditText     m_attackMin;
    EditText     m_attackMax;
    EditText     m_defense;
    EditText     m_resist;
    EditText     m_hp;
    EditText     m_mp;
    EditText     m_sp;
    EditText     m_str;
    EditText     m_dex;
    EditText     m_rec;
    EditText     m_int;
    EditText     m_wis;
    EditText     m_luc;
    EditText     m_absorb;
    EditText     m_buy;
    EditText     m_sell;
    EditText     m_dropGroup;
    EditText     m_statCount;
    EditText     m_stack;
    int          m_classMask;
    bool         m_flag4C4;
};
static_assert(sizeof(ItemEditor) == 0x4C8, "ItemEditor size");

bool openItemEditor(Window* owner, Data::Items* items, Data::Item* item);
