#pragma once
#include <windows.h>
#include "HTable.h"

class FolderTree;

struct BitmapsID : public HTable<long, HBITMAP>
{
    BitmapsID() : HTable<long, HBITMAP>(32, 32) {}
    virtual ~BitmapsID() {}
    virtual Entry* clearEntry(Entry* e) override;
};
static_assert(sizeof(BitmapsID) == 0x14, "BitmapsID size");

HRESULT loadBitmapsID(FolderTree* folder, BitmapsID* ids);
