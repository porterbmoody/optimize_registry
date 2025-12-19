// #define WIN32_LEAN_AND_MEAN
// #define UNICODE
// #define _UNICODE
#define idm_exit 100
#define idm_save 101
#define idm_theme 102
#define idm_edit 103
#define idm_file 104
#define idm_run 105

#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>

HWND hEdit;
HWND hButton;
HBRUSH hBackgroundBrush;
HBRUSH hEditBrush;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hBackgroundBrush = CreateSolidBrush(RGB(30, 30, 30));
        hEditBrush = CreateSolidBrush(RGB(20, 20, 20));
        hEdit = CreateWindowEx(0, "EDIT", NULL,
                               WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                               0, 0, 0, 0,
                               hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        break;

    case WM_SIZE:
        MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
            case idm_file:    std::cout << "file" << std::endl; break;
            case idm_edit:    std::cout << "edit" << std::endl; break;
            case idm_save:    std::cout << "save" << std::endl; break;
            case idm_theme:   std::cout << "theme" << std::endl; break;
            case idm_run:
            {
                const char* batch_path = "C:\\Users\\Owner\\Desktop\\coding\\optimize_registry\\run_registry.bat";
                std::cout << batch_path << std::endl;
                ShellExecuteA(NULL, "open", batch_path, NULL, NULL, SW_SHOWNORMAL);
                std::cout << "Running batch file..." << std::endl;
                break;
            }
            case idm_exit:    DestroyWindow(hWnd); break;
        }
        break;
    }

    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, RGB(220, 220, 220));
        SetBkColor(hdc, RGB(20, 20, 20));
        return (LRESULT)hEditBrush;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        std::cout << "Console attached!" << std::endl;
    }

    std::cout << "WinMain" << std::endl;

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
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    HMENU h_menu = CreateMenu();
    AppendMenu(h_menu, MF_STRING, idm_file, "file");
    AppendMenu(h_menu, MF_STRING, idm_edit, "edit");
    AppendMenu(h_menu, MF_STRING, idm_theme, "theme");
    AppendMenu(h_menu, MF_STRING, idm_save, "save");
    AppendMenu(h_menu, MF_STRING, idm_exit, "exit");
    AppendMenu(h_menu, MF_STRING, idm_run, "run");

    SetMenu(hWnd, h_menu);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
