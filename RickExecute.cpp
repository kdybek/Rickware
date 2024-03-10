#include <iostream>
#include <windows.h>

typedef HINSTANCE(WINAPI *ShellExecuteAType)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);

// Pointer to the original ShellExecuteA function
ShellExecuteAType g_pfnOriginalShellExecuteA = nullptr;

// Custom implementation of ShellExecuteA
HINSTANCE WINAPI RickExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd) {
    return g_pfnOriginalShellExecuteA(NULL, "open", "https://www.youtube.com/watch?v=dQw4w9WgXcQ&ab_channel=RickAstley", NULL, NULL, SW_SHOWNORMAL);
}

// Override the ShellExecuteA function
void OverrideShellExecuteA() {
    HMODULE hModule = GetModuleHandle(TEXT("shell32.dll"));
    if (hModule != nullptr) {
        void* pfnOriginalShellExecuteA = reinterpret_cast<void*>(GetProcAddress(hModule, "ShellExecuteA"));
        if (pfnOriginalShellExecuteA != nullptr) {
            // Replace the address of ShellExecuteA with the address of MyShellExecuteA
            DWORD oldProtection;
            VirtualProtect(pfnOriginalShellExecuteA, sizeof(ShellExecuteAType), PAGE_EXECUTE_READWRITE, &oldProtection);
            g_pfnOriginalShellExecuteA = reinterpret_cast<ShellExecuteAType>(pfnOriginalShellExecuteA);
            WriteProcessMemory(GetCurrentProcess(), pfnOriginalShellExecuteA, RickExecuteA, sizeof(ShellExecuteAType), nullptr);
            VirtualProtect(pfnOriginalShellExecuteA, sizeof(ShellExecuteAType), oldProtection, nullptr);
        }
    }
}

// Entry point of the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        OverrideShellExecuteA();
    }
    return TRUE;
}