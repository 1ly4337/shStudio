#pragma once
#include <windows.h>
#include "ListCtrlOD.h"

class ModelView;
class Node;

class ModelList : public ListCtrlOD
{
public:
    explicit ModelList(ModelView* view) : m_view(view) {}

    DWORD getStyle() override { return 0xD; }
    void onChanged() override;
    DWORD getExStyle() override { return 0x21; }
    bool onActivateItem(int index, int keys) override;
    void onItemChanged(int index, UINT newState) override;

public:
    ModelView* m_view;
};
static_assert(sizeof(ModelList) == 0xF8, "ModelList size");
