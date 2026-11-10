#include "GradeSet.h"
#include <typeinfo>
#include "_imports_p07.h"

GradeSet::GradeSet(unsigned short grade)
    : Data::ItemsSet()
    , m_grade(grade)
    , m_hItem(0)
{
}

GradeSet::~GradeSet()
{
}

int GradeSet::compare(const Node* other, DWORD tag) const
{
    const GradeSet* set = dynamic_cast<const GradeSet*>(other);
    if (!set)
        return 1;
    if (tag != 'grde')
        return 0;
    if (m_grade == set->m_grade)
        return 0;
    return m_grade < set->m_grade ? -1 : 1;
}
