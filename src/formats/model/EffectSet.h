#pragma once
#include "p02_common.h"

class EffectSet : public Node
{
public:
    struct Effect
    {
        Effect();
        ~Effect() {}
        void read(iStream& in, DWORD fmt);
        void write(oStream& out, DWORD fmt) const;

        int    m_id;
        float  m_value04;
        int    m_value08;
        int    m_value0C;
        int    m_value10;
        float  m_value14;
        String m_name;
        int    m_values[6];
        fPoint m_pos;
        int    m_value60;
    };

    class Effects : public ptrArray<Effect*>
    {
    public:
        Effects() : ptrArray<Effect*>(256) {}
        virtual ~Effects() {}
        void destroyItem(Effect* item) override
        {
            if (item)
                delete item;
        }
        virtual void clear();
        virtual void read(iStream& in, DWORD fmt);
        virtual void write(oStream& out, DWORD fmt) const;
        virtual void toText(String& out) const;
    };

    explicit EffectSet(int unused = 0);
    virtual ~EffectSet();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    int  getInt(DWORD tag) const override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    virtual void dump(String& out);

    int     m_id;
    Effects m_effects;
};
static_assert(sizeof(EffectSet::Effect) == 0x64, "EffectSet::Effect size");
static_assert(sizeof(EffectSet) == 0x30, "EffectSet size");
