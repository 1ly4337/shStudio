#pragma once
#include <windows.h>
#include "DocView.h"
#include "EffectList.h"

class EffectSets;
class EffectSet;
class FileTree;

class EffectView : public DocView
{
public:
    EffectView(FileTree* file, void* unused);
    ~EffectView() override;

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    int getMenuId() override;

    virtual void selectSet(int index) {}
    virtual EffectSet* getSet(int index);

public:
    EffectList  m_list;
    EffectSets* m_sets;
};
static_assert(sizeof(EffectView) == 0x1A8, "EffectView size");
