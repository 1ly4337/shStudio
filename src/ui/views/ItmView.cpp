#include "ItmView.h"
#include "ItemModels.h"
#include "ItemModel.h"
#include "ItmEditor.h"
#include "Model3D.h"
#include "StudioFrame.h"
#include "String.hpp"
#include "fPoint.h"
#include <cwchar>
#include "_imports_p06.h"

ItmView::ItmView(FileTree* file, ItemModels* models)
    : ModelView(file, models, &m_list)
    , m_list(this, file)
{
    if (models)
    {
        m_models = models;
    }
    else
    {
        m_models = new ItemModels();
        m_content = m_models;
        FIELD(Serializable*, this, 0x50) = m_models;
    }
    m_list.m_models = m_models;
}

ItmView::~ItmView()
{
}

int ItmView::getMenuId()
{
    return 0xA9;
}

bool ItmView::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    int format = getFilterIndex();
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, 0xE401, FALSE, &mii);
    if ((mii.fState & MFS_DISABLED) == 0)
    {
        mii.fState = format == 'ITM ' ? MFS_CHECKED : 0;
        SetMenuItemInfoW(menu, 0xE401, FALSE, &mii);
    }
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, 0xE402, FALSE, &mii);
    if ((mii.fState & MFS_DISABLED) == 0)
    {
        mii.fState = format == 'IT2 ' ? MFS_CHECKED : 0;
        SetMenuItemInfoW(menu, 0xE402, FALSE, &mii);
    }
    return true;
}

bool ItmView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return ModelView::onCommand(code, ctrl, id);
    if (id == 0xE401)
    {
        setFilterIndex('ITM ');
        setModified(true);
        return true;
    }
    if (id == 0xE402)
    {
        setFilterIndex('IT2 ');
        setModified(true);
        return true;
    }
    return ModelView::onCommand(code, 0, id);
}

int ItmView::getFilterIndex()
{
    if (m_models)
        return m_models->m_version;
    return 0;
}

bool ItmView::onCreate()
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

void ItmView::getViewTarget(fPoint& target)
{
    String name(FIELD(String, m_data, 0x30));
    if (name.length() >= 2)
    {
        if (!wcsncmp(name.c_str(), L"11", 2))
        {
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 135.0f;
            return;
        }
        if (!wcsncmp(name.c_str(), L"14", 2))
        {
            target.x = 0.0f;
            target.y = 25.0f;
            target.z = 135.0f;
            return;
        }
        if (!wcsncmp(name.c_str(), L"15", 2))
        {
            target.x = 280.0f;
            target.y = 95.0f;
            target.z = 0.0f;
            fPoint& pos = m_view3D.m_viewer.m_position;
            fPoint p(pos.x, pos.y, (float)(pos.z - 0.5));
            m_view3D.m_viewer.setPosition(&p);
            return;
        }
    }
    if (name.startsWith(L"19") || name.startsWith(L"34"))
    {
        target.x = 80.0f;
        target.y = name.startsWith(L"19") ? 33.0f : 0.0f;
        target.z = -10.0f;
        fPoint& pos = m_view3D.m_viewer.m_position;
        fPoint p((float)(pos.x + 0.5), (float)(pos.y + 0.25), (float)(pos.z - 0.25));
        m_view3D.m_viewer.setPosition(&p);
    }
}

void ItmView::selectModel(int index)
{
    m_selected = index;
    if (!m_view3D.isVisible())
        return;
    m_view3D.clearModel();
    ItemModel* model = m_models->get(index);
    if (!model)
        return;
    String dir(FIELD(String, m_data, 0x10));
    dir.removeFileName();
    String texture(dir);
    texture << L"\\DDS\\" << model->m_texture;
    String vertex(dir);
    vertex << L"\\3DO\\" << model->m_vertex;
    Model3D* model3d = new Model3D('k3DO');
    if (model3d->loadFiles(vertex.c_str(), texture.c_str()))
    {
        if (model3d)
            delete model3d;
    }
    else
    {
        m_view3D.setModel(model3d);
        m_view3D.vslot75(model->m_unk3 == 0);
        m_list.updateItem(index);
    }
}

void ItmView::editModel(int index)
{
    ItemModel* model = m_models->get(index);
    if (!model)
        return;
    String basePath;
    if (!g_studioFrame->isArchiveReadOnly())
    {
        String workspace;
        basePath = *g_studioFrame->getWorkspacePath(&workspace);
        basePath << L"Item\\";
    }
    ItmEditor editor(model, basePath.c_str());
    if (editor.create(m_hWnd) && editor.doModal() == 1)
        setModified(true);
}

void ItmView::setFilterIndex(int format)
{
    ItemModels* models = m_models;
    if (format == 'ITM ')
    {
        if (models->m_version == 'IT2 ')
        {
            models->m_version = 'ITM ';
            setModified(true);
        }
    }
    else if (models->m_version != 'IT2 ')
    {
        setStatusText(L"Format cannot be upgraded from ITM to IT2, the format ITM is used.", 0);
    }
}
