#include "SpotPage_p05.h"
#include "PortalDetails.h"
#include "Document.h"

DWORD PortalDetails::PortalList::getStyle()
{
    return 13;
}

bool PortalDetails::PortalList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 40, 18, 'indx');
        addColumn(L"X", 60, 130, 'posx');
        addColumn(L"Y", 60, 130, 'posy');
        addColumn(L"Z", 60, 130, 'posz');
        addColumn(L"Dest", 40, 18, 'dest');
        addColumn(L"L.Min", 40, 18, 'lmin');
        addColumn(L"L.Max", 45, 18, 'lmax');
        return true;
    }
    return result;
}

int PortalDetails::PortalList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<Portal*>(node));
}

PortalDetails::PortalDetails(MapSpots* spots, Collection<Portal>* portals)
    : SpotDetails(spots), m_portals(portals), m_list(this, portals),
      m_faction(), m_levelMin(), m_levelMax(), m_destMap(), m_pos(), m_dest(), m_btnSet(), m_btnAdd()
{
}

PortalDetails::~PortalDetails()
{
}

bool PortalDetails::onCreate()
{
    RECT client;
    getClientRect(&client);
    int listHeight = client.bottom - client.top - 234;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, listHeight, 0x10800000, 0, 0);
    m_list.setItemCount(m_portals->size());
    int y = listHeight + 27;
    ::createLabel(m_hWnd, L"Faction:", 30, y + 2, 50, 14);
    m_faction.createNumber(m_hWnd, (HMENU)110, 90, y, 40, 18, 0, 0, 0);
    ::createLabel(m_hWnd, L"Levels:", 160, y + 2, 40, 14);
    m_levelMin.createNumber(m_hWnd, (HMENU)111, 200, y, 40, 18, 0, 0, 0);
    ::createLabel(m_hWnd, L"to", 250, y + 2, 20, 14);
    m_levelMax.createNumber(m_hWnd, (HMENU)112, 270, y, 40, 18, 0, 0, 0);
    int y2 = listHeight + 51;
    ::createLabel(m_hWnd, L"Position", 30, y2 + 2, 40, 14);
    ::createLabel(m_hWnd, L"X:", 70, y2 + 2, 20, 14);
    createFloatEdit(m_pos[0], m_hWnd, 90, y2, 60, 18, 0x78);
    ::createLabel(m_hWnd, L"Y:", 160, y2 + 2, 20, 14);
    createFloatEdit(m_pos[1], m_hWnd, 180, y2, 60, 18, 0x79);
    ::createLabel(m_hWnd, L"Z:", 250, y2 + 2, 20, 14);
    createFloatEdit(m_pos[2], m_hWnd, 270, y2, 60, 18, 0x7A);
    ::createLabel(m_hWnd, L"Target map:", 30, listHeight + 83, 60, 14);
    m_destMap.createNumber(m_hWnd, (HMENU)130, 90, listHeight + 81, 40, 18, 0, 0, 0);
    int y4 = listHeight + 105;
    ::createLabel(m_hWnd, L"Position", 30, y4 + 2, 40, 14);
    ::createLabel(m_hWnd, L"X:", 70, y4 + 2, 20, 14);
    createFloatEdit(m_dest[0], m_hWnd, 90, y4, 60, 18, 0x87);
    ::createLabel(m_hWnd, L"Y:", 160, y4 + 2, 20, 14);
    createFloatEdit(m_dest[1], m_hWnd, 180, y4, 60, 18, 0x88);
    ::createLabel(m_hWnd, L"Z:", 250, y4 + 2, 20, 14);
    createFloatEdit(m_dest[2], m_hWnd, 270, y4, 60, 18, 0x89);
    ::createButton(m_btnSet, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 340, listHeight + 27, 60, 22, 0x8C);
    ::createButton(m_btnAdd, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, listHeight + 53, 60, 22, 0x8D);
    return true;
}

static void readPortal(PortalDetails* page, Portal* portal)
{
    portal->m_faction = page->m_faction.getInt(portal->m_faction);
    portal->m_levelMin = (short)page->m_levelMin.getInt(portal->m_levelMin);
    portal->m_levelMax = (short)page->m_levelMax.getInt(portal->m_levelMax);
    SPOT_X(portal) = page->m_pos[0].getFloat();
    SPOT_Y(portal) = page->m_pos[1].getFloat();
    SPOT_Z(portal) = page->m_pos[2].getFloat();
    portal->m_destMap = page->m_destMap.getInt(portal->m_destMap);
    portal->m_destX = page->m_dest[0].getFloat();
    portal->m_destY = page->m_dest[1].getFloat();
    portal->m_destZ = page->m_dest[2].getFloat();
}

bool PortalDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 0x8C) {
        Portal* portal = m_portals->get(m_list.getNextSelected(0));
        if (!portal)
            return true;
        readPortal(this, portal);
        m_list.updateItem(-2);
        destroySpotWindow(m_spots, portal);
        addPointSpotWindow(m_spots, portal, 0);
        m_spots->m_document->setModified(true);
        return true;
    }
    if (id == 0x8D) {
        Portal* portal = new Portal(m_portals->size(), (int)m_portals);
        readPortal(this, portal);
        m_portals->add(portal);
        m_list.setItemCount(m_portals->size());
        m_list.invalidate(true);
        m_spots->m_document->setModified(true);
        return true;
    }
    return Container::onCommand(code, ctrl, id);
}

void PortalDetails::showSpots()
{
    m_spots->destroySpots();
    int count = m_portals->size();
    for (int i = 0; i < count; ++i)
        addPointSpotWindow(m_spots, m_portals->m_items[i], (HMENU)(UINT_PTR)(i + 1001));
}

void PortalDetails::selectSpot(Spot* spot)
{
    displaySpot(spot);
    Portal* portal = dynamic_cast<Portal*>(spot);
    m_list.selectItem(m_portals->indexOf(portal));
}

void PortalDetails::displaySpot(Spot* spot)
{
    Portal* portal = dynamic_cast<Portal*>(spot);
    if (portal) {
        m_faction.setInt(portal->m_faction);
        m_levelMin.setInt(portal->m_levelMin);
        m_levelMax.setInt(portal->m_levelMax);
        m_pos[0].setFloat(SPOT_X(portal));
        m_pos[1].setFloat(SPOT_Y(portal));
        m_pos[2].setFloat(SPOT_Z(portal));
        m_destMap.setInt(portal->m_destMap);
        m_dest[0].setFloat(portal->m_destX);
        m_dest[1].setFloat(portal->m_destY);
        m_dest[2].setFloat(portal->m_destZ);
        m_btnSet.enable(true);
    } else {
        m_faction.setText(L"");
        m_levelMin.setText(L"");
        m_levelMax.setText(L"");
        m_pos[0].setText(L"");
        m_pos[1].setText(L"");
        m_pos[2].setText(L"");
        m_destMap.setText(L"");
        m_dest[0].setText(L"");
        m_dest[1].setText(L"");
        m_dest[2].setText(L"");
        m_btnSet.enable(false);
    }
}
