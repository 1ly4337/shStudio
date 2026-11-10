#pragma once
#include "TextView.h"
#include "FileTree.h"

class StdTextView : public TextView {
public:
    StdTextView();
    ~StdTextView() override;
    void setContent(const String& text);
    bool loadText() override;

    FileTree m_file;
    String   m_content;
};
static_assert(sizeof(StdTextView) == 0x200, "StdTextView size");
