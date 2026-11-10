#pragma once
#include "Control32.h"

class EditText : public Control32 {
public:
    EditText();
    LRESULT setText(const WCHAR* text) override;
    String* getText(String* out) override;

    bool create(HWND parent, HMENU id, int x, int y, int cx, int cy, const wchar_t* text, DWORD style,
                int unused);
    HWND createNumber(HWND parent, HMENU id, int x, int y, int cx, int cy, UINT value, DWORD style,
                      int unused);
    int   getInt(int def);
    float getFloat();
    void  setInt(int value);
    void  setFloat(float value);
};
static_assert(sizeof(EditText) == 0x0C, "EditText size");
