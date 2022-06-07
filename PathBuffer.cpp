#include "PathBuffer.h"


PathBuffer::PathBuffer(size_t count, wchar_t filler) : m_processPath(count, filler) {}

void PathBuffer::reset()
{
    std::fill(m_processPath.begin(), m_processPath.begin() + m_processPathLoadedSize, TERMINATOR);
}

size_t PathBuffer::size() const
{
    return m_processPath.size();
}

void PathBuffer::updateLoadedSize()
{
    m_processPathLoadedSize = m_processPath.find_first_of(TERMINATOR);
}

std::wstring_view PathBuffer::getProcessName() const
{
    const size_t lastSlashPosition = m_processPath.find_last_of(FOLDER_DELIMITER);

    return {
        &m_processPath[0] + lastSlashPosition + 1,
         m_processPath.size() - lastSlashPosition
    };
}

wchar_t* PathBuffer::getWritableBufferAdress()
{
    return &m_processPath[0];
}
