#include "glViewer.h"
#include "Drawable.h"
#include "MouseMoveControler.h"
#include "p10_vcall.h"
#include "glutils.h"
#include "_imports_p10.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#define GLCONTROLS_ON_DRAWABLE(c, d) P10_VCALL((c), 0x04, void, Drawable*)((c), (d))
#define GLCONTROLS_ON_ROTATION(c, p) P10_VCALL((c), 0x08, void, fPoint*)((c), (p))
#define GLCONTROLS_ON_POSITION(c, p) P10_VCALL((c), 0x0C, void, fPoint*)((c), (p))
#define CHRONO_START(c)   ((c)->start())
#define CHRONO_ELAPSED(c) ((c)->stop())

void glViewer::setDrawFlag(DWORD mask, bool on)
{
    (void)mask;
    if (on)
        m_drawFlags |= 1u;
    else
        m_drawFlags &= ~1u;
    if (m_drawable)
    {
        m_drawable->setFlag(255, false);
        m_drawable->setFlag(m_drawFlags, true);
    }
}

void glViewer::setRenderMode(int mode)
{
    m_renderMode = mode;
    Drawable* d = m_drawable;
    if (d && mode == 1)
    {
        if (!d->m_texture || d->m_texture == (GLuint)-1)
            d->loadTexture(0);
    }
}

void glViewer::setNavigationMode(int mode)
{
    HWND hwnd = m_hWnd;
    m_navigationMode = mode;
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(hwnd, &rc);
    int height = rc.bottom - rc.top;
    int width = rc.right - rc.left;
    glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLdouble fovy;
    if (m_navigationMode)
        fovy = 40.0;
    else
        fovy = 60.0;
    gluPerspective(fovy, (double)width / (double)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (m_mouse)
        m_mouse->m_flyMode = mode;
}

void glViewer::createMouseControler(int kind)
{
    (void)kind;
    if (!m_mouse)
        m_mouse = new MouseMoveControler(&m_rotation, &m_position);
    m_mouseDown = false;
}

void glViewer::setDrawable(Drawable* drawable)
{
    if (m_drawable && m_ownsDrawable)
        delete m_drawable;
    m_drawable = drawable;
    g_activeGlView = this;
    if (drawable)
    {
        drawable->setFlag(255, false);
        m_drawable->setFlag(m_drawFlags, true);
    }
}

static inline float glViewerWrapAngle(float a)
{
    if (a > 360.0f)
        return (float)fmod((double)a, 360.0);
    if (a < 0.0f)
        return (float)fmod((double)(float)(a + 360.0f), 360.0);
    return a;
}

static const double kDegToRad = 0.0174532925199433;

bool glViewer::processKeys()
{
    float oldPosX = m_position.x;
    float oldPosY = m_position.y;
    float oldPosZ = m_position.z;
    float oldRotX = m_rotation.x;
    float oldRotY = m_rotation.y;
    float oldRotZ = m_rotation.z;
    bool ctrl = (((unsigned short)GetKeyState(VK_CONTROL) >> 15) & 1) != 0;
    bool shift = GetKeyState(VK_SHIFT) < 0;

    float sizeX, sizeY, sizeZ;
    if (m_drawable)
    {
        fPoint size = m_drawable->m_max - m_drawable->m_min;
        sizeX = size.x;
        sizeY = size.y;
        sizeZ = size.z;
    }
    else
    {
        sizeX = 0;
        sizeY = 0;
        sizeZ = 0;
    }
    float m = sizeX;
    if (!(sizeX > sizeZ))
        m = sizeZ;
    float step;
    if (ctrl)
        step = m / 50.0f;
    else
        step = m / 500.0f;

    if (m_navigationMode)
    {
        if (m_keys[VK_PRIOR])
            m_position.y = m_position.y + (ctrl ? sizeY / 10.0f : sizeY / 100.0f);
        else if (m_keys[VK_NEXT])
            m_position.y = m_position.y - (ctrl ? sizeY / 10.0f : sizeY / 100.0f);

        if (m_keys[VK_UP] || m_keys['Z'])
        {
            if (shift)
            {
                m_rotation.x = m_rotation.x - 1.0f;
            }
            else
            {
                double yaw = (double)m_rotation.y * kDegToRad;
                m_position.x = (float)sin(yaw) * step + m_position.x;
                m_position.y = m_position.y - (float)sin((double)m_rotation.x * kDegToRad) * step;
                m_position.z = m_position.z - (float)cos(yaw) * step;
            }
        }
        else if (m_keys[VK_DOWN] || m_keys['S'])
        {
            if (shift)
            {
                m_rotation.x = m_rotation.x + 1.0f;
            }
            else
            {
                double yaw = (double)m_rotation.y * kDegToRad;
                m_position.x = m_position.x - (float)sin(yaw) * step;
                m_position.y = (float)sin((double)m_rotation.x * kDegToRad) * step + m_position.y;
                m_position.z = (float)cos(yaw) * step + m_position.z;
            }
        }

        if ((m_keys[VK_LEFT] && !shift) || m_keys['A'])
        {
            double yaw = (double)m_rotation.y * kDegToRad;
            m_position.x = m_position.x - (float)(cos(yaw) * 0.2) * step;
            m_position.z = m_position.z - (float)(sin(yaw) * 0.2) * step;
        }
        else if ((m_keys[VK_RIGHT] && !shift) || m_keys['E'])
        {
            double yaw = (double)m_rotation.y * kDegToRad;
            m_position.x = (float)(cos(yaw) * 0.2) * step + m_position.x;
            m_position.z = (float)(sin(yaw) * 0.2) * step + m_position.z;
        }

        if ((m_keys[VK_LEFT] && shift) || m_keys['Q'])
            m_rotation.y = m_rotation.y - 1.0f;
        else if ((m_keys[VK_RIGHT] && shift) || m_keys['D'])
            m_rotation.y = m_rotation.y + 1.0f;
    }
    else
    {
        if (m_keys[VK_DOWN])
        {
            if (shift)
                m_rotation.x = m_rotation.x + 2.0f;
            else
                m_position.y = m_position.y - step;
        }
        if (m_keys[VK_UP])
        {
            if (shift)
                m_rotation.x = m_rotation.x - 2.0f;
            else
                m_position.y = m_position.y + step;
        }
        if (m_keys[VK_LEFT])
        {
            if (shift)
                m_rotation.y = m_rotation.y - 2.0f;
            else
                m_position.x = m_position.x - step;
        }
        if (m_keys[VK_RIGHT])
        {
            if (shift)
                m_rotation.y = m_rotation.y + 2.0f;
            else
                m_position.x = m_position.x + step;
        }
        if (m_keys[VK_PRIOR])
        {
            if (shift)
                m_rotation.z = m_rotation.z + 2.0f;
            else
                m_position.z = m_position.z - (ctrl ? sizeY / 10.0f : sizeY / 100.0f);
        }
        if (m_keys[VK_NEXT])
        {
            if (shift)
                m_rotation.z = m_rotation.z - 2.0f;
            else
                m_position.z = m_position.z + (ctrl ? sizeY / 10.0f : sizeY / 100.0f);
        }
    }

    m_rotation.x = glViewerWrapAngle(m_rotation.x);
    m_rotation.y = glViewerWrapAngle(m_rotation.y);
    m_rotation.z = glViewerWrapAngle(m_rotation.z);

    if (m_controls)
    {
        if (fabsf(oldPosX - m_position.x) > 2.3841858e-7f
            || fabsf(oldPosY - m_position.y) > 2.3841858e-7f
            || fabsf(oldPosZ - m_position.z) > 2.3841858e-7f)
        {
            GLCONTROLS_ON_POSITION(m_controls, &m_position);
        }
        if (fabsf(oldRotX - m_rotation.x) > 2.3841858e-7f
            || fabsf(oldRotY - m_rotation.y) > 2.3841858e-7f
            || fabsf(oldRotZ - m_rotation.z) > 2.3841858e-7f)
        {
            GLCONTROLS_ON_ROTATION(m_controls, &m_rotation);
        }
    }
    return false;
}

bool glViewer::onLButton(WPARAM keys, POINT* pt, bool down)
{
    if (down)
    {
        SetFocus(m_hWnd);
        if (m_mouse)
        {
            m_mouseDown = true;
            m_mouse->begin(pt);
            return false;
        }
    }
    else
    {
        m_mouseDown = false;
    }
    return false;
}

bool glViewer::onRButton(WPARAM keys, POINT* pt, bool down)
{
    if (!down || !m_mouse)
        return false;
    m_mouseDown = false;
    return true;
}

bool glViewer::onMouseMove(WPARAM keys, POINT* pt)
{
    if (!m_mouse || !m_mouseDown)
        return false;
    m_mouse->move(pt);
    if (m_controls)
    {
        GLCONTROLS_ON_ROTATION(m_controls, &m_rotation);
        GLCONTROLS_ON_POSITION(m_controls, &m_position);
    }
    return true;
}

bool glViewer::onMouseWheel(int delta, POINT* pt, UINT keys)
{
    float d = (float)delta;
    if (m_navigationMode)
        m_position.y = d / 60.0f + m_position.y;
    else
        m_position.z = d / 240.0f + m_position.z;
    if (m_controls)
        GLCONTROLS_ON_POSITION(m_controls, &m_position);
    return true;
}

bool glViewer::draw()
{
    if (m_navigationMode)
    {
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(m_rotation.x, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -1.5f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawCube(0.1f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(m_rotation.y, 0.0f, 1.0f, 0.0f);
        glRotatef(m_rotation.z, 0.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, 1.5f, 5.0f);
        glTranslatef(-m_position.x, -m_position.y, -m_position.z);
    }
    else
    {
        fPoint center;
        float m = 5.0f;
        Drawable* d = m_drawable;
        if (d)
        {
            if (fabsf(d->m_min.x - d->m_max.x) < 2.3841858e-7f
                && fabsf(d->m_min.y - d->m_max.y) < 2.3841858e-7f
                && fabsf(d->m_min.z - d->m_max.z) < 2.3841858e-7f)
            {
                d->computeBounds();
            }
            float halfX = (d->m_max.x - d->m_min.x) * 0.5f;
            float halfY = (d->m_max.y - d->m_min.y) * 0.5f;
            center.x = m_drawable->m_center.x;
            center.y = m_drawable->m_center.y;
            center.z = m_drawable->m_center.z;
            m = (halfX > halfY) ? halfX : halfY;
        }
        float distance = (m - 0.89999998f) * 2.5f;
        glTranslatef(m_position.x, m_position.y, m_position.z);
        glTranslatef(0.0f, 0.0f, -distance);
        glTranslatef(-center.x, -center.y, -center.z);
        glRotatef(m_rotation.x, 1.0f, 0.0f, 0.0f);
        glRotatef(m_rotation.y, 0.0f, 1.0f, 0.0f);
        glRotatef(m_rotation.z, 0.0f, 0.0f, 1.0f);
    }
    if (m_controls)
        GLCONTROLS_ON_DRAWABLE(m_controls, m_drawable);
    if (m_drawable)
    {
        m_drawable->m_eye = &m_position;
        CHRONO_START(&m_chrono);
        m_drawable->render(m_renderMode);
        m_drawable->m_fps = 1000000.0f / (float)CHRONO_ELAPSED(&m_chrono);
    }
    return true;
}
