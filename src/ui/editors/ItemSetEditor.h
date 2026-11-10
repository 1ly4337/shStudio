#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

namespace Data { class ItemSet; }

class ItemSetEditor : public Modal
{
public:
    ItemSetEditor(Data::ItemSet* set, int index);
    virtual ~ItemSetEditor() {}

    bool onCreate() override;
    int mapCommandToResult(int id) override;

public:
    Data::ItemSet* m_set;
    int            m_index;
    EditText       m_type;
    EditText       m_typeId;
    EditText       m_bonus;
};
static_assert(sizeof(ItemSetEditor) == 0xD8, "ItemSetEditor size");
