#pragma once
#include "ptrArray.h"
#include "Serializable.h"
#include "ModelMLX.h"

class ModelMLX;
class iStream;
class oStream;
class String;

class ModelsMLX : public ptrArray<ModelMLX*>, public Serializable
{
public:
    ModelsMLX();
    virtual ~ModelsMLX();

    bool isEmpty() const override { return ptrArray<ModelMLX*>::isEmpty(); }
    void clear() override;

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;
};
static_assert(sizeof(ModelsMLX) == 0x34, "ModelsMLX size");
