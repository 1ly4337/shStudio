#pragma once
#include "ListCtrl.h"

class DocView;
class Strings;

class StringList : public ListCtrl
{
public:
    StringList(DocView* view) : m_view(view), m_strings(0) {}
    virtual ~StringList() {}

    DWORD getStyle() override;
    bool onCreate() override;
    void onChanged() override;
    bool setCellText(int index, int subItem, const wchar_t* text) override;
    DWORD getExStyle() override;
    int getColumnAtCursor(int index, int* range) override;
    bool insertNew(int index) override;
    bool removeItem(int index) override;

    virtual void setStrings(Strings* strings);
    virtual LRESULT fill();

public:
    DocView* m_view;
    Strings* m_strings;
};
static_assert(sizeof(StringList) == 0xFC, "StringList size");
