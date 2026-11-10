#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "ListCtrl.h"
#include "EditText.h"

class ChooseCodepageDialog : public Modal
{
public:
    class ListCtrlCP : public ListCtrl
    {
    public:
        explicit ListCtrlCP(ChooseCodepageDialog* owner) : m_owner(owner) {}
        virtual DWORD getStyle() { return 0x10800009; }
        virtual bool onCreate();
        virtual void onItemChanged(int item, UINT state);
        int findCodepage(int codepage);

        ChooseCodepageDialog* m_owner;
    };

    ChooseCodepageDialog(const wchar_t* fileName, int unused);
    virtual ~ChooseCodepageDialog();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual void onShowDialog();

    bool create(HWND parent);
    static int run(HWND parent, const wchar_t* fileName);

public:
    String     m_fileName;
    ListCtrlCP m_list;
    EditText   m_cpEdit;
    HWND       m_cpName;
    int        m_codepage;
};
static_assert(sizeof(ChooseCodepageDialog) == 0x1D8, "ChooseCodepageDialog size");

extern int g_lastChosenCodepage;
