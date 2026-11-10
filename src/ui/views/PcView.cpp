#include "PcView.h"
#include "CapeModels.h"
#include "CapeDataEditor.h"
#include "Application.h"
#include "_imports_p07.h"

PcView::PcView(FileTree* data)
    : DocView(data, 0), m_tab(this)
{
    CapeModels* models = new CapeModels;
    m_models = models;
    m_content = models;
    m_object = static_cast<Serializable*>(models);
    m_tab.m_models = models;
}

PcView::~PcView()
{
}

LPCWSTR PcView::getClassName()
{
    return L"PcView";
}

void PcView::onRegisterClass(WNDCLASSEXW* wcx)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wcx->hIcon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x84), IMAGE_ICON, cx, cy, LR_SHARED);
    int cySmall = GetSystemMetrics(SM_CYSMICON);
    int cxSmall = GetSystemMetrics(SM_CXSMICON);
    wcx->hIconSm = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x84), IMAGE_ICON, cxSmall, cySmall, LR_SHARED);
}

int PcView::getMenuId()
{
    return 172;
}

bool PcView::onCreate()
{
    if (!DocView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT tmp;
    RECT* rc = getClientRect(&tmp);
    if (!m_tab.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
                      0x50000000, 0, 0))
        return false;
    CapeModels* models = m_tab.m_models;
    if (models)
        m_tab.m_modelList.setItemCount(models->m_capes.size());
    return true;
}

void PcView::editModel(int index)
{
    CapeData* cape = m_models->m_capes.get(index);
    if (!cape)
        return;
    CapeDataEditor editor(cape);
    if (editor.create(m_hWnd))
    {
        if (editor.doModal())
            setModified(true);
    }
}
