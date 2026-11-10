#pragma once
#include "SpotDetails.h"
#include "SpotsList.h"
#include "NamedArea.h"
#include "EditText.h"
#include "Button.h"

class NamedAreaDetails : public SpotDetails {
public:
    class NamedAreaList : public SpotsList<NamedArea> {
    public:
        NamedAreaList(SpotDetails* owner, Collection<NamedArea>* items) : SpotsList<NamedArea>(owner, items) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        int   indexOfNode(Node* node) override;
    };

    NamedAreaDetails(MapSpots* spots, Collection<NamedArea>* areas);
    ~NamedAreaDetails() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

    Collection<NamedArea>* m_areas;
    NamedAreaList          m_list;
    EditText               m_coords[6];
    EditText               m_param1;
    EditText               m_param2;
    Button                 m_btnSet;
    Button                 m_btnAdd;
};
static_assert(sizeof(NamedAreaDetails) == 0x1E8, "NamedAreaDetails size");
