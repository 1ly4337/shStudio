#pragma once
#include "SpotDetails.h"
#include "SpotsList.h"
#include "ListCtrlOD.h"
#include "MonsterArea.h"
#include "EditText.h"
#include "Button.h"

class Monsters;

class MonsterAreaDetails : public SpotDetails {
public:
    class MonsterAreaList : public SpotsList<MonsterArea> {
    public:
        MonsterAreaList(SpotDetails* owner, Collection<MonsterArea>* items) : SpotsList<MonsterArea>(owner, items) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        bool  removeItem(int index) override;
        int   indexOfNode(Node* node) override;
    };

    class MonstersList : public ListCtrlOD {
    public:
        explicit MonstersList(MonsterAreaDetails* owner) : ListCtrlOD(), m_owner(owner), m_ids(0) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        void  onChanged() override;
        Node* getNode(int index) override;
        bool  onActivateItem(int index, int keys) override;
        void  onItemChanged(int index, UINT newState) override;
        HRESULT onGetDispInfo(NMLVDISPINFOW* di) override;
        bool  removeItem(int index) override;
        int   indexOfNode(Node* node) override;
        Node* findNode(Node* like) override { return 0; }
        virtual int getListKind();
        void  setIds(MonsterArea::MonsterIDs* ids);

        MonsterAreaDetails*       m_owner;
        MonsterArea::MonsterIDs*  m_ids;
    };

    MonsterAreaDetails(MapSpots* spots, Collection<MonsterArea>* areas, Monsters* monsters);
    ~MonsterAreaDetails() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

    Collection<MonsterArea>* m_areas;
    Monsters*                m_monsters;
    MonsterAreaList          m_areaList;
    MonstersList             m_monstersList;
    EditText                 m_coords[6];
    Button                   m_btnSet;
    Button                   m_btnAdd;
    EditText                 m_editId;
    EditText                 m_editCount;
    Button                   m_btnSetId;
    Button                   m_btnAddId;
};
static_assert(sizeof(MonsterAreaDetails::MonstersList) == 0xFC, "MonstersList size");
static_assert(sizeof(MonsterAreaDetails) == 0x300, "MonsterAreaDetails size");

bool editMonster(Node* monster, void* models, HWND parent, bool extended);
RECT* monsterAreaRect(RECT* rc, MapSpots* spots, MonsterArea* area);
