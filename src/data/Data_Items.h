#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "Data_Item.h"

class String;
class iStream;
class oStream;

namespace Data {

class Item;

class Items : public Serializable, public ptrArray<Item*>
{
public:
    Items();
    virtual ~Items();

    virtual bool isEmpty() const { return m_count == 0; }
    virtual void clear();
    virtual const wchar_t* getFilePrefix() const;
    virtual void load(iStream& in, int arg2, int arg3);
    virtual HRESULT setCodePage(int codePage);

    virtual void read(iStream& in, DWORD fmt);
    virtual void write(oStream& out, DWORD fmt) const;
    virtual bool toText(String& out, DWORD fmt) const;

    Item* sub_42FC00(int type, int typeId) const;

    int sub_42FC50(ptrArray<Item*>* out, int grade);
    bool sub_42FCC0(int mode, BYTE kind, bool flag, int slot, ptrArray<Item*>* out);
    bool sub_4301C0(iStream& in, DWORD fmt, int version);
    void sub_4303E0(iStream& in);
    void sub_430740(oStream& out);

public:
    int m_unk2C;
    int m_value30;
};

}

static_assert(sizeof(Data::Items) == 0x3C, "Data::Items size");
