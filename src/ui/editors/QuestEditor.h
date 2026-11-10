#pragma once
#include <windows.h>
#include "Modal.h"
#include "ComboBox.h"
#include "EditText.h"
#include "Button.h"

class QuestDetails;

class QuestEditor : public Modal
{
public:
    explicit QuestEditor(QuestDetails* quest);
    virtual ~QuestEditor();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;
    virtual LRESULT loadTexts();

    bool open(HWND parent);
    BOOL enableObjectives(int questType);

public:
    QuestDetails* m_quest;
    bool     m_compact;
    int      m_textCount;
    ComboBox m_faction;
    ComboBox m_mode;
    ComboBox m_questType;
    ComboBox m_obtained;
    EditText m_name;
    EditText m_welcomeMsg;
    EditText m_initialDesc;
    EditText m_summary;
    EditText m_reminder;
    EditText m_alternateResp;
    EditText m_completionMsg[6];
    Button   m_classes[6];
    Button   m_repeatable;
    EditText m_minLevel;
    EditText m_maxLevel;
    EditText m_givenByType;
    EditText m_givenById;
    EditText m_receivedByType;
    EditText m_receivedById;
    EditText m_requires1;
    EditText m_requires2;
    EditText m_requires3;
    EditText m_delay;
    EditText m_recvItemType[3];
    EditText m_recvItemTypeId[3];
    EditText m_recvItemCount[3];
    EditText m_farmItemType[3];
    EditText m_farmItemTypeId[3];
    EditText m_farmItemCount[3];
    EditText m_mobType[2];
    EditText m_mobCount[2];
    EditText m_pvpKills;
    Button   m_defaultReward;
    ComboBox m_rewardMode;
    Button   m_rewMobCounter[6];
    Button   m_rewItemCounter[6];
    EditText m_rewMobCount[6];
    EditText m_rewItemCount[6];
    EditText m_rewGold[6];
    EditText m_rewXp[6];
    EditText m_rewUnlock[6];
    EditText m_rewItemType[18];
    EditText m_rewItemTypeId[18];
    EditText m_rewItemNum[18];
};
static_assert(sizeof(QuestEditor) == 0x87C, "QuestEditor size");
