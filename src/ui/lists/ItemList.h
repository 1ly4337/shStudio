#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "ItemsSet.h"

class ItemView;
class ComboBox;
class String;
class Node;
namespace Data { class Item; class Items; }

extern const wchar_t* g_itemColumnNames[];
extern DWORD g_itemColumnTags[];

class ItemList : public ListCtrlOD
{
public:
    ItemList(ItemView* view, Data::Items* items);
    virtual ~ItemList();

    BOOL show(bool on) override;
    DWORD getStyle() override;
    bool onCommand(int code, LPARAM ctrl, int id) override;
    bool onInitMenuPopup(HMENU menu) override;
    void onChanged() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    const wchar_t* getItemText(Node* node, int columnId) override;
    int listV69() override;
    void onContextMenu(HMENU menu) override;
    DWORD getExStyle() override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    void onItemChanged(int index, UINT newState) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;
    bool confirm(const wchar_t* verb, int count) override;

    virtual Data::Item* getItem(int index);
    virtual void exportText(String& out, DWORD fmt);
    virtual void fillTypeCombo(ComboBox* combo) {}
    virtual void loadItems(Data::Items* items) = 0;
    virtual void onFilterCommand(Data::Items* items, int id);
    virtual void removeExtraColumns();
    virtual void rebuildColumns();
    virtual void toggleAllColumns();
    virtual void addClassColumns();
    virtual void addAttackColumns();
    virtual void addStatColumns();
    virtual void addRequirementColumns();
    virtual void addMiscColumns();
    virtual void addPriceColumns();
    virtual void editItem(Node* node) {}

    int addTypeItem(ComboBox* combo, int type, const wchar_t* name);

public:
    ItemView*      m_owner;
    Data::Items*   m_items;
    Data::ItemsSet m_set;
    int            m_baseColumns;
    int            m_faction;
    int            m_typeFilter;
    int            m_subFilter;
    bool           m_allColumns;
    bool           m_showEmpty;
    unsigned       m_columnGroups;
    bool           m_needsFill;
};
static_assert(sizeof(ItemList) == 0x140, "ItemList size");
