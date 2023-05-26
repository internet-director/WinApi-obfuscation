#include <Windows.h>
#include <winnt.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <LMaccess.h>
#include <LMalert.h>
#include <lmcons.h>
#include <LM.h>

#include <type_traits>
#include "winapi_hash.h"

namespace WinApi {
	LPVOID GetFuncAddrByHash(size_t lib, UINT hash);
	void Init();
}

#define API(dll, func) (reinterpret_cast<std::decay_t<decltype(func)>>(WinApi::GetFuncAddrByHash(dll, constexprApiHash(# func))))
#define API_check(func) ()

#define KERNEL32 0
#define ADVAPI32 1
#define USER32   2
#define NTDLL    3
#define SHLWAPI  4
#define GDI32    5