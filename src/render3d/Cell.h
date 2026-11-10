#pragma once

class Node;

class Cell
{
public:
    Cell(Node* node) : m_node(node) {}
    virtual ~Cell();

    Node* m_node;
};
static_assert(sizeof(Cell) == 0x8, "Cell size");
