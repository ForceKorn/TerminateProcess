#include "utis.h"

#include <iostream>
#include <memory>

#include <windows.h>
#include <Tlhelp32.h>

using HandleWrapper = std::unique_ptr<void, decltype (&CloseHandle)>;


void PrintErrorW(const std::wstring_view msg)
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

    std::wcout << msg << errorText;

    if (errorText)
    {
        LocalFree(errorText);
    }
}

bool Terminate(DWORD PID)
{
    HandleWrapper pCurrentProcess(OpenProcess(PROCESS_TERMINATE, 0, PID), CloseHandle);
    if (!pCurrentProcess.get())
    {
        PrintErrorW(L"Can`t open process. ");
        return false;
    }

    if (!TerminateProcess(pCurrentProcess.get(), 0))
    {
        PrintErrorW(L"Can`t terminate process. ");
        return false;
    }

    PrintErrorW(L"Process terminated. ");
    return true;
}

void KillProcessByName(const std::wstring_view filename)
{
    HandleWrapper pSnapShot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL), CloseHandle);
    if (pSnapShot.get() == INVALID_HANDLE_VALUE)
    {
        PrintErrorW(L"Can`t create process snapshot. ");
    }

    PROCESSENTRY32 pEntry = {};
    pEntry.dwSize = sizeof(pEntry);

    size_t terminatedProcesses = 0;

    for (BOOL currentProcess = Process32FirstW(pSnapShot.get(), &pEntry);
        currentProcess;
        currentProcess = Process32NextW(pSnapShot.get(), &pEntry))
    {
#if 0
        std::wcout << L"<" << pEntry.szExeFile << L">\n";
#endif

        if (filename == std::wstring_view(pEntry.szExeFile))
        {
            terminatedProcesses += Terminate(pEntry.th32ProcessID);
        }
    }

    if (!terminatedProcesses)
    {
        std::wcout << L"<" << filename << L"> process not found!\n";
    }
    else
    {
        std::wcout << L"Terminated " << terminatedProcesses << L" <" << filename << L"> processes!\n";
    }
}

std::wstring GetProcessNameFromUser()
{
    std::wstring input;

    std::wcout << L"Please enter process name: ";
    getline(std::wcin, input);

    return input;
}
