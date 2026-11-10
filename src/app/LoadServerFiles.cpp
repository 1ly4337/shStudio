#include "p05_common.h"
#include "LoadServerFiles.h"
#include "StudioFrame.h"
#include "ServerNode.h"

void LoadServerFiles::run()
{
    String location = g_preferences.getString(L"Server Data", L"location", 0);
    if (g_preferences.getInt(L"Server Data", L"Connect", 0) == 1) {
        if (location.m_length) {
            StudioList* list = &m_frame->m_list;
            Node* node = list->getItemNode(list->m_serverItem);
            ServerNode* server = dynamic_cast<ServerNode*>(node);
            if (server)
                server->connect(list, &location);
        }
    }
}
