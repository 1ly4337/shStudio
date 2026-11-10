#pragma once
#include <windows.h>
#include "ModelView.h"
#include "MltList.h"

class ModelsMLT;
class FileTree;

class MltView : public ModelView
{
public:
    MltView(FileTree* file, ModelsMLT* models);
    ~MltView() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    void fillScriptMenu(HMENU scripts) override;
    LPCWSTR getCommandHelp(UINT id) override;
    int getMenuId() override;
    void selectModel(int index) override;
    void editModel(int index) override;

    void removeEv10wcModel();

public:
    MltList    m_list;
    ModelsMLT* m_models;
};
static_assert(sizeof(MltView) == 0x548, "MltView size");
