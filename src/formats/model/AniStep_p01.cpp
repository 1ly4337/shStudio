#include "AniStep.h"
#include "iStream.hpp"
#include "oStream.hpp"

void AniStep::Rotation::read(iStream& in, DWORD fmt)
{
    time = in.readDword();
    value.read(in, 'file');
}

void AniStep::Rotation::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)time;
    value.write(out, 'file');
}

void AniStep::Translation::read(iStream& in, DWORD fmt)
{
    time = in.readDword();
    value.read(in, 'file');
}

void AniStep::Translation::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)time;
    value.write(out, 'file');
}
