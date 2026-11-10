#pragma once
#include <windows.h>
#include "ModelView.h"
#include "MonList.h"

class FileTree;
class MonsterModels;

class MonView : public ModelView
{
public:
    MonView(FileTree* data, MonsterModels* models);
    ~MonView() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    int getMenuId() override;
    void setFilterIndex(int format) override;
    int getFilterIndex() override;
    void selectModel(int index) override;
    void editModel(int index) override;

public:
    MonList        m_list;
    MonsterModels* m_models;
};
static_assert(sizeof(MonView) == 0x548, "MonView size");
