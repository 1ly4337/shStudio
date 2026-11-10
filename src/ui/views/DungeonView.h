#pragma once
#include "ModuleView.h"
#include "SModCtrlsPanel.h"
#include "DungeonList.h"

class DungeonView : public ModuleView {
public:
    class DungeonPanel : public SModCtrlsPanel {
    public:
        DungeonPanel(Popup* doc, glViewer* viewer) : SModCtrlsPanel(doc, viewer), m_list() {}
        ~DungeonPanel() override;
        bool onCreate() override;
        void onSize(WPARAM type, int cx, int cy) override;

        DungeonList m_list;
    };

    explicit DungeonView(FileTree* node) : ModuleView(node) {}
    ~DungeonView() override {}

    SIZE* getDefaultSize(SIZE* out, SIZE def) override;
    CtrlsPanel* createPanel() override;
    int  getPanelWidth() override;
    Drawable* createModel() override;
    bool loadModel() override;
};
static_assert(sizeof(DungeonView::DungeonPanel) == 0x324, "DungeonPanel size");
