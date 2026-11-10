#include "p05_common.h"
#include "PackArchiveDialog.h"

LPCWSTR PackArchiveDialog::getTitle()
{
    return L"Creating Archive";
}

void* PackArchiveDialog::getWorkerProc()
{
    return (void*)&PackArchiveDialog::threadProc;
}

__int64 PackArchiveDialog::getWorkSize()
{
    return FIELD(__int64, m_root, 0x80) >> 16;
}
