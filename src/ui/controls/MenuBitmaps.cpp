#include "MenuBitmaps.h"
#include "menus.h"
#include "_imports_p10.h"
#include <cstring>

HTable<unsigned int, HBITMAP> g_menuBitmaps(32, 32);

static void registerMenuIcon(WORD iconId, UINT commandId)
{
    HICON icon = (HICON)LoadImageW(GetModuleHandleW(0), MAKEINTRESOURCEW(iconId), IMAGE_ICON, 16, 16, 0);
    HBITMAP bitmap;
    if (icon)
    {
        bitmap = iconToBitmapPARGB32(icon);
        DeleteObject(icon);
    }
    else
    {
        bitmap = 0;
    }
    g_menuBitmaps.set(commandId, bitmap);
}

bool initMenuBitmaps()
{
    registerMenuIcon(0xC4, 0xE120);
    registerMenuIcon(0xC5, 0xE121);
    registerMenuIcon(0xC6, 0xE122);
    registerMenuIcon(0xC7, 0xE123);
    registerMenuIcon(0xC8, 0xE124);
    registerMenuIcon(0xC9, 0xE125);
    registerMenuIcon(0xCA, 0xE128);
    registerMenuIcon(0xCB, 0xE12A);
    registerMenuIcon(0xCC, 0xE12B);
    registerMenuIcon(0xCD, 0xE12C);
    registerMenuIcon(0xCE, 0xE102);
    registerMenuIcon(0xCF, 0xE103);
    registerMenuIcon(0xD0, 0xE11E);
    registerMenuIcon(0xD1, 0xE105);
    return true;
}

BOOL setMenuItemBitmap(HMENU menu, UINT item)
{
    MENUITEMINFOW mii;
    memset(&mii, 0, sizeof(mii));
    int i = 0;
    mii.cbSize = 48;
    mii.fMask = MIIM_BITMAP;
    HBITMAP bitmap = 0;
    if (g_menuBitmaps.m_count > 0)
    {
        while (g_menuBitmaps.m_table[i].key != item)
        {
            if (++i >= g_menuBitmaps.m_count)
                goto done;
        }
        if (i != -1)
            bitmap = g_menuBitmaps.m_table[i].value;
    }
done:
    mii.hbmpItem = bitmap;
    return SetMenuItemInfoW(menu, item, FALSE, &mii);
}
