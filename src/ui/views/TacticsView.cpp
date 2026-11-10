#include "TacticsView.h"
#include "TacticsZones.h"
#include "TacticsZone.h"
#include "ZoneEditor.h"
#include "ida_compat.h"
#include "_imports_p06.h"

TacticsView::TacticsView(FileTree* data, TacticsZones* zones)
    : DocView(data, static_cast<Serializable*>(zones))
    , m_list(this, zones)
{
    if (zones) {
        m_zones = zones;
    } else {
        TacticsZones* created = new TacticsZones();
        m_zones = created;
        m_list.m_zones = created;
        Serializable* content = static_cast<Serializable*>(created);
        m_content = content;
        m_object = content;
    }
    m_searchEnabled = false;
    m_decrypt = false;
}

TacticsView::~TacticsView()
{
}

bool TacticsView::onCreate()
{
    if (!DocView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT client;
    RECT* rc = getClientRect(&client);
    if (!m_list.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
                           0x50000000, 0, 0))
        return false;
    if (m_zones)
        m_list.setItemCount(m_zones->size());
    return true;
}

void TacticsView::editZone(int index)
{
    if (!m_zones)
        return;
    if (index < 0 || index >= m_zones->size())
        return;
    TacticsZone* zone = m_zones->get(index);
    if (!zone)
        return;
    ZoneEditor editor(zone);
    if (editor.create(m_hWnd)) {
        if (editor.doModal() == 1)
            setModified(true);
    }
}
