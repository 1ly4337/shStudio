#pragma once
#include <windows.h>
#include "MetaStruct3D.h"

class String;
class Model3D;

class MetaModel3D : public MetaStruct3D
{
public:
    explicit MetaModel3D(int unused = 0);
    ~MetaModel3D() override;

    Model3D* loadModel(const String& modelFile, const String& textureFile) override;

public:
    HGDIOBJ m_hObject;
};
static_assert(sizeof(MetaModel3D) == 0x88, "MetaModel3D size");
