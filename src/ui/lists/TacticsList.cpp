#include "TacticsList.h"
#include "TacticsView.h"
#include "TacticsZones.h"
#include "TacticsZone.h"
#include "Node.h"
#include "ida_compat.h"
#include "_imports_p06.h"

bool TacticsList::onCreate()
{
    bool ok = ListCtrlOD::onCreate();
    if (!ok)
        return ok;
    addColumn(L"Indx", 40, 18, 'indx');
    addColumn(L"Map", 40, 18, 'map ');
    addColumn(L"X1", 60, 130, 'psX1');
    addColumn(L"Z1", 60, 130, 'psZ1');
    addColumn(L"X2", 60, 130, 'psX2');
    addColumn(L"Z2", 60, 130, 'psZ2');
    addColumn(L"X3", 60, 130, 'psX3');
    addColumn(L"Z3", 60, 130, 'psZ3');
    addColumn(L"X4", 60, 130, 'psX4');
    addColumn(L"Z4", 60, 130, 'psZ4');
    addColumn(L"P1", 40, 18, 'unk1');
    addColumn(L"P2", 40, 18, 'unk2');
    addColumn(L"Desc", 160, 0, 'desc');
    return true;
}

int TacticsList::insertItem(Node* item, int index)
{
    return m_zones->insert(dynamic_cast<TacticsZone*>(item), index);
}

Node* TacticsList::getNode(int index)
{
    if (m_zones)
        return m_zones->get(index);
    return 0;
}

bool TacticsList::sort()
{
    TacticsZones* zones = m_zones;
    if (!zones)
        return false;
    bool ascending = m_sortAscending;
    int column = m_prevSortColumn;
    int key = m_sortColumn;
    int count = zones->m_count;
    if (count < 2)
        return false;
    zones->m_changes = 0;
    zones->sort(0, count - 1, key, column, ascending);
    return zones->m_changes > 0;
}

void TacticsList::onItemChanged(int index, UINT )
{
    bool enable = index != -1;
    m_owner->enableMenuItem(0xE123, enable);
    m_owner->enableMenuItem(0xE122, enable);
    m_owner->enableMenuItem(0xE12A, true);
    m_owner->enableMenuItem(0xE12B, enable);
    m_owner->enableMenuItem(0xE12C, enable);
}

Node* TacticsList::createNode()
{
    return new TacticsZone(m_zones->size(), (int)(INT_PTR)m_zones);
}

int TacticsList::indexOfNode(Node* item)
{
    if (!m_zones)
        return -1;
    return m_zones->indexOf(dynamic_cast<TacticsZone*>(item));
}
