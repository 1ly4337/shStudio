#pragma once
#include "ptrArray.h"
#include "Serializable.h"
#include "TransformWeaponModel.h"

class String;
class iStream;
class oStream;

class TransformWeaponModels : public ptrArray<TransformWeaponModel*>, public Serializable
{
public:
    TransformWeaponModels();
    virtual ~TransformWeaponModels();

    virtual bool isEmpty() const { return ptrArray<TransformWeaponModel*>::isEmpty(); }
    virtual void clear() { deleteAll(); }
    virtual void read(iStream& in, DWORD fmt);
    virtual void write(oStream& out, DWORD fmt) const;
    virtual bool toText(String& out, DWORD fmt) const;
};
static_assert(sizeof(TransformWeaponModels) == 0x34, "TransformWeaponModels size");
