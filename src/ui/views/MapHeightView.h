#pragma once
#include <windows.h>
#include "PopupView3D.h"
#include "EditText.h"

class MapHeight;
class fPoint;

class MapHeightView : public PopupView3D
{
public:
    explicit MapHeightView(MapHeight* doc);
    virtual ~MapHeightView();

    virtual DWORD getStyle() override;
    virtual bool onCreate() override;
    virtual void onDestroy() override;
    virtual bool onClose() override { return true; }
    virtual bool onCommand(UINT code, HWND ctrl, UINT id) override;
    virtual bool onInitMenu(HMENU menu) override;
    virtual bool onTimer(UINT_PTR id) override;
    virtual int vslot78() override;
    virtual bool vslot80(int x) override;

    virtual void vslot3(const fPoint* pos) override;

    bool open();

public:
    MapHeight* m_doc;
    EditText   m_posEdits[3];
    HWND       m_fpsLabel;
    bool       m_editMode;
};
static_assert(sizeof(MapHeightView) == 0x3B8, "MapHeightView size");
