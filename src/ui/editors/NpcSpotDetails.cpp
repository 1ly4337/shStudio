#include "SpotPage_p05.h"
#include "NpcSpotDetails.h"
#include "StandardEditor.h"
#include "StudioFrame.h"
#include "Document.h"

#define LOC_WND(l) FIELD(HWND, (l), 0x10)

void highlightNpcLocations(NpcLocation* npc, bool on)
{
    NpcLocation::Locations* locations = &npc->m_locations;
    int count = locations->size();
    for (int i = 0; i < count; ++i) {
        NpcLocation::Location* loc = locations->get(i);
        if (loc) {
            HWND wnd = LOC_WND(loc);
            if (wnd) {
                LONG style = GetWindowLongW(wnd, GWL_STYLE) & 0xFFFFFF00;
                if (on)
                    style |= 2;
                SetWindowLongW(wnd, GWL_STYLE, style);
                ((MapSpots*)npc)->redrawSpot(wnd, 0);
            }
        }
    }
}

static HWND addLocationWindow(MapSpots* spots, NpcLocation* npc, NpcLocation::Location* loc, HMENU id)
{
    RECT rc;
    spotPointRect(&rc, spots, loc->m_x, loc->m_z);
    HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, id, &rc, npc);
    LOC_WND(loc) = wnd;
    spots->m_spotWnds.add(wnd);
    return wnd;
}

DWORD NpcSpotDetails::NpcSpotList::getStyle()
{
    return 13;
}

bool NpcSpotDetails::NpcSpotList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 40, 18, 'indx');
        addColumn(L"Type", 40, 18, 'type');
        addColumn(L"TypeID", 40, 18, 'tyid');
        addColumn(L"Familly", 160, 0, 'fami');
        addColumn(L"Pos.", 40, 18, 'mcnt');
        return true;
    }
    return result;
}

HRESULT NpcSpotDetails::NpcSpotList::onGetDispInfo(NMLVDISPINFOW* di)
{
    di->item.pszText = 0;
    NpcLocation* npc = m_items->get(di->item.iItem);
    if (!npc)
        return E_FAIL;
    if (di->item.mask & LVIF_TEXT) {
        di->item.pszText = (LPWSTR)L"";
        if (m_columnIds.m_items) {
            long tag = *m_columnIds.at(di->item.iSubItem);
            if (tag == 'fami') {
                Node* def = ((NpcSpotDetails*)m_owner)->m_perso->find((unsigned char)npc->m_type,
                                                                     (short)npc->m_typeId);
                if (def) {
                    di->item.pszText = (LPWSTR)def->getString('fami');
                    return 0;
                }
            } else {
                unsigned short* format = m_columnFormats.at(di->item.iSubItem);
                di->item.pszText = (LPWSTR)formatValue(npc, tag, *format);
            }
        }
    }
    return 0;
}

void NpcSpotDetails::NpcSpotList::onItemChanged(int index, UINT )
{
    NpcLocation* npc = m_items->get(index);
    m_owner->displaySpot(npc);
    if (m_selected && m_selected != npc)
        highlightNpcLocations(m_selected, false);
    m_selected = npc;
    if (npc)
        highlightNpcLocations(npc, true);
}

bool NpcSpotDetails::NpcSpotList::onActivateItem(int index, int )
{
    NpcLocation* npc = m_items->get(index);
    if (!npc)
        return false;
    if (!g_studioFrame)
        return false;
    ClientData& data = g_studioFrame->m_client.m_clientData;
    data.loadSData(&data.m_quests, L"Npc\\NpcQuest.sdata", 'file', 1);
    NpcPerso* perso = &data.m_quests;
    StandardNPC* def = perso->find((unsigned char)npc->m_type, (short)npc->m_typeId);
    if (!def)
        return false;
    if (editStandardNpc((Window*)m_owner->m_spots->m_document, def))
        g_studioFrame->markFileModified(L"NpcQuest.sdata");
    return true;
}

int NpcSpotDetails::NpcSpotList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<NpcLocation*>(node));
}

DWORD NpcSpotDetails::NpcLocaList::getStyle()
{
    return 13;
}

bool NpcSpotDetails::NpcLocaList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 50, 16, 'indx');
        addColumn(L"X", 60, 130, 'posx');
        addColumn(L"Y", 60, 130, 'posy');
        addColumn(L"Z", 60, 130, 'posz');
        addColumn(L"Orient.", 60, 130, 'ornt');
        return true;
    }
    return result;
}

void NpcSpotDetails::NpcLocaList::onChanged()
{
    m_owner->m_spots->m_document->setModified(true);
}

Node* NpcSpotDetails::NpcLocaList::getNode(int index)
{
    if (m_locations)
        return m_locations->get(index);
    return 0;
}

int NpcSpotDetails::NpcLocaList::indexOfNode(Node* node)
{
    if (!m_locations)
        return -1;
    return m_locations->indexOf(dynamic_cast<NpcLocation::Location*>(node));
}

void NpcSpotDetails::NpcLocaList::setLocations(NpcLocation::Locations* locations)
{
    m_locations = locations;
    if (locations) {
        setItemCount(locations->size());
        m_owner->m_btnLocAdd.enable(true);
    } else {
        setItemCount(0);
        for (int i = 0; i < 4; ++i)
            m_owner->m_locEdits[i].setText(L"");
        m_owner->m_btnLocAdd.enable(false);
    }
}

void NpcSpotDetails::NpcLocaList::onItemChanged(int index, UINT newState)
{
    m_owner->m_btnLocSet.enable(false);
    if (newState == 3 && index >= 0 && m_locations) {
        NpcLocation::Location* loc = m_locations->get(index);
        if (loc) {
            m_owner->m_locEdits[0].setFloat(loc->m_x);
            m_owner->m_locEdits[1].setFloat(loc->m_y);
            m_owner->m_locEdits[2].setFloat(loc->m_z);
            m_owner->m_locEdits[3].setFloat(loc->m_orient);
            m_owner->m_btnLocSet.enable(true);
        }
    }
}

bool NpcSpotDetails::NpcLocaList::removeItem(int index)
{
    if (!m_locations)
        return false;
    NpcLocation::Location* loc = m_locations->get(index);
    if (!loc)
        return false;
    destroySpotWindow(m_owner->m_spots, (Spot*)loc);
    m_owner->m_spots->m_document->setModified(true);
    NpcLocation::Locations* locations = m_locations;
    if (!locations->detachItem(loc))
        return false;
    locations->destroyItem(loc);
    return true;
}

NpcSpotDetails::NpcSpotDetails(MapSpots* spots, Collection<NpcLocation>* npcs, NpcPerso* perso)
    : SpotDetails(spots), m_npcs(npcs), m_perso(perso), m_list(this, npcs),
      m_editType(), m_editTypeId(), m_btnSet(), m_btnAdd(), m_locaList(this),
      m_locEdits(), m_btnLocSet(), m_btnLocAdd()
{
}

NpcSpotDetails::~NpcSpotDetails()
{
}

bool NpcSpotDetails::onCreate()
{
    RECT client;
    getClientRect(&client);
    int listHeight = client.bottom - client.top - 260;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, listHeight, 0x10800000, 0, 0);
    m_list.setItemCount(m_npcs->size());
    int y = listHeight + 27;
    ::createLabel(m_hWnd, L"Type:", 40, y + 2, 30, 14);
    m_editType.createNumber(m_hWnd, (HMENU)104, 70, y, 50, 18, 0, 0, 0);
    ::createLabel(m_hWnd, L"Type ID:", 130, y + 2, 50, 14);
    m_editTypeId.createNumber(m_hWnd, (HMENU)105, 180, y, 50, 18, 0, 0, 0);
    ::createButton(m_btnSet, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 270, y - 2, 60, 22, 0x6A);
    ::createButton(m_btnAdd, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, y - 2, 60, 22, 0x6B);
    int y2 = y + 30;
    ::createLabel(m_hWnd, L"Position(s):", 30, y2, 100, 14);
    int y3 = y2 + 16;
    m_locaList.create(m_hWnd, (HMENU)0x6E, 30, y3, 370, 96, 0x10800000, 0, 0);
    int y4 = y3 + 108;
    ::createLabel(m_hWnd, L"X:", 40, y4 + 2, 15, 14);
    createFloatEdit(m_locEdits[0], m_hWnd, 55, y4, 55, 18, 0x72);
    ::createLabel(m_hWnd, L"Y:", 120, y4 + 2, 15, 14);
    createFloatEdit(m_locEdits[1], m_hWnd, 135, y4, 55, 18, 0x73);
    ::createLabel(m_hWnd, L"Z:", 200, y4 + 2, 15, 14);
    createFloatEdit(m_locEdits[2], m_hWnd, 215, y4, 55, 18, 0x74);
    ::createLabel(m_hWnd, L"orient°:", 280, y4 + 2, 40, 14);
    createFloatEdit(m_locEdits[3], m_hWnd, 320, y4, 55, 18, 0x75);
    int y5 = y4 + 26;
    ::createButton(m_btnLocSet, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 270, y5, 60, 22, 0x76);
    ::createButton(m_btnLocAdd, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, y5, 60, 22, 0x77);
    return true;
}

bool NpcSpotDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    switch (id) {
    case 0x6A: {
        NpcLocation* npc = m_npcs->get(m_list.getNextSelected(0));
        if (!npc)
            return true;
        npc->m_type = m_editType.getInt(0);
        npc->m_typeId = m_editTypeId.getInt(0);
        m_list.updateItem(-2);
        m_spots->m_document->setModified(true);
        return true;
    }
    case 0x6B: {
        NpcLocation* npc = new NpcLocation(m_npcs->size(), (int)m_npcs);
        npc->m_type = m_editType.getInt(0);
        npc->m_typeId = m_editTypeId.getInt(0);
        m_npcs->add(npc);
        m_list.setItemCount(m_npcs->size());
        m_list.invalidate(true);
        m_spots->m_document->setModified(true);
        return true;
    }
    case 0x77: {
        NpcLocation* npc = m_npcs->get(m_list.getNextSelected(0));
        if (npc) {
            NpcLocation::Location* loc = new NpcLocation::Location(npc->m_locations.size());
            loc->m_x = m_locEdits[0].getFloat();
            loc->m_y = m_locEdits[1].getFloat();
            loc->m_z = m_locEdits[2].getFloat();
            loc->m_orient = m_locEdits[3].getFloat();
            npc->m_locations.add(loc);
            m_locaList.setItemCount(npc->m_locations.size());
            m_spots->m_document->setModified(true);
            addLocationWindow(m_spots, npc, loc, 0);
        }
        return true;
    }
    case 0x76: {
        NpcLocation* npc = m_npcs->get(m_list.getNextSelected(0));
        if (!npc)
            return true;
        NpcLocation::Location* loc = npc->m_locations.get(m_locaList.getNextSelected(0));
        if (!loc)
            return true;
        loc->m_x = m_locEdits[0].getFloat();
        loc->m_y = m_locEdits[1].getFloat();
        loc->m_z = m_locEdits[2].getFloat();
        loc->m_orient = m_locEdits[3].getFloat();
        m_locaList.invalidate(true);
        m_spots->removeSpot((Spot*)loc);
        RECT rcBuf;
        RECT* rc = m_spots->getPointRect(&rcBuf, loc->m_x, loc->m_z);
        HWND wnd = MapSpots::createSpotWindow(m_spots->m_hWnd, 0, rc, npc);
        LOC_WND(loc) = wnd;
        m_spots->m_spotWnds.add(wnd);
        m_spots->m_document->setModified(true);
        return true;
    }
    }
    return Container::onCommand(code, ctrl, id);
}

void NpcSpotDetails::showSpots()
{
    m_spots->destroySpots();
    UINT_PTR id = 1001;
    int count = m_npcs->size();
    for (int i = 0; i < count; ++i) {
        NpcLocation* npc = m_npcs->m_items[i];
        if (npc) {
            NpcLocation::Locations* locations = &npc->m_locations;
            int n = locations->size();
            for (int k = 0; k < n; ++k)
                addLocationWindow(m_spots, npc, locations->get(k), (HMENU)id++);
        }
    }
}

void NpcSpotDetails::selectSpot(Spot* spot)
{
    NpcLocation* npc = dynamic_cast<NpcLocation*>(spot);
    m_list.selectItem(m_npcs->indexOf(npc));
}

void NpcSpotDetails::displaySpot(Spot* spot)
{
    NpcLocation* npc = dynamic_cast<NpcLocation*>(spot);
    if (npc) {
        m_editType.setInt(npc->m_type);
        m_editTypeId.setInt(npc->m_typeId);
        m_btnSet.enable(true);
        m_locaList.setLocations(&npc->m_locations);
    } else {
        m_editType.setText(L"");
        m_editTypeId.setText(L"");
        m_btnSet.enable(false);
        m_locaList.setLocations(0);
    }
}
