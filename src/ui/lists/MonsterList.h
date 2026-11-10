#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "MonstersSet.h"

class MonsterView;
class Monsters;
class Node;

class MonsterList : public ListCtrlOD
{
public:
    explicit MonsterList(MonsterView* view) : m_view(view)
    {
        m_monsters = 0;
        m_showAll = true;
        m_allColumns = false;
    }
    virtual ~MonsterList();

    DWORD getStyle() override { return 9; }
    bool onCreate() override;
    bool onCommand(int code, LPARAM ctrl, int id) override;
    void onChanged() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int listV69() override { return 'name'; }
    int getHeaderMenu() override;
    void onContextMenu(HMENU menu) override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    void onItemChanged(int index, UINT newState) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override { return 522; }

    void clearRows();
    void setupColumns();
    void addStatColumns();
    void addResistColumns();
    void addAttackColumns();
    void refresh();

    MonsterView* m_view;
    Monsters*    m_monsters;
    MonstersSet  m_items;
    bool         m_showAll;
    bool         m_allColumns;
    int          m_columnGroups;
};
static_assert(sizeof(MonsterList) == 0x11C, "MonsterList size");
