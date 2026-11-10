#pragma once
#include "Modal.h"
#include "Container.h"
#include "ListCtrlOD.h"
#include "IconPicker.h"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"
#include "BagsList.h"
#include "BmpCtrl.h"
#include "ItemsSet.h"

class Monster;

class MonsterEditor : public Modal {
public:
    class ItemsList : public ListCtrlOD {
    public:
        ItemsList() : ListCtrlOD(), m_items() {}
        ~ItemsList() override;
        DWORD getStyle() override;
        bool  onCreate() override;
        Node* getNode(int index) override;
        int   indexOfNode(Node* node) override;
        Node* findNode(Node* like) override { return 0; }
        void  showGrade(int grade);

        Data::ItemsSet m_items;
    };

    MonsterEditor(Monster* monster, void* data);
    ~MonsterEditor() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void reserved73() override;
    int  mapCommandToResult(int code) override;
    void loadMonster();
    bool updatePreview();
    void fillNpcSkillCombo(ComboBox* combo);
    bool open(HWND parent);

    Monster*   m_monster;
    void*      m_data;
    IconPicker m_icon;
    EditText   m_edits[16];
    ComboBox   m_combos[3];
    Button     m_debuffs[15];
    EditText   m_stats[6];
    EditText   m_edits2[5];
    ComboBox   m_attackTypes[3];
    EditText   m_attackA[3];
    EditText   m_attackB[3];
    EditText   m_attackC[3];
    EditText   m_attackD[3];
    ComboBox   m_attackSkills[3];
    EditText   m_attackE[3];
    ComboBox   m_attackF[3];
    Container  m_dropPanel;
    Button     m_btn6F0;
    bool       m_6FC;
    BagsList   m_bags;
    EditText   m_edit810;
    EditText   m_edit81C;
    Button     m_btn828;
    Button     m_btn834;
    ItemsList  m_items;
    Button     m_btn95C;
    BmpCtrl    m_preview;
    bool       m_9E0;
};
static_assert(sizeof(MonsterEditor::ItemsList) == 0x11C, "MonsterEditor::ItemsList size");
static_assert(sizeof(MonsterEditor) == 0x9E4, "MonsterEditor size");
