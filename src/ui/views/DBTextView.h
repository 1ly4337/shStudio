#pragma once
#include <windows.h>
#include <commctrl.h>
#include <string>
#include "DocView.h"

class FileTree;
class TableModel;
class Serializable;

class DBTextView : public DocView
{
public:
    explicit DBTextView(FileTree* file);
    ~DBTextView() override;

    LPCWSTR getClassName() override { return L"DBTextView"; }
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onNotify(NMHDR* hdr, LRESULT* result) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    void fillScriptMenu(HMENU menu) override {}
    int getMenuId() override { return 0xB1; }
    COMDLG_FILTERSPEC* getFilters(int* count) override;
    HRESULT load() override;
    bool save() override;
    void findNext() override;
    bool detectsCodepage() override { return false; }

    TableModel* table() const { return m_table; }

protected:
    DBTextView(FileTree* file, Serializable* content, TableModel* model);
    virtual std::wstring columnTitle(int field);

private:
    void fillColumns();
    void refresh(int focus);
    int focusedRow() const;
    void selectRow(int row);
    void beginEdit(int row, int field);
    void endEdit(bool commit, int move);
    void insertRow();
    void deleteSelected();
    void eraseSelected();
    void copySelected();
    void showStatus();
    static LRESULT CALLBACK editProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR ref);

    TableModel*  m_table;
    HWND         m_list;
    HWND         m_edit;
    int          m_editRow;
    int          m_editField;
    int          m_curField;
    std::wstring m_dispBuf;
};
