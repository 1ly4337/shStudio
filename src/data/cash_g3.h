#pragma once
#include <windows.h>
#include <commctrl.h>
#include <shobjidl.h>
#include <cwchar>
#include <cstring>
#include "ida_compat.h"
#include "String.hpp"

class EditText;

extern const wchar_t* g_bagNames[6];
extern const wchar_t* g_bagTabNames[6];
extern wchar_t g_bagNameBuffer[64];
extern COMDLG_FILTERSPEC g_itemSetFileTypes;
extern wchar_t g_setEditorNumBuffer[16];

inline const wchar_t* g3_bagName(const wchar_t* const* table, int n)
{
    if (n < 1)
        return nullptr;
    if (n > 6) {
        memset(g_bagNameBuffer, 0, sizeof(g_bagNameBuffer));
        swprintf_s(g_bagNameBuffer, 0x40u, L"Unknown %i", n);
        return g_bagNameBuffer;
    }
    return table[n - 1];
}

HWND g3_createLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy, int fontId);

HWND g3_createEdit(EditText& edit, HWND parent, const wchar_t* text, DWORD style,
                   int x, int y, int cx, int cy, int id);

namespace Data { class ItemSet; class Items; class Item; }
inline String& itemSet_name(Data::ItemSet* s)             { return FIELD(String, s, 0x10); }
inline short&  itemSet_type(Data::ItemSet* s, int i)      { return FIELD(short, s, 0x30 + 2 * i); }
inline short&  itemSet_typeId(Data::ItemSet* s, int i)    { return FIELD(short, s, 0x4A + 2 * i); }
inline String& itemSet_bonus(Data::ItemSet* s, int i)     { return FIELD(String, s, 0x64 + 0x20 * i); }
