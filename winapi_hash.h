#pragma once

const unsigned int hash_const1 = 0xf0000000;
const unsigned int hash_const2 = 0x0fffffff;
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
            hash = (hash << 4) + l;
            unsigned long t = 0;
            if ((t = hash & hash_const1) != 0)
                hash = ((hash ^ (t >> 24)) & (hash_const2));
        }
    }
    return hash;
}

constexpr unsigned int hashLoadLibraryA = constexprApiHash("LoadLibraryA");
constexpr unsigned int hashGetProcAddress = constexprApiHash("GetProcAddress");
constexpr unsigned int hashMessageBoxExW = constexprApiHash("MessageBoxExW");
constexpr unsigned int hashExitProcess = constexprApiHash("ExitProcess");