#pragma once
#include "p02_common.h"

class ModelMLX : public Node
{
public:
    ModelMLX(int unused = 0);
    virtual ~ModelMLX();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    virtual void assign(const ModelMLX* src);

    int    m_id;
    String m_name;
    String m_textures[4];
    String m_vertices[4];
    int    m_alpha[4];
};
static_assert(sizeof(ModelMLX) == 0x148, "ModelMLX size");
