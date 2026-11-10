#pragma once
#include "p02_common.h"

class ItemModel : public Node
{
public:
    ItemModel(int id, DWORD version, int unused = 0);
    virtual ~ItemModel();

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
    virtual void assign(const ItemModel* src);

    int    m_id;
    DWORD  m_version;
    int    m_vertexIndex;
    int    m_textureIndex;
    int    m_glow;
    int    m_unk2;
    int    m_format;
    int    m_unk3;
    DWORD  m_rgba;
    float  m_rotY;
    float  m_scale;
    int    m_unk5;
    String m_texture;
    String m_vertex;
};
static_assert(sizeof(ItemModel) == 0x84, "ItemModel size");
