#include "TransformWeaponList.h"
#include "TransformWeaponView.h"
#include "TransformWeaponModels.h"
#include "TransformWeaponModel.h"
#include "FileTree.h"
#include "_imports_p07.h"

TransformWeaponList::TransformWeaponList(TransformWeaponView* view, FileTree* data)
    : ModelList(view), m_data(data), m_models(0)
{
}

int TransformWeaponList::indexOfNode(Node* node)
{
    return m_models->indexOf(dynamic_cast<TransformWeaponModel*>(node));
}

Node* TransformWeaponList::createNode()
{
    return new TransformWeaponModel();
}

int TransformWeaponList::insertItem(Node* node, int index)
{
    return m_models->insert(dynamic_cast<TransformWeaponModel*>(node), index);
}

bool TransformWeaponList::onCreate()
{
    if (!m_models || !ListCtrlOD::onCreate())
        return false;
    addColumn(L"Type", 60, 18, 'type');
    addColumn(L"Weapon 1", 70, 18, 'wpn ');
    addColumn(L"Weapon 2", 70, 18, 'wpn1');
    return true;
}
