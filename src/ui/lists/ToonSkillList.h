#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "ToonSkillSet.h"

class Node;
class ToonSkill;
class ToonSkills;
class ToonSkillView;

class ToonSkillList : public ListCtrlOD
{
public:
    ToonSkillList(ToonSkillView* owner, ToonSkills* skills);
    virtual ~ToonSkillList();

    DWORD getStyle() override;
    bool onCreate() override;
    void onChanged() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int listV69() override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;

    virtual LRESULT clearRows();
    virtual void initNewSkill(ToonSkill* skill) = 0;

public:
    ToonSkillView* m_owner;
    ToonSkills*    m_doc;
    ToonSkillSet   m_skills;
    ToonSkillList* m_rawList;
    int            m_classFilter;
    int            m_listKind;
    bool           m_useRawEditor;
};
static_assert(sizeof(ToonSkillList) == 0x124, "ToonSkillList size");
