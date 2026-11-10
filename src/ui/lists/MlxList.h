#pragma once
#include "ModelList.h"

class String;
class ModelsMLX;

class MlxList : public ModelList {
public:
    MlxList(ModelView* view, ModelsMLX* models, const String* directory)
        : ModelList(view), m_node((Node*)models), m_directory(directory) {}

    DWORD getStyle() override;
    bool  onCreate() override;
    bool  onCommand(int code, LPARAM ctrl, int id) override;
    void  onChanged() override;
    int   insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int   getItemMenu() override;
    DWORD getExStyle() override;
    bool  onActivateItem(int index, int keys) override;
    void  onItemChanged(int index, UINT newState) override;
    bool  removeItem(int index) override;
    Node* createNode() override;
    bool  clearItem(int index) override;
    bool  moveItemUp(int index) override;
    bool  moveItemDown(int index) override;
    int   indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT  getClipboardFormat() override;
    bool  onCopied(Node* node) override;
    void  onPasted(Node* node) override;

    ModelsMLX* models() const { return (ModelsMLX*)m_node; }

    Node*         m_node;
    const String* m_directory;
};
static_assert(sizeof(MlxList) == 0x100, "MlxList size");
