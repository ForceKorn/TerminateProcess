#include "utils.h"
#include "PathBuffer.h"

#include <iostream>
#include <memory>

#include <psapi.h>

using HandleWrapper = std::unique_ptr<void, decltype (&CloseHandle)>;

static constexpr size_t MAX_PROCESS_COUNT = 1024;

std::wstring GetProcessNameFromUser()
{
    std::wstring input;

    std::wcout << L"Please enter process name: ";
    getline(std::wcin, input);

    return input;
}

void KillProcessByName(const std::wstring& processName)
{
    const size_t termintedCount = TerminateAllProcessesWithName(processName);
    PrintTerminationResult(processName, termintedCount);
}

size_t TerminateAllProcessesWithName(const std::wstring& processName)
{
    size_t termintedCount = 0;

    std::vector<DWORD> processesID = GetProcessesID();
    if (processesID.empty())
    {
        std::wcout << L"Error. Can`t enumerate processes!\n";
        return 0;
    }


    PathBuffer processPath(MAX_PATH, PathBuffer::TERMINATOR);

    for (const DWORD& processID : processesID)
    {
        HandleWrapper pProcessWrapper(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, processID), CloseHandle);
        if (!pProcessWrapper.get()) { continue; }

        processPath.reset();

        DWORD hResultGetFilePath = GetModuleFileNameExW(pProcessWrapper.get(), NULL, processPath.getWritableBufferAdress(), processPath.size());
        if (!hResultGetFilePath) { continue; }

        processPath.updateLoadedSize();
#if 1
        std::wcout << L"<" << processPath.getProcessName() << ">\n";
#endif

        if (!std::equal(processName.begin(), processName.end(), processPath.getProcessName().begin())) { continue; }

        HandleWrapper pTerminateProcessWrapper(OpenProcess(PROCESS_TERMINATE, TRUE, processID), CloseHandle);
        if (!pTerminateProcessWrapper.get()) { continue; }

        BOOL isTerminated = TerminateProcess(pTerminateProcessWrapper.get(), 0);
        if (!isTerminated) { continue; }

        ++termintedCount;
    }

    return termintedCount;
}


void PrintTerminationResult(const std::wstring& processName, size_t termintedProcesses)
{
    std::wcout << L'\n';

    if (!!termintedProcesses)
    {
        std::wcout << L"Terminated " << termintedProcesses << " <" << processName << L"> processes";
    }
    else
    {
        std::wcout << L"Process <" << processName << L"> not found\n";
    }

    std::wcout << L'\n';
}

std::vector<DWORD> GetProcessesID()
{
    std::vector<DWORD> processes(MAX_PROCESS_COUNT);
    DWORD processesUsed = 0;

    if (EnumProcesses(&processes[0], processes.size() * sizeof(DWORD), &processesUsed))
    {
        processes.resize(processesUsed / sizeof(DWORD));
    }
    else
    {
        processes.clear();
    }

    return processes;
}

void PrintError()
{
    LPWSTR errorText = NULL;

    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&errorText),
        0,
        NULL);

    std::wcout << errorText;

    if (errorText)
    {
        LocalFree(errorText);
    }
}
