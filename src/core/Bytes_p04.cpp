#include <windows.h>
#include <cstring>
#include "Bytes.h"
#include "ida_compat.h"

Bytes& Bytes::append(const Bytes& other)
{
    const aStream* src = static_cast<const aStream*>(&other);
    aStream* dst = static_cast<aStream*>(this);
    dst->append(src->m_data, src->m_size);
    return *this;
}

HGLOBAL Bytes::toHGlobal() const
{
    const aStream& s = *this;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, s.m_size);
    void* p = GlobalLock(hMem);
    memcpy(p, s.m_data, s.m_size);
    GlobalUnlock(hMem);
    return hMem;
}
