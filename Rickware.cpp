#include <Windows.h>
#include <iostream>

#define DLL_PATH "RickExecute.dll"

void printLastError()
{
    DWORD errorCode = GetLastError();
    if (errorCode) {
        LPSTR errorMessage = nullptr;
        DWORD result = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR) &errorMessage,
                0,
                nullptr
        );
        if (!result) {
            std::cout << "Error Code: " << errorCode << '\n';
            std::cout << "Error Message: " << errorMessage << '\n';
            LocalFree(errorMessage);
        } else {
            std::cerr << "Failed to retrieve error message for error code: " << errorCode << '\n';
        }
    } else {
        std::cerr << "No error occurred." << '\n';
    }
}

BOOL injectDLL(DWORD processId, const char* dllPath)
{
    BOOL result = TRUE;
    LPVOID dllPathAddress;
    LPTHREAD_START_ROUTINE loadLibraryAddr;
    HANDLE hThread;

    // Open the target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess == nullptr) {
        std::cerr << "Failed to open target process" << '\n';
        printLastError();
        return FALSE;
    }

    // Allocate memory in the target process for the DLL path
    dllPathAddress = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (dllPathAddress == nullptr) {
        std::cerr << "Failed to allocate memory in target process" << '\n';
        printLastError();
        result = FALSE;
        goto error1;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, dllPathAddress, dllPath, strlen(dllPath) + 1, nullptr)) {
        std::cerr << "Failed to write DLL path to target process" << '\n';
        printLastError();
        result = FALSE;
        goto error2;
    }

    // Get the address of the LoadLibraryA function in the kernel32.dll module
    loadLibraryAddr = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandleA("kernel32.dll"),
                                                                              "LoadLibraryA"));
    if (loadLibraryAddr == nullptr) {
        std::cerr << "Failed to get address of LoadLibraryA function" << '\n';
        printLastError();
        result = FALSE;
        goto error2;
    }

    // Create a remote thread in the target process to load the DLL
    hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLibraryAddr, dllPathAddress, 0, nullptr);
    if (hThread == nullptr) {
        std::cerr << "Failed to create remote thread in target process" << '\n';
        printLastError();
        result = FALSE;
        goto error2;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);

    error2:
    VirtualFreeEx(hProcess, dllPathAddress, 0, MEM_RELEASE);

    error1:
    CloseHandle(hProcess);

    return result;
}

int main()
{
    DWORD processId;
    const char* dllPath = DLL_PATH;

    std::cout << "Enter target PID: ";
    std::cin >> processId;

    if (injectDLL(processId, dllPath)) {
        std::cout << "DLL injected successfully" << '\n';
    } else {
        std::cerr << "Failed to inject DLL" << '\n';
    }

    return 0;
}

