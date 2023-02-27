typedef unsigned int uint32_t;

#include "winapi_hash.h"
#include "winapi_type.h"

namespace WinApi {
	LPVOID GetFuncAddrByHash(size_t lib, UINT hash);
	void Init();
}

#define API(dll, func) ((type ## func)WinApi::GetFuncAddrByHash(dll, constexprApiHash(# func)))
#define API_check(func) ()

#define KERNEL32 0
#define ADVAPI32 1
#define USER32   2
#define NTDLL    3
#define SHLWAPI  4
#define GDI32    5