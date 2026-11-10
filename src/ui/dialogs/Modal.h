#pragma once
#include <windows.h>
#include "Container.h"

class Modal : public Container
{
public:
    Modal();
    virtual ~Modal();

    virtual BOOL resize(int cx, int cy);
    virtual bool onCreate();
    virtual void onNcDestroy();
    virtual bool onClose();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual bool onKey(WPARAM vk, LPARAM flags, bool down);

    virtual bool createDialog(HWND parent, int cx, int cy, int type, LPCWSTR title);
    virtual int doModal();
    virtual void reserved73() {}
    virtual HWND showModeless(Window* owner);
    virtual void onShowDialog() {}
    virtual int mapCommandToResult(int id);
    virtual LPCWSTR getButtonText(int id);

public:
    HWND    m_owner;
    int     m_result;
    bool    m_running;
    int     m_buttons;
    HWND    m_btnOk;
    HWND    m_btnCancel;
    HWND    m_btnYes;
    HWND    m_btnNo;
    HWND    m_btnAbort;
    HWND    m_btnRetry;
    HWND    m_btnIgnore;
    int     m_btnWidth;
    int     m_btnMarginRight;
    int     m_btnMarginBottom;
    bool    m_rightAlignButtons;
    int     m_btnX;
    Window* m_modelessOwner;
};
static_assert(sizeof(Modal) == 0xAC, "Modal size");

extern Modal* g_currentModal;

HWND Modal_getTopLevelWindow(HWND hWnd);
