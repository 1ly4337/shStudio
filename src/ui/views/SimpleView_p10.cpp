#include "SimpleView.h"
#include "FileTree.h"
#include "p10_vcall.h"
#include "_imports_p10.h"

bool SimpleView::onCreate()
{
    if (!Popup::onCreate())
        return false;
    FileTree* data = m_data;
    if (data)
    {
        if ((P10_VCALL(data, 0x08, int)(data) & 2) != 0)
            setModified(true);
    }
    m_modified = true;
    setModified(false);
    return true;
}
