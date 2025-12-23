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
#define IDC_MY_EDIT 110

#include "resource.h"
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include "embedded_terminal.h"

HWND hEdit;
HWND hButton;
HBRUSH hBackgroundBrush;
HBRUSH hEditBrush;
HWND hConsoleChild;
HWND hTerminal;

embedded_terminal* terminal = nullptr;
bool terminalVisible = true;

std::string current_source_file = "new_file1.cpp";
std::string current_exe = "new_file1.exe";

// terminal = new embedded_terminal(hWnd, 0, 300, 1000, 300);

void update_terminal_position(HWND parent)
{
    if (!hConsoleChild || !terminalVisible) return;
    RECT rcClient;
    GetClientRect(parent, &rcClient);
    int terminalHeight = rcClient.bottom / 3;
    int editorHeight = rcClient.bottom - terminalHeight;
    MoveWindow(hEdit, 0, 40, rcClient.right, editorHeight - 40, TRUE);
    SetWindowPos(hConsoleChild, NULL,
                 0, editorHeight,
                 rcClient.right, terminalHeight,
                 SWP_NOZORDER);
}

bool write_text_file(const char* path, const std::string& text)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;

    file.write(text.data(), text.size());
    return true;
}

void initialize_terminal(HWND parent)
{
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    hConsoleChild = GetConsoleWindow();
    SetParent(hConsoleChild, parent);
    LONG style = GetWindowLong(hConsoleChild, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME);
    SetWindowLong(hConsoleChild, GWL_STYLE, style);
    RECT rcClient;
    GetClientRect(parent, &rcClient);
    SetWindowPos(hConsoleChild, NULL, 
                 0, rcClient.bottom / 2,
                 rcClient.right, rcClient.bottom / 2,
                 SWP_NOZORDER);
}

void on_file(HWND)  { std::cout << "file\n"; }
void on_edit(HWND)  { std::cout << "edit\n"; }
void on_directory(HWND)  { std::cout << "directory\n"; }
void on_save(HWND)
{
    int length = GetWindowTextLengthA(hEdit);
    std::string code(length, '\0');
    std::cout << "length: " << length << std::endl;
    GetWindowTextA(hEdit, code.data(), length + 1);
    std::string normalized;
    normalized.reserve(code.size());
    std::cout << "saving: " << code << std::endl;
    for (size_t i = 0; i < code.size(); ++i)
    {
        if (code[i] == '\r' && i + 1 < code.size() && code[i + 1] == '\n')
        {
            normalized.push_back('\n');
            ++i;
        }
        else
        {
            normalized.push_back(code[i]);
        }
    }
    write_text_file(current_source_file, normalized);
}
void on_theme(HWND) { std::cout << "theme\n"; }
void on_terminal(HWND hWnd) {
    std::cout << "terminal\n" << std::endl;
    initialize_terminal(hWnd);
}
void on_run(HWND hWnd)
{
    on_save(hWnd);
    std::string compileCmd = "C:/msys64/mingw64/bin/c++.exe new_file1.cpp -o new_file1.exe -std=c++17 -Wall";
    std::string compileCmd = "C:/msys64/mingw64/bin/c++.exe " + current_source_file + " -o " + current_exe + " -std=c++17 -Wall";
    terminal = new embedded_terminal(hWnd, 0, 300, 1000, 300);
    terminal->appendOutput("Compiling...\n");
    int compileResult = system(compileCmd.c_str());
    if (compileResult != 0) {
        terminal->appendOutput("Compilation failed!\n");
        return;
    }
    terminal->appendOutput("Compilation successful.\nRunning program...\n");
    terminal->runProgram("new_file1.exe");
    terminal->appendOutput("Program finished.\n");
}

// void on_run(HWND hWnd)
// {
//     on_save(hWnd);
//     int compileResult = system(
//         "C:/msys64/mingw64/bin/c++.exe new_file1.cpp -o new_file1_tmp.exe -std=c++17 -Wall"
//     );
//     if (compileResult != 0) {
//         std::cerr << "Compilation failed!" << std::endl;
//         return;
//     }
//     DeleteFileA("new_file1.exe");
//     MoveFileA("new_file1_tmp.exe", "new_file1.exe");
//     int runResult = system("new_file1.exe");
//     std::cout << "Program exited with code: " << runResult << std::endl;
// }

// void on_run(HWND hWnd)
// {
//     on_save(hWnd);
//     int compileResult = system("C:/msys64/mingw64/bin/c++.exe new_file1.cpp -o new_file1.exe -std=c++17 -Wall");
//     if (compileResult != 0) {
//         std::cerr << "Compilation failed!" << std::endl;
//         return;
//     }
//     int runResult = system("new_file1.exe");
//     std::cout << "Program exited with code: " << runResult << std::endl;
// }
void on_git_push(HWND)  
{ 
    std::cout << "git push\n"; 
    system("git_push.bat");
}
void on_exit(HWND hWnd){DestroyWindow(hWnd);}
struct CommandHandler
{
	UINT id;
	void (*handler)(HWND);
};
    CommandHandler commands[] =
    {
        { idm_file,      on_file },
        { idm_edit,      on_edit },
        { idm_theme,     on_theme },
        { idm_run,       on_run  },
        { idm_directory, on_directory },
        { idm_save,      on_save },
        { idm_terminal,  on_terminal },
        { idm_git_push,  on_git_push },
        { idm_exit,      on_exit },
    };
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    int toolbarHeight = 40;
    int terminalHeight = HIWORD(lParam) / 3;
    int editHeight = HIWORD(lParam) - toolbarHeight - terminalHeight;
    switch (message) 
    {
        case WM_CREATE:
            terminal = new embedded_terminal(hWnd, 0, 300, 1000, 300);
            hBackgroundBrush = CreateSolidBrush(RGB(30, 30, 30));
            hEditBrush = CreateSolidBrush(RGB(20, 20, 20));
            hEdit = CreateWindowEx(0, "EDIT", NULL,
                                   WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                                   0, 0, 0, 0,
                                   hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);
            break;
        case WM_SIZE:
        {
            update_terminal_position(hWnd);
            RECT rc;
            GetClientRect(hWnd, &rc);
            int toolbarHeight = 40;
            int terminalHeight = rc.bottom / 3;
            int editHeight = rc.bottom - toolbarHeight - terminalHeight;
            MoveWindow(hEdit, 0, toolbarHeight, rc.right, editHeight, TRUE);
        }
        break;
        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(220, 220, 220));
            SetBkColor(hdc, RGB(20, 20, 20));
            return (LRESULT)hEditBrush;
        }
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
    HWND hEdit = CreateWindowEx(0, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
        0, 50, 1000, 600, hWnd, NULL, hInstance, NULL);
    HFONT hToolbarFont = CreateFont(
        18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Roboto");
    // HMENU h_menu = CreateMenu();
    // AppendMenu(h_menu, MF_STRING, idm_file, "file");
    // AppendMenu(h_menu, MF_STRING, idm_edit, "edit");
    // AppendMenu(h_menu, MF_STRING, idm_theme, "theme");
    // AppendMenu(h_menu, MF_STRING, idm_save, "save");
    // AppendMenu(h_menu, MF_STRING, idm_directory, "directory");
    // AppendMenu(h_menu, MF_STRING, idm_run, "run");
    // AppendMenu(h_menu, MF_STRING, idm_terminal, "terminal");
    // AppendMenu(h_menu, MF_STRING, idm_git_push, "git_push");
    // AppendMenu(h_menu, MF_STRING, idm_exit, "exit");
    // SetMenu(hWnd, h_menu);
    HWND h_button_file = CreateWindowEx(0, "BUTTON", "file",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 10, 80, 30, hWnd, (HMENU)idm_file, hInstance, NULL);
    HWND h_button_edit = CreateWindowEx(0, "BUTTON", "edit",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        100, 10, 80, 30, hWnd, (HMENU)idm_edit, hInstance, NULL);
    HWND h_button_theme = CreateWindowEx(0, "BUTTON", "theme",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        190, 10, 80, 30, hWnd, (HMENU)idm_theme, hInstance, NULL);
    HWND h_button_save = CreateWindowEx(0, "BUTTON", "save",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        280, 10, 80, 30, hWnd, (HMENU)idm_save, hInstance, NULL);
    HWND h_button_directory = CreateWindowEx(0, "BUTTON", "directory",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        370, 10, 80, 30, hWnd, (HMENU)idm_directory, hInstance, NULL);
    HWND h_button_run = CreateWindowEx(0, "BUTTON", "run",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        460, 10, 80, 30, hWnd, (HMENU)idm_run, hInstance, NULL);
    HWND h_button_terminal = CreateWindowEx(0, "BUTTON", "terminal",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        550, 10, 80, 30, hWnd, (HMENU)idm_terminal, hInstance, NULL);
    HWND h_button_git_push = CreateWindowEx(0, "BUTTON", "git push",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        640, 10, 80, 30, hWnd, (HMENU)idm_git_push, hInstance, NULL);
    HWND h_button_exit = CreateWindowEx(0, "BUTTON", "exit",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        730, 10, 80, 30, hWnd, (HMENU)idm_exit, hInstance, NULL);
    SendMessage(h_button_file,  WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_edit,  WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_theme, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_save, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_directory, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_run, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_terminal, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_git_push, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    SendMessage(h_button_exit, WM_SETFONT, (WPARAM)hToolbarFont, TRUE);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
