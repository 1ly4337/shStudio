#pragma once
#include "ida_compat.h"

#define G4_ITEM_NAME_BUF(it)  FIELD(const wchar_t*, it, 0x18)
#define G4_ITEM_NAME_LEN(it)  FIELD(unsigned, it, 0x1C)
#define G4_ITEM_TYPE(it)      FIELD(unsigned char, it, 0x54)
#define G4_ITEM_MODEL(it)     FIELD(unsigned char, it, 0x56)
#define G4_ITEM_LEVEL(it)     FIELD(short, it, 0x58)
#define G4_ITEM_FACTION(it)   FIELD(unsigned char, it, 0x5A)
#define G4_ITEM_SLOT(it)      FIELD(unsigned char, it, 0x7B)
#define G4_ITEM_FLAG_E0(it)   FIELD(unsigned char, it, 0xE0)
