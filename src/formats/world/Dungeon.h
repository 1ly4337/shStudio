#pragma once
#include "p02_common.h"
#include <GL/gl.h>
#include "Drawable.h"
#include "Vertex.h"
#include "Names.h"
#include "Collection.hpp"
#include "primitiveArray.h"
#include "fPoints.h"
#include "Faces.h"
#include "Face.h"

class ListCtrl;

extern wchar_t g_dungeonItemName[32];

class Dungeon : public Drawable, public Serializable
{
public:
    class Segment : public Drawable, public Node
    {
    public:
        struct Matrice
        {
            bool toText(String& out, DWORD fmt) const;
            fPoint p[5];
        };

        class Bound
        {
        public:
            static const int kMinRecordSize = 8;
            Bound(int  = 0, int  = 0) {}
            void read(iStream& in, DWORD fmt)
            {
                m_points.read(in, fmt);
                m_faces.read(in, fmt);
            }
            void write(oStream& out, DWORD fmt) const
            {
                out << (unsigned long)m_points.m_count;
                for (int i = 0; i < m_points.m_count; ++i)
                    m_points.m_items[i]->write(out, fmt);
                m_faces.write(out, fmt);
            }

            fPoints m_points;
            Faces   m_faces;
        };

        class Module : public Drawable, public Node
        {
        public:
            class pvBlock : public Streamable
            {
            public:
                static const int kMinRecordSize = 88;
                pvBlock(int index = 0, int unused = 0);
                virtual ~pvBlock() {}
                void read(iStream& in, DWORD fmt) override;
                void write(oStream& out, DWORD fmt) const override;
                bool toText(String& out, DWORD fmt) const override;

                int    m_id;
                fPoint m_pos;
                float  m_value;
                int    m_type;
                fPoint m_dir;
                int    m_params[4];
                fPoint m_points[3];
            };

            class Block : public Drawable, public Displayable, public Serializable
            {
            public:
                class Vertex : public ::Vertex
                {
                public:
                    static const int kMinRecordSize = 44;
                    Vertex(int index = 0, int unused = 0);
                    virtual ~Vertex() {}
                    void read(iStream& in, DWORD fmt) override;
                    void write(oStream& out, DWORD fmt) const override;
                    bool toText(String& out, DWORD fmt) const override;

                    fPoint m_normal;
                    int    m_color;
                    float  m_u2;
                    float  m_v2;
                };

                static const int kMinRecordSize = 12;
                Block(int index = 0, int unused = 0);
                virtual ~Block();

                void clear() override;
                void translate(const fPoint* origin) override;
                void computeBounds() override;
                void fillProperties(ListCtrl* list) override;
                void drawWireframe(GLenum front, GLenum back) override;
                void drawImmediate() override;
                void scale(const fPoint* factor) override;
                short getImage(int state, short defImage) const override;
                const wchar_t* getString(DWORD tag) const override;
                int getInt(DWORD tag) const override;
                bool isEmpty() const override;
                void read(iStream& in, DWORD fmt) override;
                void write(oStream& out, DWORD fmt) const override;
                bool toText(String& out, DWORD fmt) const override;

                int                 m_id;
                Collection<Vertex>  m_vertices;
                Faces               m_faces;
                Collection<pvBlock> m_pvBlocks;
            };

            static const int kMinRecordSize = 8;
            Module(int index = 0, int unused = 0);
            virtual ~Module();

            void clear() override;
            void translate(const fPoint* origin) override;
            void computeBounds() override;
            void fillProperties(ListCtrl* list) override;
            void drawWireframe(GLenum front, GLenum back) override;
            void drawImmediate() override;
            void scale(const fPoint* factor) override;
            virtual bool isEmpty();
            short getImage(int state, short defImage) const override;
            int getId() const override;
            const wchar_t* getString(DWORD tag) const override;
            int getInt(DWORD tag) const override;
            void reset() override;
            int compare(const Node* other, DWORD tag) const override;
            void read(iStream& in, DWORD fmt) override;
            void write(oStream& out, DWORD fmt) const override;
            bool toText(String& out, DWORD fmt) const override;

            int               m_index;
            int               m_textureIndex;
            int               m_unk7C;
            Collection<Block> m_blocks;
        };

        static const int kMinRecordSize = 12;
        Segment(int index = 0, int unused = 0);
        virtual ~Segment();

        void clear() override;
        void translate(const fPoint* origin) override;
        void computeBounds() override;
        void fillProperties(ListCtrl* list) override;
        void drawWireframe(GLenum front, GLenum back) override;
        void drawImmediate() override;
        void scale(const fPoint* factor) override;
        DWORD setFlag(DWORD mask, bool on) override;
        short getImage(int state, short defImage) const override;
        int getId() const override;
        const wchar_t* getString(DWORD tag) const override;
        int getInt(DWORD tag) const override;
        void reset() override;
        int compare(const Node* other, DWORD tag) const override;
        void read(iStream& in, DWORD fmt) override;
        void write(oStream& out, DWORD fmt) const override;
        bool toText(String& out, DWORD fmt) const override;

        int                m_index;
        Matrice            m_matrice;
        Collection<Module> m_modules;
        Collection<Bound>  m_bounds;
    };

    explicit Dungeon(int unused = 0);
    virtual ~Dungeon();

    void clear() override;
    void translate(const fPoint* origin) override;
    void computeBounds() override;
    void fillProperties(ListCtrl* list) override;
    void drawWireframe(GLenum front, GLenum back) override;
    void drawImmediate() override;
    void scale(const fPoint* factor) override;
    DWORD setFlag(DWORD mask, bool on) override;
    virtual bool loadTextures();
    bool isEmpty() const override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    Names                  m_names;
    int                    m_recordCount;
    primitiveArray<long>   m_slots;
    Collection<Segment>    m_segments;
    GLuint*                m_textures;
};
static_assert(sizeof(Dungeon::Segment::Module::Block::Vertex) == 0x38, "Block::Vertex size");
static_assert(sizeof(Dungeon::Segment::Module::pvBlock) == 0x70, "pvBlock size");
static_assert(sizeof(Dungeon::Segment::Bound) == 0x34, "Bound size");
static_assert(sizeof(Dungeon::Segment::Module::Block) == 0xE0, "Block size");
static_assert(sizeof(Dungeon::Segment::Module) == 0xA0, "Module size");
static_assert(sizeof(Dungeon::Segment) == 0x100, "Segment size");
static_assert(sizeof(Dungeon) == 0xD4, "Dungeon size");
