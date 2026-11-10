#include "String.hpp"

void String::toUpperAscii()
{
    if (m_length) {
        unsigned i = 0;
        do {
            wchar_t c = m_buffer[i];
            if ((unsigned short)(c - L'a') <= 0x19u)
                m_buffer[i] = (wchar_t)(c - 32);
            ++i;
        } while (i < m_length);
    }
}
