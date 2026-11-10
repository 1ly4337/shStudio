#include "glView.h"
#include "_imports_p10.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
#include <cmath>

glView* g_activeGlView = 0;
HDC g_currentGlDC = 0;

glView::glView()
{
    memset(&m_pfd, 0, sizeof(m_pfd));
    memset(m_keys, 0, 0x100u);
    m_hDC = 0;
    m_hRC = 0;
    m_lookAt.z = 0;
    m_lookAt.y = 0;
    m_lookAt.x = 0;
    m_position.z = -2.5f;
    m_redraw = 1;
}

void glView::onRegisterClass(WNDCLASSEXW* wcx)
{
    wcx->style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    if (wcx->hbrBackground)
    {
        DeleteObject(wcx->hbrBackground);
        wcx->hbrBackground = 0;
    }
}

bool glView::createAt(HWND parent, HMENU id, int x, int y, int cx, int cy, DWORD style, DWORD exStyle)
{
    RECT rc;
    SetRect(&rc, x, y, x + cx, y + cy);
    if (Window::create(parent, id, &rc, style | WS_CHILD, exStyle, 0))
        return true;
    if (m_hRC)
    {
        wglMakeCurrent(0, 0);
        wglDeleteContext(m_hRC);
        m_hRC = 0;
    }
    if (m_hDC)
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = 0;
    }
    return false;
}

void glView::activate(bool active)
{
    if (m_hRC)
    {
        if (active)
        {
            memset(m_keys, 0, 0x100u);
            if (m_hDC != g_currentGlDC)
            {
                wglMakeCurrent(m_hDC, m_hRC);
                g_currentGlDC = m_hDC;
            }
        }
    }
    else
    {
        wglMakeCurrent(0, 0);
    }
}

bool glView::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (glGetError())
    {
        wglMakeCurrent(m_hDC, m_hRC);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
    }
    if (glGetError())
        return false;
    bool result = draw();
    glFlush();
    return result;
}

bool glView::onCreate()
{
    m_pfd.nSize = 40;
    m_pfd.nVersion = 1;
    m_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    m_pfd.iPixelType = PFD_TYPE_RGBA;
    m_pfd.cColorBits = 32;
    m_pfd.cDepthBits = 32;
    m_pfd.iLayerType = 0;
    m_hDC = GetDC(m_hWnd);
    if (!m_hDC)
        return false;
    int format = ChoosePixelFormat(m_hDC, &m_pfd);
    if (!format || !SetPixelFormat(m_hDC, format, &m_pfd))
        return false;
    m_hRC = wglCreateContext(m_hDC);
    if (!m_hRC || !wglMakeCurrent(m_hDC, m_hRC) || !glGetString(GL_VERSION))
        return false;
    return initGL();
}

bool glView::initGL()
{
    GLclampf blue = (GLclampf)((double)((m_bkColor >> 16) & 0xFF) / 255.0);
    GLclampf green = (GLclampf)((double)((m_bkColor >> 8) & 0xFF) / 255.0);
    GLclampf red = (GLclampf)((double)(m_bkColor & 0xFF) / 255.0);
    glClearColor(red, green, blue, 1.0f);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    SIZE size;
    SIZE* s = getSize(&size);
    int cx = s->cx;
    int cy = s->cy;
    glViewport(0, 0, cx, cy);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)cx / (double)cy, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return glGetError() == 0;
}

bool glView::recreateContext(bool clearFirst)
{
    g_activeGlView = 0;
    if (m_hRC)
    {
        if (clearFirst)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            SwapBuffers(m_hDC);
        }
        wglMakeCurrent(0, 0);
        wglDeleteContext(m_hRC);
        m_hRC = 0;
    }
    m_hRC = wglCreateContext(m_hDC);
    if (!m_hRC || !wglMakeCurrent(m_hDC, m_hRC) || !initGL())
        return false;
    g_activeGlView = this;
    return true;
}

void glView::onDestroy()
{
    glView* active = g_activeGlView;
    if (g_activeGlView == this)
        active = 0;
    g_activeGlView = active;
    if (m_hRC)
    {
        wglMakeCurrent(0, 0);
        wglDeleteContext(m_hRC);
        m_hRC = 0;
    }
    if (m_hDC)
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = 0;
    }
}

bool glView::onLButton(WPARAM keys, POINT* pt, bool down)
{
    if (down)
        SetFocus(m_hWnd);
    return false;
}

bool glView::onMouseWheel(int delta, POINT* pt, UINT keys)
{
    m_position.z = (float)delta / 600.0f + m_position.z;
    return true;
}

bool glView::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (IsWindow(m_hWnd) && (WORD)vk < 0x100u)
        m_keys[(WORD)vk] = down;
    return true;
}

void glView::onSize(WPARAM type, int cx, int cy)
{
    if (cx > 0 && cy > 0)
    {
        glViewport(0, 0, cx, cy);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, (double)cx / (double)cy, 0.1, 10000.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

LRESULT glView::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_PAINT)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    PAINTSTRUCT paint;
    HWND hwnd = m_hWnd;
    HDC hdc = BeginPaint(hwnd, &paint);
    COLORREF color = m_bkColor;
    RECT rc;
    const RECT* client = getClientRect(&rc);
    HBRUSH brush = CreateSolidBrush(color);
    UnrealizeObject(brush);
    FillRect(hdc, client, brush);
    DeleteObject(brush);
    EndPaint(hwnd, &paint);
    return 1;
}

static inline float glViewWrapAngle(float a)
{
    if (a > 360.0f)
        return (float)fmod((double)a, 360.0);
    if (a < 0.0f)
        return (float)fmod((double)(float)(a + 360.0f), 360.0);
    return a;
}

bool glView::processKeys()
{
    int ctrl = (unsigned short)GetKeyState(VK_CONTROL) >> 15;
    bool shift = GetKeyState(VK_SHIFT) < 0;
    float factor = (ctrl & 1) ? 4.0f : 1.0f;
    if (m_keys[VK_DOWN])
    {
        if (shift)
            m_rotation.x = m_rotation.x + 2.0f;
        else
            m_position.y = m_position.y - factor * 0.050000001f;
    }
    if (m_keys[VK_UP])
    {
        if (shift)
            m_rotation.x = m_rotation.x - 2.0f;
        else
            m_position.y = factor * 0.050000001f + m_position.y;
    }
    if (m_keys[VK_LEFT])
    {
        if (shift)
            m_rotation.y = m_rotation.y - 2.0f;
        else
            m_position.x = m_position.x - factor * 0.050000001f;
    }
    if (m_keys[VK_RIGHT])
    {
        if (shift)
            m_rotation.y = m_rotation.y + 2.0f;
        else
            m_position.x = factor * 0.050000001f + m_position.x;
    }
    if (m_keys[VK_PRIOR])
    {
        if (shift)
            m_rotation.z = m_rotation.z + 2.0f;
        else
            m_position.z = m_position.z - factor * 0.0099999998f;
    }
    if (m_keys[VK_NEXT])
    {
        if (shift)
            m_rotation.z = m_rotation.z - 2.0f;
        else
            m_position.z = factor * 0.0099999998f + m_position.z;
    }
    m_rotation.x = glViewWrapAngle(m_rotation.x);
    m_rotation.y = glViewWrapAngle(m_rotation.y);
    m_rotation.z = glViewWrapAngle(m_rotation.z);
    return shift;
}

void glView::idle()
{
    char vk = 32;
    while (!m_keys[vk])
    {
        if (++vk > 40)
        {
            if (!m_keys['A'] && !m_keys['Z'] && !m_keys['E'] && !m_keys['Q'] && !m_keys['S'] && !m_keys['D'])
                Sleep(25u);
            return;
        }
    }
}

BOOL glView::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return SwapBuffers(m_hDC);
}
