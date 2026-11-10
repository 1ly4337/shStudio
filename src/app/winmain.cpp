#include <windows.h>
#include "StudioFrame.h"
#include "MenuBitmaps.h"
#include "_imports_p10.h"
#include "dark_mode.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    darkModeInit();
    initMenuBitmaps();
    {
        StudioFrame frame(hInstance);
        if (frame.open(0, 0, 0))
            frame.run();
    }
    return 0;
}
