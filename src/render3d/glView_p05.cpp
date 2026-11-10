#include "p05_common.h"
#include <cmath>
#include "glView.h"
#include "fPoint.h"

glView::~glView()
{
}

bool glView::createView(HWND parent, HMENU id, const RECT* rect, DWORD style, DWORD exStyle)
{
    return createAt(parent, id, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top,
                    style, exStyle);
}

BOOL glView::swapBuffers()
{
    return SwapBuffers(m_hDC);
}

void glView::setPosition(const fPoint* p)
{
    m_position.x = p->x;
    m_position.y = p->y;
    m_position.z = p->z;
}

void glView::setRotation(float x, float y, float z)
{
    if (x > 360.0f)
        x = (float)fmod(x, 360.0);
    else if (x < 0.0f)
        x = (float)fmod((float)(x + 360.0f), 360.0);
    m_rotation.x = x;
    if (y > 360.0f)
        y = (float)fmod(y, 360.0);
    else if (y < 0.0f)
        y = (float)fmod((float)(y + 360.0f), 360.0);
    m_rotation.y = y;
    if (z > 360.0f)
        m_rotation.z = (float)fmod(z, 360.0);
    else {
        if (z < 0.0f)
            z = (float)fmod((float)(z + 360.0f), 360.0);
        m_rotation.z = z;
    }
}

int glView::vfunc_11C(int arg)
{
    const fPoint* p = (const fPoint*)arg;
    setRotation(p->x, p->y, p->z);
    return 0;
}

LPCWSTR glView::getClassName()
{
    return L"glView";
}
