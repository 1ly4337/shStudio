#pragma once
#include "ptrArray.h"
#include "SqlNode.h"

class SqlNodes : public ptrArray<SqlNode*>
{
public:
    SqlNodes() {}
    virtual ~SqlNodes() {}
};
static_assert(sizeof(SqlNodes) == 0x18, "SqlNodes size");
