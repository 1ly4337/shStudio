#pragma once
#include <windows.h>
#include "DocView.h"
#include "PopupView3D.h"

class Control;
class FileTree;
class Serializable;
class fPoint;

class ModelView : public DocView
{
public:
    ModelView(FileTree* data, Serializable* content, Control* child);
    ~ModelView() override;

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    void onActivate(bool active) override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onEnable(bool enabled) override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    virtual void selectModel(int index) = 0;
    virtual void editModel(int index) {}
    virtual void getViewTarget(fPoint& target) {}
    virtual void placeView3D();

public:
    Control*    m_child;
    int         m_padAC;
    PopupView3D m_view3D;
    int         m_selected;
    float       m_targetY;
    bool        m_showModels;
    BYTE        m_pad441[7];
};
static_assert(sizeof(ModelView) == 0x448, "ModelView size");
