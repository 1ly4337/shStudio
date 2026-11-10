#pragma once
#include "p02_common.h"

class ModelMLT : public Node
{
public:
    ModelMLT(int id, const wchar_t* texture, const wchar_t* vertex, int alpha, int unused = 0);
    ModelMLT(const ModelMLT& src, int unused);
    virtual ~ModelMLT();

    void read(iStream& in, DWORD fmt) override {}
    void write(oStream& out, DWORD fmt) const override {}
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    virtual void assignModel(const ModelMLT* src);

    int    m_id;
    int    m_vrs3;
    int    m_alpha;
    String m_texture;
    String m_vertex;
};
static_assert(sizeof(ModelMLT) == 0x60, "ModelMLT size");
