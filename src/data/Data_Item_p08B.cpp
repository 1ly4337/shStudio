#include <windows.h>
#include "ida_compat.h"
#include "Data_Item.h"
#include "RecordSet.h"
#include "String.hpp"
#include "Data_Item_fields_B.h"
#include "_imports_B.h"

static const wchar_t* rsString(RecordSet* rs, const wchar_t* column)
{
    void* impl = FIELD(void*, rs, 0x04);
    if (!impl)
        return 0;
    void* set = P08B_VFN(impl, 0x04, void*)(impl);
    return P08B_VFN(set, 0x10, const wchar_t*, const wchar_t*)(set, column);
}

static int rsInt(RecordSet* rs, const wchar_t* column)
{
    void* impl = FIELD(void*, rs, 0x04);
    if (!impl)
        return 0;
    void* set = P08B_VFN(impl, 0x04, void*)(impl);
    return P08B_VFN(set, 0x18, int, const wchar_t*)(set, column);
}

bool Data::Item::readSQLRow(RecordSet* rs, bool withName)
{
    Data::Item* item = this;
    if (withName)
        DI_ItemName(item) = rsString(rs, L"ItemName");
    DI_ReqLevel(item)   = (short)rsInt(rs, L"ReqLevel");
    DI_Country(item)    = (unsigned char)rsInt(rs, L"Country");
    DI_Class(item, 0)   = (unsigned char)rsInt(rs, L"Attackfighter");
    DI_Class(item, 1)   = (unsigned char)rsInt(rs, L"Defensefighter");
    DI_Class(item, 2)   = (unsigned char)rsInt(rs, L"Patrolrogue");
    DI_Class(item, 3)   = (unsigned char)rsInt(rs, L"Shootrogue");
    DI_Class(item, 4)   = (unsigned char)rsInt(rs, L"Attackmage");
    DI_Class(item, 5)   = (unsigned char)rsInt(rs, L"Defensemage");
    DI_Grow(item)       = (unsigned char)rsInt(rs, L"Grow");
    DI_ReqStr(item)     = (unsigned short)rsInt(rs, L"ReqStr");
    DI_ReqDex(item)     = (unsigned short)rsInt(rs, L"ReqDex");
    DI_ReqRec(item)     = (unsigned short)rsInt(rs, L"ReqRec");
    DI_ReqInt(item)     = (unsigned short)rsInt(rs, L"ReqInt");
    DI_ReqWis(item)     = (unsigned short)rsInt(rs, L"ReqWis");
    DI_ReqLuc(item)     = (unsigned short)rsInt(rs, L"ReqLuc");
    DI_ReqVg(item)      = (unsigned short)rsInt(rs, L"ReqVg");
    DI_ReqOg(item)      = (unsigned char)rsInt(rs, L"ReqOg");
    DI_ReqIg(item)      = (unsigned char)rsInt(rs, L"ReqIg");
    DI_Range(item)      = (unsigned short)rsInt(rs, L"Range");
    DI_AttackTime(item) = (unsigned char)rsInt(rs, L"AttackTime");
    DI_Attrib(item)     = (unsigned char)rsInt(rs, L"Attrib");
    DI_Special(item)    = (unsigned char)rsInt(rs, L"Special");
    DI_Slot(item)       = (unsigned char)rsInt(rs, L"Slot");
    DI_Quality(item)    = (unsigned short)rsInt(rs, L"Quality");
    DI_Effect1(item)    = (unsigned short)rsInt(rs, L"Effect1");
    DI_Effect2(item)    = (unsigned short)rsInt(rs, L"Effect2");
    DI_Effect3(item)    = (unsigned short)rsInt(rs, L"Effect3");
    DI_Effect4(item)    = (unsigned short)rsInt(rs, L"Effect4");
    DI_ConstHP(item)    = (unsigned short)rsInt(rs, L"ConstHP");
    DI_ConstSP(item)    = (unsigned short)rsInt(rs, L"ConstSP");
    DI_ConstMP(item)    = (unsigned short)rsInt(rs, L"ConstMP");
    DI_ConstStr(item)   = (unsigned short)rsInt(rs, L"ConstStr");
    DI_ConstDex(item)   = (unsigned short)rsInt(rs, L"ConstDex");
    DI_ConstRec(item)   = (unsigned short)rsInt(rs, L"ConstRec");
    DI_ConstInt(item)   = (unsigned short)rsInt(rs, L"ConstInt");
    DI_ConstWis(item)   = (unsigned short)rsInt(rs, L"ConstWis");
    DI_ConstLuc(item)   = (unsigned short)rsInt(rs, L"ConstLuc");
    DI_Speed(item)      = (unsigned char)rsInt(rs, L"Speed");
    DI_Exp(item)        = (unsigned char)rsInt(rs, L"Exp");
    DI_Buy(item)        = rsInt(rs, L"Buy");
    DI_Sell(item)       = rsInt(rs, L"Sell");
    DI_Grade(item)      = (short)rsInt(rs, L"Grade");
    DI_Drop(item)       = (short)rsInt(rs, L"Drop");
    DI_Server(item)     = (unsigned char)rsInt(rs, L"Server");
    DI_Count(item)      = (unsigned char)rsInt(rs, L"Count");
    return true;
}
