#pragma once
#include <windows.h>
#include <commctrl.h>
#include <ole2.h>
#include "Window.h"
#include "TreeListCtrl.h"
#include "primitiveArray.h"
#include "primitiveVector.h"
#include "String.hpp"

class Node;
class Cell;
class GDC;
class WindowDC;

class TreeList : public Window
{
public:
    TreeList();
    virtual ~TreeList();

    virtual bool onCreate();
    virtual void onDestroy();
    virtual void onActivate(bool active);
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual void onFocus(bool focused);
    virtual bool onNotify(NMHDR* hdr, LRESULT* result);
    virtual bool onScroll(bool horizontal, int code, int pos);
    virtual void onSize(WPARAM type, int cx, int cy);
    virtual LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual LRESULT deleteAllItems();
    virtual bool deleteChildren(HTREEITEM item);
    virtual Node* getItemNode(HTREEITEM item);
    virtual String getItemText(HTREEITEM item);
    virtual void onItemsChanged() {}
    virtual bool treeV70(int arg) { return false; }
    virtual Cell* getItemCell(HTREEITEM item);
    virtual HRESULT onGetDispInfo(TVITEMW* item);
    virtual int getNodeImage(Node* node);
    virtual int getItemMenu(HTREEITEM item) { return 0; }
    virtual void onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item) {}
    virtual void onSelChanged(Node* oldNode, Node* newNode) {}
    virtual bool onItemActivate(HTREEITEM item, WPARAM keys);
    virtual void clearSelection(bool resetCaret);
    virtual LRESULT setItemSelected(HTREEITEM item, bool selected);
    bool setFirstVisible(LPARAM item);
    virtual LRESULT selectRange(HTREEITEM from, HTREEITEM to);
    virtual bool onCmdE122() { return false; }
    virtual bool copySelected();
    virtual bool pasteSelected();
    virtual bool editSelected();
    virtual bool deleteSelected();
    virtual bool updateSelected();
    virtual bool editNode(Node* node) { return false; }
    virtual bool removeChildNode(Node* parent, HTREEITEM item) { return false; }
    virtual bool updateChildNode(Node* parent, HTREEITEM item) { return false; }
    virtual IDataObject* createDataObject() { return 0; }
    virtual DWORD getDragEffects();
    virtual bool onBeginDrag(TVITEMW* item);
    virtual bool canMultiSelect(HTREEITEM item) { return false; }
    virtual bool onCtrlDropFile(int index, POINT* pt, const wchar_t* path) { return false; }
    virtual LRESULT onCustomDraw(NMTVCUSTOMDRAW* cd);
    virtual void drawFirstColumn(GDC* dc, Node* node, RECT* rc) {}

    bool insertColumn(const LVCOLUMNW* column, int id);
    HTREEITEM insertItem(HTREEITEM parent, LPCWSTR text, int image, int children, HTREEITEM after);
    HTREEITEM insertNode(HTREEITEM parent, Node* node, int image, int children, HTREEITEM after);
    HTREEITEM insertCell(Cell* cell, HTREEITEM parent, int image, int children, HTREEITEM after);
    BOOL setItemCut(HTREEITEM item, UINT state);
    int recalcColumnWidths();
    BOOL invalidateItem(int item);

public:
    TreeListCtrl m_tree;
    HWND         m_hHeader;
    HWND         m_hScroll;
    int          m_64;
    HTREEITEM    m_anchor;
    primitiveVector<HTREEITEM> m_selection;
    IUnknown*    m_dropTarget;
    int          m_columnCount;
    int          m_totalWidth;
    DWORD        m_8C;
    primitiveArray<long>           m_columnIds;
    primitiveArray<unsigned short> m_columnFormats;
    primitiveArray<long>           m_columnWidths;
    int          m_treeBottom;
    int          m_scrollX;
    int          m_lastWidth;
    WindowDC*    m_trackDC;
    int          m_trackX;
};
static_assert(sizeof(TreeList) == 0xD4, "TreeList size");
