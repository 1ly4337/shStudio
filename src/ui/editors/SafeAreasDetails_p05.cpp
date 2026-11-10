#include "p05_common.h"
#include "SafeAreasDetails.h"

bool SafeAreasDetails::SafeAreasList::onCreate()
{
    bool result = ListCtrl::onCreate();
    if (result) {
        addColumn(L"Indx", 40, 18, 'indx');
        addColumn(L"X2", 55, 130, 'psX2');
        addColumn(L"Z2", 55, 130, 'psZ2');
        addColumn(L"X4", 55, 130, 'psX4');
        addColumn(L"Z4", 55, 130, 'psZ4');
        addColumn(L"P1", 40, 18, 'unk1');
        addColumn(L"P2", 40, 18, 'unk2');
        return true;
    }
    return result;
}
