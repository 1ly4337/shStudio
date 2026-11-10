#pragma once
#include <windows.h>
#include "SpotDetails.h"
#include "SpotsList.h"
#include "Collection.hpp"
#include "Spawn.h"
#include "EditText.h"
#include "Button.h"

class Spot;
class Node;

class SpawnAreaDetails : public SpotDetails
{
public:
    class SpawnAreaList : public SpotsList<Spawn>
    {
    public:
        SpawnAreaList(SpotDetails* owner, Collection<Spawn>* spawns) : SpotsList<Spawn>(owner, spawns) {}

        bool onCreate() override;
        int indexOfNode(Node* node) override;
    };

    SpawnAreaDetails(MapSpots* spots, Collection<Spawn>* spawns);
    virtual ~SpawnAreaDetails();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void selectSpot(Spot* spot) override;
    void showSpots() override;
    void displaySpot(Spot* spot) override;

public:
    Collection<Spawn>* m_spawns;
    SpawnAreaList m_list;
    EditText      m_x1;
    EditText      m_y1;
    EditText      m_z1;
    EditText      m_x2;
    EditText      m_y2;
    EditText      m_z2;
    EditText      m_faction;
    EditText      m_param1;
    EditText      m_param2;
    Button        m_setButton;
    Button        m_addButton;
};
static_assert(sizeof(SpawnAreaDetails) == 0x1F4, "SpawnAreaDetails size");
