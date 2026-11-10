#pragma once
#include "DocView.h"
#include "WorldList.h"
#include "World.h"
#include "PopupView3D.h"

class WorldView : public DocView {
public:
    WorldView(FileTree* node, int worldId);
    ~WorldView() override;

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wc) override;
    void fillScriptMenu(HMENU menu) override;
    int  getMenuId() override;
    void onActivated() override;
    COMDLG_FILTERSPEC* getFilters(int* count) override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    virtual void vslot100();
    virtual void vslot101();
    virtual void vslot102(Drawable* model);

    WorldList   m_list;
    World       m_world;
    PopupView3D m_popup;
    int         m_558;
    int         m_pad55C;
};
static_assert(sizeof(WorldView) == 0x560, "WorldView size");
