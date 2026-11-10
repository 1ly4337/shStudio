#include "Globals.h"
#include "_imports_p10.h"

void** Globals::resetItem(void** item)
{
    if (*item)
        GlobalFree((HGLOBAL)*item);
    *item = 0;
    return item;
}
