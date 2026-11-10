#include "Options.h"
#include "OptionPanel.h"
#include "FolderOption.h"
#include "ItemOption.h"
#include "SDataFileOption.h"
#include "View3DOption.h"
#include "FontOption.h"
#include "_imports_p07.h"

bool Options::createPanels()
{
    RECT rc;
    SetRect(&rc, 20, 60, 330, 260);

    FolderOption* folders = new FolderOption;
    if (m_hWnd && folders->create(m_hWnd, (HMENU)(UINT_PTR)'fold', &rc, 0x42800000, WS_EX_CONTROLPARENT, 0))
        addPanel(folders, L"Folders");
    else if (folders)
        delete folders;

    ItemOption* items = new ItemOption;
    if (m_hWnd && items->create(m_hWnd, (HMENU)(UINT_PTR)'item', &rc, 0x42800000, WS_EX_CONTROLPARENT, 0))
        addPanel(items, L"Items Description");
    else if (items)
        delete items;

    SDataFileOption* sdata = new SDataFileOption;
    if (m_hWnd && sdata->create(m_hWnd, (HMENU)(UINT_PTR)'sdta', &rc, 0x42800000, WS_EX_CONTROLPARENT, 0))
        addPanel(sdata, L"DB SData File");
    else if (sdata)
        delete sdata;

    View3DOption* views = new View3DOption;
    if (m_hWnd && views->create(m_hWnd, (HMENU)(UINT_PTR)'view', &rc, 0x42800000, WS_EX_CONTROLPARENT, 0))
        addPanel(views, L"3D Views");
    else if (views)
        delete views;

    FontOption* fonts = new FontOption;
    if (m_hWnd && fonts->create(m_hWnd, (HMENU)(UINT_PTR)'font', &rc, 0x42800000, WS_EX_CONTROLPARENT, 0))
        addPanel(fonts, L"Fonts");
    else if (fonts)
        delete fonts;
    return true;
}
