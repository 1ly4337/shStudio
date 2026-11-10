#include "Doc.h"
#include "Application.h"
#include "_imports_p10.h"

void Doc::onDestroy()
{
    SendMessageW(g_hMainWnd, 0x404, 0, (LPARAM)this);
}
