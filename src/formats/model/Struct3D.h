#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "Drawable.h"
#include "Serializable.h"
#include "Vertices.h"
#include "Faces.h"
#include "Face.h"

class String;
class iStream;
class oStream;
class ListCtrl;

class Struct3D : public Drawable, public Serializable
{
public:
    Struct3D();
    ~Struct3D() override;

    void   clear() override;
    void   translate(const fPoint* origin) override;
    void   computeBounds() override;
    void   fillProperties(ListCtrl* list) override;
    void   drawWireframe(GLenum front, GLenum back) override;
    void   drawImmediate() override;
    void   scale(const fPoint* scale) override;
    virtual Vertex* getVertex(int index);
    virtual void    drawFaces();
    virtual GLenum  drawTextured();

    bool isEmpty() const override;

    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    Vertices      m_vertices;
    Faces         m_faces;
    int           m_type;
    bool          m_centered;
    int           m_unkB8;
    int           m_unkBC;
    void*         m_bufC0;
    void*         m_bufC4;
};
static_assert(sizeof(Struct3D) == 0xD0, "Struct3D size");

bool loadTextureBitmap(String& path, HBITMAP* outBitmap);
