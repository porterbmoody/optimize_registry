#define idm_exit 100
#define idm_save 101
#define idm_theme 102
#define idm_edit 103
#define idm_file 104
#define idm_run 105
#define idm_directory 106
#define IDI_APP_ICON 107

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

void on_file(HWND)  { std::cout << "file\n"; }
void on_edit(HWND)  { std::cout << "edit\n"; }
void on_directory(HWND)  { std::cout << "directory\n"; }
void on_save(HWND)
{ 
    std::cout << "saving file..." << std::endl; 
    std::ofstream text_file;
    int length = GetWindowTextLengthA(hEdit);
    std::string code(length, '\0');  
    GetWindowTextA(hEdit, code.data(), length + 1);
    std::cout << "saving: " << code << std::endl;
    text_file.open("text_file.cpp");
    text_file << code;
    text_file.close();
}
void on_theme(HWND) { std::cout << "theme\n"; }
void on_run(HWND hWnd)
{
    std::cout << "length \n" << std::endl;
    int length = GetWindowTextLength(hEdit);
    if (length == 0) {
        std::cout << "empty text" << std::endl;
        return;
    }
    std::cout << "running script..." << std::endl;
    // std::cout << "length " << length << "\n" << std::endl;
    std::string code;
    code.resize(length + 1);
    // GetWindowTextA(hEdit, &code[0], length + 1);
	// const char* batch_path = "C:\\Users\\Owner\\Desktop\\coding\\optimize_registry\\run_registry.bat";
    // const char* temp_path = "C:/Users/Owner/Desktop/coding/optimize_registry/run_registry.bat";
    // std::ofstream temp_file(temp_path);
    // std::cout << temp_path << std::endl;
    // system("dir");
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
	{ idm_exit,  on_exit  }
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
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(107), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
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
    AppendMenu(h_menu, MF_STRING, idm_exit, "exit");

    SetMenu(hWnd, h_menu);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

    // case WM_COMMAND:
    // {
    //     switch (LOWORD(wParam))
    //     {
    //         case idm_file:    std::cout << "file" << std::endl; break;
    //         case idm_edit:    std::cout << "edit" << std::endl; break;
    //         case idm_save:    std::cout << "save" << std::endl; break;
    //         case idm_theme:   std::cout << "theme" << std::endl; break;
    //         case idm_run:
    //         {
    //             // const char* batch_path = "C:\\Users\\Owner\\Desktop\\coding\\optimize_registry\\run_registry.bat";
    //             // std::cout << batch_path << std::endl;
    //             system("dir");
    //             // system("C:/msys64/mingw64/bin/g++.exe text_editor.cpp -o text_editor.exe -mwindows -lgdi32 -lcomdlg32");
    //             // ShellExecuteA(
    //             //     NULL,
    //             //     "open",
    //             //     "cmd.exe",
    //             //     "/k \"C:\\Users\\Owner\\Desktop\\coding\\optimize_registry\\run_registry.bat\"",
    //             //     NULL,
    //             //     SW_SHOWNORMAL
    //             // );

    //             // std::cout << "Running batch file..." << std::endl;
    //             std::cout << "dir" << std::endl;
    //             break;
    //         }
    //         case idm_exit:    DestroyWindow(hWnd); break;
    //     }
    //     break;
    // }