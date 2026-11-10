#include <windows.h>
#include <shlwapi.h>
#include <cstring>
#include <typeinfo>
#include "SqlNode.h"
#include "SQLSchema.h"
#include "RecordSet.h"
#include "Time.hpp"
#include "_imports_p09.h"
#include "_imports_g4.h"

#pragma comment(lib, "shlwapi.lib")

SqlNode::SqlNode(SQLSchema* schema)
{
    m_schema = schema;
    int textCount = schema->m_textCount;
    m_strings = textCount ? new String[textCount] : 0;
    int numberCount = m_schema->m_numberCount;
    m_values = numberCount ? new int[numberCount] : 0;
    if (numberCount > 0)
        memset(m_values, 0, 4 * numberCount);
}

SqlNode::~SqlNode()
{
    if (m_values)
        delete[] m_values;
    delete[] m_strings;
}

void SqlNode::load(RecordSet& rs)
{
    for (int column = 0; column < m_schema->m_columnCount; ++column)
    {
        int index = *m_schema->m_types.at(column) & SQLCOL_INDEX;
        if (m_schema->hasFlag(column, SQLCOL_TEXT))
        {
            m_strings[index] = rs.getString(column);
        }
        else if (m_schema->hasFlag(column, SQLCOL_TIME))
        {
            Time time(rs.getTime(column));
            m_strings[index] = time.toString();
        }
        else if (m_schema->hasFlag(column, SQLCOL_INT))
        {
            m_values[index] = rs.getInt(column);
        }
        else
        {
            float value = (float)rs.getDouble(column);
            ((float*)m_values)[index] = value;
        }
    }
}

const wchar_t* SqlNode::getString(DWORD tag) const
{
    return m_strings[tag & SQLCOL_INDEX].m_buffer;
}

int SqlNode::getInt(DWORD tag) const
{
    return m_values[tag & SQLCOL_INDEX];
}

void SqlNode::reset()
{
    int textCount = m_schema->m_textCount;
    if (m_values)
        memset(m_values, 0, 4 * m_schema->m_numberCount);
    if (m_strings && textCount > 0)
    {
        for (int i = 0; i < textCount; ++i)
            m_strings[i].clear();
    }
}

int SqlNode::compare(const Node* other, DWORD tag) const
{
    const SqlNode& node = dynamic_cast<const SqlNode&>(*other);
    int index = tag & SQLCOL_INDEX;
    if ((tag & 0x7F000000) == SQLCOL_TEXT || (tag & 0x7F000000) == SQLCOL_TIME)
    {
        const wchar_t* mine = m_strings[index].m_buffer;
        const wchar_t* theirs = node.m_strings[index].m_buffer;
        if (mine)
        {
            if (theirs)
                return StrCmpLogicalW(mine, theirs);
            return 1;
        }
        return -(theirs != 0);
    }
    int theirs = node.m_values[index];
    int mine = m_values[index];
    if (mine == theirs)
        return 0;
    return 2 * (mine >= theirs) - 1;
}
