#pragma once

#include <Windows.h>
#include <winnt.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <LMaccess.h>
#include <LMalert.h>
#include <lmcons.h>
#include <LM.h>

typedef int(WINAPI* typeMessageBoxExW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId);
typedef VOID(WINAPI* typeExitProcess)(UINT uExitCode);