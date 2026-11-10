#include "SpotPage_p05.h"
#include "MonsterAreaDetails.h"
#include "StudioFrame.h"
#include "MonsterEditor.h"
#include "Monsters.h"
#include "Monster.h"
#include "Document.h"
#include "Application.h"

#define DOC_MODIFIED(spots) ((spots)->m_document->setModified(true))

RECT* monsterAreaRect(RECT* rc, MapSpots* spots, MonsterArea* area)
{
    float scale = spots->m_scale;
    int height = spots->m_height;
    rc->left = (int)(SPOT_X(area) * scale);
    rc->right = (int)(area->m_x2 * scale);
    rc->top = height - (int)(area->m_z2 * scale);
    rc->bottom = height - (int)(SPOT_Z(area) * scale);
    return rc;
}

static void addAreaWindow(MapSpots* spots, MonsterArea* area, HMENU id)
{
    RECT rc;
    monsterAreaRect(&rc, spots, area);
    HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, id, &rc, area);
    spots->m_spotWnds.add(wnd);
}

bool editMonster(Node* monster, void* models, HWND parent, bool extended)
{
    if (!monster || !models)
        return false;
    bool result;
    MonsterEditor editor((Monster*)monster, models);
    if (editor.open(parent) && editor.doModal() == 1) {
        StudioFrame* frame = g_studioFrame;
        if (extended) {
            uint8_t* file = (uint8_t*)frame->findFile(L"monster\\monsterEx.sdata");
            if (file)
                FIELD(int, file, 0xFC) = 2;
        }
        uint8_t* file = (uint8_t*)frame->findFile(L"monster\\monster.sdata");
        if (file)
            FIELD(int, file, 0xFC) = 2;
        result = true;
    } else {
        result = false;
    }
    return result;
}

DWORD MonsterAreaDetails::MonsterAreaList::getStyle()
{
    return 13;
}

bool MonsterAreaDetails::MonsterAreaList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 50, 18, 'indx');
        addColumn(L"X1", 70, 130, 'posx');
        addColumn(L"Z1", 70, 130, 'posz');
        addColumn(L"X2", 70, 130, 'psx2');
        addColumn(L"Z2", 70, 130, 'psz2');
        return true;
    }
    return result;
}

bool MonsterAreaDetails::MonsterAreaList::removeItem(int index)
{
    bool result = SpotsList<MonsterArea>::removeItem(index);
    if (result) {
        MonstersList* list = &((MonsterAreaDetails*)m_owner)->m_monstersList;
        list->m_ids = 0;
        list->setItemCount(0);
        list->m_owner->m_editId.setText(L"");
        list->m_owner->m_editCount.setText(L"");
        list->m_owner->m_btnSetId.enable(false);
        return true;
    }
    return result;
}

int MonsterAreaDetails::MonsterAreaList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<MonsterArea*>(node));
}

DWORD MonsterAreaDetails::MonstersList::getStyle()
{
    return WS_VISIBLE;
}

int MonsterAreaDetails::MonstersList::getListKind()
{
    return 32781;
}

bool MonsterAreaDetails::MonstersList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"MobID", 50, 18, 'mbid');
        addColumn(L"Count", 50, 18, 'mcnt');
        addColumn(L"Name", 240, 4096, 'name');
        return true;
    }
    return result;
}

void MonsterAreaDetails::MonstersList::onChanged()
{
    m_owner->m_spots->m_document->setModified(true);
}

Node* MonsterAreaDetails::MonstersList::getNode(int index)
{
    if (m_ids)
        return m_ids->get(index);
    return 0;
}

int MonsterAreaDetails::MonstersList::indexOfNode(Node* node)
{
    if (!m_ids)
        return -1;
    return m_ids->indexOf(dynamic_cast<MonsterArea::MonsterID*>(node));
}

void MonsterAreaDetails::MonstersList::setIds(MonsterArea::MonsterIDs* ids)
{
    m_ids = ids;
    int count = ids ? ids->size() : 0;
    setItemCount(count);
    m_owner->m_editId.setText(L"");
    m_owner->m_editCount.setText(L"");
    m_owner->m_btnSetId.enable(false);
}

HRESULT MonsterAreaDetails::MonstersList::onGetDispInfo(NMLVDISPINFOW* di)
{
    if (di->item.iSubItem != 2)
        return ListCtrl::onGetDispInfo(di);
    di->item.pszText = (LPWSTR)L"Unknown";
    Node* node = getNode(di->item.iItem);
    MonsterArea::MonsterID* id = dynamic_cast<MonsterArea::MonsterID*>(node);
    if (id) {
        ptrArray<Node*>* monsters = (ptrArray<Node*>*)m_owner->m_monsters;
        int count = monsters->m_count;
        int i = 0;
        if (count > 0) {
            Node** items = monsters->m_items;
            int mobId = id->m_mobId;
            while (FIELD(int, items[i], 20) != mobId) {
                if (++i >= count)
                    return 0;
            }
            Node* monster = items[i];
            if (monster)
                di->item.pszText = FIELD(LPWSTR, monster, 28);
        }
    }
    return 0;
}

bool MonsterAreaDetails::MonstersList::removeItem(int index)
{
    MonsterArea::MonsterIDs* ids = m_ids;
    if (!ids)
        return false;
    MonsterArea::MonsterID* id = ids->get(index);
    if (!ids->detachItem(id))
        return false;
    ids->destroyItem(id);
    onItemChanged(-1, 0);
    DOC_MODIFIED(m_owner->m_spots);
    return true;
}

void MonsterAreaDetails::MonstersList::onItemChanged(int index, UINT )
{
    if (index >= 0 && m_ids) {
        MonsterArea::MonsterID* id = m_ids->get(index);
        if (id) {
            m_owner->m_editId.setInt(id->m_mobId);
            m_owner->m_editCount.setInt(id->m_count);
            m_owner->m_btnSetId.enable(true);
        }
    } else {
        m_owner->m_editId.setText(L"");
        m_owner->m_editCount.setText(L"");
        m_owner->m_btnSetId.enable(false);
    }
}

bool MonsterAreaDetails::MonstersList::onActivateItem(int index, int )
{
    if (index >= 0 && m_ids) {
        MonsterArea::MonsterID* id = m_ids->get(index);
        if (id) {
            Node* monster = m_owner->m_monsters->findById(id->m_mobId);
            HWND parent = m_hWnd;
            void* models = g_studioFrame->m_client.m_clientData.getMonsterModels(L"monsters");
            editMonster(monster, models, parent, m_owner->m_monsters->m_extended);
        }
    }
    return false;
}

MonsterAreaDetails::MonsterAreaDetails(MapSpots* spots, Collection<MonsterArea>* areas, Monsters* monsters)
    : SpotDetails(spots), m_areas(areas), m_monsters(monsters),
      m_areaList(this, areas), m_monstersList(this),
      m_coords(), m_btnSet(), m_btnAdd(), m_editId(), m_editCount(), m_btnSetId(), m_btnAddId()
{
}

MonsterAreaDetails::~MonsterAreaDetails()
{
}

bool MonsterAreaDetails::onCreate()
{
    RECT client;
    getClientRect(&client);
    int listHeight = client.bottom - client.top - 228;
    m_areaList.create(m_hWnd, (HMENU)0x65, 30, 15, 370, listHeight, 0x10800000, 0, 0);
    m_areaList.setItemCount(m_areas->size());
    int y = listHeight + 27;
    ::createLabel(m_hWnd, L"X1:", 40, y + 2, 20, 14);
    createFloatEdit(m_coords[0], m_hWnd, 60, y, 60, 18, 0x6F);
    ::createLabel(m_hWnd, L"Y1:", 140, y + 2, 20, 14);
    createFloatEdit(m_coords[1], m_hWnd, 160, y, 60, 18, 0x70);
    ::createLabel(m_hWnd, L"Z1:", 230, y + 2, 20, 14);
    createFloatEdit(m_coords[2], m_hWnd, 260, y, 60, 18, 0x71);
    int y2 = y + 24;
    ::createLabel(m_hWnd, L"X2:", 40, y2 + 2, 20, 14);
    createFloatEdit(m_coords[3], m_hWnd, 60, y2, 60, 18, 0x79);
    ::createLabel(m_hWnd, L"Y2:", 140, y2 + 2, 20, 14);
    createFloatEdit(m_coords[4], m_hWnd, 160, y2, 60, 18, 0x7A);
    ::createLabel(m_hWnd, L"Z2:", 230, y2 + 2, 20, 14);
    createFloatEdit(m_coords[5], m_hWnd, 260, y2, 60, 18, 0x7B);
    ::createButton(m_btnSet, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 340, y2 - 26, 60, 22, 0x83);
    ::createButton(m_btnAdd, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, y2 - 2, 60, 22, 0x84);
    int y3 = y2 + 30;
    m_monstersList.create(m_hWnd, (HMENU)0xC9, 30, y3, 370, 96, 0x10800000, 0, 0);
    int y4 = y3 + 108;
    ::createLabel(m_hWnd, L"ID:", 40, y4 + 2, 20, 14);
    m_editId.createNumber(m_hWnd, (HMENU)301, 60, y4, 60, 18, 0, 0, 0);
    ::createLabel(m_hWnd, L"Count:", 140, y4 + 2, 40, 14);
    m_editCount.createNumber(m_hWnd, (HMENU)302, 180, y4, 40, 18, 0, 0, 0);
    ::createButton(m_btnSetId, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 270, y4 - 2, 60, 22, 0xD3);
    ::createButton(m_btnAddId, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, y4 - 2, 60, 22, 0xD4);
    return true;
}

static void readArea(MonsterAreaDetails* page, MonsterArea* area)
{
    SPOT_X(area) = page->m_coords[0].getFloat();
    SPOT_Y(area) = page->m_coords[1].getFloat();
    SPOT_Z(area) = page->m_coords[2].getFloat();
    area->m_x2 = page->m_coords[3].getFloat();
    area->m_y2 = page->m_coords[4].getFloat();
    area->m_z2 = page->m_coords[5].getFloat();
}

bool MonsterAreaDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    switch (id) {
    case 0x83: {
        MonsterArea* area = m_areas->get(m_areaList.getNextSelected(0));
        if (!area)
            return true;
        readArea(this, area);
        m_areaList.updateItem(-2);
        destroySpotWindow(m_spots, area);
        addAreaWindow(m_spots, area, 0);
        DOC_MODIFIED(m_spots);
        return true;
    }
    case 0x84: {
        MonsterArea* area = new MonsterArea(m_areas->size(), (int)m_areas);
        readArea(this, area);
        m_areas->add(area);
        m_areaList.setItemCount(m_areas->size());
        DOC_MODIFIED(m_spots);
        addAreaWindow(m_spots, area, (HMENU)(UINT_PTR)(m_areas->size() + 1001));
        return true;
    }
    case 0xD3: {
        MonsterArea* area = m_areas->get(m_areaList.getNextSelected(0));
        if (!area)
            return true;
        MonsterArea::MonsterID* mob = area->m_ids.get(m_monstersList.getNextSelected(0));
        if (mob) {
            mob->m_mobId = m_editId.getInt(0);
            mob->m_count = m_editCount.getInt(0);
        }
        m_monstersList.updateItem(-2);
        DOC_MODIFIED(m_spots);
        return true;
    }
    case 0xD4: {
        MonsterArea* area = m_areas->get(m_areaList.getNextSelected(0));
        if (!area)
            return true;
        MonsterArea::MonsterID* mob = new MonsterArea::MonsterID();
        mob->m_mobId = m_editId.getInt(0);
        mob->m_count = m_editCount.getInt(0);
        area->m_ids.add(mob);
        m_monstersList.setItemCount(area->m_ids.size());
        DOC_MODIFIED(m_spots);
        return true;
    }
    default:
        return Container::onCommand(code, ctrl, id);
    }
}

void MonsterAreaDetails::showSpots()
{
    m_spots->destroySpots();
    int count = m_areas->size();
    for (int i = 0; i < count; ++i)
        addAreaWindow(m_spots, m_areas->m_items[i], (HMENU)(UINT_PTR)(i + 1001));
}

void MonsterAreaDetails::selectSpot(Spot* spot)
{
    displaySpot(spot);
    MonsterArea* area = dynamic_cast<MonsterArea*>(spot);
    m_areaList.selectItem(m_areas->indexOf(area));
}

void MonsterAreaDetails::displaySpot(Spot* spot)
{
    MonsterArea* area = dynamic_cast<MonsterArea*>(spot);
    if (area) {
        m_coords[0].setFloat(SPOT_X(area));
        m_coords[1].setFloat(SPOT_Y(area));
        m_coords[2].setFloat(SPOT_Z(area));
        m_coords[3].setFloat(area->m_x2);
        m_coords[4].setFloat(area->m_y2);
        m_coords[5].setFloat(area->m_z2);
        m_btnSet.enable(true);
        m_monstersList.setIds(&area->m_ids);
    } else {
        for (int i = 0; i < 6; ++i)
            m_coords[i].setText(L"");
        m_btnSet.enable(false);
        m_monstersList.m_ids = 0;
        m_monstersList.setItemCount(0);
        m_monstersList.m_owner->m_editId.setText(L"");
        m_monstersList.m_owner->m_editCount.setText(L"");
        m_monstersList.m_owner->m_btnSetId.enable(false);
    }
}
