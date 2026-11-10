#pragma once
#include "ptrArray.h"
#include "Doc.h"

class Doc;

class Documents : public ptrArray<Doc*>
{
public:
    Documents() {}
};
