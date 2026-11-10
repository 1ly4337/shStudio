#include "DualLayerEditor.h"
#include "DualLayerModel.h"
#include "gdi_cache.h"
#include "_imports_p06.h"

#include "StudioFrame.h"
#include "Modal.h"
#include "Window.h"

namespace {

void createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy)
{
    HWND label = CreateWindowExW(0, L"Static", text, 0x50000000, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
}

}

DualLayerEditor::DualLayerEditor(LPCWSTR title, DualLayerModel* model)
    : Modal()
    , m_title(title)
    , m_model(model)
{
}

DualLayerEditor::~DualLayerEditor()
{
}

bool DualLayerEditor::create(HWND parent)
{
    m_readOnly = g_studioFrame->isArchiveReadOnly();
    RECT rc;
    m_buttons = 1;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 380, 200);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, m_title))
        return false;
    HWND top = Modal_getTopLevelWindow(parent);
    m_owner = top;
    Window_centerOn(m_hWnd, top);
    show(true);
    return true;
}

bool DualLayerEditor::onCreate()
{
    createLabel(m_hWnd, L"Index:", 20, 22, 45, 16);
    m_index.createNumber(m_hWnd, (HMENU)'indx', 70, 20, 50, 18, m_model->m_index, 0, 0);
    m_index.enable(false);

    createLabel(m_hWnd, L"Top:", 20, 46, 45, 16);
    m_top.createNumber(m_hWnd, (HMENU)'itm1', 70, 44, 50, 18, m_model->m_top, 0, 0);
    SendMessageW(m_top.m_hWnd, EM_LIMITTEXT, 3, 0);

    createLabel(m_hWnd, L"Hands:", 130, 46, 45, 16);
    m_hands.createNumber(m_hWnd, (HMENU)'itm2', 180, 44, 50, 18, m_model->m_hand, 0, 0);
    SendMessageW(m_hands.m_hWnd, EM_LIMITTEXT, 3, 0);

    createLabel(m_hWnd, L"Bottom:", 240, 46, 45, 16);
    m_bottom.createNumber(m_hWnd, (HMENU)'itm3', 290, 44, 50, 18, m_model->m_bottom, 0, 0);
    SendMessageW(m_bottom.m_hWnd, EM_LIMITTEXT, 3, 0);

    createLabel(m_hWnd, L"Shoes:", 20, 70, 45, 16);
    m_shoes.createNumber(m_hWnd, (HMENU)'itm4', 70, 68, 50, 18, m_model->m_shoe, 0, 0);
    SendMessageW(m_shoes.m_hWnd, EM_LIMITTEXT, 3, 0);

    createLabel(m_hWnd, L"Empty:", 130, 70, 45, 16);
    m_empty.createNumber(m_hWnd, (HMENU)'itm5', 180, 68, 50, 18, m_model->m_empty, 0, 0);
    SendMessageW(m_empty.m_hWnd, EM_LIMITTEXT, 3, 0);

    createLabel(m_hWnd, L"Helmet:", 240, 70, 45, 16);
    m_helmet.createNumber(m_hWnd, (HMENU)'itm6', 290, 68, 50, 18, m_model->m_helmet, 0, 0);
    SendMessageW(m_helmet.m_hWnd, EM_LIMITTEXT, 3, 0);

    createLabel(m_hWnd, L"Fill-in the fields with the TypeID value of items to be used.", 20, 98, 280, 24);
    return Modal::onCreate();
}

int DualLayerEditor::mapCommandToResult(int id)
{
    int result = id;
    if (id == IDOK)
    {
        m_model->m_index = (short)m_index.getInt(-1);
        if (m_model->m_index < 1)
            result = -1;
        m_model->m_top = (short)m_top.getInt(-1);
        m_model->m_hand = (short)m_hands.getInt(-1);
        m_model->m_bottom = (short)m_bottom.getInt(-1);
        m_model->m_shoe = (short)m_shoes.getInt(-1);
        m_model->m_empty = (short)m_empty.getInt(-1);
        m_model->m_helmet = (short)m_helmet.getInt(-1);
    }
    return result;
}
