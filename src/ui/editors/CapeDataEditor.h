#pragma once
#include "Modal.h"
#include "EditText.h"
#include "ListCtrlOD.h"
#include "ptrArray.h"
#include "CapeData.h"

class Node;

class CapeDataEditor : public Modal
{
public:
    class BlockList : public ListCtrlOD
    {
    public:
        BlockList(CapeData* cape);
        virtual ~BlockList();
        bool onCreate() override;
        Node* getNode(int index) override;
        int indexOfNode(Node* node) override { return -1; }
        Node* findNode(Node* like) override { return 0; }
        bool copy() override;
        bool paste() override;
        void save();

    public:
        CapeData*                 m_cape;
        CapeData::Block           m_blocks[20];
        ptrArray<CapeData::Block*> m_list;
    };

    CapeDataEditor(CapeData* cape);
    virtual ~CapeDataEditor();
    bool create(HWND parent);
    bool onCreate() override;
    int mapCommandToResult(int id) override;

public:
    CapeData* m_cape;
    EditText  m_edit3DC;
    EditText  m_editDDS;
    EditText  m_edits[9];
    BlockList m_blockList;
};
static_assert(sizeof(CapeDataEditor::BlockList) == 0x480, "CapeDataEditor::BlockList size");
static_assert(sizeof(CapeDataEditor) == 0x5B4, "CapeDataEditor size");
