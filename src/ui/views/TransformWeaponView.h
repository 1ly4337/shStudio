#pragma once
#include "ModelView.h"
#include "TransformWeaponList.h"
#include "TransformWeaponModels.h"

class TransformWeaponModels;
class FileTree;

class TransformWeaponView : public ModelView
{
public:
    TransformWeaponView(FileTree* data);
    virtual ~TransformWeaponView();

    bool onCreate() override
    {
        if (!ModelView::onCreate())
            return false;
        RECT rcBuf;
        RECT* rc = getClientRect(&rcBuf);
        int height = rc->bottom - rc->top;
        if (!m_list.create(m_hWnd, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, height,
                           0x50000000, 0, height))
            return false;
        m_list.setItemCount(m_models->size());
        return true;
    }
    void fillScriptMenu(HMENU menu) override {}
    int getMenuId() override { return 0xB1; }
    void selectModel(int index) override { m_selected = index; }
    void editModel(int index) override;

public:
    TransformWeaponList    m_list;
    TransformWeaponModels* m_models;
};
static_assert(sizeof(TransformWeaponView) == 0x54C, "TransformWeaponView size");
