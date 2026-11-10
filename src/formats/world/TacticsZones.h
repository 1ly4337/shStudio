#pragma once
#include "Collection.hpp"
#include "Serializable.h"
#include "TacticsZone.h"

class TacticsZones : public Collection<TacticsZone>, public Serializable {
public:
    TacticsZones();
    ~TacticsZones() override;

    bool isEmpty() const override { return m_count == 0; }
    void clear() override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    int m_version;
};
static_assert(sizeof(TacticsZones) == 0x38, "TacticsZones size");
