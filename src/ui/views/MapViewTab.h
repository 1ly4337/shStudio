#pragma once
#include "TabCtrl.h"
#include "SpotDetails.h"

class MapSpots;

class MapViewTab : public TabCtrl<SpotDetails*>
{
public:
    explicit MapViewTab(MapSpots* spots) : m_spots(spots) {}

    void onPageChanged(int index) override;

public:
    MapSpots* m_spots;
};
static_assert(sizeof(MapViewTab) == 0x3C, "MapViewTab size");
