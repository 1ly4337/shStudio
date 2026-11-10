#include "ThreeDxView.h"
#include "_imports_p06.h"

bool ThreeDxView::onClose()
{
    g_activeGlView = 0;
    return Doc::onClose();
}

bool ThreeDxView::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (GetKeyState(VK_CONTROL) < 0 && (short)vk == 'C')
    {
        onCommand(0, 0, 0xE123);
        return true;
    }
    if (GetKeyState(VK_CONTROL) < 0 && (short)vk == 'W')
    {
        onCommand(0, 0, 0xE102);
        return true;
    }
    return false;
}
