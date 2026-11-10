#pragma once
#include "Serializable.h"
#include "Strings.h"

class ClothTextures : public Serializable {
public:
    ClothTextures();
    ~ClothTextures() override;

    bool isEmpty() const override;
    void clear() override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;

    Strings m_names;
    int     m_value;
};
static_assert(sizeof(ClothTextures) == 0x4C, "ClothTextures size");
