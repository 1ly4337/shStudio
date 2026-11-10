#pragma once
#include <windows.h>
#include "ListCtrlOD.h"

class AltView;
class CharActions;
class Node;

class AltList : public ListCtrlOD
{
public:
    explicit AltList(AltView* view) : m_view(view), m_data(nullptr) {}

    DWORD getStyle() override;
    bool onCreate() override;
    bool onInitMenuPopup(HMENU menu) override;
    void onChanged() override;
    Node* getNode(int index) override;
    int listV69() override;
    DWORD getExStyle() override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    void onItemChanged(int index, UINT newState) override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;

public:
    AltView*     m_view;
    CharActions* m_data;
};
static_assert(sizeof(AltList) == 0xFC, "AltList size");
