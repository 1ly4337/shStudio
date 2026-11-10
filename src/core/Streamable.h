#pragma once
#include <windows.h>

class iStream;
class oStream;
class String;

class Streamable
{
public:
    Streamable() {}
    virtual ~Streamable() {}
    virtual void read(iStream& in, DWORD fmt) {}
    virtual void write(oStream& out, DWORD fmt) const {}
    virtual bool toText(String& out, DWORD fmt) const { return false; }
    virtual int getVersion() const { return 0; }
    virtual bool writeClip(oStream& out) { write(out, 'clip'); return true; }
    virtual bool readClip(iStream& in) { read(in, 'clip'); return true; }
    virtual bool copyFrom(const Streamable* src) { return false; }
};
static_assert(sizeof(Streamable) == 4, "Streamable size");
