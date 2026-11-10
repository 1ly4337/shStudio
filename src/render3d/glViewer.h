#pragma once
#include "glView.h"

class Drawable;
class MouseControler;
class glControls;

class glViewer : public glView
{
public:
    glViewer()
    {
        m_renderMode = 1;
        m_navigationMode = 0;
        m_controls = 0;
        m_mouse = 0;
        m_drawable = 0;
        m_drawFlags = 0;
        m_mouseDown = false;
        m_ownsDrawable = true;
    }
    ~glViewer() override;

    bool onMouseMove(WPARAM keys, POINT* pt) override;
    bool onLButton(WPARAM keys, POINT* pt, bool down) override;
    bool onRButton(WPARAM keys, POINT* pt, bool down) override;
    bool onMouseWheel(int delta, POINT* pt, UINT keys) override;
    bool processKeys() override;
    bool draw() override;

    void setDrawFlag(DWORD mask, bool on);
    void setRenderMode(int mode);
    void setNavigationMode(int mode);
    void createMouseControler(int kind);
    void setDrawable(Drawable* drawable);
public:
    Drawable*       m_drawable;
    glControls*     m_controls;
    MouseControler* m_mouse;
    int             m_renderMode;
    int             m_navigationMode;
    DWORD           m_drawFlags;
    bool            m_mouseDown;
    bool            m_ownsDrawable;
};
static_assert(sizeof(glViewer) == 0x1CC, "glViewer size");
