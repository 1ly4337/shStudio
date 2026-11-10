#pragma once
#include "DocView.h"
#include "PcTab.h"

class CapeModels;
class FileTree;

class PcView : public DocView
{
public:
    PcView(FileTree* data);
    virtual ~PcView();

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    int getMenuId() override;
    virtual void selectModel(int index) {}
    virtual void editModel(int index);

public:
    CapeModels* m_models;
    PcTab       m_tab;
};
static_assert(sizeof(PcView) == 0x3CC, "PcView size");
