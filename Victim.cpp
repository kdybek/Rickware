#include <Windows.h>

// Window procedure callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_COMMAND:
            // Handle hyperlink click event
            if (LOWORD(wParam) == 1001) {
                ShellExecuteA(nullptr, "open", "https://www.example.com", nullptr, nullptr, SW_SHOWNORMAL);
            }
            break;

        default:
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Sample Window Class";
    RegisterClass(&wc);

    // Create window
    HWND hwnd = CreateWindowExW(
            0,                              // Optional window styles
            L"Sample Window Class",         // Window class
            L"Sample Window",               // Window title
            WS_OVERLAPPEDWINDOW,            // Window style
            CW_USEDEFAULT, CW_USEDEFAULT,   // Position
            640, 480,                       // Size
            nullptr,                        // Parent window
            nullptr,                        // Menu
            hInstance,                      // Instance handle
            nullptr                         // Additional application data
    );

    if (hwnd == nullptr) {
        return 0;
    }

    // Create hyperlink control
    HWND hLink = CreateWindowW(
            L"STATIC",                      // Predefined class; Unicode assumed
            L"<a href=\"https://www.example.com\">Click Here</a>",  // Link text
            WS_CHILD | WS_VISIBLE | SS_NOTIFY, // Styles
            10, 10, 100, 20,                // Position and size
            hwnd,                           // Parent window
            (HMENU) 1001,                   // Control identifier
            hInstance,                      // Instance handle
            nullptr                         // No window creation data
    );

    if (hLink == nullptr) {
        return 0;
    }

    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}