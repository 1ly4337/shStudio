#pragma once
#include "ptrArray.h"
#include "Serializable.h"
#include "CharAction.h"

class CharAction;

class CharActions : public ptrArray<CharAction*>, public Serializable {
public:
    CharActions();
    ~CharActions() override;

    bool isEmpty() const override { return m_count == 0; }
    void clear() override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
};
static_assert(sizeof(CharActions) == 0x34, "CharActions size");
