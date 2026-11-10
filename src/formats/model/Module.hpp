#pragma once
#include "Struct3D.h"
#include "Vertex.h"
#include "String.hpp"

class iStream;
class oStream;

class Module : public Struct3D
{
public:
    class Vertex : public ::Vertex
    {
    public:
        Vertex() : m_mask(0) {}
        void read(iStream& s, DWORD tag) override;
        void write(oStream& s, DWORD tag) const override;
        bool toText(String& out, DWORD fmt) const override;
        fPoint   m_normal;
        COLORREF m_mask;
    };

public:
    Module();
    ~Module() override;

    void clear() override;
    bool loadTexture(const wchar_t* dir) override;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    String m_textureName;
};
static_assert(sizeof(Module) == 0xF0, "Module size");
static_assert(sizeof(Module::Vertex) == 0x30, "Module::Vertex size");
