#include "StdTextView.h"
#include "_imports_p10.h"

void StdTextView::setContent(const String& text)
{
    if (m_edit.m_hWnd)
    {
        m_edit.setText(text.m_buffer);
        return;
    }
    if (&text != &m_content)
    {
        const wchar_t* buffer = text.m_buffer;
        unsigned length = text.m_length;
        m_content.m_length = 0;
        m_content.m_capacity = 0;
        if (m_content.m_buffer)
        {
            operator delete[](m_content.m_buffer);
            m_content.m_buffer = 0;
        }
        m_content.append(buffer, length);
    }
}
