#pragma once
#include "Modal.h"
#include "StreamNotifier.h"

class FolderRoot;

class ArchiveDialog : public Modal, public StreamNotifier {
public:
    ArchiveDialog(FolderRoot* root, bool packing);
    ~ArchiveDialog() override {}
    bool create(HWND parent);
    bool onCreate() override;
    int mapCommandToResult(int id) override;

    virtual LPCWSTR getTitle() = 0;
    virtual void* getWorkerProc() = 0;
    virtual __int64 getWorkSize() = 0;

    bool setStatus(LPCWSTR text) override;
    bool progress(LPCWSTR text, int stepLow, int stepHigh) override;
    LRESULT setRange(LPARAM max) override;

    FolderRoot* m_root;
    HWND        m_animation;
    HWND        m_progress;
    HWND        m_status;
    HWND        m_file;
    HANDLE      m_worker;
    bool        m_packing;
    int         m_pos;
    int         m_result;
    bool        m_continue;
};
static_assert(sizeof(ArchiveDialog) == 0xD8, "ArchiveDialog size");
