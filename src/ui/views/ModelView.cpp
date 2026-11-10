#include "ModelView.h"
#include "PopupView3D.h"
#include "Control.hpp"
#include "Preferences.h"
#include "fPoint.h"
#include "_imports_p07.h"

ModelView::ModelView(FileTree* data, Serializable* content, Control* child)
    : DocView(data, content)
{
    m_child = child;
    m_selected = -1;
    m_targetY = 0;
    m_searchEnabled = false;
    m_decrypt = false;
    m_showModels = true;
}

bool ModelView::onCreate()
{
    bool result = DocView::onCreate();
    if (result)
    {
        RECT rc;
        SetRectEmpty(&rc);
        GetWindowRect(m_hWnd, &rc);
        rc.left = rc.right;
        rc.right += rc.bottom - rc.top;
        m_view3D.vslot72(m_hWnd, &rc);
        fPoint target(0.0f, m_targetY, 0.0f);
        getViewTarget(target);
        FIELD(fPoint, &m_view3D, 0x374) = target;
        if (m_showModels)
        {
            if (g_preferences.getInt(L"3D View", L"show models", 0) == 1)
                placeView3D();
        }
        return true;
    }
    return result;
}

void ModelView::onActivate(bool active)
{
    Control* child = m_child;
    if (child)
        child->onActivate(active);
}

void ModelView::onEnable(bool enabled)
{
    if (m_view3D.isVisible())
        m_view3D.enable(enabled);
}

bool ModelView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return false;
    if (id != 58374 || !m_showModels)
        return DocView::onCommand(code, 0, id);
    if (m_view3D.isVisible())
        m_view3D.show(false);
    else
        placeView3D();
    return true;
}

bool ModelView::onNotify(NMHDR* hdr, LRESULT* result)
{
    if ((int)hdr->code == -121)
    {
        *result = 1;
        return true;
    }
    Control* child = m_child;
    if (child && child->m_id == (int)hdr->idFrom)
        return child->onNotify(hdr, result);
    return false;
}

LRESULT ModelView::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_MOVING)
        return DocView::defWindowProc(msg, wParam, lParam);
    if (m_view3D.isVisible())
    {
        const RECT* rc = (const RECT*)lParam;
        SetWindowPos(m_view3D.m_hWnd, 0, rc->right, rc->top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    return 1;
}

void ModelView::placeView3D()
{
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(m_hWnd, &rc);
    int size = rc.bottom - rc.top;
    if (rc.bottom - rc.top > 450)
        size = 450;
    SetWindowPos(m_view3D.m_hWnd, 0, rc.right, rc.top, size, size,
                 SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    if (m_selected != -1)
        selectModel(m_selected);
}
