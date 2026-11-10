#include "GradeSets.h"
#include "GradeSet.h"
#include "_imports_p07.h"

GradeSets::~GradeSets()
{
    deleteAll();
}

void GradeSets::detachAll()
{
    for (int i = 0; i < m_count; ++i)
    {
        GradeSet* set = m_items[i];
        for (int j = 0; j < set->m_count; ++j)
            set->m_items[j] = 0;
        set->m_count = 0;
    }
}

GradeSet* GradeSets::find(short grade) const
{
    for (int i = 0; i < m_count; ++i)
    {
        if ((int)m_items[i]->m_grade == (int)grade)
            return m_items[i];
    }
    return 0;
}

void GradeSets::sort(int first, int last, DWORD tag1, DWORD tag2, bool ascending)
{
    if (last <= first)
        return;
    for (;;)
    {
        int j = last;
        int i = first;
        GradeSet* pivot = m_items[(first + last) / 2];
        do
        {
            if (i < last)
            {
                do
                {
                    GradeSet* item = m_items[i];
                    int cmp;
                    if (item)
                    {
                        if (pivot)
                        {
                            cmp = item->compare(pivot, tag1);
                            if (!cmp && tag2)
                                cmp = item->compare(pivot, tag2);
                            if (!ascending)
                                cmp = -cmp;
                        }
                        else
                        {
                            cmp = 2 * (ascending == false) - 1;
                        }
                    }
                    else
                    {
                        if (!pivot)
                            break;
                        cmp = 2 * (ascending != false) - 1;
                    }
                    if (cmp >= 0)
                        break;
                    ++i;
                } while (i < last);
            }
            if (j > 0)
            {
                do
                {
                    GradeSet* item = m_items[j];
                    int cmp;
                    if (item)
                    {
                        if (pivot)
                        {
                            cmp = item->compare(pivot, tag1);
                            if (!cmp && tag2)
                                cmp = item->compare(pivot, tag2);
                            if (!ascending)
                                cmp = -cmp;
                        }
                        else
                        {
                            cmp = 2 * (ascending == false) - 1;
                        }
                    }
                    else
                    {
                        if (!pivot)
                            break;
                        cmp = 2 * (ascending != false) - 1;
                    }
                    if (cmp <= 0)
                        break;
                    --j;
                } while (j > 0);
            }
            if (j < i)
                break;
            if (j != i)
            {
                GradeSet* tmp = m_items[j];
                m_items[j] = m_items[i];
                m_items[i] = tmp;
                ++m_changes;
            }
            --j;
            ++i;
        } while (i <= j);

        if (first < j)
            sort(first, j, tag1, tag2, ascending);
        if (i >= last)
            break;
        first = i;
    }
}
