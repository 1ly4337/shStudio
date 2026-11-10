#pragma once
#include "Exception.h"

class NoSuchElementException : public Exception
{
public:
    NoSuchElementException() {}
};
static_assert(sizeof(NoSuchElementException) == 0x0C, "NoSuchElementException size");
