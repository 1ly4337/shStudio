#pragma once
#include <windows.h>

class iStream;

class DropTarget
{
public:
    DropTarget() {}
    virtual ~DropTarget() {}
    virtual bool acceptsFormat(int format) = 0;
    virtual void setDropHighlight(bool on) = 0;
    virtual void onDropStream(int format, iStream& stream) = 0;
    virtual void dropGlobal(int format, HGLOBAL hMem);

};
static_assert(sizeof(DropTarget) == 4, "DropTarget size");
