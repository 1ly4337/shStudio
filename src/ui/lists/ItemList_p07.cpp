#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "ItemList.h"
#include "ComboBox.h"
#include "Data_Items.h"
#include "_imports_p07.h"

int ItemList::addTypeItem(ComboBox* combo, int type, const wchar_t* name)
{
    int count = 0;
    int n = m_items->m_count;
    if (n > 0) {
        Data::Item** items = m_items->m_items;
        do {
            if (FIELD(BYTE, *items, 0x54) == type)
                ++count;
            ++items;
            --n;
        } while (n);
        if (count >= 255)
            return 0;
    }
    wchar_t buffer[256];
    memset(buffer, 0, sizeof(buffer));
    swprintf_s(buffer, 0x100u, L"Type %u: %s", type, name);
    return combo->addItem(buffer, type);
}
