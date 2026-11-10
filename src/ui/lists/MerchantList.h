#pragma once
#include "NpcList.h"

class MerchantList : public NpcList
{
public:
    MerchantList(DocView* view, NpcData* data);
    virtual ~MerchantList() {}

    bool onCreate() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override { return 533; }

public:
    StandardNPCs*    m_list;
};
