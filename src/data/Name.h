#pragma once
#include <windows.h>
#include "Node.h"
#include "String.hpp"

class iStream;
class oStream;

class Name : public Node
{
public:
    Name(int id, int owner);
    virtual ~Name();

    int getId() const override;
    int displayV2() const override;
    const wchar_t* getString(DWORD tag) const override;
    int getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* text) override;
    void reset() override;
    int compare(const Node* other, DWORD tag) const override;

    virtual void clearAll();
    virtual bool isEmpty();
    virtual int compareById(Node* other);

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD tag) const override;

    int    m_id;
    int    m_owner;
    String m_file;
    String m_name;
};
static_assert(sizeof(Name) == 0x5C, "Name size");

bool readFixedString256(iStream& s, String& dst, UINT codePage);
void writeFixedString256(oStream& s, const String& src, UINT codePage);
