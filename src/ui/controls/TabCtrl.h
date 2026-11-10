#pragma once
#include <windows.h>
#include <commctrl.h>
#include "TabControl.h"
#include "HTablePtr.h"
#include "Container.h"

inline bool tabCtrlCreatePage(Container* page, HWND parent, HMENU id, const RECT* rc)
{
    return page->createChild(parent, id, rc, 0, 0, 0);
}
inline bool tabCtrlCreatePage(Control* page, HWND parent, HMENU id, const RECT* rc)
{
    return page->create(parent, id, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, 0, 0, 0);
}

template <class T>
class TabCtrl : public TabControl
{
public:
    TabCtrl() : m_pages(32, 32) {}

    virtual ~TabCtrl()
    {
        m_pages.clear();
    }

    HWND getPage(int index) override
    {
        T page = pageAt(index);
        if (page)
            return page->m_hWnd;
        return 0;
    }

    bool destroyPage(int index) override
    {
        T page = pageAt(index);
        if (!page)
            return false;
        removePage(index);
        delete page;
        return true;
    }

    virtual bool addPage(T page, long key, LPCWSTR title, int index)
    {
        if (!page)
            return false;
        int count = (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
        RECT rc;
        if (tabCtrlCreatePage(page, m_hWnd, (HMENU)(INT_PTR)(count + 101), getClientRect(&rc))
            && insertPage(title, page->m_hWnd, (LPARAM)page, index)) {
            long k = key;
            if (!k)
                k = m_pages.m_count;
            m_pages.set(k, page);
            return true;
        }
        delete page;
        return false;
    }

    virtual T pageAt(int index)
    {
        int i = index;
        if (i == -1)
            i = (int)SendMessageW(m_hWnd, TCM_GETCURSEL, 0, 0);
        if (i < 0 || i >= (int)SendMessageW(m_hWnd, TCM_GETITEMCOUNT, 0, 0))
            return 0;
        TCITEMW item;
        item.dwStateMask = 0;
        item.dwState = 0;
        item.pszText = 0;
        item.cchTextMax = 0;
        item.iImage = 0;
        item.lParam = 0;
        item.mask = TCIF_PARAM;
        if (SendMessageW(m_hWnd, TCM_GETITEMW, i, (LPARAM)&item) != 1)
            return 0;
        return (T)item.lParam;
    }

    virtual T getActivePage()
    {
        int i = 0;
        if (m_pages.m_count <= 0)
            return 0;
        for (;;) {
            T page = pageAt(i);
            if (page->m_hWnd == m_currentPage)
                return page;
            if (++i >= m_pages.m_count)
                return 0;
        }
    }

    virtual T getPageByKey(long key)
    {
        int n = m_pages.m_count;
        int i = 0;
        if (n <= 0)
            return 0;
        typename HTable<long, T>::Entry* table = m_pages.m_table;
        for (typename HTable<long, T>::Entry* e = table; e->key != key; ++e)
            if (++i >= n)
                return 0;
        if (i == -1)
            return 0;
        return table[i].value;
    }

public:
    HTablePtr<long, T> m_pages;
};
