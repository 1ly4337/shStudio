#pragma once
#include "p02_common.h"
#include "Matrix4.h"
#include "Point4.h"

class AniStep
{
public:
    class Rotation
    {
    public:
        Rotation() : time(0) {}
        virtual void read(iStream& in, DWORD fmt);
        virtual void write(oStream& out, DWORD fmt) const;

        DWORD  time;
        Point4 value;
    };
    class Translation
    {
    public:
        Translation() : time(0) {}
        virtual void read(iStream& in, DWORD fmt);
        virtual void write(oStream& out, DWORD fmt) const;

        DWORD  time;
        fPoint value;
    };

    AniStep()
        : m_id(0), m_rotationCount(0), m_rotations(0), m_translationCount(0), m_translations(0)
    {
        m_matrix.m[0] = 1.0f;
        m_matrix.m[5] = 1.0f;
        m_matrix.m[10] = 1.0f;
        m_matrix.m[15] = 1.0f;
    }
    virtual ~AniStep()
    {
        if (m_rotations)
        {
            delete[] m_rotations;
            m_rotations = 0;
        }
        if (m_translations)
        {
            delete[] m_translations;
            m_translations = 0;
        }
    }
    virtual void read(iStream& in, DWORD fmt);
    virtual void write(oStream& out, DWORD fmt) const;
    virtual bool toText(String& out, DWORD fmt) const;

    int          m_id;
    Matrix4      m_matrix;
    int          m_rotationCount;
    Rotation*    m_rotations;
    int          m_translationCount;
    Translation* m_translations;
};
static_assert(sizeof(AniStep::Rotation) == 0x1C, "AniStep::Rotation size");
static_assert(sizeof(AniStep::Translation) == 0x18, "AniStep::Translation size");
static_assert(sizeof(AniStep) == 0x5C, "AniStep size");
