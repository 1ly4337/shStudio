#pragma once
#include <windows.h>
#include "ModelView.h"
#include "ItmList.h"

class ItemModels;
class FileTree;
class fPoint;

class ItmView : public ModelView
{
public:
    ItmView(FileTree* file, ItemModels* models);
    ~ItmView() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    int getMenuId() override;
    void setFilterIndex(int format) override;
    int getFilterIndex() override;
    void selectModel(int index) override;
    void editModel(int index) override;
    void getViewTarget(fPoint& target) override;

public:
    ItmList     m_list;
    ItemModels* m_models;
    int         m_pad54C;
};
static_assert(sizeof(ItmView) == 0x550, "ItmView size");
