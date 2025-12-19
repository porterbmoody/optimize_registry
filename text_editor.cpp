
#include <Windows.h>
HWND hEdit;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hEdit = CreateWindowEx(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 0, 0, hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        break;
    case WM_SIZE:
        MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TextEditorClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindowEx(0, "TextEditorClass", "Text Editor", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    HMENU hMenu = CreateMenu();
    HMENU file_menu1 = CreatePopupMenu();
    HMENU file_menu2 = CreatePopupMenu();
    AppendMenu(file_menu1, MF_STRING, 1, "Open");
    AppendMenu(file_menu1, MF_STRING, 2, "Save");
    AppendMenu(file_menu1, MF_STRING, 3, "Exit");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)file_menu1, "File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)file_menu2, "Edit");
    SetMenu(hWnd, hMenu);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}