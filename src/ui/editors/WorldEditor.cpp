#include "WorldEditor.h"
#include "DocView.h"
#include "WorldView.h"
#include "kWorld_CaptionNode.h"
#include "String.hpp"
#include "ida_compat.h"
#include "_imports_p06.h"

WorldEditor::WorldEditor(DocView* view, kWorld::CaptionNode* caption)
    : Modal()
    , m_view(dynamic_cast<WorldView*>(view))
    , m_caption(caption)
{
}

bool WorldEditor::onCreate()
{
    createControls(10);
    bool result = Modal::onCreate();
    if (result)
    {
        load();
        return true;
    }
    return result;
}

void WorldEditor::load()
{
    if (m_caption && m_caption1Edit.m_hWnd)
    {
        m_caption1Edit.setFont(getStdFont(6));
        m_caption1Edit.setText(FIELD(String, m_caption, 0x0C).c_str());
    }
    if (m_caption2Edit.m_hWnd)
    {
        m_caption2Edit.setFont(getStdFont(6));
        m_caption2Edit.setText(FIELD(String, m_caption, 0x2C).c_str());
    }
}

void WorldEditor::save()
{
    if (m_caption)
    {
        if (m_caption1Edit.m_hWnd)
        {
            String text;
            FIELD(String, m_caption, 0x0C) = *m_caption1Edit.getText(&text);
        }
        if (m_caption2Edit.m_hWnd)
        {
            String text;
            FIELD(String, m_caption, 0x2C) = *m_caption2Edit.getText(&text);
        }
    }
}

int WorldEditor::mapCommandToResult(int id)
{
    if (id == 1)
        save();
    return id;
}
