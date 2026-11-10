#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "ListCtrl.h"
#include "EditText.h"

class Document;

class ChangeCodepageDialog : public Modal
{
public:
    class ListCtrlCP : public ListCtrl
    {
    public:
        explicit ListCtrlCP(ChangeCodepageDialog* owner) : m_owner(owner) {}
        virtual DWORD getStyle() { return 0x10800009; }
        virtual bool onCreate();
        virtual void onItemChanged(int item, UINT state);
        int findCodepage(int codepage);

        ChangeCodepageDialog* m_owner;
    };

    ChangeCodepageDialog(const wchar_t* fileName, int codepage);
    virtual ~ChangeCodepageDialog();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual void onShowDialog();

    bool create(HWND parent);
    static int run(HWND parent, Document* doc);

public:
    String     m_fileName;
    ListCtrlCP m_list;
    EditText   m_cpEdit;
    HWND       m_cpName;
    int        m_codepage;
};
static_assert(sizeof(ChangeCodepageDialog) == 0x1D8, "ChangeCodepageDialog size");
