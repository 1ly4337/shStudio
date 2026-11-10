#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "ptrArray.h"
#include "Node.h"

class MonsterEditor;
class Monster;

class BagsList : public ListCtrlOD
{
public:
    class Item : public Node
    {
    public:
        Item(int grade, float rate) : m_grade(grade), m_rate(rate) {}
        virtual ~Item() {}

        int getInt(DWORD tag) const override;
        bool setInt(DWORD tag, int value) override;
        void reset() override;
        int compare(const Node* other, DWORD tag) const override { return 0; }

        int   m_grade;
        float m_rate;

    };

    class Items : public ptrArray<Item*>
    {
    public:
        Items();
        virtual ~Items();

    };

    BagsList(MonsterEditor* editor, Monster* monster);
    virtual ~BagsList();

    DWORD getStyle() override;
    bool onCreate() override;
    Node* getNode(int index) override;
    void onItemChanged(int index, UINT newState) override;
    bool moveItemUp(int index) override;
    bool moveItemDown(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override { return 0; }
    virtual int getListKind();

    void setSelectedBag(int grade, float rate);

public:
    MonsterEditor* m_editor;
    Items          m_items;
};
static_assert(sizeof(BagsList) == 0x110, "BagsList size");
