#pragma once
#include <windows.h>
#include "CtrlsPanel.h"
#include "ListCtrl.h"
#include "Button.h"
#include "EditText.h"

class Faces;

class ThreeDxCtrlsPanel : public CtrlsPanel
{
public:
    class ListFacet : public ListCtrl
    {
    public:
        ListFacet(Popup* doc, glViewer* viewer) : m_doc(doc), m_viewer(viewer), m_faces(0) {}

        bool onCreate() override;
        bool onKey(short vk, int flags, bool down) override;
        LRESULT setItemCount(int count) override;
        void onItemChanged(int index, UINT newState) override;
        HRESULT onGetDispInfo(NMLVDISPINFOW* di) override;

    public:
        Popup*    m_doc;
        glViewer* m_viewer;
        Faces*    m_faces;
    };
    static_assert(sizeof(ListFacet) == 0x100, "ListFacet size");

    ThreeDxCtrlsPanel(Popup* doc, glViewer* viewer);
    virtual ~ThreeDxCtrlsPanel();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void onSize(WPARAM type, int cx, int cy) override;

public:
    Button    m_alpha;
    Button    m_texCheck;
    Button    m_browse;
    EditText  m_texture;
    ListFacet m_list;
    Button    m_show;
    Button    m_hide;
};
static_assert(sizeof(ThreeDxCtrlsPanel) == 0x32C, "ThreeDxCtrlsPanel size");
