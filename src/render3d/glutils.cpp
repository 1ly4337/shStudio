#include "glutils.h"
#include <GL/glu.h>
#include "_imports_p10.h"

#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT 0x80E1
#endif

void glClearColorRef(COLORREF color)
{
    GLclampf blue = (GLclampf)((double)((color >> 16) & 0xFF) / 255.0);
    GLclampf green = (GLclampf)((double)((color >> 8) & 0xFF) / 255.0);
    glClearColor((GLclampf)((double)(unsigned char)color / 255.0), green, blue, 1.0f);
}

void setupDefaultLight()
{
    GLfloat ambient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position[4] = { 0.0f, 0.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glEnable(GL_LIGHT1);
}

void glRotateXYZ(float angleX, float angleY, float angleZ)
{
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
}

bool createTextureFromBitmap(HBITMAP bitmap, GLuint* texture)
{
    *texture = 0;
    if (!bitmap)
        return false;
    DIBPixels pixels;
    bool ok;
    if (!pixels.load(bitmap) || (glGenTextures(1, texture), glGetError()))
    {
        ok = false;
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *texture);
        GLenum error = glGetError();
        if (error
            || (glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR), (error = glGetError()) != 0)
            || (glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST), (error = glGetError()) != 0)
            || (gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, pixels.m_width, pixels.m_height, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
                                  pixels.m_bits),
                (error = glGetError()) != 0))
        {
            if (*texture)
            {
                glDeleteTextures(1, texture);
                *texture = 0;
            }
        }
        ok = error == 0;
    }
    return ok;
}

void drawCube(float size)
{
    float h = size * 0.5f;
    glBegin(GL_QUADS);
    glNormal3d(1.0, 0.0, 0.0);
    float n = -(size * 0.5f);
    glVertex3d(h, -h, h);
    glVertex3d(size * 0.5f, n, n);
    glVertex3d(h, h, n);
    glVertex3d(h, h, h);
    glNormal3d(0.0, 1.0, 0.0);
    glVertex3d(h, h, h);
    glVertex3d(h, h, n);
    glVertex3d(n, size * 0.5f, n);
    glVertex3d(n, h, h);
    glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(h, h, h);
    glVertex3d(n, h, h);
    glVertex3d(n, n, size * 0.5f);
    glVertex3d(h, n, h);
    glNormal3d(-1.0, 0.0, 0.0);
    glVertex3d(n, n, size * 0.5f);
    glVertex3d(n, h, h);
    glVertex3d(n, size * 0.5f, n);
    glVertex3d(n, n, n);
    glNormal3d(0.0, -1.0, 0.0);
    glVertex3d(n, n, size * 0.5f);
    glVertex3d(n, n, n);
    glVertex3d(size * 0.5f, n, n);
    glVertex3d(h, n, h);
    glNormal3d(0.0, 0.0, -1.0);
    glVertex3d(n, n, n);
    glVertex3d(n, size * 0.5f, n);
    glVertex3d(h, h, n);
    glVertex3d(size * 0.5f, n, n);
    glEnd();
}
