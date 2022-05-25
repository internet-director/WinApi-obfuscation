#include "winapi.h"

#define RVATOVA(base, offset) ((SIZE_T)base + (SIZE_T)offset)
constexpr unsigned int KERNEL_32 = constexprApiHash("kernel32.dll");
constexpr unsigned int NT_DLL = constexprApiHash("ntdll.dll");
const int functionsCount = 64;
int api_counter = 0;

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
        dll_name[sz] = NULL;
        if (constexprApiHash(dll_name) == dllHash) return (HMODULE)module_ptr->Reserved2[0];
        else module_ptr = (PLDR_DATA_TABLE_ENTRY)module_ptr->Reserved1[0];

    } while (module_ptr && module_ptr != first_mod);

    return NULL;
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

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lib;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)RVATOVA(lib, dos->e_lfanew);
    IMAGE_FILE_HEADER f = nt->FileHeader;
    IMAGE_OPTIONAL_HEADER opt = nt->OptionalHeader;
    PIMAGE_EXPORT_DIRECTORY data = (PIMAGE_EXPORT_DIRECTORY)RVATOVA(lib, opt.DataDirectory[0].VirtualAddress);
    PDWORD name = (PDWORD)RVATOVA(lib, data->AddressOfNames);

    char* n = nullptr;
    for (int i = 0; i < data->NumberOfNames; i++) {
        n = (char*)RVATOVA(lib, *name);
        if (fHash == constexprApiHash(n, strlen_(n))) {
            break;
        }
        name++;
    }
    api_mass[api_counter].addr = _GetProcAddress((HMODULE)lib, n);
    return api_mass[api_counter++].addr;
}

HANDLE GetApiAddr3(HANDLE lib, DWORD fHash) 
{
    IMAGE_DOS_HEADER* dos_header;
#ifndef _WIN64
    IMAGE_NT_HEADERS* nt_headers;
#else
    IMAGE_NT_HEADERS64* nt_headers;
#endif
    IMAGE_EXPORT_DIRECTORY* export_dir;
    DWORD* names, * funcs;
    WORD* nameords;

    dos_header = (IMAGE_DOS_HEADER*)lib;
#ifndef _WIN64
    nt_headers = (IMAGE_NT_HEADERS*)RVATOVA(lib, dos_header->e_lfanew);
#else
    nt_headers = (IMAGE_NT_HEADERS64*)RVATOVA(lib, dos_header->e_lfanew);
#endif
    export_dir = (IMAGE_EXPORT_DIRECTORY*)RVATOVA(lib, nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    names = (DWORD*)RVATOVA(lib, export_dir->AddressOfNames);
    funcs = (DWORD*)RVATOVA(lib, export_dir->AddressOfFunctions);
    nameords = (WORD*)RVATOVA(lib, export_dir->AddressOfNameOrdinals);

    for (DWORD i = 0; i < export_dir->NumberOfNames; i++)
    {
        char* string = (char*)((SIZE_T)lib + (SIZE_T)names[i]);
        if (fHash == constexprApiHash(string, strlen_(string)))
        {
            WORD nameord = nameords[i];
            DWORD funcrva = funcs[nameord];
            HANDLE functionAddr = (HANDLE)RVATOVA(lib, funcrva);
            {
                // TODO
            }
            return functionAddr;
        }
    }

    return nullptr;
}

LPVOID WinApi::GetFuncAddrByHash(size_t lib, UINT hash) 
{
    if(!dll_mass[lib].addr)
        dll_mass[lib].addr = (_LoadLibrary)(dll_mass[lib].name);
    return GetApiAddr2(dll_mass[lib].addr, hash);
}

void WinApi::Init()
{
    HINSTANCE kernel32 = GetDllBase(KERNEL_32);
    _LoadLibrary = (typeLoadLibraryA)(GetApiAddr3(kernel32, hashLoadLibraryA));
    _GetProcAddress = (typeGetProcAddress)(GetApiAddr3(kernel32, hashGetProcAddress));
}