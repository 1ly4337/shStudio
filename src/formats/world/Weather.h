#pragma once
#include "Serializable.h"
#include "Strings.h"

class Weather : public Serializable {
public:
    Weather();
    ~Weather() override;

    bool isEmpty() const override;
    void clear() override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    float   m_level;
    int     m_current;
    int     m_flags;
    Strings m_names;
};
static_assert(sizeof(Weather) == 0x54, "Weather size");
