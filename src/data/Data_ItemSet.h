#pragma once
#include "p02_common.h"

namespace Data {

class ItemSet : public Node
{
public:
    ItemSet(short index = 0, int unused = 0);
    virtual ~ItemSet();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    virtual int  nodeType() const;
    virtual bool exportText(String& out, DWORD fmt) const;
    virtual bool assign(const ItemSet* src);

    short  m_index;
    String m_name;
    short  m_types[13];
    short  m_ids[13];
    String m_bonus[13];
};

}

static_assert(sizeof(Data::ItemSet) == 0x20C, "Data::ItemSet size");
