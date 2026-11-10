#include "GeneralInfo.h"
#include "SVMap.h"
#include "MapSpots.h"
#include "SVMapViewDoc.h"
#include "Collection.hpp"
#include "ida_compat.h"
#include "_imports_p06.h"

static inline void setCounter(HWND hwnd, UINT value)
{
    LONG id = GetWindowLongW(hwnd, GWL_ID);
    if (id > 0)
        SetDlgItemInt(GetParent(hwnd), id, value, TRUE);
}

static inline HWND createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy, int id)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent,
                                (HMENU)(INT_PTR)id, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return hwnd;
}

GeneralInfo::GeneralInfo(MapSpots* owner, SVMap* map)
    : SpotDetails(owner)
{
    m_map = map;
}

void GeneralInfo::showSpots()
{
    UINT ladders = FIELD(Collection<Ladder>, m_map, 0x50).size();
    setCounter(m_ladders, ladders);
    UINT monsterAreas = FIELD(Collection<MonsterArea>, m_map, 0x68).size();
    setCounter(m_monsterAreas, monsterAreas);
    UINT namedAreas = FIELD(Collection<NamedArea>, m_map, 0xC8).size();
    setCounter(m_namedAreas, namedAreas);
    UINT npcs = FIELD(Collection<NpcLocation>, m_map, 0x80).size();
    setCounter(m_npcs, npcs);
    UINT spawns = FIELD(Collection<Spawn>, m_map, 0xB0).size();
    setCounter(m_spawns, spawns);
}

bool GeneralInfo::onCreate()
{
    HWND parent = m_hWnd;
    createLabel(parent, L"Map Size:", 30, 47, 60, 18, 0);
    m_mapSize.createNumber(m_hWnd, (HMENU)101, 90, 45, 60, 18, FIELD(UINT, m_map, 0x18), 0, 0);

    m_change.m_hWnd = CreateWindowExW(0, L"Button", L"Change", WS_CHILD | WS_VISIBLE, 160, 44, 50, 20,
                                      m_hWnd, (HMENU)0x66, 0, 0);
    if (m_change.m_hWnd)
        m_change.setFontId(m_change.m_fontId);
    else
        GetLastError();

    createLabel(m_hWnd, L"Count of ladders:", 30, 81, 140, 18, 0);
    m_ladders = createLabel(m_hWnd, L"", 180, 81, 50, 18, 0x6E);
    createLabel(m_hWnd, L"Count of monsters areas:", 30, 101, 140, 18, 0);
    m_monsterAreas = createLabel(m_hWnd, L"", 180, 101, 50, 18, 0x6F);
    createLabel(m_hWnd, L"Count of named areas:", 30, 121, 140, 18, 0);
    m_namedAreas = createLabel(m_hWnd, L"", 180, 121, 50, 18, 0x70);
    createLabel(m_hWnd, L"Count of NPCs:", 30, 141, 140, 18, 0);
    m_npcs = createLabel(m_hWnd, L"", 180, 141, 50, 18, 0x71);
    createLabel(m_hWnd, L"Count of spawn points:", 30, 161, 140, 18, 0);
    m_spawns = createLabel(m_hWnd, L"", 180, 161, 50, 18, 0x72);
    return true;
}

bool GeneralInfo::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code || id != 102)
        return SpotDetails::onCommand(code, ctrl, id);
    int size = m_mapSize.getInt(-1);
    if (size > 0) {
        m_map->setMapSize(size);
        FIELD(SVMapViewDoc*, m_spots, 0x6C)->setModified(true);
    }
    return true;
}
