#pragma once

#include <string_view>
#include <string>
#include <vector>

#include <windows.h>

void KillProcessByName(const std::wstring& processName);

size_t TerminateAllProcessesWithName(const std::wstring& procName);

void PrintTerminationResult(const std::wstring& processName, size_t termintedProcesses);

std::vector<DWORD> GetProcessesID();

std::wstring GetProcessNameFromUser();

void PrintError();