#pragma once
#include "Node.h"
#include "ptrArray.h"
#include "Serializable.h"
#include "MonsterModel.h"

class MonsterModel;

class MonsterModels : public Node, public ptrArray<MonsterModel*>, public Serializable {
public:
    MonsterModels();
    ~MonsterModels() override;

    const wchar_t* getString(DWORD tag) const override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    bool isEmpty() const override { return m_count == 0; }
    void clear() override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    int m_signature;
};
static_assert(sizeof(MonsterModels) == 0x44, "MonsterModels size");
