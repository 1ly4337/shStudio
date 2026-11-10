#include "ToggleNode.h"

ToggleNode::ToggleNode(const wchar_t* caption)
    : Node()
    , m_caption()
{
    m_caption = caption;
}

const wchar_t* ToggleNode::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_caption.m_buffer;
    return 0;
}
