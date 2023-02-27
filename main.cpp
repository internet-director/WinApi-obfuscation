#include "winapi.h"

int main(int argc, char *argv[]) {
    WinApi::Init();
    API(USER32, MessageBoxExW)(0, L"Test", L"Test", 0, 0);
    API(KERNEL32, ExitProcess)(0);
}