#include "CashProducts.h"
#include "_imports_p07.h"

CashProducts::CashProducts(int version)
    : ptrArray<CashProduct*>(32)
    , Serializable()
    , m_bags()
{
    m_version = version;
}

CashProducts::~CashProducts()
{
}

const wchar_t* CashProducts::getFilePrefix() const
{
    return L"BinarySData\\DBItemSellText_";
}
