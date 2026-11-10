#pragma once
#include "NpcList.h"

class QuestsDetails;

class QuestsList : public NpcList
{
public:
    QuestsList(DocView* view, NpcData* data);
    virtual ~QuestsList() {}

    DWORD getStyle() override { return 0x201; }
    bool onCreate() override;
    bool onCommand(int code, LPARAM ctrl, int id) override;
    bool onInitMenuPopup(HMENU menu) override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int getHeaderMenu() override { return 193; }
    void onContextMenu(HMENU menu) override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override { return 535; }

public:
    QuestsDetails*   m_list;
    bool             m_filter0;
    int              m_filterFlags;
};
