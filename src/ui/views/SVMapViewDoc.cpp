#include "SVMapViewDoc.h"
#include "GeneralInfo.h"
#include "LaddersDetails.h"
#include "MonsterAreaDetails.h"
#include "MonsterDetails.h"
#include "NamedAreaDetails.h"
#include "NpcSpotDetails.h"
#include "PortalDetails.h"
#include "SpawnAreaDetails.h"
#include "SafeAreasDetails.h"
#include "Application.h"
#include "StudioFrame.h"
#include "ClientData.h"
#include "String.hpp"
#include "FileTree.h"
#include "Bytes.h"
#include "TacticsZones.h"
#include "Collection.hpp"
#include "clipboard_A.h"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <cwchar>
#include <cstdlib>

static inline Application* studioApp()
{
    return g_studioFrame;
}

const wchar_t* g_svMapTabGeneral      = L"General";
const wchar_t* g_svMapTabLadders      = L"Ladders";
const wchar_t* g_svMapTabMonsterAreas = L"Monster Areas";
const wchar_t* g_svMapTabMonsters     = L"Monsters";
const wchar_t* g_svMapTabNamedAreas   = L"Named Areas";
const wchar_t* g_svMapTabNpc          = L"NPC";
const wchar_t* g_svMapTabPortals      = L"Portals";
const wchar_t* g_svMapTabSpawns       = L"Spawns";
const wchar_t* g_svMapTabSafeAreas    = L"Safe Areas";

SVMapViewDoc::SVMapViewDoc(FileTree* data)
    : DocView(data, &m_map)
    , m_map()
    , m_spots(this, &m_map, &m_tab)
    , m_tab(&m_spots)
{
    m_initialSize.cy = 0;
    m_initialSize.cx = 0;
    m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    FIELD(int, &m_map, 0x14) = -1;
    m_bigMap = false;
    m_unusedAA = 0;
    m_unused194 = 0;
    m_unused198 = 0;
    m_minimap = 0;
    m_minimapBig = 0;

    String name(m_data->m_name);
    name.removeExtension();
    long id = _wtol(name.m_buffer);
    {
        String number;
        number << id;
        if (number.m_length == name.m_length
            && !_wcsnicmp(number.m_buffer, name.m_buffer, number.m_length))
            FIELD(int, &m_map, 0x14) = id;

        if (FIELD(int, &m_map, 0x14) >= 0) {
            Bytes data(0);
            name.m_length = 0;
            name.m_capacity = 0;
            if (name.m_buffer) {
                delete[] name.m_buffer;
                name.m_buffer = 0;
            }
            name << L"Interface\\MiniMap\\minimap_" << id << L".tga";
            if (!studioApp()->loadFromFile(&data, &name, false)) {
                if (!loadTGA(data, (HGDIOBJ*)&m_minimap)) {
                    m_minimapBig = resizeBitmap(m_minimap, 768, 768);
                    DeleteObject(m_minimap);
                    m_minimap = 0;
                }
                m_bigMap = m_minimapBig != 0;
            }
            name.removeFileName();
            name << id << L".tga";
            if (!studioApp()->loadFromFile(&data, &name, false))
                loadTGA(data, (HGDIOBJ*)&m_minimap);
        }
    }
}

SVMapViewDoc::~SVMapViewDoc()
{
}

SIZE* SVMapViewDoc::getDefaultSize(SIZE* out, SIZE )
{
    GetSystemMetrics(SM_CXFULLSCREEN);
    int cyFull = GetSystemMetrics(SM_CYFULLSCREEN);
    RECT rc;
    SetRectEmpty(&rc);
    char rctrl = (char)GetKeyState(VK_RCONTROL);
    bool ctrl = ((GetKeyState(VK_LCONTROL) | rctrl) & 0x80) != 0;
    if (!ctrl && m_bigMap && cyFull > 800) {
        SetRect(&rc, 0, 0, 1250, 798);
        DWORD exStyle = getExStyle();
        DWORD style = getStyle();
        rc.bottom += 24;
        if (AdjustWindowRectEx(&rc, style, TRUE, exStyle)) {
            out->cx = rc.right - rc.left;
            out->cy = rc.bottom - rc.top;
            return out;
        }
    }
    m_bigMap = false;
    SetRect(&rc, 0, 0, 994, 542);
    DWORD exStyle = getExStyle();
    DWORD style = getStyle();
    rc.bottom += 24;
    AdjustWindowRectEx(&rc, style, TRUE, exStyle);
    out->cx = rc.right - rc.left;
    out->cy = rc.bottom - rc.top;
    return out;
}

bool SVMapViewDoc::onCreate()
{
    if (!DocView::onCreate())
        return false;
    RECT client;
    getClientRect(&client);
    SIZE size;
    SIZE* sz = getSize(&size);
    m_initialSize.cx = sz->cx;
    m_initialSize.cy = sz->cy;

    int side = m_bigMap ? 768 : 512;
    HBITMAP bitmap = m_bigMap ? m_minimapBig : m_minimap;
    if (!m_spots.createChildAt(m_hWnd, (HMENU)110, 15, 15, side, side, 0x50000000, 0, 0))
        return false;
    FIELD(BYTE, &m_spots, 0xAC) = 0;
    m_spots.m_picture.setBitmap(bitmap);
    m_spots.m_height = side;
    m_spots.m_scale =
        (float)((double)side / (double)FIELD(int, (SVMap*)m_spots.m_74, 0x18));

    Application* app = studioApp();
    ClientData& clientData = FIELD(ClientData, app, 0x230);
    clientData.loadMonsters();
    Monsters* monsters = &clientData.m_monsters;
    ClientData& client2 = FIELD(ClientData, studioApp(), 0x230);
    client2.loadSData(static_cast<Serializable*>(&client2.m_quests), L"Npc\\NpcQuest.sdata", 'file', 1);
    NpcData* npcQuests = &client2.m_quests;

    if (!m_tab.create(m_hWnd, (HMENU)0x70, side + 30, 15, 435, side, 0x200, 0, 0))
        return false;

    GeneralInfo* general = new GeneralInfo(&m_spots, &m_map);
    m_tab.addPage(general, 'gnrl', g_svMapTabGeneral, -1);

    LaddersDetails* ladders = new LaddersDetails(&m_spots, &FIELD(Collection<Ladder>, &m_map, 0x50));
    m_tab.addPage(ladders, 'ladd', g_svMapTabLadders, -1);

    MonsterAreaDetails* areas =
        new MonsterAreaDetails(&m_spots, &FIELD(Collection<MonsterArea>, &m_map, 0x68), monsters);
    m_tab.addPage(areas, 'mare', g_svMapTabMonsterAreas, -1);

    if (!monsters->isEmpty()) {
        MonsterDetails* mdet =
            new MonsterDetails(&m_spots, &FIELD(Collection<MonsterArea>, &m_map, 0x68), monsters);
        m_tab.addPage(mdet, 'mdet', g_svMapTabMonsters, -1);
    }

    NamedAreaDetails* named = new NamedAreaDetails(&m_spots, &FIELD(Collection<NamedArea>, &m_map, 0xC8));
    m_tab.addPage(named, 'nars', g_svMapTabNamedAreas, -1);

    NpcSpotDetails* npcs =
        new NpcSpotDetails(&m_spots, &FIELD(Collection<NpcLocation>, &m_map, 0x80),
                           npcQuests);
    m_tab.addPage(npcs, 'npca', g_svMapTabNpc, -1);

    PortalDetails* portals = new PortalDetails(&m_spots, &FIELD(Collection<Portal>, &m_map, 0x98));
    m_tab.addPage(portals, 'port', g_svMapTabPortals, -1);

    SpawnAreaDetails* spawns = new SpawnAreaDetails(&m_spots, &FIELD(Collection<Spawn>, &m_map, 0xB0));
    m_tab.addPage(spawns, 'spwn', g_svMapTabSpawns, -1);

    SafeAreasDetails* safe = 0;
    void* mem = operator new(0x2B4);
    if (mem) {
        Application* a = studioApp();
        ClientData& c = FIELD(ClientData, a, 0x230);
        TacticsZones* zones = &c.m_tacticsZones;
        c.loadSData(static_cast<Serializable*>(zones),
                    L"World\\TacticsZone.zon", 'file', 0);
        safe = new (mem) SafeAreasDetails(&m_spots, zones, FIELD(int, &m_map, 0x14));
    }
    m_tab.addPage(safe, 'safe', g_svMapTabSafeAreas, -1);

    SendMessageW(m_tab.m_hWnd, WM_SIZE, 0, 0);
    m_tab.select(0);
    m_tab.pageAt(0)->showSpots();
    HWND page = m_tab.getPage(0);
    InvalidateRect(page, 0, TRUE);
    UpdateWindow(page);
    return true;
}

bool SVMapViewDoc::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl || id != 0xE123)
        return DocView::onCommand(code, ctrl, id);
    HBITMAP bitmap = m_spots.renderBitmap();
    clipboard_setBitmap(bitmap);
    DeleteObject(bitmap);
    return true;
}

HRESULT SVMapViewDoc::store()
{
    if (m_map.isEmpty())
        return E_FAIL;
    Bytes data(0);
    static_cast<Streamable&>(m_map).write(static_cast<oStream&>(data), 'file');
    HRESULT hr = studioApp()->saveToFile(&data, &m_data->m_path, false);
    return hr;
}

void SVMapViewDoc::setModified(bool modified)
{
    DocView::setModified(modified);
    vslot100();
}
