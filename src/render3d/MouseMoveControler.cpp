#include "MouseMoveControler.h"
#include "fPoint.h"
#include "_imports_p10.h"
#include <cmath>

void MouseMoveControler::begin(const POINT* pt)
{
    m_last.x = pt->x;
    m_last.y = pt->y;
}

void MouseMoveControler::move(const POINT* pt)
{
    int dx = pt->x - m_last.x;
    int dy = pt->y - m_last.y;
    m_last.x = pt->x;
    m_last.y = pt->y;

    float yaw = (float)dx + m_rotation->y;
    if (yaw > 360.0f)
        yaw = (float)fmod((double)yaw, 360.0);
    else if (yaw < 0.0f)
        yaw = (float)fmod((double)(float)(yaw + 360.0f), 360.0);
    m_rotation->y = yaw;

    float pitch = (float)dy / 5.0f + m_rotation->x;
    if (pitch > 360.0f)
        m_rotation->x = (float)fmod((double)pitch, 360.0);
    else if (pitch < 0.0f)
        m_rotation->x = (float)fmod((double)(float)(pitch + 360.0f), 360.0);
    else
        m_rotation->x = pitch;
}
