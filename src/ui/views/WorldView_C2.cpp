#include "WorldView.h"
#include "WorldList.h"
#include "World.h"
#include "PopupView3D.h"
#include "glViewer.h"
#include "Drawable.h"
#include "StdTextView.h"
#include "fPoint.h"
#include "String.hpp"
#include "world_common.h"

#define WV_LIST    m_list
#define WV_WORLD   m_world
#define WV_PREVIEW m_popup

void WorldView::vslot100()
{
    m_558 = -1;
    WV_LIST.deleteAllItems();
    m_modified = false;
}

bool WorldView::onCreate()
{
    if (!DocView::onCreate())
        return false;

    HWND parent = m_hWnd;
    RECT rc;
    if (!WV_LIST.create(parent, (HMENU)0x65, getClientRect(&rc), WS_CHILD | WS_VISIBLE, WS_EX_WINDOWEDGE, 0))
        return false;

    WV_LIST.populate(FIELD(LPCWSTR, m_data, 0x34));

    RECT frame;
    getWindowRect(&frame);
    frame.left = frame.right;
    frame.right += frame.bottom - frame.top;
    WV_PREVIEW.vslot72(m_hWnd, &frame);
    m_modified = false;
    return true;
}

bool WorldView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return false;

    if (id == 0xE406)
    {
        if (WV_PREVIEW.isVisible())
        {
            WV_PREVIEW.show(false);
            return true;
        }
        vslot101();
        return true;
    }
    if (id != 0xE412 && id != 0xE411)
        return DocView::onCommand(code, 0, id);

    StdTextView* text = new StdTextView();
    if (!text->openWindow())
    {
        delete text;
        return true;
    }
    String dump;
    WV_WORLD.toText(dump, 'dump');
    text->setContent(dump);
    text->show(true);
    return true;
}

void WorldView::vslot101()
{
    RECT rc;
    getWindowRect(&rc);
    SetWindowPos(WV_PREVIEW.m_hWnd, 0, rc.right, rc.top, rc.bottom - rc.top, rc.bottom - rc.top,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOACTIVATE);
    WV_PREVIEW.activate(true);
}

void WorldView::vslot102(Drawable* model)
{
    PopupView3D& preview = WV_PREVIEW;
    if (!preview.isVisible())
        return;

    fPoint position(0.0f, 0.0f, -2.5f);
    preview.clearModel();
    {
        fPoint rotation;
        preview.vslot73(&rotation);
    }
    preview.vslot74(&position);

    glViewer& viewer = preview.m_viewer;
    Drawable* drawable = viewer.m_drawable;
    viewer.m_renderMode = 1;
    if (drawable && (drawable->m_texture == 0 || drawable->m_texture == (GLuint)-1))
        drawable->loadTexture(0);
    preview.setModel(model);
}
