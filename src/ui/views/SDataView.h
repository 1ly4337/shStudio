#pragma once
#include <windows.h>
#include "DocView.h"
#include "String.hpp"

class FileTree;
class Serializable;
class RecordSet;

class SDataView : public DocView
{
public:
    SDataView(void* file, Serializable* data);
    virtual ~SDataView() {}

    LPCWSTR getClassName() override { return L"SDataView"; }
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onInitMenu(HMENU menu) override;
    void fillScriptMenu(HMENU menu) override;
    int getMenuId() override { return 166; }
    COMDLG_FILTERSPEC* getFilters(int* count) override;
    bool hasTranslation() override { return true; }

    virtual bool getScriptPath(String* path, const wchar_t* name);
    virtual void exportFile();
    virtual String getMergeFilter() { return String(); }
    virtual void mergeLanguageFile();
    virtual void importSQLDefinition();
    virtual void onDataUpdated();
    virtual bool exportData(String* out, DWORD format) = 0;
    virtual const wchar_t* getSQLTableName() { return 0; }
    virtual void importSQLRow(RecordSet* rs, bool captions) = 0;
    virtual void setViewMode(int mode) {}
    virtual int getViewMode() { return 0; }

};
