#pragma once
#include "Control32.h"
#include "StringTable.h"

class ComboBox : public Control32 {
public:
    ComboBox();
    ~ComboBox() override;
    String* getText(String* out) override;

    LRESULT getSelectedData();
    LRESULT getCurSel();
    bool addString(const wchar_t* text);
    bool addItem(const wchar_t* text, int data);
    bool setItemText(int data, const wchar_t* text);
    bool selectData(WPARAM data);
    static LRESULT CALLBACK ownerDrawSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                  UINT_PTR idSubclass, DWORD_PTR refData);

    int         m_dropLines;
    short       m_flags;
    StringTable m_items;
};
static_assert(sizeof(ComboBox) == 0x28, "ComboBox size");
