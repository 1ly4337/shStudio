#pragma once
#include <windows.h>
#include "Modal.h"
#include "ListCtrl.h"
#include "ListCtrlOD.h"
#include "ptrArray.h"

class Drawable;
class Node;

class View3DInfo : public Modal
{
public:
    class Coord;

    class Coords : public ptrArray<Coord*>
    {
    public:
        Coords() : ptrArray<Coord*>(256) {}
    };

    class CoordsList : public ListCtrlOD
    {
    public:
        CoordsList() {}
        virtual ~CoordsList();

        DWORD getStyle() override { return 0x10000000; }
        Node* getNode(int index) override { return (Node*)m_coords.get(index); }
        int indexOfNode(Node* node) override;
        Node* findNode(Node* like) override { return 0; }

        Coords m_coords;
    };

    class ItemsList : public ListCtrl
    {
    public:
        ItemsList() {}
    };

    explicit View3DInfo(Drawable* model);
    virtual ~View3DInfo();

    bool onCreate() override;
    virtual bool vslot78(HWND parent);

public:
    CoordsList m_coordsList;
    ItemsList  m_itemsList;
    Drawable*  m_model;
};
static_assert(sizeof(View3DInfo) == 0x2B0, "View3DInfo size");

#include "View3DInfo_Coord.h"
