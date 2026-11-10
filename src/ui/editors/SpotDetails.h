#pragma once
#include <windows.h>
#include "Container.h"

class MapSpots;
class Spot;

class SpotDetails : public Container
{
public:
    explicit SpotDetails(MapSpots* spots);

    virtual void selectSpot(Spot* spot) = 0;
    virtual void showSpots() = 0;
    virtual void displaySpot(Spot* spot) = 0;

public:
    MapSpots* m_spots;
};
static_assert(sizeof(SpotDetails) == 0x6C, "SpotDetails size");
