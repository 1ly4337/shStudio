#pragma once
#include <windows.h>
#include <ole2.h>
#include <commctrl.h>
#include "TreeList.h"
#include "ImageList.h"
#include "GradeSets.h"

class ItemView;
class GDC;
class Node;
namespace Data { class Items; class Item; }

class GradeList : public TreeList
{
public:
    GradeList(ItemView* owner, Data::Items* data);
    virtual ~GradeList();

    virtual BOOL show(bool show);
    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);

    virtual LRESULT deleteAllItems();
    virtual int getNodeImage(Node* node);
    virtual int getItemMenu(HTREEITEM item);
    virtual bool onItemActivate(HTREEITEM item, WPARAM keys);
    virtual bool removeChildNode(Node* parent, HTREEITEM item);
    virtual IDataObject* createDataObject();
    virtual bool canMultiSelect(HTREEITEM item);
    virtual void drawFirstColumn(GDC* dc, Node* node, RECT* rc);

    virtual DWORD getNameTag() { return 'name'; }
    virtual DWORD getDetailsTag();
    virtual bool isGroupItem(HTREEITEM item);
    virtual void dropSelection(HTREEITEM target, LPARAM data, int effect);

    void refresh();
    HTREEITEM findInsertPos(short grade);
    short gradeOf(HTREEITEM item);

public:
    ItemView*    m_owner;
    Data::Items* m_data;
    GradeSets    m_sets;
    ImageList    m_images;
};
static_assert(sizeof(GradeList) == 0x114, "GradeList size");
