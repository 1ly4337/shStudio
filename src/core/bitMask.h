#pragma once

template <class T>
class bitMask
{
public:
    bitMask() : m_bits(0) {}
    virtual ~bitMask() {}

    bool set(int bit, bool on)
    {
        const T mask = (T)(1 << bit);
        bool old = (m_bits & mask) != 0;
        if (on)
            m_bits |= mask;
        else
            m_bits &= (T)~mask;
        return old;
    }

    bool test(int bit) const { return (m_bits & (T)(1 << bit)) != 0; }

    T get() const { return m_bits; }
    void setAll(T bits) { m_bits = bits; }

public:
    T m_bits;
};
static_assert(sizeof(bitMask<unsigned char>) == 8, "bitMask<unsigned char> size");
