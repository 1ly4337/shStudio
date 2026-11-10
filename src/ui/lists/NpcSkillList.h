#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "MobsSkillSet.h"

class Node;
class NpcSkillView;
class MobsSkills;

class NpcSkillList : public ListCtrlOD
{
public:
    NpcSkillList(NpcSkillView* owner, MobsSkills* skills) : m_owner(owner), m_skills(skills) {}
    virtual ~NpcSkillList();

    DWORD getStyle() override;
    void onChanged() override;
    int insertItem(Node* node, int index) override;
    Node* getNode(int index) override;
    int listV69() override;
    int getHeaderMenu() override;
    bool sort() override;
    bool onActivateItem(int index, int keys) override;
    bool removeItem(int index) override;
    Node* createNode() override;
    bool clearItem(int index) override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override;
    UINT getClipboardFormat() override;

    virtual LRESULT clearRows();

public:
    NpcSkillView* m_owner;
    MobsSkills*   m_skills;
    MobsSkillSet  m_set;
};
static_assert(sizeof(NpcSkillList) == 0x114, "NpcSkillList size");
