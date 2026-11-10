#include "MonView.h"
#include "MonsterModels.h"
#include "MonsterModel.h"
#include "MonEditor.h"
#include "MetaModel3D.h"
#include "PopupView3D.h"
#include "glViewer.h"
#include "String.hpp"
#include "mon_g1.h"
#include "_imports_p07.h"

MonView::MonView(FileTree* data, MonsterModels* models)
    : ModelView(data, models ? (Serializable*)models : 0, &m_list),
      m_list(this)
{
    if (models)
    {
        m_models = models;
    }
    else
    {
        m_models = new MonsterModels();
        Serializable* content = m_models ? (Serializable*)m_models : 0;
        m_content = content;
        FIELD(Serializable*, this, 0x50) = content;
    }
    m_list.m_node = (Node*)m_models;
}

MonView::~MonView()
{
}

bool MonView::onCreate()
{
    if (!ModelView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rc;
    RECT* r = getClientRect(&rc);
    if (!m_list.create(parent, (HMENU)0x65, r->left, r->top, r->right - r->left, r->bottom - r->top,
                       0x50000000, 0, r->bottom - r->top))
        return false;
    ptrArray<MonsterModel*>& list = *m_models;
    m_list.setItemCount(list.size());
    m_view3D.m_viewer.createMouseControler(0);
    return true;
}

bool MonView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return ModelView::onCommand(code, ctrl, id);
    if (id == 0xE401)
    {
        setFilterIndex(MON_FORMAT_MO2);
        setModified(true);
        return true;
    }
    if (id == 0xE402)
    {
        setFilterIndex(MON_FORMAT_MO4);
        setModified(true);
        return true;
    }
    return ModelView::onCommand(code, 0, id);
}

bool MonView::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    int format = getFilterIndex();
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, 0xE401, FALSE, &mii);
    if ((mii.fState & 3) == 0)
    {
        UINT state = 0;
        if (format == MON_FORMAT_MO2)
            state = MFS_CHECKED;
        mii.fState = state;
        SetMenuItemInfoW(menu, 0xE401, FALSE, &mii);
    }
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, 0xE402, FALSE, &mii);
    if ((mii.fState & 3) == 0)
    {
        UINT state = 0;
        if (format == MON_FORMAT_MO4)
            state = MFS_CHECKED;
        mii.fState = state;
        SetMenuItemInfoW(menu, 0xE402, FALSE, &mii);
    }
    return true;
}

int MonView::getMenuId()
{
    return 171;
}

void MonView::setFilterIndex(int format)
{
    MonsterModels* models = m_models;
    if (format == MON_FORMAT_MO2)
    {
        if (models->m_signature != MON_FORMAT_MO4)
            return;
        models->m_signature = MON_FORMAT_MO2;
        for (int i = 0; i < models->m_count; ++i)
            MM_FORMAT(models->m_items[i]) = MON_FORMAT_MO2;
    }
    else
    {
        if (models->m_signature != MON_FORMAT_MO2)
            return;
        models->m_signature = MON_FORMAT_MO4;
        for (int i = 0; i < models->m_count; ++i)
            MM_FORMAT(models->m_items[i]) = MON_FORMAT_MO4;
    }
    setModified(true);
}

int MonView::getFilterIndex()
{
    return m_models->m_signature;
}

void MonView::selectModel(int index)
{
    m_selected = index;
    if (index < 0)
        return;
    if (!m_view3D.isVisible())
        return;
    m_view3D.clearModel();
    MonsterModels* models = m_models;
    if (index >= models->m_count)
        return;
    MonsterModel* model = models->m_items[index];
    if (!model)
        return;
    String folder(FIELD(String, m_data, 0x10));
    folder.removeFileName();
    MetaModel3D* meta = new MetaModel3D(0);
    if (meta->loadModels(folder, &MM_VERTICES(model), &MM_TEXTURES(model)))
        m_view3D.setModel(meta);
    else
        delete meta;
}

void MonView::editModel(int index)
{
    MonsterModels* models = m_models;
    if (index < 0 || index >= models->m_count)
        return;
    MonsterModel* model = models->m_items[index];
    if (!model)
        return;
    String folder(FIELD(String, m_data, 0x10));
    folder.removeFileName();
    MonEditor editor(model, folder);
    if (editor.create(m_hWnd) && editor.doModal() == 1)
        setModified(true);
}
