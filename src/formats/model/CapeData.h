#pragma once
#include "p02_common.h"

extern wchar_t g_capeBlockName[16];
extern wchar_t g_capeDataName[16];

int sub_43CBF0(wchar_t* buffer, const wchar_t* format, ...);

class CapeData : public Node
{
public:
    class Block : public Node
    {
    public:

        void read(iStream& in, DWORD fmt) override;
        void write(oStream& out, DWORD fmt) const override;
        bool toText(String& out, DWORD fmt) const override;
        short getImage(int state, short def) const override;
        const wchar_t* getString(DWORD tag) const override;
        int  getInt(DWORD tag) const override;
        void reset() override;
        int  compare(const Node* other, DWORD tag) const override;

        int   m_index;
        int   m_points[3];
        float m_values[3];
    };
    static_assert(sizeof(Block) == 0x2C, "CapeData::Block size");

    CapeData(int index, int unused = 0);
    virtual ~CapeData();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;

    int   m_index;
    int   m_dds;
    int   m_3dc;
    int   m_unk1;
    int   m_unk1C;
    int   m_unk2;
    int   m_unk3;
    int   m_unk4[4];
    int   m_unk5;
    Block m_blocks[20];
};
static_assert(sizeof(CapeData) == 0x3B4, "CapeData size");
