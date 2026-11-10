#pragma once
#include "Window.h"
#include "String.hpp"

class SearchPanel : public Window {
public:
    SearchPanel();

    BOOL show(bool visible) override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onMove(int x, int y) override;
    LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

    LRESULT setStatus(const wchar_t* text);
    String  getText();
    static LRESULT CALLBACK editSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                             UINT_PTR idSubclass, DWORD_PTR refData);

    HWND m_button;
    HWND m_edit;
    HWND m_status;
};
static_assert(sizeof(SearchPanel) == 0x40, "SearchPanel size");
