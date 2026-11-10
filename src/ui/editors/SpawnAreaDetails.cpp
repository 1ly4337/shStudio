#include "SpawnAreaDetails.h"
#include "MapSpots.h"
#include "Window.h"
#include "Container.h"
#include "Document.h"
#include "Spot.h"
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "_imports_p06.h"

bool SpawnAreaDetails::SpawnAreaList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (!result)
        return result;
    addColumn(L"Indx", 40, 18, 'indx');
    addColumn(L"X1", 60, 130, 'posx');
    addColumn(L"Z1", 60, 130, 'posy');
    addColumn(L"X2", 60, 130, 'psx2');
    addColumn(L"Z2", 60, 130, 'psz2');
    addColumn(L"Fct°", 40, 18, 'fact');
    return true;
}

int SpawnAreaDetails::SpawnAreaList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<Spawn*>(node));
}

SpawnAreaDetails::SpawnAreaDetails(MapSpots* spots, Collection<Spawn>* spawns)
    : SpotDetails(spots), m_spawns(spawns), m_list(this, spawns)
{
}

SpawnAreaDetails::~SpawnAreaDetails()
{
}

bool SpawnAreaDetails::onCreate()
{
    RECT rc;
    getClientRect(&rc);
    int top = rc.bottom - rc.top - 234;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, top, 0x10800000, 0, 0);
    m_list.setItemCount(m_spawns->size());

    struct Field { EditText* edit; const wchar_t* label; int labelX; int editX; int id; };
    int row = top + 27;
    const Field rows[2][3] = {
        { { &m_x1, L"X1:", 40, 60, 0x66 }, { &m_y1, L"Y1:", 140, 160, 0x67 }, { &m_z1, L"Z1:", 230, 260, 0x68 } },
        { { &m_x2, L"X2:", 40, 60, 0x69 }, { &m_y2, L"Y2:", 140, 160, 0x6A }, { &m_z2, L"Z2:", 230, 260, 0x6B } },
    };
    for (int r = 0; r < 2; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            const Field& f = rows[r][c];
            HWND label = CreateWindowExW(0, L"Static", f.label, 0x50000000, f.labelX, row + 2, 20, 14, m_hWnd, 0, 0, 0);
            if (label)
                SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
            EditText& edit = *f.edit;
            edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, f.editX, row, 60, 18, m_hWnd,
                                          (HMENU)(INT_PTR)f.id, 0, 0);
            if (edit.m_hWnd)
            {
                edit.setFontId(edit.m_fontId);
                SetWindowSubclass(edit.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&edit);
                edit.setFloat(0.0f);
            }
        }
        row += 24;
    }

    HWND label = CreateWindowExW(0, L"Static", L"Faction:", 0x50000000, 40, row + 2, 50, 14, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_faction.createNumber(m_hWnd, (HMENU)108, 90, row, 30, 18, 0, 0, 0);
    label = CreateWindowExW(0, L"Static", L"Params:", 0x50000000, 140, row + 2, 40, 14, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_param1.createNumber(m_hWnd, (HMENU)109, 190, row, 40, 18, 0, 0, 0);
    label = CreateWindowExW(0, L"Static", L"-", 0x50000000, 235, row + 2, 10, 14, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_param2.createNumber(m_hWnd, (HMENU)110, 245, row, 40, 18, 0, 0, 0);

    m_setButton.m_hWnd = CreateWindowExW(0, L"Button", L"Set", 0x50000000, 340, top + 27, 60, 22, m_hWnd,
                                         (HMENU)0x6F, 0, 0);
    if (m_setButton.m_hWnd)
        m_setButton.setFontId(m_setButton.m_fontId);
    else
        GetLastError();
    m_addButton.m_hWnd = CreateWindowExW(0, L"Button", L"Add", 0x50000000, 340, top + 51, 60, 22, m_hWnd,
                                         (HMENU)0x70, 0, 0);
    if (!m_addButton.m_hWnd)
    {
        GetLastError();
        return true;
    }
    m_addButton.setFontId(m_addButton.m_fontId);
    return true;
}

bool SpawnAreaDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    Spawn* spawn;
    if (id == 111)
    {
        spawn = m_spawns->get(m_list.getNextSelected(0));
        if (!spawn)
            return true;
        spawn->m_x = m_x1.getFloat();
        spawn->m_y = m_y1.getFloat();
        spawn->m_z = m_z1.getFloat();
        spawn->m_x2 = m_x2.getFloat();
        spawn->m_y2 = m_y2.getFloat();
        spawn->m_z2 = m_z2.getFloat();
        spawn->m_faction = m_faction.getInt(0);
        spawn->m_unk20 = m_param1.getInt(-1);
        spawn->m_unk28 = m_param2.getInt(-1);
        m_list.updateItem(-2);
        HWND wnd = m_spots->findSpotWnd(spawn);
        if (wnd)
        {
            DestroyWindow(wnd);
            m_spots->m_spotWnds.remove(wnd);
        }
    }
    else if (id == 112)
    {
        spawn = new Spawn(m_spawns->size(), (int)(INT_PTR)m_spawns);
        spawn->m_x = m_x1.getFloat();
        spawn->m_y = m_y1.getFloat();
        spawn->m_z = m_z1.getFloat();
        spawn->m_x2 = m_x2.getFloat();
        spawn->m_y2 = m_y2.getFloat();
        spawn->m_z2 = m_z2.getFloat();
        spawn->m_faction = m_faction.getInt(0);
        spawn->m_unk20 = m_param1.getInt(-1);
        spawn->m_unk28 = m_param2.getInt(-1);
        m_spawns->add(spawn);
        m_list.setItemCount(m_spawns->size());
        m_list.invalidate(true);
    }
    else
    {
        return Container::onCommand(code, ctrl, id);
    }

    MapSpots* spots = m_spots;
    float scale = spots->m_scale;
    RECT rc;
    rc.left = (int)(float)(scale * spawn->m_x);
    rc.right = (int)(scale * spawn->m_x2);
    int height = spots->m_height;
    rc.top = height - (int)(spawn->m_z2 * scale);
    rc.bottom = height - (int)(scale * spawn->m_z);
    HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, 0, &rc, spawn);
    m_spots->m_spotWnds.add(wnd);
    m_spots->m_document->setModified(true);
    return true;
}

void SpawnAreaDetails::showSpots()
{
    int count = m_spawns->size();
    for (int i = 0; i < count; ++i)
    {
        MapSpots* spots = m_spots;
        float scale = spots->m_scale;
        Spawn* spawn = m_spawns->m_items[i];
        int height = spots->m_height;
        RECT rc;
        rc.left = (int)(float)(spawn->m_x * scale);
        rc.right = (int)(spawn->m_x2 * scale);
        rc.top = height - (int)(spawn->m_z2 * scale);
        rc.bottom = height - (int)(float)(spawn->m_z * scale);
        HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, (HMENU)(INT_PTR)(i + 1001), &rc, spawn);
        m_spots->m_spotWnds.add(wnd);
    }
}

void SpawnAreaDetails::selectSpot(Spot* spot)
{
    displaySpot(spot);
    Spawn* spawn = dynamic_cast<Spawn*>(spot);
    m_list.selectItem(m_spawns->indexOf(spawn));
}

void SpawnAreaDetails::displaySpot(Spot* spot)
{
    Spawn* spawn = dynamic_cast<Spawn*>(spot);
    if (spawn)
    {
        m_x1.setFloat(spawn->m_x);
        m_y1.setFloat(spawn->m_y);
        m_z1.setFloat(spawn->m_z);
        m_x2.setFloat(spawn->m_x2);
        m_y2.setFloat(spawn->m_y2);
        m_z2.setFloat(spawn->m_z2);
        m_faction.setInt(spawn->m_faction);
        m_param1.setInt(spawn->m_unk20);
        m_param2.setInt(spawn->m_unk28);
        m_setButton.enable(true);
    }
    else
    {
        m_x1.setText(L"");
        m_y1.setText(L"");
        m_z1.setText(L"");
        m_x2.setText(L"");
        m_y2.setText(L"");
        m_z2.setText(L"");
        m_faction.setText(L"");
        m_param1.setText(L"");
        m_param2.setText(L"");
        m_setButton.enable(false);
    }
}
