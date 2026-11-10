#include "Drawable.h"
#include "glBitmapBuilder.h"
#include "glutils.h"
#include "_imports_p10.h"
#include <GL/glu.h>
#include <cmath>

PFN_P10_GLBINDBUFFER g_glBindBuffer = 0;
PFN_P10_GLDELETE     g_glDeleteVertexArrays = 0;
PFN_P10_GLDELETE     g_glDeleteBuffers = 0;
PFN_P10_GLDELETE     g_glDeleteBuffersArr = 0;

Drawable::Drawable()
{
    m_vao = 0;
    m_buffers[0] = 0;
    m_buffers[1] = 0;
    m_buffers[2] = 0;
    m_buffers[3] = 0;
    m_count = 0;
    m_vertexVbo = 0;
    m_texCoordVbo = 0;
    m_texture = 0;
    m_bitmap = 0;
    m_flags = 0;
    m_eye = 0;
    m_fps = 0;
}

Drawable::~Drawable()
{
    Drawable::clear();
}

void Drawable::clear()
{
    m_center.z = 0;
    m_center.y = 0;
    m_center.x = 0;
    m_min.z = 0;
    m_min.y = 0;
    m_min.x = 0;
    m_max.z = 0;
    m_max.y = 0;
    m_max.x = 0;
    if (m_bitmap)
    {
        DeleteObject(m_bitmap);
        m_bitmap = 0;
    }
    if (m_texture && m_texture != (GLuint)-1)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    GLuint* buffer = m_buffers;
    int n = 4;
    do
    {
        if (*buffer)
        {
            g_glDeleteBuffersArr(1, buffer);
            *buffer = 0;
        }
        ++buffer;
        --n;
    } while (n);
    if (m_vao)
    {
        g_glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vertexVbo)
    {
        g_glDeleteBuffers(1, &m_vertexVbo);
        m_vertexVbo = 0;
    }
    if (m_texCoordVbo)
    {
        g_glDeleteBuffers(1, &m_texCoordVbo);
        m_texCoordVbo = 0;
    }
}

int Drawable::releaseTexture()
{
    int result;
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    result = (int)(INT_PTR)m_bitmap;
    if (m_bitmap)
    {
        result = DeleteObject(m_bitmap);
        m_bitmap = 0;
    }
    return result;
}

void Drawable::setTexture(GLuint texture)
{
    if (m_texture && m_texture != (GLuint)-1)
        glDeleteTextures(1, &m_texture);
    if (!texture)
        OutputDebugStringW(L"setting texmap to nil\n");
    m_texture = texture;
}

bool Drawable::setBitmap(HBITMAP bitmap)
{
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    if (m_bitmap && m_bitmap != bitmap)
        DeleteObject(m_bitmap);
    m_bitmap = bitmap;
    return createTextureFromBitmap(bitmap, &m_texture);
}

void Drawable::render(int mode)
{
    if (mode == 1 && !m_texture)
        loadTexture(0);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    if (mode == 1)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_TEXTURE_2D);
        if (m_buffers[0])
        {
            drawElements(1);
        }
        else if (m_vertexVbo && m_texCoordVbo)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            g_glBindBuffer(0x8892 , m_vertexVbo);
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glEnable(GL_TEXTURE_2D);
            g_glBindBuffer(0x8892 , m_texCoordVbo);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
            glDrawArrays(GL_TRIANGLES, 0, m_count);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        else
        {
            drawImmediate();
        }
    }
    else if (m_buffers[0])
    {
        drawElements(0);
    }
    else if (m_vertexVbo)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        g_glBindBuffer(0x8892 , m_vertexVbo);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, m_count);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        drawWireframe(GL_LINE, GL_POINT);
    }
}

void Drawable::drawElements(int mode)
{
    glGetError();
    int textured = mode;
    if (mode == 1)
    {
        if (m_buffers[3])
        {
            glEnable(GL_TEXTURE_2D);
            glShadeModel(GL_SMOOTH);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            goto draw;
        }
        textured = 0;
    }
    else if (mode)
    {
        return;
    }
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_POINT);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
draw:
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
    if (m_buffers[2])
        glEnableClientState(GL_NORMAL_ARRAY);
    if (textured == 1)
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_SHORT, 0);
    glGetError();
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

HBITMAP Drawable::makeBitmap(COLORREF background, SIZE size, int reserved, DWORD kind)
{
    (void)reserved;
    HBITMAP result;
    glBitmapBuilder builder;
    if (builder.begin(&size, background))
    {
        result = 0;
    }
    else
    {
        computeBounds();
        float halfX = (m_max.x - m_min.x) * 0.5f;
        float halfZ = (m_max.z - m_min.z) * 0.5f;
        float halfY = (m_max.y - m_min.y) * 0.5f;
        float m = halfX;
        if (!(halfX > halfY))
            m = halfY;
        float distance = (m - 0.89999998f) * 2.0f;
        glTranslatef(0.0f, 0.0f, -2.0f);
        glTranslatef(-m_center.x, -m_center.y, -distance);
        if (kind == 0x6B334443)
        {
            glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(-170.0f, 0.0f, 1.0f, 0.0f);
            glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
        }
        else if (kind == 0x63617065)
        {
            glTranslatef(0.0f, 0.0f, -0.5f);
        }
        else if (kind == 0x77696E67)
        {
            m_flags |= 1;
            glRotateXYZ(0.0f, -170.0f, 0.0f);
        }
        else if (kind == 0x6D6F6273)
        {
            glTranslatef(0.0f, 0.0f, -1.0f);
            glRotateXYZ(0.0f, -170.0f, 0.0f);
        }
        else if (kind == 0x4E504373)
        {
            glRotateXYZ(0.0f, 180.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, distance * 2.0f);
        }
        else
        {
            if (kind & 0x10000)
                m_flags |= 1;
            unsigned type = kind & 0xFFFF;
            if (type == 5)
            {
                glTranslatef(halfX, halfY * 0.5f, 0.0f);
                glScalef(0.5f, 0.5f, 0.5f);
                glRotateXYZ(80.0f, 33.0f, -10.0f);
            }
            else
            {
                float diff = (float)fabs((double)(halfX - halfZ));
                float smaller = (halfZ > halfX) ? halfX : halfZ;
                if ((double)(diff / smaller) > 0.1 && halfZ > halfX)
                    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
                if (type == 7)
                {
                    glScalef(0.67000002f, 0.67000002f, 0.67000002f);
                    glRotateXYZ(0.0f, 0.0f, 135.0f);
                }
                else if (type == 8)
                {
                    glScalef(0.25f, 0.25f, 0.25f);
                    glRotateXYZ(90.0f, 0.0f, 0.0f);
                }
                else if (type == 10)
                {
                    glTranslatef(0.0f, 0.0f, -0.1f);
                }
            }
        }
        render(1);
        HBITMAP bitmap = 0;
        builder.end(&bitmap);
        result = bitmap;
    }
    return result;
}
