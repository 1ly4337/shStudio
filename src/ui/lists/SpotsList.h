#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "Collection.hpp"
#include "SpotDetails.h"
#include "MapSpots.h"
#include "Document.h"

class Node;

template <class T>
class SpotsList : public ListCtrlOD
{
public:
    SpotsList(SpotDetails* owner, Collection<T>* items) : m_owner(owner), m_items(items), m_selectedWnd(0) {}

    void onChanged() override
    {
        m_owner->m_spots->m_document->setModified(true);
    }

    Node* getNode(int index) override
    {
        return m_items->get(index);
    }

    bool sort() override
    {
        int column = m_prevSortColumn;
        Collection<T>* items = m_items;
        bool ascending = m_sortAscending;
        int count = items->m_count;
        int key = m_sortColumn;
        if (count < 2)
            return false;
        items->m_changes = 0;
        items->sort(0, count - 1, key, column, ascending);
        return items->m_changes > 0;
    }

    void onItemChanged(int index, UINT newState) override
    {
        T* item = m_items->get(index);
        m_owner->displaySpot(item);
        HWND prev = m_selectedWnd;
        if (prev)
        {
            MapSpots* spots = m_owner->m_spots;
            SetWindowLongW(prev, GWL_STYLE, GetWindowLongW(prev, GWL_STYLE) & 0xFFFFFF00);
            spots->redrawSpot(prev, 0);
        }
        HWND wnd = item ? item->m_hItem : 0;
        m_selectedWnd = wnd;
        if (wnd)
        {
            MapSpots* spots = m_owner->m_spots;
            SetWindowLongW(wnd, GWL_STYLE, (GetWindowLongW(wnd, GWL_STYLE) & 0xFFFFFF00) | 2);
            spots->redrawSpot(wnd, 0);
        }
    }

    bool removeItem(int index) override
    {
        T* item = m_items->get(index);
        if (!item)
            return false;
        MapSpots* spots = m_owner->m_spots;
        HWND wnd = spots->findSpotWnd(item);
        if (wnd)
        {
            DestroyWindow(wnd);
            spots->m_spotWnds.remove(wnd);
        }
        Collection<T>* items = m_items;
        if (!items->detachItem(item))
            return false;
        items->destroyItem(item);
        m_owner->m_spots->m_document->setModified(true);
        return true;
    }

    bool moveItemUp(int index) override
    {
        if (!m_items->swap(index, index - 1))
            return false;
        m_owner->m_spots->m_document->setModified(true);
        return true;
    }

    bool moveItemDown(int index) override
    {
        if (!m_items->swap(index, index + 1))
            return false;
        m_owner->m_spots->m_document->setModified(true);
        return true;
    }

    Node* findNode(Node* like) override { return 0; }

public:
    SpotDetails*   m_owner;
    Collection<T>* m_items;
    HWND           m_selectedWnd;
};
