#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <cwchar>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <typeinfo>
#include "ida_compat.h"

#include "String.hpp"
#include "Streamable.h"
#include "Displayable.h"
#include "Node.h"
#include "Serializable.h"
#include "ptrArray.h"
#include "primitiveVector.h"
#include "aStream.h"
#include "iStream.h"
#include "oStream.h"
#include "Exception.h"
#include "fPoint.h"
#include "String_p02.h"

#include "_imports_p02.h"

#pragma warning(disable : 4066 4498)

inline void p02_unindent(String& s)
{
    --s.m_indent;
    if (s.m_buffer && s.m_buffer[s.m_length - 1] == L'\t')
        s.m_buffer[s.m_length] = 0;
    if (s.m_buffer && s.m_buffer[s.m_length - 1] == L'\t')
    {
        --s.m_length;
        s.m_buffer[s.m_length] = 0;
    }
}
inline void p02_chop(String& s, int n)
{
    for (int k = 0; k < n; ++k)
        if (s.m_length)
        {
            --s.m_length;
            s.m_buffer[s.m_length] = 0;
        }
}
