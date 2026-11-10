#include "p05_common.h"
#include "LoadSQLDefs.h"
#include "StudioFrame.h"
#include "SqlSvrNode.h"

void LoadSQLDefs::run()
{
    if (g_preferences.getInt(L"SQL Server", L"save", 0) == 1) {
        StudioList* list = &m_frame->m_list;
        Node* node = list->getItemNode(list->m_sqlItem);
        SqlSvrNode* sql = dynamic_cast<SqlSvrNode*>(node);
        if (sql)
            sql->connect(list, false);
    }
}
