#include "View3DInfo.h"
#include "View3DInfo_Coord.h"
#include "Drawable.h"
#include "ida_compat.h"
#include "_imports_p06.h"

static inline void createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(1), 0);
}

bool View3DInfo::vslot78(HWND parent)
{
    m_buttons = 0;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 320, 380);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"3D Item Information"))
        return false;
    HWND owner = Modal_getTopLevelWindow(parent);
    m_owner = owner;
    Window_centerOn(m_hWnd, owner);
    show(true);
    return true;
}

bool View3DInfo::onCreate()
{
    CoordsList& coordsList = FIELD(CoordsList, this, 0xAC);
    Coords& coords = FIELD(Coords, this, 0x1A0);
    ItemsList& itemsList = FIELD(ItemsList, this, 0x1B8);

    createLabel(m_hWnd, L"Bounding Box", 10, 10, 300, 18);
    int height = dword_5A0124 + 3 + dword_5A0120 + 2 * (dword_5A0124 + 3);
    if (!coordsList.create(m_hWnd, (HMENU)0x65, 10, 28, 280, height, 0x10800000, 0, 0))
        return false;
    int y = height + 46;
    coordsList.addColumn(L"Axe", 40, 0, 'axe ');
    coordsList.addColumn(L"Min", 70, 130, 'min ');
    coordsList.addColumn(L"Max", 70, 130, 'max ');
    coordsList.addColumn(L"Ampl", 70, 130, 'ampl');
    FIELD(Drawable*, this, 0x2AC)->computeBounds();
    Drawable* d = FIELD(Drawable*, this, 0x2AC);
    coords.add(new Coord(L"X", FIELD(float, d, 0x40), FIELD(float, d, 0x50)));
    coords.add(new Coord(L"Y", FIELD(float, d, 0x44), FIELD(float, d, 0x54)));
    coords.add(new Coord(L"Z", FIELD(float, d, 0x48), FIELD(float, d, 0x58)));
    coordsList.setItemCount(3);
    createLabel(m_hWnd, L"Properties", 10, y, 300, 18);
    if (!itemsList.create(m_hWnd, (HMENU)0x66, 10, y + 18, 280, dword_5A0120 + 4 * (dword_5A0124 + 1),
                          0x10800000, 0, 0))
        return false;
    itemsList.addColumn(L"Property", 175, 0, 'prop');
    itemsList.addColumn(L"Value", 75, 18, 'valu');
    FIELD(Drawable*, this, 0x2AC)->fillProperties(&itemsList);
    return Modal::onCreate();
}
