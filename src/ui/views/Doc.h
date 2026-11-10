#pragma once
#include "Popup.h"
#include "String.hpp"
#include <shobjidl.h>

class FileTree;

class Doc : public Popup
{
public:
    explicit Doc(FileTree* data)
    {
        m_modified = false;
        m_data = data;
        m_object = 0;
        m_ownedData = 0;
    }
    ~Doc() override;

    bool onCreate() override;
    void onDestroy() override;
    void onNcDestroy() override;
    bool onClose() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    bool onTimer(UINT_PTR id) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    void updateMenu(HMENU menu) override;
    LPCWSTR getCommandHelp(UINT id) override;

    virtual LPCWSTR getName();
    virtual bool openWindow();
    virtual bool close();
    virtual bool isPath(const wchar_t* path);
    virtual void setModified(bool modified);
    virtual void setOwnedData(FileTree* data);
    virtual int getType();
    virtual LPCWSTR getPath();
    virtual int notifyFrame(int value);
    virtual String getDirectory();
    virtual bool enableMenuItem(UINT id, bool enable);
    virtual bool isModified();
    virtual bool vfunc_150(int arg) { return false; }
    virtual SIZE* getDefaultSize(SIZE* out, SIZE def);
    virtual int getMenuId() = 0;
    virtual bool save();
    virtual bool saveAs();
    virtual void onActivated() {}
    virtual LRESULT setState(int state);
    virtual HRESULT load();
    virtual HRESULT store();
    virtual void setFilterIndex(int index) {}
    virtual int getFilterIndex() { return -1; }
    virtual COMDLG_FILTERSPEC* getFilters(int* count) { return 0; }

    bool loadData();

public:
    bool      m_modified;
    FileTree* m_data;
    void*     m_object;
    FileTree* m_ownedData;
};
static_assert(sizeof(Doc) == 0x58, "Doc size");
