#pragma once
#include "Streamable.h"

class Displayable : public virtual Streamable
{
public:
    Displayable() : m_data(0) {}
    virtual short getImage(int state, short defImage) const { return defImage; }
    virtual int getId() const { return -1; }
    virtual int displayV2() const { return 0; }
    virtual void displayV3() {}
    virtual const wchar_t* getString(DWORD tag) const { return 0; }
    virtual int getInt(DWORD tag) const { return -1; }

    int m_data;
};
