#include "MapHeightView.h"
#include "Application.h"
#include "_imports_p06.h"

MapHeightView::MapHeightView(MapHeight* doc)
    : PopupView3D()
    , m_doc(doc)
    , m_editMode(true)
{
}

MapHeightView::~MapHeightView()
{
}

bool MapHeightView::open()
{
    RECT buffer;
    RECT empty;
    const RECT* frame;
    if (g_mainFrame)
    {
        frame = g_mainFrame->getClientRect(&buffer);
    }
    else
    {
        SetRectEmpty(&empty);
        frame = &empty;
    }
    RECT rc = *frame;
    rc.left += 70;
    rc.top += 60;
    rc.right = rc.left + 700;
    rc.bottom = rc.top + 480;
    bool result = PopupView3D::vslot72(0, &rc);
    if (result)
    {
        setText(L"3D Map-Height");
        return true;
    }
    return result;
}
