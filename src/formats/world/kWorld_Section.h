#pragma once
#include "p04_common.h"
#include "ToggleNode.h"
#include "Name.h"

namespace kWorld
{
    extern const wchar_t* g_sectionNames[12];
    extern const wchar_t* g_sectionCaptions[12];
    extern wchar_t g_textBuffer[256];

    class Section : public ToggleNode
    {
    public:
        explicit Section(int index)
            : ToggleNode(g_sectionNames[index])
            , m_index(index)
        {
        }
        virtual ~Section() {}

        virtual bool hasItem(const wchar_t* name) = 0;
        virtual Node* addItem(const wchar_t* name) = 0;
        virtual bool removeItem(Node* item) = 0;

        class wItem;

    public:
        int m_index;
    };

    class Section::wItem : public Name
    {
    public:
        wItem(int index, void* owner)
            : Name(index, (int)owner)
        {
        }
        virtual ~wItem() {}
    };
}
