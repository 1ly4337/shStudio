#pragma once
#include "WorkspaceNode.h"
#include "FolderRoot.h"

class TreeList;
class DataProvider;
class String;

#pragma vtordisp(push, 2)
class WorkspaceDataSet : public WorkspaceNode {
public:
    explicit WorkspaceDataSet(TreeList* tree);
    ~WorkspaceDataSet() override;

    const wchar_t* getString(DWORD tag) const override;
    void onCommand(int cmd, int arg) override;
    virtual bool isClosed() { return m_source == 0; }
    virtual void unload();
    virtual bool open(const String& path, int format);
    virtual void refresh(TreeList* tree);

    DataProvider* m_source;
    TreeList*     m_tree;
    FolderRoot    m_root;
};
#pragma vtordisp(pop)
static_assert(sizeof(WorkspaceDataSet) == 0xC0, "WorkspaceDataSet size");
