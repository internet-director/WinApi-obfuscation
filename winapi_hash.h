#pragma once

const unsigned int hash_seed[4] = {0, 0, 0, 0};

constexpr unsigned long constexprApiHash(const char* str, int sz = 0) {
    unsigned long hash = 0;
    unsigned int size = -1;
    if (!sz) {
        while (true) {
            if (str[++size] == 0) break;
        }
    }
    else size = sz;

    if (str && size > 0)
    {
        for (int i = 0; i < size; i++, str++)
        {
            char l = *str;
            if (l >= 'A' && l <= 'Z') l -= 'A' - 'a';
            hash = (hash << 6) + (hash << 16) - hash + l;
        }
    }
    return hash;
}

constexpr unsigned int hashKERNEL32 = constexprApiHash("kernel32.dll");
constexpr unsigned int hashNTDLL = constexprApiHash("ntdll.dll");
constexpr unsigned int hashLoadLibraryA = constexprApiHash("LoadLibraryA");
constexpr unsigned int hashGetProcAddress = constexprApiHash("GetProcAddress");