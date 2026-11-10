#pragma once
#include <windows.h>
#include <commctrl.h>
#include "Control.hpp"

class TreeList;

class TreeListCtrl : public Control
{
public:
    explicit TreeListCtrl(TreeList* owner) : m_owner(owner), m_hotItem(0), m_tracking(false), m_totalWidth(0) {}

    virtual bool onDropFile(int index, POINT* pt, const wchar_t* path) override;
    const wchar_t* getClassName() override;
    DWORD getStyle() override;
    bool onKey(short vk, int flags, bool down) override;
    bool onChar(WPARAM ch, LPARAM flags) override { return true; }
    bool onLButton(int keys, POINT* pt, bool down) override;
    bool onRButton(int keys, POINT* pt, bool down) override;
    bool onDoubleClick(int keys, POINT* pt, int button) override;
    bool onMouseMove(int keys, POINT* pt) override;
    bool onMouseLeave() override;
    LRESULT onCustomDraw(NMLVCUSTOMDRAW* cd, int ctrlId) override;

    virtual LPCWSTR getThemeClass();

    LPRECT getItemRect(LPRECT out, HTREEITEM item);

public:
    TreeList*  m_owner;
    HTREEITEM  m_hotItem;
    bool       m_tracking;
    int        m_totalWidth;
};
static_assert(sizeof(TreeListCtrl) == 0x28, "TreeListCtrl size");
