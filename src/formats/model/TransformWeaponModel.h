#pragma once
#include "p04_common.h"
#include "Node.h"

class iStream;
class oStream;
class String;

class TransformWeaponModel : public Node
{
public:
    TransformWeaponModel();

    virtual int getInt(DWORD tag) const override;
    virtual bool setInt(DWORD tag, int value) override;
    virtual void reset() override;
    virtual int compare(const Node* other, DWORD tag) const override;
    virtual void assign(const TransformWeaponModel* other);

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;
    virtual bool copyFrom(const Streamable* src) override;

public:
    int m_type;
    int m_weapon;
    int m_weapon1;
};
static_assert(sizeof(TransformWeaponModel) == 0x20, "TransformWeaponModel size");
