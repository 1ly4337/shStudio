#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "ModelMLT.h"

class iStream;
class oStream;
class String;

class ModelsMLT : public ptrArray<ModelMLT*>, public Serializable
{
public:
    ModelsMLT();
    ~ModelsMLT() override;

    ModelMLT* detach(int index) override;

    bool isEmpty() const override { return m_count == 0; }
    void clear() override { deleteAll(); }

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
};
static_assert(sizeof(ModelsMLT) == 0x34, "ModelsMLT size");
