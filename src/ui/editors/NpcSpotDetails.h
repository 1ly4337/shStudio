#pragma once
#include "SpotDetails.h"
#include "SpotsList.h"
#include "ListCtrlOD.h"
#include "NpcLocation.h"
#include "NpcPerso.h"
#include "EditText.h"
#include "Button.h"

class NpcSpotDetails : public SpotDetails {
public:
    class NpcSpotList : public SpotsList<NpcLocation> {
    public:
        NpcSpotList(SpotDetails* owner, Collection<NpcLocation>* items)
            : SpotsList<NpcLocation>(owner, items), m_selected(0) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        bool  onActivateItem(int index, int keys) override;
        void  onItemChanged(int index, UINT newState) override;
        HRESULT onGetDispInfo(NMLVDISPINFOW* di) override;
        int   indexOfNode(Node* node) override;

        NpcLocation* m_selected;
    };

    class NpcLocaList : public ListCtrlOD {
    public:
        explicit NpcLocaList(NpcSpotDetails* owner) : ListCtrlOD(), m_owner(owner), m_locations(0) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        void  onChanged() override;
        Node* getNode(int index) override;
        void  onItemChanged(int index, UINT newState) override;
        bool  removeItem(int index) override;
        int   indexOfNode(Node* node) override;
        Node* findNode(Node* like) override { return 0; }
        void  setLocations(NpcLocation::Locations* locations);

        NpcSpotDetails*         m_owner;
        NpcLocation::Locations* m_locations;
    };

    NpcSpotDetails(MapSpots* spots, Collection<NpcLocation>* npcs, NpcPerso* perso);
    ~NpcSpotDetails() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

    Collection<NpcLocation>* m_npcs;
    NpcPerso*                m_perso;
    NpcSpotList              m_list;
    EditText                 m_editType;
    EditText                 m_editTypeId;
    Button                   m_btnSet;
    Button                   m_btnAdd;
    NpcLocaList              m_locaList;
    EditText                 m_locEdits[4];
    Button                   m_btnLocSet;
    Button                   m_btnLocAdd;
};
void highlightNpcLocations(NpcLocation* npc, bool on);

static_assert(sizeof(NpcSpotDetails::NpcSpotList) == 0x104, "NpcSpotList size");
static_assert(sizeof(NpcSpotDetails::NpcLocaList) == 0xFC, "NpcLocaList size");
static_assert(sizeof(NpcSpotDetails) == 0x2EC, "NpcSpotDetails size");
