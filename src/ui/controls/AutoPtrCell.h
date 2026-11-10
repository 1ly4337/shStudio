#pragma once
#include "Cell.h"

class Node;

class AutoPtrCell : public Cell {
public:
    explicit AutoPtrCell(Node* node) : Cell(node) {}
    ~AutoPtrCell() override;
};
static_assert(sizeof(AutoPtrCell) == 8, "AutoPtrCell size");
