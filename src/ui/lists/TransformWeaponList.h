#pragma once
#include "ModelList.h"
#include "TransformWeaponModels.h"
#include "Collection.hpp"

class TransformWeaponView;
class TransformWeaponModels;
class Node;
class FileTree;

class TransformWeaponList : public ModelList
{
public:
    TransformWeaponList(TransformWeaponView* view, FileTree* data);

    bool onCreate() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override { return m_models->get(index); }
    bool sort() override
    {
        TransformWeaponModels* models = m_models;
        int count = models->m_count;
        if (count < 2)
            return false;
        models->m_changes = 0;
        quickSort(*models, 0, count - 1, m_sortColumn, m_prevSortColumn, m_sortAscending);
        return models->m_changes > 0;
    }
    bool removeItem(int index) override { return m_models->remove(index); }
    UINT getClipboardFormat() override { return 0x20D; }
    Node* createNode() override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override { return 0; }

public:
    FileTree*              m_data;
    TransformWeaponModels* m_models;
};
static_assert(sizeof(TransformWeaponList) == 0x100, "TransformWeaponList size");
