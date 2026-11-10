#include "SafeAreasDetails.h"
#include "MapSpots.h"
#include "Window.h"
#include "Container.h"
#include "StudioFrame.h"
#include "Document.h"
#include "Spot.h"
#include <cstdio>
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "Application.h"
#include "_imports_p06.h"

bool SafeAreasDetails::SafeAreasList::removeItem(int index)
{
    SafeAreasDetails* owner = dynamic_cast<SafeAreasDetails*>(m_owner);
    if (!owner)
        return false;
    TacticsZone* zone = m_items->get(index);
    if (!zone)
        return false;
    HWND wnd = owner->m_spots->findSpotWnd(zone);
    if (wnd)
    {
        DestroyWindow(wnd);
        owner->m_spots->m_spotWnds.remove(wnd);
    }
    TacticsZones* all = owner->m_allZones;
    if (!all->detachItem(zone))
        return false;
    all->destroyItem(zone);
    m_items->detachItem(zone);
    onChanged();
    return true;
}

void SafeAreasDetails::SafeAreasList::onChanged()
{
    FileTree* file = g_studioFrame->findFile(L"World\\TacticsZone.zon");
    if (file)
        FIELD(int, file, 0xFC) = 2;
}

int SafeAreasDetails::SafeAreasList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<TacticsZone*>(node));
}

SafeAreasDetails::SafeAreasDetails(MapSpots* spots, TacticsZones* allZones, int zoneType)
    : SpotDetails(spots),
      m_allZones(allZones),
      m_zones(),
      m_list(this, &m_zones),
      m_picture(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
{
    m_zoneType = zoneType;
    int count = m_allZones->size();
    for (int i = 0; i < count; ++i)
    {
        TacticsZone* zone = m_allZones->get(i);
        if (zone && zone->m_map == m_zoneType)
            m_zones.add(zone);
    }
}

SafeAreasDetails::~SafeAreasDetails()
{
    for (int i = 0; i < m_zones.m_count; ++i)
        m_zones.m_items[i] = 0;
    m_zones.m_count = 0;
}

bool SafeAreasDetails::onCreate()
{
    RECT rc;
    getClientRect(&rc);
    int top = rc.bottom - rc.top - 234;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, top, 0x10800000, 0, 0);
    m_list.setItemCount(m_zones.size());

    wchar_t buffer[32];
    int x = 55;
    unsigned i = 0;
    do
    {
        unsigned n = i + 1;
        buffer[0] = 0;
        swprintf_s(buffer, 8, L"#%u", n);
        HWND label = CreateWindowExW(0, L"Static", buffer, 0x50000001, x, top + 23, 60, 16, m_hWnd, 0, 0, 0);
        if (label)
            SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
        i = n;
        x += 70;
    } while ((int)i < 4);

    HWND labelX = CreateWindowExW(0, L"Static", L"X:", 0x50000000, 35, top + 41, 20, 14, m_hWnd, 0, 0, 0);
    if (labelX)
        SendMessageW(labelX, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    HWND labelZ = CreateWindowExW(0, L"Static", L"Z:", 0x50000000, 35, top + 65, 20, 14, m_hWnd, 0, 0, 0);
    if (labelZ)
        SendMessageW(labelZ, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    x = 55;
    int zId = 106;
    for (int k = 0; k < 4; ++k)
    {
        EditText& editX = m_xEdits[k];
        editX.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, x, top + 39, 60, 18, m_hWnd,
                                       (HMENU)(INT_PTR)(zId - 4), 0, 0);
        if (editX.m_hWnd)
        {
            editX.setFontId(editX.m_fontId);
            SetWindowSubclass(editX.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&editX);
            HWND hwnd = editX.m_hWnd;
            swprintf_s(buffer, 32, L"%.4f", 0.0);
            SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)buffer);
        }
        EditText& editZ = m_zEdits[k];
        editZ.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, x, top + 63, 60, 18, m_hWnd,
                                       (HMENU)(INT_PTR)zId, 0, 0);
        if (editZ.m_hWnd)
        {
            editZ.setFontId(editZ.m_fontId);
            SetWindowSubclass(editZ.m_hWnd, decimalEditSubclassProc, 'edit', (DWORD_PTR)&editZ);
            HWND hwnd = editZ.m_hWnd;
            swprintf_s(buffer, 32, L"%.4f", 0.0);
            SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)buffer);
        }
        x += 70;
        ++zId;
    }

    int prmTop = top + 87;
    HWND labelPrm = CreateWindowExW(0, L"Static", L"Prm:", 0x50000000, 30, top + 89, 20, 14, m_hWnd, 0, 0, 0);
    if (labelPrm)
        SendMessageW(labelPrm, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    for (int k = 0; k < 2; ++k)
    {
        HWND parent = m_hWnd;
        int id = 110 + k;
        EditText& edit = m_prmEdits[k];
        edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50012080, k == 0 ? 55 : 125, prmTop, 60, 18,
                                      parent, (HMENU)(INT_PTR)id, 0, 0);
        if (edit.m_hWnd)
        {
            edit.setFontId(edit.m_fontId);
            SendMessageW(edit.m_hWnd, EM_LIMITTEXT, 16, 0);
            if (!SetDlgItemInt(parent, id, 0, TRUE))
            {
                swprintf_s(buffer, 16, L"%ld", 0);
                edit.setText(buffer);
            }
            GetDlgItemInt(parent, id, 0, TRUE);
            SetWindowSubclass(edit.m_hWnd, integerEditSubclassProc, 'edit', (DWORD_PTR)&edit);
            GetDlgItemInt(parent, id, 0, TRUE);
        }
    }

    HWND help = CreateWindowExW(0, L"Static", L"The four points must be defined counterclockwise, as follows:",
                                0x50000000, 30, top + 117, 370, 18, m_hWnd, 0, 0, 0);
    if (help)
        SendMessageW(help, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    HWND parent = m_hWnd;
    HANDLE image = LoadImageW(hModule, MAKEINTRESOURCEW(0xAB), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (image)
    {
        m_picture.setBitmap((HBITMAP)image);
        m_picture.m_drawFlags = 0;
        SetRect(&rc, 60, top + 137, 124, top + 201);
        m_picture.create(parent, (HMENU)0x73, &rc, 0x50000000, WS_EX_TRANSPARENT, 0);
    }

    m_setButton.m_hWnd = CreateWindowExW(0, L"Button", L"Set", 0x50000000, 340, top + 37, 60, 22, m_hWnd,
                                         (HMENU)0x70, 0, 0);
    if (m_setButton.m_hWnd)
        m_setButton.setFontId(m_setButton.m_fontId);
    else
        GetLastError();
    m_addButton.m_hWnd = CreateWindowExW(0, L"Button", L"Add", 0x50000000, 340, top + 61, 60, 22, m_hWnd,
                                         (HMENU)0x71, 0, 0);
    if (m_addButton.m_hWnd)
        m_addButton.setFontId(m_addButton.m_fontId);
    else
        GetLastError();
    return true;
}

bool SafeAreasDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    TacticsZone* zone;
    if (id == 0x71)
    {
        zone = new TacticsZone(m_allZones->size(), (int)(INT_PTR)m_allZones);
    }
    else
    {
        if (id != 0x70)
            return Container::onCommand(code, ctrl, id);
        zone = m_zones.get(m_list.getNextSelected(0));
    }
    if (zone)
    {
        *zone->m_pX1 = m_xEdits[0].getFloat();
        *zone->m_pZ1 = m_zEdits[0].getFloat();
        zone->m_x2 = m_xEdits[1].getFloat();
        zone->m_z2 = m_zEdits[1].getFloat();
        zone->m_x3 = m_xEdits[2].getFloat();
        zone->m_z3 = m_zEdits[2].getFloat();
        zone->m_x4 = m_xEdits[3].getFloat();
        zone->m_z4 = m_zEdits[3].getFloat();
        zone->m_unk1 = (BYTE)m_prmEdits[0].getInt(0);
        zone->m_unk2 = (BYTE)m_prmEdits[1].getInt(0);
        HWND zoneWnd = zone->m_hItem;
        int wndId = zoneWnd ? GetDlgCtrlID(zoneWnd) : m_allZones->size() + 1001;
        if (id == 0x70)
        {
            m_list.updateItem(-2);
            MapSpots* spots = m_spots;
            HWND wnd = spots->findSpotWnd(zone);
            if (wnd)
            {
                DestroyWindow(wnd);
                spots->m_spotWnds.remove(wnd);
            }
        }
        else
        {
            m_allZones->add(zone);
            m_zones.add(zone);
            m_list.setItemCount(m_zones.size());
            m_list.invalidate(true);
        }
        MapSpots* spots = m_spots;
        Scaler* scaler = spots ? static_cast<Scaler*>(spots) : 0;
        HWND parent = spots->m_hWnd;
        RECT rc;
        HWND wnd = MapSpots::createSpotWindow(parent, (HMENU)(INT_PTR)wndId, zone->calcRect(&rc, scaler), zone);
        m_spots->m_spotWnds.add(wnd);
        g_studioFrame->markFileModified(L"World\\TacticsZone.zon");
    }
    return true;
}

void SafeAreasDetails::showSpots()
{
    int count = m_zones.size();
    for (int i = 0; i < count; ++i)
    {
        TacticsZone* zone = m_zones.get(i);
        MapSpots* spots = m_spots;
        Scaler* scaler = spots ? static_cast<Scaler*>(spots) : 0;
        HWND parent = spots->m_hWnd;
        RECT rc;
        HWND wnd = MapSpots::createSpotWindow(parent, (HMENU)(INT_PTR)(i + 1001), zone->calcRect(&rc, scaler), zone);
        m_spots->m_spotWnds.add(wnd);
    }
}

void SafeAreasDetails::selectSpot(Spot* spot)
{
    displaySpot(spot);
    TacticsZone* zone = dynamic_cast<TacticsZone*>(spot);
    m_list.selectItem(m_zones.indexOf(zone));
}

void SafeAreasDetails::displaySpot(Spot* spot)
{
    TacticsZone* zone = dynamic_cast<TacticsZone*>(spot);
    if (zone)
    {
        m_xEdits[0].setFloat(*zone->m_pX1);
        m_zEdits[0].setFloat(*zone->m_pZ1);
        m_xEdits[1].setFloat(zone->m_x2);
        m_zEdits[1].setFloat(zone->m_z2);
        m_xEdits[2].setFloat(zone->m_x3);
        m_zEdits[2].setFloat(zone->m_z3);
        m_xEdits[3].setFloat(zone->m_x4);
        m_zEdits[3].setFloat(zone->m_z4);
        m_prmEdits[0].setInt(zone->m_unk1);
        m_prmEdits[1].setInt(zone->m_unk2);
        m_setButton.enable(true);
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            m_xEdits[i].setText(L"");
            m_zEdits[i].setText(L"");
        }
        m_prmEdits[0].setText(L"");
        m_prmEdits[1].setText(L"");
        m_setButton.enable(false);
    }
}
