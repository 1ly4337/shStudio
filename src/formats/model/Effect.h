#pragma once
#include "Serializable.h"
#include "Strings.h"
#include "Scenes.h"
#include "Sequences.h"

class Effect : public Serializable {
public:
    Effect();
    ~Effect() override;

    bool isEmpty() const override;
    void clear() override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    int       m_flags;
    Strings   m_names;
    Strings   m_textures;
    Scenes    m_scenes;
    Sequences m_sequences;
};
static_assert(sizeof(Effect) == 0xA8, "Effect size");
