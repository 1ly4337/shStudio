#include "AltList.h"
#include "AltView.h"
#include "AltEditor.h"
#include "CharActions.h"
#include "CharAction.h"
#include "Collection.hpp"
#include "_imports_p06.h"

DWORD AltList::getStyle()
{
    return 1;
}

bool AltList::onCreate()
{
    if (!m_data || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"Indx", 45, 16, 'indx');
    addColumn(L"Anim", 175, 0, 'name');
    addColumn(L"Mode", 50, 18, 'mode');
    addColumn(L"Byte 1", 50, 18, 'byt1');
    addColumn(L"Byte 2", 50, 18, 'byt2');
    addColumn(L"Byte 3", 50, 18, 'byt3');
    addColumn(L"Long", 60, 18, 'unk1');
    addColumn(L"Flt 1", 60, 130, 'flt1');
    addColumn(L"Flt 2", 60, 130, 'flt2');
    addColumn(L"Flt 3", 60, 130, 'flt3');
    addColumn(L"Flt 4", 60, 130, 'flt4');
    setItemCount(m_data->size());
    return true;
}

void AltList::onChanged()
{
    m_view->setModified(1);
}

DWORD AltList::getExStyle()
{
    return 0x21;
}

bool AltList::sort()
{
    CharActions* data = m_data;
    int prevColumn = m_prevSortColumn;
    bool ascending = m_sortAscending;
    int count = data->m_count;
    int column = m_sortColumn;
    if (count < 2)
        return false;
    data->m_changes = 0;
    quickSort(*data, 0, count - 1, column, prevColumn, ascending);
    return data->m_changes > 0;
}

bool AltList::onActivateItem(int index, int keys)
{
    if (!m_data)
        return false;
    if (index < 0)
        return false;
    if (index >= m_data->size())
        return false;
    CharAction* action = m_data->get(index);
    if (!action)
        return false;
    AltEditor editor(action);
    if (editor.create(m_hWnd))
    {
        if (editor.doModal() == IDOK)
            onChanged();
    }
    return true;
}

bool AltList::clearItem(int index)
{
    m_data->get(index)->reset();
    return true;
}
