#include "DropTarget.h"
#include "iStream.hpp"
#include "_imports_p10.h"

void DropTarget::dropGlobal(int format, HGLOBAL hMem)
{
    if (!hMem)
        return;
    SIZE_T size = GlobalSize(hMem);
    void* data = GlobalLock(hMem);
    if (!data)
        return;
    if (!size)
        return;
    iStream stream;
    stream.assign(data, (unsigned int)size);
    GlobalUnlock(hMem);
    try
    {
        onDropStream(format, stream);
    }
    catch (...)
    {
    }
}
