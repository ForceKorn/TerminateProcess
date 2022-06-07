#pragma once

#include <string_view>
#include <string>

std::wstring GetProcessNameFromUser();
void KillProcessByName(const std::wstring_view filename);
