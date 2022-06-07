#pragma once

#include <string_view>
#include <string>


class PathBuffer
{
public:
    static constexpr wchar_t TERMINATOR       = L'\0';
    static constexpr wchar_t FOLDER_DELIMITER = L'\\';

public:
    PathBuffer(size_t count, wchar_t filler);
    size_t size() const;

    void reset();
    void updateLoadedSize();

    std::wstring_view getProcessName() const;
    wchar_t* getWritableBufferAdress();

private:
    std::wstring m_processPath;
    size_t       m_processPathLoadedSize = 0;
};
