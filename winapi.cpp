#include "winapi.h"

#define RVATOVA(base, offset) ((SIZE_T)base + (SIZE_T)offset)
const int functionsCount = 64;
int api_counter = 0;
bool inited = false;

typedef LPVOID(WINAPI* typeGetProcAddress)(HMODULE lib, LPCSTR func);
typedef HMODULE(WINAPI* typeLoadLibraryA)(LPCSTR func);

typeGetProcAddress _GetProcAddress;
typeLoadLibraryA _LoadLibrary;

struct func_addr {
    LPVOID addr = 0;
    DWORD hash = 0;
};

struct dll_addr {
    LPVOID      addr = 0;
    const char* name = 0;
};

func_addr api_mass[functionsCount];

dll_addr dll_mass[] = {
    { NULL, ("kernel32.dll") },
    { NULL, ("Advapi32.dll") },
    { NULL, ("user32.dll") },
    { NULL, ("ntdll.dll") },
    { NULL, ("Shlwapi.dll") },
    { NULL, ("Gdi32.dll") }
};

template<typename T>
size_t strlen_(const T* str) 
{
    size_t sz = -1;
    while (str[++sz]);
    return sz;
}
void Wide2Char(const WCHAR* data, char* out, UINT len)
{
    for (int i = 0; i < len; i++) {
        if (!data[i]) {
            out[i] = 0;
            break;
        }
        // TODO
        out[i] = data[i];
    }
}

PPEB GetPEB()
{
#ifdef _WIN64
    return  (PPEB)__readgsqword(0x60);
#else
    return  (PPEB)__readfsdword(0x30);
#endif
}

HMODULE GetDllBase(UINT dllHash)
{
    PPEB peb;
    LDR_DATA_TABLE_ENTRY* module_ptr, * first_mod;

    peb = GetPEB();

    module_ptr = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InMemoryOrderModuleList.Flink;
    first_mod = module_ptr;

    char dll_name[64];

    do
    {
        UINT sz = strlen_(module_ptr->FullDllName.Buffer);
        Wide2Char(module_ptr->FullDllName.Buffer, dll_name, sz);
        dll_name[sz] = 0;
        if (constexprApiHash(dll_name) == dllHash) return (HMODULE)module_ptr->Reserved2[0];
        else module_ptr = (PLDR_DATA_TABLE_ENTRY)module_ptr->Reserved1[0];

    } while (module_ptr && module_ptr != first_mod);

    return nullptr;
}


HANDLE GetApiAddr2(HANDLE lib, size_t fHash) 
{
    for (int i = 0; i < api_counter; i++) {
        if (api_mass[i].hash == fHash) {
            return api_mass[i].addr;
        }
    }
    api_mass[api_counter].hash = fHash;

    if (lib == nullptr) return nullptr;

    auto dos = (PIMAGE_DOS_HEADER)lib;
    auto nt = (PIMAGE_NT_HEADERS)RVATOVA(lib, dos->e_lfanew);
    IMAGE_FILE_HEADER f = nt->FileHeader;
    IMAGE_OPTIONAL_HEADER opt = nt->OptionalHeader;
    auto export_dir = (PIMAGE_EXPORT_DIRECTORY)RVATOVA(lib, opt.DataDirectory[0].VirtualAddress);
    auto name = (PDWORD)RVATOVA(lib, export_dir->AddressOfNames);
    auto funcs = (DWORD*)RVATOVA(lib, export_dir->AddressOfFunctions);
    auto nameords = (WORD*)RVATOVA(lib, export_dir->AddressOfNameOrdinals);

    char* n = nullptr;
    for (int i = 0; i < export_dir->NumberOfNames; i++) {
        n = (char*)RVATOVA(lib, *name);
        if (fHash == constexprApiHash(n, strlen_(n))) {
            if(inited) break;
            return (HANDLE)RVATOVA(lib, funcs[nameords[i]]);
        }
        name++;
    }
    api_mass[api_counter].addr = _GetProcAddress((HMODULE)lib, n);
    return api_mass[api_counter++].addr;
}

LPVOID WinApi::GetFuncAddrByHash(size_t lib, UINT hash) 
{
    if(!inited)
        Init();
    if(!dll_mass[lib].addr)
        dll_mass[lib].addr = (_LoadLibrary)(dll_mass[lib].name);
    return GetApiAddr2(dll_mass[lib].addr, hash);
}

void WinApi::Init()
{
    HINSTANCE kernel32 = GetDllBase(hashKERNEL32);
    _LoadLibrary = (typeLoadLibraryA)(GetApiAddr2(kernel32, hashLoadLibraryA));
    _GetProcAddress = (typeGetProcAddress)(GetApiAddr2(kernel32, hashGetProcAddress));
    inited = true;
}