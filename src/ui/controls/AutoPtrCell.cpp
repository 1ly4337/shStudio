#include "p05_common.h"
#include "AutoPtrCell.h"
#include "Displayable.h"
#include "Node.h"

AutoPtrCell::~AutoPtrCell()
{
    if (m_node)
        delete m_node;
}
