#pragma once
#include <windows.h>
#include <shobjidl.h>
#include "Document.h"
#include "glViewer.h"

class FileTree;
class Drawable;
class CtrlsPanel;

class ModuleView : public Document
{
public:
    explicit ModuleView(FileTree* data);
    virtual ~ModuleView();

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wc) override;
    bool onCreate() override;
    void onActivate(bool active) override;
    bool onClose() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    void onSize(WPARAM type, int cx, int cy) override;
    bool onTimer(UINT_PTR id) override;
    void updateMenu(HMENU menu) override;
    void fillScriptMenu(HMENU menu) override;
    LPCWSTR getCommandHelp(UINT id) override;
    int getMenuId() override;
    HRESULT store() override;
    COMDLG_FILTERSPEC* getFilters(int* count) override;

    virtual void setModel(Drawable* model);
    virtual CtrlsPanel* createPanel();
    virtual int getPanelWidth();
    virtual Drawable* createModel();
    virtual bool loadModel();

public:
    glViewer    m_viewer;
    DWORD       m_224;
    CtrlsPanel* m_panel;
    Drawable*   m_model;
};
static_assert(sizeof(ModuleView) == 0x230, "ModuleView size");
