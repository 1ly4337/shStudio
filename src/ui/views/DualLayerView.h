#pragma once
#include <windows.h>
#include "ModelView.h"
#include "DualLayerList.h"

class DualLayerModels;
class FileTree;

class DualLayerView : public ModelView
{
public:
    DualLayerView(FileTree* file, LPCWSTR title);
    ~DualLayerView() override;

    bool onCreate() override;
    void fillScriptMenu(HMENU scripts) override {}
    int getMenuId() override;
    bool detectsCodepage() override { return false; }
    void selectModel(int index) override;
    void editModel(int index) override;

public:
    LPCWSTR          m_title;
    DualLayerList    m_list;
    DualLayerModels* m_models;
};
static_assert(sizeof(DualLayerView) == 0x550, "DualLayerView size");
