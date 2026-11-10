#pragma once
#include "NpcList.h"

class StandardsList : public NpcList
{
public:
    StandardsList(DocView* view, NpcData* data);
    virtual ~StandardsList() {}

    bool onCreate() override;
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    UINT getClipboardFormat() override { return 532; }

    void refill();

};
