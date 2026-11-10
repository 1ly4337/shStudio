#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "Drawable.h"
#include "ptrArray.h"
#include "Struct3D.h"

class Struct3D;
class Model3D;
class String;
class ListCtrl;
class fPoint;

class MetaStruct3D : public Drawable
{
public:
    class Structs3D : public ptrArray<Struct3D*>
    {
    public:
        Structs3D();
    };

public:
    MetaStruct3D() : m_type(0) {}
    ~MetaStruct3D() override;

    void   clear() override;
    void   translate(const fPoint* origin) override;
    void   computeBounds() override;
    void   fillProperties(ListCtrl* list) override;
    void   drawWireframe(GLenum front, GLenum back) override;
    void   drawImmediate() override;
    bool   loadTexture(const wchar_t* dir) override;
    virtual BOOL     isEmpty();
    virtual bool     addPart(Struct3D* part);
    virtual bool     loadModels(const String& dir, ptrArray<String*>* models, ptrArray<String*>* textures);
    virtual Model3D* loadModel(const String& modelFile, const String& textureFile) = 0;
    virtual int      vslot22() const { return 0; }

public:
    Structs3D m_parts;
    int       m_type;
};
static_assert(sizeof(MetaStruct3D) == 0x84, "MetaStruct3D size");
