#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "fPoint.h"

class ListCtrl;

typedef void (APIENTRY* PFN_P10_GLBINDBUFFER)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFN_P10_GLDELETE)(GLsizei n, const GLuint* ids);
extern PFN_P10_GLBINDBUFFER g_glBindBuffer;
extern PFN_P10_GLDELETE     g_glDeleteVertexArrays;
extern PFN_P10_GLDELETE     g_glDeleteBuffers;
extern PFN_P10_GLDELETE     g_glDeleteBuffersArr;

class Drawable
{
public:
    Drawable();
    virtual ~Drawable();
    virtual void clear();
    virtual void translate(const fPoint* origin) = 0;
    virtual void computeBounds() = 0;
    virtual void fillProperties(ListCtrl* list) = 0;
    virtual void drawWireframe(GLenum front, GLenum back) {}
    virtual void drawImmediate() {}
    virtual void scale(const fPoint* factor) {}
    virtual HBITMAP makeBitmap(COLORREF background, SIZE size, int reserved, DWORD kind);
    virtual DWORD setFlag(DWORD mask, bool on)
    {
        if (on)
        {
            m_flags |= mask;
            return mask;
        }
        m_flags &= ~mask;
        return ~mask;
    }
    virtual int releaseTexture();
    virtual bool loadTexture(const wchar_t* dir) { return false; }
    virtual void setTexture(GLuint texture);
    virtual bool setBitmap(HBITMAP bitmap);
    virtual void* getPositions(int* outSize) { return nullptr; }
    virtual void* getNormals(int* outSize) { return nullptr; }
    virtual void* getIndices(int* outSize) { return nullptr; }
    virtual void* getTexCoords(int* outSize) { return nullptr; }

    void render(int mode);
    void drawElements(int mode);

public:
    GLuint   m_vao;
    GLuint   m_buffers[4];
    GLuint   m_vertexVbo;
    GLuint   m_texCoordVbo;
    GLsizei  m_count;
    GLuint   m_texture;
    HBITMAP  m_bitmap;
    fPoint   m_center;
    fPoint   m_min;
    fPoint   m_max;
    fPoint*  m_eye;
    DWORD    m_flags;
    float    m_fps;
};
static_assert(sizeof(Drawable) == 0x68, "Drawable size");
