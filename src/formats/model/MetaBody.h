#pragma once
#include "MetaModel3D.h"

class MetaBody : public MetaModel3D
{
public:
    MetaBody() : MetaModel3D(0) {}
    ~MetaBody() override {}
};
static_assert(sizeof(MetaBody) == 0x88, "MetaBody size");
