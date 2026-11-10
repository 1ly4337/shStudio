#include "DualLayerView.h"
#include "DualLayerModels.h"
#include "DualLayerEditor.h"
#include "_imports_p06.h"

DualLayerView::DualLayerView(FileTree* file, LPCWSTR title)
    : ModelView(file, nullptr, &m_list)
    , m_title(title)
    , m_list(this, file)
{
    DualLayerModels* models = new DualLayerModels();
    m_models = models;
    m_list.m_models = models;
    m_content = models;
    FIELD(Serializable*, this, 0x50) = models;
    m_decrypt = true;
    m_showModels = false;
}

DualLayerView::~DualLayerView()
{
}

bool DualLayerView::onCreate()
{
    if (!ModelView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rcBuf;
    RECT* rc = getClientRect(&rcBuf);
    int height = rc->bottom - rc->top;
    if (!m_list.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, height,
                           0x50000000, 0, height))
        return false;
    m_list.setItemCount(m_models->size());
    return true;
}

int DualLayerView::getMenuId()
{
    return 0xB1;
}

void DualLayerView::selectModel(int index)
{
    m_selected = index;
}

void DualLayerView::editModel(int index)
{
    DualLayerModel* model = m_models->get(index);
    if (!model)
        return;
    DualLayerEditor editor(m_title, model);
    if (editor.create(m_hWnd) && editor.doModal() == 1)
        setModified(1);
}
