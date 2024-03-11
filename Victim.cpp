#include <Windows.h>

// Window procedure callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            // Handle hyperlink click event
            if (LOWORD(wParam) == 1001)
            {
                ShellExecuteA(NULL, "open", "https://www.example.com", NULL, NULL, SW_SHOWNORMAL);
            }
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register window class
    const char* CLASS_NAME = "Sample Window Class";
    const wchar_t* CLASS_NAME2 = L"Sample Window Class";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Create window
    HWND hwnd = CreateWindowExW(
            0,                              // Optional window styles
            CLASS_NAME2,                     // Window class
            L"Sample Window",               // Window title
            WS_OVERLAPPEDWINDOW,            // Window style
            CW_USEDEFAULT, CW_USEDEFAULT,   // Position
            640, 480,                       // Size
            NULL,                           // Parent window
            NULL,                           // Menu
            hInstance,                      // Instance handle
            NULL                            // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    // Create hyperlink control
    HWND hLink = CreateWindowW(
            L"STATIC",                      // Predefined class; Unicode assumed
            L"<a href=\"https://www.example.com\">Click Here</a>",  // Link text
            WS_CHILD | WS_VISIBLE | SS_NOTIFY, // Styles
            10, 10, 100, 20,                // Position and size
            hwnd,                           // Parent window
            (HMENU)1001,                    // Control identifier
            hInstance,                      // Instance handle
            NULL                            // No window creation data
    );

    if (hLink == NULL)
    {
        return 0;
    }

    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}