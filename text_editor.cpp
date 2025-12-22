#define idm_exit 100
#define idm_save 101
#define idm_theme 102
#define idm_edit 103
#define idm_file 104
#define idm_run 105
#define idm_directory 106
#define idm_terminal 107
#define IDI_APP_ICON 108
#define idm_git_push 109

#include "resource.h"
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <cstdlib>

HWND hEdit;
HWND hButton;
HBRUSH hBackgroundBrush;
HBRUSH hEditBrush;
HWND hConsoleChild;

void createEmbeddedConsole(HWND parent) {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    hConsoleChild = FindWindowA("ConsoleWindowClass", NULL);
    SetParent(hConsoleChild, parent);
    SetWindowPos(hConsoleChild, NULL, 0, 200, 800, 400, SWP_NOZORDER);
}

void on_file(HWND)  { std::cout << "file\n"; }
void on_edit(HWND)  { std::cout << "edit\n"; }
void on_directory(HWND)  { std::cout << "directory\n"; }
void on_save(HWND)
{ 
    std::cout << "saving..." << std::endl; 
    std::ofstream text_file;
    int length = GetWindowTextLengthA(hEdit);
    std::string code(length, '\0');
    GetWindowTextA(hEdit, code.data(), length + 1);
    std::cout << "saving: " << code << std::endl;
    text_file.open("new_file.cpp");
    text_file << code;
    text_file.close();
}
void on_theme(HWND) { std::cout << "theme\n"; }
void on_terminal(HWND hWnd) {
    std::cout << "theme\n" << std::endl;
    createEmbeddedConsole(hWnd);
}

void on_run(HWND hWnd)
{
    on_save(hWnd);
    int compileResult = system("C:/msys64/mingw64/bin/c++.exe new_file.cpp -o new_file.exe -std=c++17 -Wall");
    if (compileResult != 0) {
        std::cerr << "Compilation failed!" << std::endl;
        return;
    }
    int runResult = system("new_file.exe");
    std::cout << "Program exited with code: " << runResult << std::endl;
}

void on_git_push(HWND)  
{ 
    std::cout << "git push\n"; 
    system("git_push.bat");
}
void on_exit(HWND hWnd)
{
	DestroyWindow(hWnd);
}

struct CommandHandler
{
	UINT id;
	void (*handler)(HWND);
};
CommandHandler commands[] =
{
	{ idm_file,  on_file  },
	{ idm_edit,  on_edit  },
	{ idm_theme, on_theme },
	{ idm_run,   on_run   },
	{ idm_directory,   on_directory   },
	{ idm_save,  on_save  },
	{ idm_terminal,  on_terminal  },
	{ idm_git_push,  on_git_push  },
	{ idm_exit,  on_exit  },
};
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
        
        case WM_COMMAND:
        {
            UINT id = LOWORD(wParam);
            
            for (auto& cmd : commands)
            {
                if (cmd.id == id)
                {
                    cmd.handler(hWnd);
                    return 0;
                }
            }
            break;
        }
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
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TextEditorClass";
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindowEx(0, "TextEditorClass", "C++ Integrated Development Environment", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    HMENU h_menu = CreateMenu();
    AppendMenu(h_menu, MF_STRING, idm_file, "file");
    AppendMenu(h_menu, MF_STRING, idm_edit, "edit");
    AppendMenu(h_menu, MF_STRING, idm_theme, "theme");
    AppendMenu(h_menu, MF_STRING, idm_save, "save");
    AppendMenu(h_menu, MF_STRING, idm_directory, "directory");
    AppendMenu(h_menu, MF_STRING, idm_run, "run");
    AppendMenu(h_menu, MF_STRING, idm_terminal, "terminal");
    AppendMenu(h_menu, MF_STRING, idm_git_push, "git_push");
    AppendMenu(h_menu, MF_STRING, idm_exit, "exit");
    SetMenu(hWnd, h_menu);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}