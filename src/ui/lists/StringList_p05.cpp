#include "p05_common.h"
#include "StringList.h"
#include "DocView.h"

void StringList::setStrings(Strings* strings)
{
    m_strings = strings;
}

DWORD StringList::getStyle()
{
    return 525;
}

DWORD StringList::getExStyle()
{
    return 33;
}

void StringList::onChanged()
{
    m_view->setModified(true);
}

bool StringList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Index", 45, 16, 'indx');
        addColumn(L"Filename", 200, 0, 'name');
        fill();
        return true;
    }
    return result;
}
