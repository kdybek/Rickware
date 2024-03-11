#include <iostream>
#include <cstring>

#include <Windows.h>

typedef HINSTANCE(WINAPI* ShellExecuteAType)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);

// Pointer to the original ShellExecuteA function
ShellExecuteAType g_pfnOriginalShellExecuteA = nullptr;

// Custom implementation of ShellExecuteA
HINSTANCE WINAPI
RickExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
    return g_pfnOriginalShellExecuteA(nullptr, "open",
                                      "https://www.youtube.com/watch?v=dQw4w9WgXcQ&ab_channel=RickAstley",
                                      nullptr, nullptr, SW_SHOWNORMAL);
}

// Override the ShellExecuteA function
void OverrideShellExecuteA(FARPROC* pImportTableAddress)
{
    DWORD oldProtection;
    if (!VirtualProtect(pImportTableAddress, sizeof(ShellExecuteAType), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        MessageBox(nullptr, "Failed to override memory", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    *pImportTableAddress = reinterpret_cast<FARPROC>(&RickExecuteA);
    VirtualProtect(pImportTableAddress, sizeof(ShellExecuteAType), oldProtection, nullptr);
}

FARPROC* RetrieveDLLFunctionImportTableAddress(LPCSTR lpstrModuleName, LPCSTR lpstrFuncName)
{
    // Get the base address of the current process
    HMODULE hModule = GetModuleHandle(nullptr);
    if (hModule == nullptr) {
        MessageBox(nullptr, "Failed to get module handle", "Error", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    // Get the DOS header
    auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);

    // Navigate to the PE header
    auto pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD_PTR>(pDosHeader) +
                                                         pDosHeader->e_lfanew);

    // Navigate to the optional header and locate the import table directory
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pNTHeader->OptionalHeader;
    auto pImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
            reinterpret_cast<DWORD_PTR>(hModule) +
            pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    // Iterate over the import table entries
    while (pImportDescriptor->OriginalFirstThunk != 0) {

        // Read the name of the imported DLL
        char* pszDLLName = reinterpret_cast<char*>(reinterpret_cast<DWORD_PTR>(hModule) + pImportDescriptor->Name);

        if (std::strcmp(pszDLLName, TEXT(lpstrModuleName)) == 0) {
            // Iterate over the array of function pointers (import address table)
            auto* pAddressTable = reinterpret_cast<FARPROC*>(reinterpret_cast<DWORD_PTR>(hModule) +
                                                             pImportDescriptor->FirstThunk);
            while (*pAddressTable != nullptr) {
                // Read the address of the imported function
                if (*pAddressTable == GetProcAddress(GetModuleHandle(TEXT(lpstrModuleName)), TEXT(lpstrFuncName))) {
                    return pAddressTable;
                }

                // Move to the next function pointer
                pAddressTable++;
            }
        }

        // Move to the next import descriptor
        pImportDescriptor++;
    }

    MessageBox(nullptr, "Failed to find the function", "Error", MB_OK | MB_ICONERROR);
    return nullptr;
}

// Entry point of the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        FARPROC* pFunctionImportTableAddress = RetrieveDLLFunctionImportTableAddress("SHELL32.dll", "ShellExecuteA");
        g_pfnOriginalShellExecuteA = reinterpret_cast<ShellExecuteAType>(*pFunctionImportTableAddress);
        OverrideShellExecuteA(pFunctionImportTableAddress);
    }
    return TRUE;
}