#include <windows.h>
#include "ida_compat.h"
#include "RecordSet.h"
#include "_imports_B.h"

bool RecordSet::next()
{
    void* impl = FIELD(void*, this, 0x04);
    if (!impl)
        return false;
    void* rs = P08B_VFN(impl, 0x04, void*)(impl);
    return P08B_VFN(rs, 0x0C, bool)(rs);
}
