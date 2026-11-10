#pragma once
#include "Struct3D.h"
#include "Vertex.h"
#include "Matrix4.h"
#include "String.hpp"

class iStream;
class oStream;
class ListCtrl;

class Model3D : public Struct3D
{
public:
    class Vertex3DE : public Vertex
    {
    public:
        Vertex3DE() : m_bone(0) {}
        void read(iStream& s, DWORD tag) override;
        void write(oStream& s, DWORD tag) const override;
        bool toText(String& out, DWORD fmt) const override;
        void assign(const Vertex* src) override;
        int m_bone;
    };

    class Vertex3DO : public Vertex
    {
    public:
        void read(iStream& s, DWORD tag) override;
        void write(oStream& s, DWORD tag) const override;
        bool toText(String& out, DWORD fmt) const override;
        void assign(const Vertex* src) override;
        fPoint m_normal;
    };

    class Vertex3DC : public Vertex3DO
    {
    public:
        Vertex3DC(int version);
        void read(iStream& s, DWORD tag) override;
        void write(oStream& s, DWORD tag) const override;
        bool toText(String& out, DWORD fmt) const override;
        void assign(const Vertex* src) override;
        int   m_version;
        float m_w;
        int   m_bones;
        float m_u1;
        float m_u2;
    };

    struct Float3 { float x, y, z; };
    struct Short3 { unsigned short a, b, c; };
    struct Float2 { float u, v; };

public:
    Model3D(int type);
    ~Model3D() override;

    void  clear() override;
    void  fillProperties(ListCtrl* list) override;
    bool  loadTexture(const wchar_t* dir) override;
    void* getPositions(int* outSize) override;
    void* getNormals(int* outSize) override;
    void* getIndices(int* outSize) override;
    void* getTexCoords(int* outSize) override;

    int  serialV5() const override;
    HRESULT setCodePage(int version) override;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;

    void    readModel(iStream& s, int version, bool hasMatrices);
    HRESULT loadFiles(const wchar_t* modelFile, const wchar_t* textureFile);

public:
    int      m_version;
    String   m_textureName;
    int      m_matrixCount;
    Matrix4* m_matrices;
    HGDIOBJ  m_hBitmap;
};
static_assert(sizeof(Model3D) == 0x100, "Model3D size");
static_assert(sizeof(Model3D::Vertex3DE) == 0x20, "Vertex3DE size");
static_assert(sizeof(Model3D::Vertex3DO) == 0x2C, "Vertex3DO size");
static_assert(sizeof(Model3D::Vertex3DC) == 0x40, "Vertex3DC size");
