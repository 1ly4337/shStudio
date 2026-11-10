#include "SpotPage_p05.h"
#include "NamedAreaDetails.h"
#include "Document.h"

static void addNamedAreaWindow(MapSpots* spots, NamedArea* area, HMENU id)
{
    float scale = spots->m_scale;
    int height = spots->m_height;
    RECT rc;
    rc.left = (int)(SPOT_X(area) * scale);
    rc.right = (int)(area->m_x2 * scale);
    rc.top = height - (int)(area->m_z2 * scale);
    rc.bottom = height - (int)(SPOT_Z(area) * scale);
    HWND wnd = MapSpots::createSpotWindow(spots->m_hWnd, id, &rc, area);
    spots->m_spotWnds.add(wnd);
}

DWORD NamedAreaDetails::NamedAreaList::getStyle()
{
    return 13;
}

bool NamedAreaDetails::NamedAreaList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 40, 18, 'indx');
        addColumn(L"X1", 60, 130, 'posx');
        addColumn(L"Z1", 60, 130, 'posz');
        addColumn(L"X2", 60, 130, 'psx2');
        addColumn(L"Z2", 60, 130, 'psz2');
        return true;
    }
    return result;
}

int NamedAreaDetails::NamedAreaList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<NamedArea*>(node));
}

NamedAreaDetails::NamedAreaDetails(MapSpots* spots, Collection<NamedArea>* areas)
    : SpotDetails(spots), m_areas(areas), m_list(this, areas),
      m_coords(), m_param1(), m_param2(), m_btnSet(), m_btnAdd()
{
}

NamedAreaDetails::~NamedAreaDetails()
{
}

bool NamedAreaDetails::onCreate()
{
    RECT client;
    getClientRect(&client);
    int listHeight = client.bottom - client.top - 234;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, listHeight, 0x10800000, 0, 0);
    m_list.setItemCount(m_areas->size());
    int y = listHeight + 27;
    ::createLabel(m_hWnd, L"X1:", 40, y + 2, 20, 14);
    createFloatEdit(m_coords[0], m_hWnd, 60, y, 60, 18, 0x6F);
    ::createLabel(m_hWnd, L"Y1:", 140, y + 2, 20, 14);
    createFloatEdit(m_coords[1], m_hWnd, 160, y, 60, 18, 0x70);
    ::createLabel(m_hWnd, L"Z1:", 230, y + 2, 20, 14);
    createFloatEdit(m_coords[2], m_hWnd, 260, y, 60, 18, 0x71);
    int y2 = y + 24;
    ::createLabel(m_hWnd, L"X2:", 40, y2 + 2, 20, 14);
    createFloatEdit(m_coords[3], m_hWnd, 60, y2, 60, 18, 0x72);
    ::createLabel(m_hWnd, L"Y2:", 140, y2 + 2, 20, 14);
    createFloatEdit(m_coords[4], m_hWnd, 160, y2, 60, 18, 0x73);
    ::createLabel(m_hWnd, L"Z2:", 230, y2 + 2, 20, 14);
    createFloatEdit(m_coords[5], m_hWnd, 260, y2, 60, 18, 0x74);
    int y3 = y2 + 24;
    ::createLabel(m_hWnd, L"Extra Parameters:", 40, y3 + 2, 120, 14);
    m_param1.createNumber(m_hWnd, (HMENU)120, 160, y3, 60, 18, 0, 0, 0);
    HWND dash = CreateWindowExW(0, L"Static", L"-", WS_CHILD | WS_VISIBLE | SS_CENTER, 220, y3 + 2, 40, 14,
                                m_hWnd, 0, 0, 0);
    if (dash)
        SendMessageW(dash, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_param2.createNumber(m_hWnd, (HMENU)121, 260, y3, 60, 18, 0, 0, 0);
    ::createButton(m_btnSet, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 340, y3 - 38, 60, 22, 0x82);
    ::createButton(m_btnAdd, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, y3 - 14, 60, 22, 0x83);
    return true;
}

static void readNamedArea(NamedAreaDetails* page, NamedArea* area)
{
    SPOT_X(area) = page->m_coords[0].getFloat();
    SPOT_Y(area) = page->m_coords[1].getFloat();
    SPOT_Z(area) = page->m_coords[2].getFloat();
    area->m_x2 = page->m_coords[3].getFloat();
    area->m_y2 = page->m_coords[4].getFloat();
    area->m_z2 = page->m_coords[5].getFloat();
    area->m_unk1 = page->m_param1.getInt(-1);
    area->m_unk2 = page->m_param2.getInt(-1);
}

bool NamedAreaDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 0x82) {
        NamedArea* area = m_areas->get(m_list.getNextSelected(0));
        if (!area)
            return true;
        readNamedArea(this, area);
        m_list.updateItem(-2);
        destroySpotWindow(m_spots, area);
        addNamedAreaWindow(m_spots, area, 0);
        m_spots->m_document->setModified(true);
        return true;
    }
    if (id == 0x83) {
        NamedArea* area = new NamedArea(m_areas->size(), (int)m_areas);
        readNamedArea(this, area);
        m_areas->add(area);
        m_list.setItemCount(m_areas->size());
        m_list.invalidate(true);
        m_spots->m_document->setModified(true);
        return true;
    }
    return Container::onCommand(code, ctrl, id);
}

void NamedAreaDetails::showSpots()
{
    int count = m_areas->size();
    for (int i = 0; i < count; ++i)
        addNamedAreaWindow(m_spots, m_areas->m_items[i], (HMENU)(UINT_PTR)(i + 1001));
}

void NamedAreaDetails::selectSpot(Spot* spot)
{
    displaySpot(spot);
    NamedArea* area = dynamic_cast<NamedArea*>(spot);
    m_list.selectItem(m_areas->indexOf(area));
}

void NamedAreaDetails::displaySpot(Spot* spot)
{
    NamedArea* area = dynamic_cast<NamedArea*>(spot);
    if (area) {
        m_coords[0].setFloat(SPOT_X(area));
        m_coords[1].setFloat(SPOT_Y(area));
        m_coords[2].setFloat(SPOT_Z(area));
        m_coords[3].setFloat(area->m_x2);
        m_coords[4].setFloat(area->m_y2);
        m_coords[5].setFloat(area->m_z2);
        m_param1.setInt(area->m_unk1);
        m_param2.setInt(area->m_unk2);
        m_btnSet.enable(true);
    } else {
        for (int i = 0; i < 6; ++i)
            m_coords[i].setText(L"");
        m_param1.setText(L"");
        m_param2.setText(L"");
        m_btnSet.enable(false);
    }
}
