#pragma once
#include <windows.h>
#include "ListCtrl.h"
#include "CashProducts.h"

class CashView;
class ProductTab;
class Node;

class ProductTile : public ListCtrl
{
public:
    ProductTile(CashView* view, ProductTab* tab, long bag, int version);
    virtual ~ProductTile();

    bool onCreate() override;
    bool onCommand(int code, LPARAM ctrl, int id) override;
    bool onInitMenuPopup(HMENU menu) override;
    LRESULT windowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    int compareItems(Node* a, Node* b) override;
    bool canMoveItem(int from, int to) override;
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;

    virtual Node* getNodeAt(int index);
    virtual int indexOfNode(Node* node);
    virtual UINT getClipboardFormat() { return 513; }
    virtual Node* createNode();
    virtual CashProduct* findNode(CashProduct* like);
    virtual int addNode(Node* node, int index);

    bool createChild(HWND parent, HMENU id, const RECT* rc, DWORD style, DWORD exStyle, int unused)
    {
        return Control::create(parent, id, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
                               style, exStyle, unused);
    }

    BOOL fill(CashProducts* all);

public:
    CashView*    m_owner;
    ProductTab*  m_tab;
    CashProducts m_products;
    long         m_bag;
};
static_assert(sizeof(ProductTile) == 0x150, "ProductTile size");
