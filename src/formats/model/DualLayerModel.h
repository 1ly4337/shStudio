#pragma once
#include "p02_common.h"

class DualLayerModel : public Node
{
public:
    DualLayerModel(int version, int unused);
    virtual ~DualLayerModel() {}

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;

    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;

    virtual void copyParts(const DualLayerModel* src);

    int   m_version;
    short m_index;
    short m_top;
    short m_hand;
    short m_bottom;
    short m_shoe;
    short m_empty;
    short m_helmet;
};
static_assert(sizeof(DualLayerModel) == 0x28, "DualLayerModel size");
