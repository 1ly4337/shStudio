#include "ItemSetView.h"
#include "Data_ItemSets.h"
#include "String.hpp"
#include "cash_g3.h"
#include "_imports_p07.h"

ItemSetView::ItemSetView(void* file, Serializable* data)
    : SDataView(file, data)
    , m_list(this)
{
    if (data) {
        m_sets = dynamic_cast<Data::ItemSets*>(data);
    } else {
        Data::ItemSets* sets = new Data::ItemSets();
        m_sets = sets;
        Serializable* serial = sets ? static_cast<Serializable*>(sets) : 0;
        m_content = serial;
        FIELD(Serializable*, this, 0x50) = serial;
    }
    m_list.m_sets = m_sets;
}

ItemSetView::~ItemSetView()
{
}

bool ItemSetView::onCreate()
{
    if (!SDataView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rc;
    RECT* r = getClientRect(&rc);
    if (!m_list.create(parent, (HMENU)0x65, r->left, r->top, r->right - r->left, r->bottom - r->top,
                                0x50000000u, 0x100, 0))
        return false;
    m_list.setItemCount(m_sets->size());
    return true;
}

COMDLG_FILTERSPEC* ItemSetView::getFilters(int* count)
{
    *count = 1;
    return &g_itemSetFileTypes;
}

bool ItemSetView::exportData(String* out, DWORD format)
{
    return m_sets->exportText(*out, format);
}
