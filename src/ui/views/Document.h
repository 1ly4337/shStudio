#pragma once
#include "Doc.h"

class Document : public Doc
{
public:
    explicit Document(FileTree* data) : Doc(data) {}
    ~Document() override;
};
static_assert(sizeof(Document) == 0x58, "Document size");
