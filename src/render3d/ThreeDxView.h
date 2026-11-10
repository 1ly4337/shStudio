#pragma once
#include <windows.h>
#include "Document.h"
#include "glViewer.h"
#include "String.hpp"

class CtrlsPanel;
class Drawable;
class FileTree;

class ThreeDxView : public Document
{
public:
    ThreeDxView(FileTree* data, DWORD type);
    virtual ~ThreeDxView();

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    void onActivate(bool active) override;
    bool onClose() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    void onSize(WPARAM type, int cx, int cy) override;
    SIZE* getDefaultSize(SIZE* out, SIZE def) override;
    int getMenuId() override;
    HRESULT store() override;
    virtual void setModel(Drawable* model);
    virtual bool vslot97(void* arg) { (void)arg; return false; }

public:
    glViewer    m_viewer;
    int         m_224;
    CtrlsPanel* m_panel;
    Drawable*   m_drawable;
    String      m_textureDir;
    DWORD       m_type;
    int         m_254;
};
static_assert(sizeof(ThreeDxView) == 0x258, "ThreeDxView size");
