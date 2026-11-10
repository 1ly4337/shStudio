#pragma once
#include "p02_common.h"
#include "Point4.h"
#include "vLong.h"

class Point5;

class Scene
{
public:
    explicit Scene(int format);
    ~Scene();
    void read(iStream& in, DWORD fmt);
    bool toText(String& out, DWORD fmt) const;

    int     m_format;
    String  m_name;
    int     m_ints[10];
    float   m_floats[23];
    int     m_valueA8;
    int     m_valueAC;
    int     m_valueB0;
    Point4  m_color;
    int     m_valueC8;
    int     m_valueCC;
    float   m_valueD0;
    int     m_valueD4;
    int     m_valueD8;
    int     m_valueDC;
    int     m_keyCount;
    Point5* m_keys;
    int     m_pairCount;
    float*  m_pairsA;
    float*  m_pairsB;
    int     m_pointCount;
    fPoint* m_points;
    int     m_valueFC;
    int     m_value100;
    float   m_value104;
    int     m_value108;
    vLong   m_textures;
};
static_assert(sizeof(Scene) == 0x120, "Scene size");
