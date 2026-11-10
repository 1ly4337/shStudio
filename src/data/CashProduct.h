#pragma once
#include "p02_common.h"

class RecordSet;

class CashProduct : public Node
{
public:
    CashProduct(int version, int row, int unused);
    virtual ~CashProduct();
    CashProduct& operator=(const CashProduct& src);

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    int  getVersion() const override;
    bool copyFrom(const Streamable* src) override;

    int  getId() const override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;

    virtual bool loadRecord(RecordSet& rs, bool full);

    int    m_version;
    int    m_row;
    int    m_bag;
    int    m_icon;
    int    m_cost;
    int    m_cost2;
    int    m_itemId[24];
    BYTE   m_itemCount[24];
    String m_name;
    String m_code;
    String m_desc;
    String m_title;
    int    m_itemTotal;
};
static_assert(sizeof(CashProduct) == 0x128, "CashProduct size");
