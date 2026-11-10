#pragma once
#include "ListCtrlOD.h"
#include "CapesData.h"
#include "DocView.h"
#include "Collection.hpp"

class DocView;
class CapesData;
class Node;

class PcList : public ListCtrlOD
{
public:
    PcList(DocView* view) : m_view(view), m_capes(0) {}

    DWORD getStyle() override { return 9; }
    bool onCreate() override;
    DWORD getExStyle() override { return 0x21; }
    void onChanged() override { m_view->setModified(true); }
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override { return (Node*)m_capes->get(index); }
    bool sort() override
    {
        CapesData* data = m_capes;
        int count = data->m_count;
        if (count < 2)
            return false;
        data->m_changes = 0;
        quickSort(*data, 0, count - 1, m_sortColumn, m_prevSortColumn, m_sortAscending);
        return data->m_changes > 0;
    }
    bool removeItem(int index) override { return m_capes->remove(index); }
    bool onActivateItem(int index, int keys) override;
    void onItemChanged(int index, UINT newState) override;
    Node* createNode() override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;

public:
    DocView*   m_view;
    CapesData* m_capes;
};
static_assert(sizeof(PcList) == 0xFC, "PcList size");
