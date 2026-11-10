#include <windows.h>
#include "TableNode.h"
#include "SqlTableView.h"
#include "Table.h"
#include "FileTree.h"
#include "StudioFrame.h"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g4.h"

#pragma warning(disable : 4066 4498)

TableNode::TableNode(Table* table)
{
    m_table = table;
}

TableNode::~TableNode()
{
}

const wchar_t* TableNode::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_table->m_name.m_buffer;
    if (tag != 'stat')
        return 0;
    return L"";
}

int TableNode::getInt(DWORD tag) const
{
    return (tag != 'size') - 1;
}

void TableNode::activate(int arg)
{
    StudioFrame* frame = g_studioFrame;
    String name = m_table->getFullName();
    if (!frame->findDocument(name.m_buffer))
    {
        FileTree* file = new FileTree(0, name.m_buffer, 0);
        SqlTableView* view = new SqlTableView(file, m_table);
        view->setOwnedData(file);
        if (!view->openWindow())
        {
            frame->setStatusText(L"Error, the table %s can not be loaded or processed",
                                        (int)m_table->m_name.m_buffer);
            delete view;
            return;
        }
        const wchar_t* orderBy = L"SkillID, SkillLevel";
        if (!name.equalsNoCase(L"[PS_GameDefs].[dbo].[Skills]"))
            orderBy = 0;
        view->setText(name.m_buffer);
        view->show(true);
        view->vfunc_150((int)(INT_PTR)orderBy);
        frame->addDocument(view);
    }
    SetTimer(frame->m_hWnd, 0x82, 0x32, 0);
}
