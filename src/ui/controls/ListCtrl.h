#pragma once
#include <windows.h>
#include <commctrl.h>
#include "Control.hpp"
#include "primitiveArray.h"
#include "HTable.h"
#include "String.hpp"
#include "oStream.hpp"
#include "iStream.hpp"

class Node;
class iStream;
struct ImageList;
class ImageStrip;

enum ListColumnFormat
{
    LCF_ALIGN_MASK = 0x0003,
    LCF_VALUE_MASK = 0x0FF0,
    LCF_SIGNED     = 0x0010,
    LCF_UNSIGNED   = 0x0020,
    LCF_HEX        = 0x0040,
    LCF_FLOAT      = 0x0080,
    LCF_NONE       = 0x0100,
    LCF_BOLD       = 0x1000,
    LCF_OWNERTEXT  = 0x2000,
};

class ListCtrl : public Control
{
public:
    ListCtrl();
    virtual ~ListCtrl();

    const wchar_t* getClassName() override;
    DWORD getStyle() override;
    bool onCreate() override;
    bool onCommand(int code, LPARAM ctrl, int id) override;
    void onFocus(bool gained) override;
    bool onKey(short vk, int flags, bool down) override;
    void onSize(int type, int cx, int cy) override;
    bool onLButton(int keys, POINT* pt, bool down) override;
    bool onRButton(int keys, POINT* pt, bool down) override;
    bool onMouseMove(int keys, POINT* pt) override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    LRESULT onCustomDraw(NMLVCUSTOMDRAW* cd, int ctrlId) override;
    LRESULT windowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    virtual void setImageList(HIMAGELIST list, bool isSmall);
    virtual bool addColumn(const wchar_t* title, int width, int format, int columnId);
    virtual bool deleteColumn(int column);
    virtual void onChanged() {}
    virtual void setHighlight(bool on);
    virtual int getCount(int selectedOnly);
    virtual LRESULT setItemCount(int count);
    virtual int insertItem(Node* node, int index);
    virtual bool deleteItem(int index);
    virtual Node* getNode(int index);
    virtual RECT* getItemRect(RECT* rc, int index, int code);
    virtual Node* getItemParam(int index);
    virtual int getItemImage(int index);
    virtual String getCellText(int index, int subItem);
    virtual bool setCellText(int index, int subItem, const wchar_t* text);
    virtual const wchar_t* getItemText(Node* node, int columnId);
    virtual int listV69() { return 0; }
    virtual int getHeaderMenu() { return 0; }
    virtual void onContextMenu(HMENU menu) {}
    virtual int getItemMenu() { return 0; }
    virtual void selectItem(int index);
    virtual void focusItem(int index, bool redraw);
    virtual void updateItem(int index);
    virtual void updateAll();
    virtual DWORD getExStyle();
    virtual HRESULT loadColumnOrder(bool force);
    virtual bool sort();
    virtual int compareItems(Node* a, Node* b) { return 0; }
    virtual bool canMoveItem(int from, int to) { return false; }
    virtual void onDropOutside(int flags, POINT* screenPt, int item) {}
    virtual bool onActivateItem(int index, int keys) { return false; }
    virtual void onItemChanged(int index, UINT newState) {}
    virtual void onBeginDrag(NMLISTVIEW* nm);
    virtual void onColumnClick(NMLISTVIEW* nm);
    virtual HRESULT onGetDispInfo(NMLVDISPINFOW* di);
    virtual int getColumnAtCursor(int index, int* range);
    virtual bool onBeginLabelEdit(NMLVDISPINFOW* di);
    virtual bool onEndLabelEdit(NMLVDISPINFOW* di);
    virtual bool insertNew(int index) { return false; }
    virtual bool deleteSelected(const wchar_t* verb);
    virtual bool removeItem(int index) { return false; }
    virtual int drawItem(HDC dc, int index, POINT* origin, int flags, Node* node);
    virtual bool activateItem(int index, int keys);

    int setSmallImages(const ImageList* images);
    void setImages(const ImageStrip* strip);
    BOOL refreshLayout();
    int columnFromPoint(const POINT* screenPt, LONG* range, int unused);
    bool setView(int view);
    bool setTileView(int unused, const SIZE* tileSize, const primitiveArray<unsigned int>* columns);
    int insertTextItem(const wchar_t* text, int image, int index);
    static int insertRow(ListCtrl* list, const wchar_t* text, ...);
    bool insertNumberRow(const wchar_t* text, int value, int unused);
    const wchar_t* formatValue(Node* node, int columnId, short format);
    LRESULT setItemState(int index, UINT state, UINT mask);
    Node* nodeFromPoint(POINT pt);
    int getNextSelected(int start);
    static int CALLBACK compareProc(LPARAM a, LPARAM b, LPARAM list);
    static LRESULT CALLBACK editSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                             UINT_PTR id, DWORD_PTR refData);

    int  m_columnCount;
    primitiveArray<long> m_columnIds;
    primitiveArray<unsigned short> m_columnFormats;
    primitiveArray<int> m_columnOrder;
    int  m_scrollPos;
    COLORREF m_textColor;
    HWND m_hHeader;
    int  m_headerHeight;
    int  m_totalWidth;
    int  m_headerBottom;
    HIMAGELIST m_smallImages;
    HIMAGELIST m_largeImages;
    SIZE m_smallIconSize;
    SIZE m_largeIconSize;
    HTable<long, long> m_imageMap;
    bool m_trackingMouse;
    int  m_view;
    int  m_hotItem;
    bool m_highlight;
    primitiveArray<unsigned int> m_tileColumns;
    int  m_tileLines;
    SIZE m_tileSize;
    void* m_dropTarget;
    int  m_dragMode;
    int  m_sortColumn;
    int  m_prevSortColumn;
    bool m_sortAscending;
    RECT m_editRect;
    Node* m_editNode;
    int  m_editColumn;
    int  m_editColumnId;
    bool m_editMoving;
    unsigned short m_editFormat;
    bool m_editCancelled;
};
static_assert(sizeof(ListCtrl) == 0xF4, "ListCtrl size");

extern wchar_t g_listValueBuffer[64];
extern NMLVDISPINFOW g_listEditDispInfo;
