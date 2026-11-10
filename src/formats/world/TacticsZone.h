#pragma once
#include <windows.h>
#include "Spot.h"
#include "String.hpp"

class GDC;
class Scaler;
class iStream;
class oStream;

class TacticsZone : public Spot
{
public:
    TacticsZone(int id, int unused = 0);
    ~TacticsZone() override;

    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    bool copyPos(const Spot* other) override;
    virtual void draw(GDC* dc);

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

    LPRECT calcRect(LPRECT rc, Scaler* scaler);

public:
    BYTE    m_unk0;
    BYTE    m_unk1;
    BYTE    m_unk2;
    float*  m_pX1;
    float*  m_pY1;
    float*  m_pZ1;
    float   m_x2;
    float   m_y2;
    float   m_z2;
    float   m_x3;
    float   m_y3;
    float   m_z3;
    float   m_x4;
    float   m_y4;
    float   m_z4;
    int     m_unk3;
    int     m_unk4;
    BYTE    m_map;
    BYTE    m_unk5;
    String  m_desc;
    Scaler* m_scaler;
    int     m_originX;
    int     m_originY;
    static const int kMinRecordSize = 65;
};
static_assert(sizeof(TacticsZone) == 0x94, "TacticsZone size");
