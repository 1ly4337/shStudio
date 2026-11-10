#pragma once
#include <windows.h>
#include "SpotDetails.h"
#include "SpotsList.h"
#include "TacticsZones.h"
#include "TacticsZone.h"
#include "EditText.h"
#include "Button.h"
#include "BmpCtrl.h"

class Spot;
class Node;

class SafeAreasDetails : public SpotDetails
{
public:
    class SafeAreasList : public SpotsList<TacticsZone>
    {
    public:
        SafeAreasList(SpotDetails* owner, TacticsZones* zones) : SpotsList<TacticsZone>(owner, zones) {}

        bool onCreate() override;
        void onChanged() override;
        bool removeItem(int index) override;
        int indexOfNode(Node* node) override;
    };

    SafeAreasDetails(MapSpots* spots, TacticsZones* allZones, int zoneType);
    virtual ~SafeAreasDetails();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

public:
    TacticsZones* m_allZones;
    TacticsZones  m_zones;
    int           m_zoneType;
    SafeAreasList m_list;
    EditText      m_xEdits[4];
    EditText      m_zEdits[4];
    EditText      m_prmEdits[2];
    Button        m_setButton;
    Button        m_addButton;
    BmpCtrl       m_picture;
};
static_assert(sizeof(SafeAreasDetails) == 0x2B4, "SafeAreasDetails size");
