#pragma once
#include <windows.h>
#include "MetaStruct3D.h"
#include "Serializable.h"
#include "fPoint.h"
#include "fPoints.h"
#include "Faces.h"

class iStream;
class oStream;
class String;
class Model3D;

class SuperModule : public MetaStruct3D, public Serializable
{
public:
    SuperModule();
    virtual ~SuperModule();

    void clear() override;
    Model3D* loadModel(const String& modelFile, const String& textureFile) override;
    virtual void vslot23(int mode);
    virtual void vslot24(const fPoint* factor);

    bool isEmpty() const override;
    using MetaStruct3D::isEmpty;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    fPoint  m_98;
    float   m_A8;
    fPoint  m_AC;
    fPoint  m_BC;
    fPoint  m_CC[2];
    fPoints m_points;
    Faces   m_faces;
};
static_assert(sizeof(SuperModule) == 0x128, "SuperModule size");
