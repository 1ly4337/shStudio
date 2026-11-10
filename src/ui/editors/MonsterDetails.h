#pragma once
#include "SpotDetails.h"
#include "ListCtrlOD.h"
#include "FlatButton.h"
#include "MonsterSpot.h"

class Monsters;

class MonsterDetails : public SpotDetails {
public:
    class MonstersList : public ListCtrlOD {
    public:
        explicit MonstersList(MonsterDetails* owner) : ListCtrlOD(), m_owner(owner) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        void  onChanged() override;
        Node* getNode(int index) override;
        bool  sort() override;
        bool  onActivateItem(int index, int keys) override;
        void  onItemChanged(int index, UINT newState) override;
        int   indexOfNode(Node* node) override;
        Node* findNode(Node* like) override { return 0; }
        virtual int getListKind();

        MonsterDetails* m_owner;
    };

    MonsterDetails(MapSpots* spots, Collection<MonsterArea>* areas, Monsters* monsters);
    ~MonsterDetails() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot*) override {}
    void showSpots() override {}
    void displaySpot(Spot*) override {}

    Collection<MonsterArea>* m_areas;
    Monsters*                m_monsters;
    MonsterSpots             m_monsterSpots;
    MonstersList             m_list;
    FlatButton               m_filters[5];
};
static_assert(sizeof(MonsterDetails::MonstersList) == 0xF8, "MonsterDetails::MonstersList size");
static_assert(sizeof(MonsterDetails) == 0x238, "MonsterDetails size");
