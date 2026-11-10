#pragma once
#include "SpotDetails.h"
#include "SpotsList.h"
#include "Ladder.h"
#include "EditText.h"
#include "Button.h"

class LaddersDetails : public SpotDetails {
public:
    class LadderList : public SpotsList<Ladder> {
    public:
        LadderList(SpotDetails* owner, Collection<Ladder>* items) : SpotsList<Ladder>(owner, items) {}
        DWORD getStyle() override;
        bool  onCreate() override;
        int   indexOfNode(Node* node) override;
    };

    LaddersDetails(MapSpots* spots, Collection<Ladder>* ladders);
    ~LaddersDetails() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

    Collection<Ladder>* m_ladders;
    LadderList          m_list;
    EditText            m_x;
    EditText            m_y;
    EditText            m_z;
    Button              m_btnSet;
    Button              m_btnAdd;
};
static_assert(sizeof(LaddersDetails) == 0x1AC, "LaddersDetails size");
