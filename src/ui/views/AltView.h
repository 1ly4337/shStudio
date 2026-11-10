#pragma once
#include <windows.h>
#include "DocView.h"
#include "AltList.h"

class CharActions;
class FileTree;

class AltView : public DocView
{
public:
    AltView(FileTree* file, CharActions* actions);
    ~AltView() override;

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    bool onCreate() override;
    void onActivate(bool active) override;
    int getMenuId() override;

public:
    AltList      m_list;
    CharActions* m_actions;
};
static_assert(sizeof(AltView) == 0x1A8, "AltView size");
