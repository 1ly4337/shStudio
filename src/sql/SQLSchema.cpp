#include <windows.h>
#include "SQLSchema.h"
#include "ListCtrl.h"
#include "Table.h"
#include "_imports_p09.h"
#include "_imports_g4.h"

SQLSchema::SQLSchema()
{
    m_textCount = 0;
    m_numberCount = 0;
    m_columnCount = 0;
}

SQLSchema::~SQLSchema()
{
}

int SQLSchema::build(Table* table, ListCtrl* list)
{
    for (int index = 0;; ++index)
    {
        Column* column;
        if (index < 0 || index >= table->m_columns.size())
            column = 0;
        else
            column = table->m_columns.at(index);
        if (!column)
            return 0;

        const wchar_t* name = column->m_name.c_str();
        m_names.add(name);

        long columnId;
        int width;
        int format;
        switch (column->m_type)
        {
        case 2:
            columnId = m_numberCount++ | 0x4000000;
            format = 18;
            width = 60;
            break;
        case 3:
            columnId = m_numberCount++ | 0x5000000;
            format = 130;
            width = 75;
            break;
        case 4:
            columnId = m_textCount++ | 0x2000000;
            format = 512;
            width = 100;
            break;
        case 5:
            columnId = m_textCount++ | 0x1000000;
            format = 0;
            width = 200;
            break;
        default:
            return 13;
        }
        m_types.add(columnId);
        list->addColumn(name, width, format, columnId);
        ++m_columnCount;
    }
}
