#include "SpotPage_p05.h"
#include "LaddersDetails.h"
#include "Document.h"

DWORD LaddersDetails::LadderList::getStyle()
{
    return 13;
}

bool LaddersDetails::LadderList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 40, 18, 'indx');
        addColumn(L"X", 60, 130, 'posx');
        addColumn(L"Y", 60, 130, 'posy');
        addColumn(L"Z", 60, 130, 'posz');
        return true;
    }
    return result;
}

int LaddersDetails::LadderList::indexOfNode(Node* node)
{
    return m_items->indexOf(dynamic_cast<Ladder*>(node));
}

LaddersDetails::LaddersDetails(MapSpots* spots, Collection<Ladder>* ladders)
    : SpotDetails(spots),
      m_ladders(ladders),
      m_list(this, ladders),
      m_x(), m_y(), m_z(), m_btnSet(), m_btnAdd()
{
}

LaddersDetails::~LaddersDetails()
{
}

bool LaddersDetails::onCreate()
{
    RECT client;
    getClientRect(&client);
    int listHeight = client.bottom - client.top - 234;
    m_list.create(m_hWnd, (HMENU)0x65, 30, 15, 370, listHeight, 0x10800000, 0, 0);
    m_list.setItemCount(m_ladders->size());
    int editY = listHeight + 27;
    int labelY = listHeight + 29;
    ::createLabel(m_hWnd, L"X:", 40, labelY, 20, 14);
    createFloatEdit(m_x, m_hWnd, 60, editY, 60, 18, 0x6F);
    ::createLabel(m_hWnd, L"Y:", 140, labelY, 20, 14);
    createFloatEdit(m_y, m_hWnd, 160, editY, 60, 18, 0x70);
    ::createLabel(m_hWnd, L"Z:", 230, labelY, 20, 14);
    createFloatEdit(m_z, m_hWnd, 260, editY, 60, 18, 0x71);
    ::createButton(m_btnSet, m_hWnd, L"Set", WS_CHILD | WS_VISIBLE, 340, editY, 60, 22, 0x82);
    ::createButton(m_btnAdd, m_hWnd, L"Add", WS_CHILD | WS_VISIBLE, 340, listHeight + 57, 60, 22, 0x83);
    return true;
}

bool LaddersDetails::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 0x82) {
        int index = m_list.getNextSelected(0);
        Ladder* ladder = m_ladders->get(index);
        if (ladder) {
            SPOT_X(ladder) = m_x.getFloat();
            SPOT_Y(ladder) = m_y.getFloat();
            SPOT_Z(ladder) = m_z.getFloat();
            m_list.updateItem(-2);
            destroySpotWindow(m_spots, ladder);
            addPointSpotWindow(m_spots, ladder, 0);
        }
        return true;
    }
    if (id == 0x83) {
        Ladder* ladder = new Ladder(m_ladders->size(), (int)m_ladders);
        SPOT_X(ladder) = m_x.getFloat();
        SPOT_Y(ladder) = m_y.getFloat();
        SPOT_Z(ladder) = m_z.getFloat();
        m_ladders->add(ladder);
        m_list.setItemCount(m_ladders->size());
        m_list.invalidate(true);
        m_spots->m_document->setModified(true);
        addPointSpotWindow(m_spots, ladder, (HMENU)(UINT_PTR)(m_ladders->size() + 1001));
        return true;
    }
    return Container::onCommand(code, ctrl, id);
}

void LaddersDetails::showSpots()
{
    int count = m_ladders->size();
    for (int i = 0; i < count; ++i) {
        Ladder* ladder = m_ladders->m_items[i];
        addPointSpotWindow(m_spots, ladder, (HMENU)(UINT_PTR)(i + 1001));
    }
}

void LaddersDetails::selectSpot(Spot* spot)
{
    displaySpot(spot);
    Ladder* ladder = dynamic_cast<Ladder*>(spot);
    m_list.selectItem(m_ladders->indexOf(ladder));
}

void LaddersDetails::displaySpot(Spot* spot)
{
    Ladder* ladder = dynamic_cast<Ladder*>(spot);
    if (ladder) {
        m_x.setFloat(SPOT_X(ladder));
        m_y.setFloat(SPOT_Y(ladder));
        m_z.setFloat(SPOT_Z(ladder));
        m_btnSet.enable(true);
    } else {
        m_x.setText(L"");
        m_y.setText(L"");
        m_z.setText(L"");
        m_btnSet.enable(false);
    }
}
