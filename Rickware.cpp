#include <Windows.h>
#include <iostream>

void printLastError()
{
    DWORD errorCode = GetLastError();
    if (!errorCode) {
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
            std::cout << "Error Code: " << errorCode << std::endl;
            std::cout << "Error Message: " << errorMessage << std::endl;
            LocalFree(errorMessage);
        } else {
            std::cerr << "Failed to retrieve error message for error code: " << errorCode << std::endl;
        }
    } else {
        std::cerr << "No error occurred." << std::endl;
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
        std::cerr << "Failed to open target process" << std::endl;
        printLastError();
        return FALSE;
    }

    // Allocate memory in the target process for the DLL path
    dllPathAddress = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (dllPathAddress == nullptr) {
        std::cerr << "Failed to allocate memory in target process" << std::endl;
        printLastError();
        result = FALSE;
        goto error1;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, dllPathAddress, dllPath, strlen(dllPath) + 1, nullptr)) {
        std::cerr << "Failed to write DLL path to target process" << std::endl;
        printLastError();
        result = FALSE;
        goto error2;
    }

    // Get the address of the LoadLibraryA function in the kernel32.dll module
    loadLibraryAddr = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
    if (loadLibraryAddr == nullptr) {
        std::cerr << "Failed to get address of LoadLibraryA function" << std::endl;
        printLastError();
        result = FALSE;
        goto error2;
    }

    // Create a remote thread in the target process to load the DLL
    hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLibraryAddr, dllPathAddress, 0, nullptr);
    if (hThread == nullptr) {
        std::cerr << "Failed to create remote thread in target process" << std::endl;
        printLastError();
        result = FALSE;
        goto error2;
    }

    // Wait for the remote thread to terminate
    WaitForSingleObject(hThread, INFINITE);

    // Clean up resources
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
    const char* dllPath = "RickExecute.dll"; // Replace this with the path to your DLL file

    std::cout << "Enter target process ID: ";
    std::cin >> processId;

    if (injectDLL(processId, dllPath)) {
        std::cout << "DLL injected successfully" << std::endl;
    } else {
        std::cerr << "Failed to inject DLL" << std::endl;
    }

    return 0;
}

