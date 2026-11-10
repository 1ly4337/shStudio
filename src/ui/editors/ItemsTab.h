#pragma once
#include <windows.h>
#include "TabCtrl.h"
#include "ItemList.h"

class ItemView;
class GradeList;
namespace Data { class Items; }

class ItemsTab : public TabCtrl<ItemList*>
{
public:
    explicit ItemsTab(ItemView* view) : m_view(view), m_items(0), m_grades(0) {}

    void onActivate(bool active) override;
    HWND getPage(int index) override;
    bool createPages(RECT* rc) override;

    void reloadAll();
    void setGrade(short grade);

public:
    ItemView*    m_view;
    Data::Items* m_items;
    GradeList*   m_grades;
};
