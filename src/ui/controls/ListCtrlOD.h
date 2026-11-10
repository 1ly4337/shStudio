#pragma once
#include "ListCtrl.h"

class ListCtrlOD : public ListCtrl
{
public:
    ListCtrlOD() {}
    virtual ~ListCtrlOD() {}

    DWORD getStyle() override;
    DWORD getCtrlStyle() override { return 0x1000; }
    bool onCommand(int code, LPARAM ctrl, int id) override;
    bool onInitMenuPopup(HMENU menu) override;
    bool onKey(short vk, int flags, bool down) override;
    int insertItem(Node* node, int index) override;
    Node* getItemParam(int index) override;
    void updateItem(int index) override;
    void updateAll() override;
    bool insertNew(int index) override;
    bool deleteSelected(const wchar_t* verb) override;

    virtual Node* createNode() { return 0; }
    virtual bool clearItem(int index) { return false; }
    virtual bool moveItemUp(int index) { return false; }
    virtual bool moveItemDown(int index) { return false; }
    virtual int indexOfNode(Node* node) = 0;
    virtual Node* findNode(Node* like) = 0;
    virtual UINT getClipboardFormat() { return 0; }
    virtual bool cut();
    virtual bool copy();
    virtual bool paste();
    virtual bool selectAll();
    virtual bool clearSelected();
    virtual Node* readNode(iStream& in);
    virtual bool confirm(const wchar_t* verb, int count);
    virtual bool onCopied(Node* node);
    virtual void onPasted(Node* node) {}
    virtual void onPasteDone(bool added) {}
};
static_assert(sizeof(ListCtrlOD) == 0xF4, "ListCtrlOD size");
