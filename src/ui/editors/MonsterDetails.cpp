#include "SpotPage_p05.h"
#include "MonsterDetails.h"
#include "MonsterAreaDetails.h"
#include "Monsters.h"
#include "StudioFrame.h"
#include "gdiutil.h"
#include "Document.h"
#include "Collection.hpp"

static void addAreaWindow(MapSpots* spots, MonsterArea* area, HMENU id)
{
    RECT rc;
    monsterAreaRect(&rc, spots, area);
    HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, id, &rc, area);
    spots->m_spotWnds.add(wnd);
}

DWORD MonsterDetails::MonstersList::getStyle()
{
    return WS_VISIBLE;
}

int MonsterDetails::MonstersList::getListKind()
{
    return 13;
}

bool MonsterDetails::MonstersList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"MobID", 50, 18, 'mbid');
        addColumn(L"Level", 40, 18, 'lvl ');
        addColumn(L"Elem", 50, 2, 'attr');
        addColumn(L"Name", 170, 4096, 'name');
        return true;
    }
    return result;
}

void MonsterDetails::MonstersList::onChanged()
{
    m_owner->m_spots->m_document->setModified(true);
}

Node* MonsterDetails::MonstersList::getNode(int index)
{
    return m_owner->m_monsterSpots.get(index);
}

int MonsterDetails::MonstersList::indexOfNode(Node* node)
{
    return m_owner->m_monsterSpots.indexOf(dynamic_cast<MonsterSpot*>(node));
}

bool MonsterDetails::MonstersList::sort()
{
    int column = m_prevSortColumn;
    MonsterSpots* spots = &m_owner->m_monsterSpots;
    bool ascending = m_sortAscending;
    int count = spots->m_count;
    int key = m_sortColumn;
    if (count < 2)
        return false;
    spots->m_changes = 0;
    quickSort(*spots, 0, count - 1, key, column, ascending);
    return spots->m_changes > 0;
}

void MonsterDetails::MonstersList::onItemChanged(int index, UINT )
{
    m_owner->m_spots->destroySpots();
    MonsterSpot* spot = m_owner->m_monsterSpots.get(index);
    if (spot) {
        Collection<MonsterArea>* areas = &spot->m_areas;
        int count = areas->size();
        for (int i = 0; i < count; ++i)
            addAreaWindow(m_owner->m_spots, areas->get(i), (HMENU)(UINT_PTR)(i + 1001));
    }
}

bool MonsterDetails::MonstersList::onActivateItem(int index, int )
{
    MonsterSpot* spot = m_owner->m_monsterSpots.get(index);
    if (spot) {
        HWND parent = m_hWnd;
        void* models = g_studioFrame->m_client.m_clientData.getMonsterModels(L"monsters");
        editMonster(spot->m_monster, models, parent, m_owner->m_monsters->m_extended);
    }
    return false;
}

MonsterDetails::MonsterDetails(MapSpots* spots, Collection<MonsterArea>* areas, Monsters* monsters)
    : SpotDetails(spots), m_areas(areas), m_monsters(monsters), m_monsterSpots(), m_list(this), m_filters()
{
}

MonsterDetails::~MonsterDetails()
{
}

bool MonsterDetails::onCreate()
{
    int areaCount = m_areas->size();
    for (int a = 0; a < areaCount; ++a) {
        MonsterArea* area = m_areas->get(a);
        if (!area)
            continue;
        MonsterArea::MonsterIDs* ids = &area->m_ids;
        int idCount = ids->size();
        for (int k = 0; k < idCount; ++k) {
            MonsterArea::MonsterID* mob = ids->get(k);
            if (!mob)
                continue;
            MonsterSpot* spot = 0;
            int spotCount = m_monsterSpots.m_count;
            for (int j = 0; j < spotCount; ++j) {
                MonsterSpot* s = m_monsterSpots.m_items[j];
                if (FIELD(int, s->m_monster, 20) == mob->m_mobId) {
                    spot = s;
                    break;
                }
            }
            if (!spot) {
                ptrArray<Node*>* monsters = (ptrArray<Node*>*)m_monsters;
                int count = monsters->m_count;
                int m = 0;
                if (count <= 0)
                    continue;
                while (FIELD(int, monsters->m_items[m], 20) != mob->m_mobId) {
                    if (++m >= count)
                        break;
                }
                if (m >= count)
                    continue;
                Node* monster = monsters->m_items[m];
                if (!monster)
                    continue;
                spot = new MonsterSpot(monster);
                m_monsterSpots.add(spot);
                if (!spot)
                    continue;
            }
            spot->m_areas.add(area);
        }
    }
    RECT client;
    getClientRect(&client);
    int listHeight = client.bottom - client.top - 56;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, listHeight, 0x10800000, 0, 0);
    m_list.setItemCount(m_monsterSpots.size());
    int y = listHeight + 27;
    HANDLE strip = LoadImageW(GetModuleHandleW(0), (LPCWSTR)0xAA, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (strip) {
        int k = 0;
        for (int x = 30; x < 150; x += 24, ++k) {
            HBITMAP cell = cropBitmap(strip, 16 * k, 0, 16, 32);
            HWND parent = m_hWnd;
            if (cell) {
                FlatButton& btn = m_filters[k];
                btn.m_bmpNormal = cropBitmap(cell, 0, 0, 16, 16);
                BITMAP bm;
                GetObjectW(cell, sizeof(bm), &bm);
                if (bm.bmWidth >= 32)
                    btn.m_bmpHover = cropBitmap(cell, 16, 0, 16, 16);
                else if (bm.bmHeight >= 32)
                    btn.m_bmpHover = cropBitmap(cell, 0, 16, 16, 16);
                btn.m_width = 16;
                btn.m_height = 16;
                HWND h = CreateWindowExW(0, L"Button", 0, 0x5000000B, x, y, 16, 16, parent,
                                         (HMENU)(UINT_PTR)(k + 121), 0, 0);
                btn.m_hWnd = h;
                if (h)
                    btn.setFontId(btn.m_fontId);
                else
                    GetLastError();
                if (btn.m_hWnd)
                    SetWindowSubclass(btn.m_hWnd, FlatButton::subclassProc, 0x666C6174 , (DWORD_PTR)&btn);
            }
            DeleteObject(cell);
        }
        DeleteObject(strip);
        return true;
    }
    return true;
}

bool MonsterDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    unsigned int element = id - 121;
    if (element > 4)
        return Container::onCommand(code, ctrl, id);
    m_spots->destroySpots();
    int count = m_monsterSpots.size();
    for (int i = 0; i < count; ++i) {
        MonsterSpot* spot = m_monsterSpots.get(i);
        if (spot && FIELD(unsigned char, spot->m_monster, 144) == element) {
            Collection<MonsterArea>* areas = &spot->m_areas;
            int n = areas->size();
            for (int j = 0; j < n; ++j)
                addAreaWindow(m_spots, areas->get(j), (HMENU)(UINT_PTR)(j + 1001));
        }
    }
    return true;
}
