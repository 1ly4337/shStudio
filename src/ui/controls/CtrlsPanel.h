#pragma once
#include <windows.h>
#include "Container.h"
#include "glControls.h"
#include "FlatButton.h"
#include "EditText.h"
#include "UpDownCtrl.h"

class Popup;
class glViewer;
class fPoint;

class CtrlsPanel : public Container, public glControls
{
public:
    CtrlsPanel(Popup* doc, glViewer* viewer);
    virtual ~CtrlsPanel();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    virtual void setRotation(fPoint* rotation);
    virtual void setPosition(fPoint* position);

    void vslot2(const fPoint* rotation) override;
    void vslot3(const fPoint* position) override;

public:
    Popup*      m_doc;
    glViewer*   m_viewer;
    bool        m_updating;
    FlatButton  m_rotReset[3];
    EditText    m_rotEdit[3];
    UpDownCtrl  m_rotSpin[3];
    FlatButton  m_posReset[3];
    EditText    m_posEdit[3];
    UpDownCtrl  m_posSpin[3];
    int         m_bottom;
};
static_assert(sizeof(CtrlsPanel) == 0x1E4, "CtrlsPanel size");
