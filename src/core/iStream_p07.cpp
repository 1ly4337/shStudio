#include "iStream.h"
#include "Bytes.h"
#include "_imports_p07.h"

void iStream::readBytes(Bytes& dst, int size)
{
    aStream& d = dst;
    if (d.m_capacity < size)
        d.grow(size - d.m_capacity);
    read(d.m_data, size);
    if (size <= d.m_capacity)
        d.m_size = size;
}
