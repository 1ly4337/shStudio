#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class DocView;
class WorldView;
namespace kWorld { class CaptionNode; }

class WorldEditor : public Modal
{
public:
    WorldEditor(DocView* view, kWorld::CaptionNode* caption);
    virtual ~WorldEditor() {}

    virtual bool onCreate() override;
    virtual int mapCommandToResult(int id) override;

    virtual bool create(HWND parent) = 0;
    virtual int createControls(int y) = 0;
    virtual void load();
    virtual void save();

public:
    WorldView*           m_view;
    kWorld::CaptionNode* m_caption;
    EditText             m_caption1Edit;
    EditText             m_caption2Edit;
};
static_assert(sizeof(WorldEditor) == 0xCC, "WorldEditor size");
