#include "p05_common.h"
#include "ClientNode.h"
#include "TreeList.h"

ClientNode::~ClientNode()
{
    m_name = L"Client";
    m_clientData.clear();
    m_tree->deleteChildren(FIELD(HTREEITEM, this, 0x08));
    m_root.reset();
    DataProvider* src = m_source;
    g_contentProvider = 0;
    if (src) {
        VCALL(src, 0, void, int)(src, 1);
        m_source = 0;
    }
}

const wchar_t* ClientNode::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_name.m_buffer;
    return 0;
}
