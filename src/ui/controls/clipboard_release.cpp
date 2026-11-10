#include "clipboard_release.h"
#include "Clipboard.h"
#include "_imports_p10.h"

void Clipboard_releaseGlobals()
{
    Globals& globals = g_clipboard.m_globals;
    for (int i = 0; i < globals.m_capacity; ++i)
        globals.resetItem(&globals.m_items[i]);
    globals.m_count = 0;
}
