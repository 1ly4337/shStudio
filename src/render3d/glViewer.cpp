#include "p05_common.h"
#include "glViewer.h"
#include "Drawable.h"
#include "MouseControler.h"

glViewer::~glViewer()
{
    if (m_mouse)
        delete m_mouse;
    if (m_drawable)
        delete m_drawable;
}
