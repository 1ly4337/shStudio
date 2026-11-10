#pragma once
#include "fPoint.h"
#include <windows.h>
#include "Popup.h"
#include "glControls.h"
#include "glViewer.h"
#include "FlatButton.h"
#include "EditText.h"
#include "UpDownCtrl.h"
#include "Button.h"

class Drawable;

class PopupView3D : public Popup, public glControls
{
public:
    PopupView3D();
    virtual ~PopupView3D();

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wc) override;
    DWORD getStyle() override;
    bool onCreate() override;
    void onActivate(bool active) override;
    LRESULT onCtlColor(UINT msg, HDC hdc, HWND ctrl) override;
    bool onClose() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    virtual bool vslot72(HWND parent, const RECT* client);
    virtual void vslot73(const fPoint* rotation);
    virtual void vslot74(const fPoint* position);
    virtual void vslot75(bool on);
    virtual void clearModel();
    virtual void setModel(Drawable* model);
    virtual int vslot78() { return 0; }
    virtual void vslot79();
    virtual bool vslot80(int x);

    void vslot1(Drawable* drawable) override;
    void vslot2(const fPoint* rotation) override;
    void vslot3(const fPoint* position) override;

public:
    FlatButton m_rotReset[3];
    EditText   m_rotEdit[3];
    UpDownCtrl m_rotSpin[3];
    FlatButton m_posReset[4];
    Button     m_alpha;
    int        m_19C;
    glViewer   m_viewer;
    int        m_36C;
    bool       m_updating;
    fPoint     m_angles;
    int        m_384;
};
static_assert(sizeof(PopupView3D) == 0x388, "PopupView3D size");

LRESULT CALLBACK pfnSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
                             DWORD_PTR dwRefData);
