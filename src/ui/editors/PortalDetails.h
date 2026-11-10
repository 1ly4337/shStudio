#pragma once
#include "SpotDetails.h"
#include "SpotsList.h"
#include "Portal.h"
#include "EditText.h"
#include "Button.h"

class PortalDetails : public SpotDetails {
public:
    class PortalList : public SpotsList<Portal> {
    public:
        PortalList(SpotDetails* owner, Collection<Portal>* items) : SpotsList<Portal>(owner, items) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        int   indexOfNode(Node* node) override;
    };

    PortalDetails(MapSpots* spots, Collection<Portal>* portals);
    ~PortalDetails() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

    Collection<Portal>* m_portals;
    PortalList          m_list;
    EditText            m_faction;
    EditText            m_levelMin;
    EditText            m_levelMax;
    EditText            m_destMap;
    EditText            m_pos[3];
    EditText            m_dest[3];
    Button              m_btnSet;
    Button              m_btnAdd;
};
static_assert(sizeof(PortalDetails) == 0x200, "PortalDetails size");
