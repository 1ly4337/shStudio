#pragma once
#include "Document.h"
#include "StreamCallback.h"
#include "SearchPanel.h"

class Serializable;

class DocView : public Document, public StreamCallback
{
public:
    DocView(FileTree* data, Serializable* content);
    ~DocView() override;

    RECT* getClientRect(RECT* out) override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    void onSize(WPARAM type, int cx, int cy) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;
    void fillScriptMenu(HMENU menu) override;
    HRESULT load() override;
    HRESULT store() override;
    LRESULT onStreamProgress(WPARAM pos, LPARAM range) override;

    virtual RECT* getContentRect(RECT* out);
    virtual void findNext();
    virtual bool hasTranslation() { return false; }
    virtual bool detectsCodepage() { return true; }

public:
    SearchPanel   m_search;
    Serializable* m_content;
    int           m_margin;
    bool          m_searchEnabled;
    bool          m_decrypt;
};
static_assert(sizeof(DocView) == 0xA8, "DocView size");
