#include "glBitmapBuilder.h"
#include "glutils.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
#include "_imports_p10.h"

glBitmapBuilder::glBitmapBuilder()
{
    m_hglrc = 0;
    m_hdc = 0;
    m_hDib = 0;
    m_hOld = 0;
    m_bits = 0;
    memset(&m_pfd, 0, sizeof(m_pfd));
    m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    m_pfd.nVersion = 1;
    m_pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_GDI | PFD_SUPPORT_OPENGL;
    m_pfd.iPixelType = PFD_TYPE_RGBA;
    m_pfd.cColorBits = 32;
    m_pfd.cDepthBits = 32;
    m_pfd.iLayerType = PFD_MAIN_PLANE;
}

glBitmapBuilder::~glBitmapBuilder()
{
    if (m_hglrc)
    {
        wglMakeCurrent(0, 0);
        wglDeleteContext(m_hglrc);
        m_hglrc = 0;
    }
    if (m_hdc)
    {
        SelectObject(m_hdc, m_hOld);
        DeleteDC(m_hdc);
        m_hdc = 0;
    }
    if (m_hDib)
    {
        DeleteObject(m_hDib);
        m_hDib = 0;
    }
}

DWORD glBitmapBuilder::begin(const SIZE* size, COLORREF background)
{
    m_size.cx = size->cx;
    m_size.cy = size->cy;
    BITMAPINFO bmi;
    LONG width = m_size.cx;
    LONG height = m_size.cy;
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = height * ((int)((32 * width + 31) & 0xFFFFFFE0) / 8);
    bmi.bmiHeader.biXPelsPerMeter = 96;
    bmi.bmiHeader.biYPelsPerMeter = 96;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;
    m_hdc = CreateCompatibleDC(0);
    if (!m_hdc)
        return GetLastError();
    m_hDib = CreateDIBSection(m_hdc, &bmi, DIB_RGB_COLORS, &m_bits, 0, 0);
    if (!m_hDib)
        return GetLastError();
    m_hOld = SelectObject(m_hdc, m_hDib);
    int format = ChoosePixelFormat(m_hdc, &m_pfd);
    if (!format || !SetPixelFormat(m_hdc, format, &m_pfd))
        return GetLastError();
    m_hglrc = wglCreateContext(m_hdc);
    if (!m_hglrc || !wglMakeCurrent(m_hdc, m_hglrc))
        return (DWORD)E_FAIL;
    glClearColorRef(background);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    setupDefaultLight();
    glViewport(0, 0, m_size.cx, m_size.cy);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)m_size.cx / (double)m_size.cy, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    return glGetError();
}

DWORD glBitmapBuilder::end(HBITMAP* bitmap)
{
    glFlush();
    DWORD error = 0;
    HDC hdc = GetDC(0);
    HBITMAP copy = CreateCompatibleBitmap(hdc, m_size.cx, m_size.cy);
    if (!copy)
        error = GetLastError();
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    if (!error)
    {
        error = getBitmapInfo(copy, &bmi);
        if (!error)
        {
            if (SetDIBits(hdc, copy, 0, m_size.cy, m_bits, &bmi, DIB_RGB_COLORS))
            {
                *bitmap = copy;
            }
            else
            {
                DeleteObject(copy);
                error = GetLastError();
            }
        }
    }
    ReleaseDC(0, hdc);
    return error;
}
