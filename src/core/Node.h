#pragma once
#include "Displayable.h"

class Node : public Displayable
{
public:
    Node() {}
    virtual bool nodeV6(int arg) { return false; }
    virtual bool setInt(DWORD tag, int value) { return false; }
    virtual bool setString(DWORD tag, const wchar_t* value) { return false; }
    virtual void reset() = 0;
    virtual int compare(const Node* other, DWORD tag) const = 0;
    virtual int nodeV11(int arg) { return 0; }
    virtual bool nodeV12(int arg) { return false; }
};
