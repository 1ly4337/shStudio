#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "AniStep.h"

class AniStep;
class iStream;
class String;

class AniFile {
public:
    AniFile() : m_header(0), m_version(0), m_unk(0), m_flags(0), m_steps() {}
    ~AniFile();

    virtual bool isEmpty();
    virtual void clear();

    void read(iStream& in);
    String& dump(String& out);

    int              m_header;
    short            m_version;
    short            m_unk;
    short            m_flags;
    ptrArray<AniStep*> m_steps;
};
static_assert(sizeof(AniFile) == 0x28, "AniFile size");
