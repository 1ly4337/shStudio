#pragma once
#include <shobjidl.h>
#include "SimpleView.h"
#include "Bytes.h"
#include "BmpCtrl.h"

class FileTree;

class ImageView : public SimpleView
{
public:
    explicit ImageView(FileTree* node);
    ~ImageView() override;

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    int getMenuId() override;

    bool onCreate() override;
    void onResized() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    bool onKey(WPARAM vk, LPARAM flags, bool down) override;
    COMDLG_FILTERSPEC* getFilters(int* count) override;

public:
    int     m_field58;
    Bytes   m_bytes;
    BmpCtrl m_bitmap;
};
static_assert(sizeof(ImageView) == 0x104, "ImageView size");
