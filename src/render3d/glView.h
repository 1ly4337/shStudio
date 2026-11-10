#pragma once
#include "Window.h"
#include "fPoint.h"
#include "QpcChrono.h"

class glView : public Window
{
public:
    glView();
    ~glView() override;

    void activate(bool active) override;
    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    void onDestroy() override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    void onSize(WPARAM type, int cx, int cy) override;
    bool onLButton(WPARAM keys, POINT* pt, bool down) override;
    bool onMouseWheel(int delta, POINT* pt, UINT keys) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    virtual bool createView(HWND parent, HMENU id, const RECT* rect, DWORD style, DWORD exStyle);
    virtual bool createAt(HWND parent, HMENU id, int x, int y, int cx, int cy, DWORD style, DWORD exStyle);
    virtual bool render();
    virtual bool processKeys();
    virtual BOOL swapBuffers();
    virtual void setPosition(const fPoint* p);
    virtual int vfunc_11C(int arg);
    void setRotation(float x, float y, float z);
    virtual void idle();
    virtual bool recreateContext(bool clearFirst);
    virtual BOOL clear();
    virtual bool initGL();
    virtual bool draw() { return false; }

public:
    PIXELFORMATDESCRIPTOR m_pfd;
    HDC      m_hDC;
    HGLRC    m_hRC;
    fPoint   m_lookAt;
    fPoint   m_position;
    fPoint   m_rotation;
    int      m_redraw;
    BYTE     m_keys[256];
    QpcChrono m_chrono;
};
static_assert(sizeof(glView) == 0x1B0, "glView size");

extern glView* g_activeGlView;
extern HDC g_currentGlDC;
