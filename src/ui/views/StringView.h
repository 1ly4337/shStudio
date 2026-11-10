#pragma once
#include "DocView.h"
#include "StringList.h"
#include "p05_common.h"

template <class T>
class StringView : public DocView {
public:
    explicit StringView(FileTree* node)
        : DocView(node, &m_object), m_list(this), m_object()
    {
        m_list.m_strings = &m_object.m_names;
        m_strings = &m_object.m_names;
    }
    ~StringView() override {}

    const wchar_t* getClassName() override { return L"StringView"; }

    void onRegisterClass(WNDCLASSEXW* wc) override
    {
        int cy = GetSystemMetrics(SM_CYICON);
        int cx = GetSystemMetrics(SM_CXICON);
        wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x81, IMAGE_ICON, cx, cy, LR_SHARED);
        int cySm = GetSystemMetrics(SM_CYSMICON);
        int cxSm = GetSystemMetrics(SM_CXSMICON);
        wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x81, IMAGE_ICON, cxSm, cySm, LR_SHARED);
    }

    bool onCreate() override
    {
        bool result = DocView::onCreate();
        if (result) {
            HWND hWnd = m_hWnd;
            RECT rcBuf;
            RECT* rc = getClientRect(&rcBuf);
            return m_list.create(hWnd, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left,
                              rc->bottom - rc->top, 0x50000000, 0, rc->bottom - rc->top);
        }
        return result;
    }

    void updateMenu(HMENU menu) override
    {
        Doc::updateMenu(menu);
        enableMenuItem(0xE12A, true);
    }

    int getMenuId() override { return 164; }

    StringList m_list;
    T          m_object;
    Strings*   m_strings;
};
