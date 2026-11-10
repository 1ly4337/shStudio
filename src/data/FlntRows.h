#pragma once
#include <windows.h>
#include <exception>
#include "iStream.hpp"
#include "oStream.hpp"
#include "Bytes.h"

namespace FlntRows {

void remember(const void* record, const BYTE* data, int size, const BYTE* canon, int canonSize);
void restore(const void* record, BYTE* data, int size);
void forget(const void* record);
void rememberColumns(const void* owner, const BYTE* data, int size);
const BYTE* columns(const void* owner, int* size);
bool capturing();
void setCapturing(bool on);

}

template <class R>
class FlntRowRead
{
public:
    FlntRowRead(R* record, iStream& in, DWORD fmt, int lead = 0)
        : m_record(record), m_in(in), m_active(fmt == 'flnt' && static_cast<aStream&>(in).m_pos >= lead),
          m_start(static_cast<aStream&>(in).m_pos - lead), m_exceptions(std::uncaught_exceptions()) {}
    ~FlntRowRead()
    {
        if (!m_active || std::uncaught_exceptions() != m_exceptions)
            return;
        aStream& in = static_cast<aStream&>(m_in);
        int end = in.m_pos;
        if (end <= m_start || end > in.m_size)
            return;
        try {
            Bytes canon;
            FlntRows::setCapturing(true);
            m_record->R::write(canon, 'flnt');
            FlntRows::setCapturing(false);
            aStream& c = static_cast<aStream&>(canon);
            FlntRows::remember(m_record, in.m_data + m_start, end - m_start, c.m_data, c.m_size);
        } catch (...) {
            FlntRows::setCapturing(false);
        }
    }
private:
    R* m_record;
    iStream& m_in;
    bool m_active;
    int m_start;
    int m_exceptions;
};

class FlntRowWrite
{
public:
    FlntRowWrite(const void* record, oStream& out, DWORD fmt)
        : m_record(record), m_out(out), m_active(fmt == 'flnt' && !FlntRows::capturing()),
          m_start(static_cast<aStream&>(out).m_size), m_exceptions(std::uncaught_exceptions()) {}
    ~FlntRowWrite()
    {
        if (!m_active || std::uncaught_exceptions() != m_exceptions)
            return;
        aStream& out = static_cast<aStream&>(m_out);
        if (out.m_size > m_start)
            FlntRows::restore(m_record, out.m_data + m_start, out.m_size - m_start);
    }
private:
    const void* m_record;
    oStream& m_out;
    bool m_active;
    int m_start;
    int m_exceptions;
};
