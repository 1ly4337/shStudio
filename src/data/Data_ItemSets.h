#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "Data_ItemSet.h"

class String;
class iStream;
class oStream;

namespace Data {

class ItemSets : public ptrArray<ItemSet*>, public Serializable
{
public:
    ItemSets();
    virtual ~ItemSets();

    bool isEmpty() const override { return ptrArray<ItemSet*>::isEmpty(); }
    void clear() override { deleteAll(); }
    void load(iStream& in, int arg2, int arg3) override;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;

    virtual bool exportText(String& out, DWORD fmt);

};

}

static_assert(sizeof(Data::ItemSets) == 0x34, "Data::ItemSets size");
