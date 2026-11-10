#include <windows.h>
#include <typeinfo>
#include "SqlListView.h"
#include "SqlNode.h"
#include "Table.h"
#include "RecordSet.h"
#include "Collection.hpp"
#include "_imports_p09.h"
#include "_imports_g4.h"

SqlListView::~SqlListView()
{
}

bool SqlListView::loadRows(const wchar_t* orderBy)
{
    RecordSet rs = m_table->select(L"*", orderBy);
    m_schema.build(m_table, this);
    while (rs.next())
    {
        SqlNode* node = new SqlNode(&m_schema);
        node->load(rs);
        m_nodes.add(node);
    }
    setItemCount(m_nodes.size());
    return true;
}

bool SqlListView::onCreate()
{
    return ListCtrl::onCreate() != 0;
}

Node* SqlListView::getNode(int index)
{
    return m_nodes.get(index);
}

int SqlListView::indexOfNode(Node* node)
{
    return m_nodes.indexOf(dynamic_cast<SqlNode*>(node));
}

bool SqlListView::sort()
{
    int previousColumn = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = m_nodes.m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    m_nodes.m_changes = 0;
    quickSort(m_nodes, 0, count - 1, column, previousColumn, ascending);
    return m_nodes.m_changes > 0;
}
