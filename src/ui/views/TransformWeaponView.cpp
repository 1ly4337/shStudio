#include "TransformWeaponView.h"
#include "TransformWeaponModels.h"
#include "TransformWeaponEditor.h"
#include "_imports_p07.h"

TransformWeaponView::TransformWeaponView(FileTree* data)
    : ModelView(data, 0, &m_list), m_list(this, data)
{
    TransformWeaponModels* models = new TransformWeaponModels;
    m_models = models;
    Serializable* content = models;
    m_list.m_models = models;
    m_content = content;
    m_object = content;
    m_decrypt = false;
    m_showModels = false;
}

TransformWeaponView::~TransformWeaponView()
{
}

void TransformWeaponView::editModel(int index)
{
    TransformWeaponModel* model = m_models->get(index);
    if (!model)
        return;
    TransformWeaponEditor editor(model);
    if (editor.create(m_hWnd) && editor.doModal() == IDOK)
        setModified(true);
}
