#pragma once
#include <windows.h>
#include "Modal.h"
#include "StreamNotifier.h"

class FolderRoot;

class BrowseFoldersDialog : public Modal, public StreamNotifier
{
public:
    BrowseFoldersDialog(FolderRoot* root)
        : m_root(root), m_animate(0), m_folderText(0), m_fileText(0), m_continue(true) {}
    virtual ~BrowseFoldersDialog();

    virtual bool onCreate() override;
    virtual int mapCommandToResult(int id) override;

    virtual bool setStatus(LPCWSTR text) override;
    virtual bool progress(LPCWSTR text, int stepLow, int stepHigh) override;
    virtual LRESULT setRange(LPARAM max) override { return 0; }

    static DWORD WINAPI threadProc(LPVOID param);

    FolderRoot* m_root;
    HWND        m_animate;
    HWND        m_folderText;
    HWND        m_fileText;
    int         m_unusedC0;
    HANDLE      m_thread;
    int         m_threadResult;
    bool        m_continue;
};
static_assert(sizeof(BrowseFoldersDialog) == 0xD0, "BrowseFoldersDialog size");
