#include "PcTab.h"
#include "PcView.h"
#include "CapeModels.h"
#include "_imports_p07.h"

PcTab::PcTab(PcView* view)
    : m_view(view), m_modelList(view), m_capeList(view), m_shoulderList(view)
{
    m_models = 0;
}

PcTab::~PcTab()
{
}

HWND PcTab::getPage(int index)
{
    switch (index)
    {
    case 0:
        return m_modelList.m_hWnd;
    case 1:
        return m_capeList.m_hWnd;
    case 2:
        return m_shoulderList.m_hWnd;
    }
    return 0;
}

DWORD PcTab::getTabStyle()
{
    return 1024;
}

bool PcTab::createPages(RECT* rc)
{
    CapeModels* models = m_models;
    if (!models)
        return false;
    m_modelList.m_capes = &models->m_capes;
    m_capeList.setStrings(&models->m_capeFiles);
    m_shoulderList.setStrings(&models->m_shoulderFiles);

    RECT tmp;
    CopyRect(rc, getClientRect(&tmp));
    HWND parent = m_hWnd;
    if (!m_modelList.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left,
                                rc->bottom - rc->top, 0, 0, 0)
        || !m_capeList.create(parent, (HMENU)0x66, rc->left, rc->top, rc->right - rc->left,
                                  rc->bottom - rc->top, 0, 0, 0)
        || !m_shoulderList.create(parent, (HMENU)0x67, rc->left, rc->top, rc->right - rc->left,
                                      rc->bottom - rc->top, 0, 0, 0))
    {
        return false;
    }
    addPage(L"Models", m_modelList.m_hWnd, -1);
    addPage(L"Capes", m_capeList.m_hWnd, -1);
    addPage(L"Shoulders", m_shoulderList.m_hWnd, -1);
    return true;
}
