#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "Struct3D.h"
#include "Vertex.h"
#include "fPoint.h"
#include "String.hpp"

class iStream;
class oStream;
class ListCtrl;

class VaniSubStruct : public Struct3D
{
public:
    class VertexVANI : public Vertex
    {
    public:
        VertexVANI() {}

        virtual void read(iStream& in, DWORD fmt) override;
        virtual void write(oStream& out, DWORD fmt) const override;
        virtual bool toText(String& out, DWORD fmt) const override;
        virtual void assign(const Vertex* src) override;

        fPoint m_normal;
        long   m_bone;
    };

    explicit VaniSubStruct(int nSteps);
    virtual ~VaniSubStruct();

    virtual void translate(const fPoint* origin) override {}
    virtual void computeBounds() override;
    virtual void fillProperties(ListCtrl* list) override {}
    virtual Vertex* getVertex(int index) override;

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override { return false; }

    bool loadTexture(const String& dir);

    String m_textureName;
    int    m_nSteps;
    int    m_nVertsPerStep;
    int    m_curStep;
};
static_assert(sizeof(VaniSubStruct::VertexVANI) == 0x30, "VertexVANI size");
static_assert(sizeof(VaniSubStruct) == 0xFC, "VaniSubStruct size");
