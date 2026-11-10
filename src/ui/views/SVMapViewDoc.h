#pragma once
#include <windows.h>
#include "DocView.h"
#include "SVMap.h"
#include "MapSpots.h"
#include "MapViewTab.h"

class SVMapViewDoc : public DocView
{
public:
    explicit SVMapViewDoc(FileTree* data);
    virtual ~SVMapViewDoc();

    LPCWSTR getClassName() override { return L"SvMapView"; }
    DWORD getStyle() override { return 0x86CF0000; }
    void onRegisterClass(WNDCLASSEXW* wc) override;
    int getMenuId() override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void setModified(bool modified) override;
    SIZE* getDefaultSize(SIZE* out, SIZE def) override;
    HRESULT store() override;
    virtual void vslot100() {}

public:
    bool       m_bigMap;
    WORD       m_unusedAA;
    SVMap      m_map;
    DWORD      m_unused194;
    DWORD      m_unused198;
    HBITMAP    m_minimap;
    HBITMAP    m_minimapBig;
    MapSpots   m_spots;
    MapViewTab m_tab;
    SIZE       m_initialSize;
};
static_assert(sizeof(SVMapViewDoc) == 0x2F0, "SVMapViewDoc size");

extern const wchar_t* g_svMapTabGeneral;
extern const wchar_t* g_svMapTabLadders;
extern const wchar_t* g_svMapTabMonsterAreas;
extern const wchar_t* g_svMapTabMonsters;
extern const wchar_t* g_svMapTabNamedAreas;
extern const wchar_t* g_svMapTabNpc;
extern const wchar_t* g_svMapTabPortals;
extern const wchar_t* g_svMapTabSpawns;
extern const wchar_t* g_svMapTabSafeAreas;
