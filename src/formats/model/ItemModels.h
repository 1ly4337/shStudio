#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "ItemModel.h"

class iStream;
class oStream;
class String;

class ItemModels : public ptrArray<ItemModel*>, public Serializable
{
public:
    ItemModels();
    ~ItemModels() override;

    bool isEmpty() const override { return m_count == 0; }
    void clear() override { deleteAll(); }

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    DWORD m_version;
};
static_assert(sizeof(ItemModels) == 0x38, "ItemModels size");
