#pragma once
#include <windows.h>
#include "Document.h"
#include "SqlListView.h"

class FileTree;
class Table;

extern COMDLG_FILTERSPEC g_sqlFileTypes[1];

class SqlTableView : public Document
{
public:
    SqlTableView(FileTree* file, Table* table);
    virtual ~SqlTableView() {}

    LPCWSTR getClassName() override { return L"SqlTableView"; }
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    bool vfunc_150(int arg) override;
    int getMenuId() override { return 174; }
    COMDLG_FILTERSPEC* getFilters(int* count) override;

public:
    SqlListView m_list;
};
static_assert(sizeof(SqlTableView) == 0x1B4, "SqlTableView size");
