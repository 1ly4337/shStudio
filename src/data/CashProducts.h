#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "vLong.h"
#include "CashProduct.h"

class String;
class iStream;
class oStream;

class CashProducts : public ptrArray<CashProduct*>, public Serializable
{
public:
    explicit CashProducts(int version);
    virtual ~CashProducts();

    bool isEmpty() const override { return ptrArray<CashProduct*>::isEmpty(); }
    void clear() override;
    const wchar_t* getFilePrefix() const override;
    void load(iStream& in, int arg2, int arg3) override;
    int serialV5() const override { return m_version; }
    HRESULT setCodePage(int version) override;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    virtual void exportText(String& out, DWORD fmt) {}

    int sub_420770();
    CashProduct* sub_420800(wchar_t* code);
    bool toSql(String& out, DWORD fmt) const;

public:
    int     m_version;
    vLong   m_bags;
    DWORD   m_unk44;
};
static_assert(sizeof(CashProducts) == 0x50, "CashProducts size");
