#pragma once
#include <windows.h>
#include "Container.h"
#include "Scaler.h"
#include "BmpCtrl.h"
#include "primitiveArray.h"

class Spot;
class Document;

class MapSpots : public Container, public Scaler
{
public:
    MapSpots(Document* document, void* arg3, TabCtrl<SpotDetails*>* tabs);
    virtual ~MapSpots();

    bool onCreate() override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    POINT* toPoint(POINT* out, float x, float z) override;

    HBITMAP renderBitmap();
    RECT* getPointRect(RECT* out, float x, float z);
    HWND findSpotWnd(Spot* spot);
    void destroySpots();
    bool removeSpot(Spot* spot);
    BOOL redrawSpot(HWND hWnd, int unused);

    static LRESULT CALLBACK spotWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static HWND createSpotWindow(HWND parent, HMENU id, const RECT* rc, void* spot);

public:
    void*                m_74;
    BmpCtrl              m_picture;
    primitiveArray<HWND> m_spotWnds;
    int                  m_height;
    float                m_scale;
};
static_assert(sizeof(MapSpots) == 0x108, "MapSpots size");
