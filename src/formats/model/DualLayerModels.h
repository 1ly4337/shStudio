#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "DualLayerModel.h"

class iStream;
class oStream;
class String;

class DualLayerModels : public ptrArray<DualLayerModel*>, public Serializable
{
public:
    DualLayerModels();
    ~DualLayerModels() override;

    bool isEmpty() const override { return m_count == 0; }
    void clear() override { deleteAll(); }

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    DWORD m_format;
};
static_assert(sizeof(DualLayerModels) == 0x38, "DualLayerModels size");
