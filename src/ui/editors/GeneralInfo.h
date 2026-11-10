#pragma once
#include <windows.h>
#include "SpotDetails.h"
#include "EditText.h"
#include "Button.h"

class MapSpots;
class SVMap;

class GeneralInfo : public SpotDetails
{
public:
    GeneralInfo(MapSpots* owner, SVMap* map);

    virtual ~GeneralInfo() {}

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override {}
    void showSpots() override;
    void displaySpot(Spot* spot) override {}

public:
    SVMap*   m_map;
    EditText m_mapSize;
    Button   m_change;
    HWND     m_ladders;
    HWND     m_monsterAreas;
    HWND     m_namedAreas;
    HWND     m_npcs;
    HWND     m_unused98;
    HWND     m_spawns;
};
static_assert(sizeof(GeneralInfo) == 0xA0, "GeneralInfo size");
