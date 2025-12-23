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
#define idm_open_file 111

#include "resource.h"
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include "embedded_terminal.h"
#include <thread>

HWND hEdit;
HWND hButton;
HBRUSH hBackgroundBrush;
HBRUSH hEditBrush;
HWND hConsoleChild;
HWND hTerminal;
HBRUSH hButtonBrush;

embedded_terminal* terminal = nullptr;
bool terminalVisible = true;

std::string output_source_file = "new_file.cpp";
std::string output_exe = "new_file.exe";
std::string cpp_executable_path = "C:/msys64/mingw64/bin/c++.exe";
int toolbarWidth = 70;
int buttonHeight = 40;

struct ButtonInfo {
    const char* text;
    int id;
};
void update_terminal_position(HWND parent)
{
    if (!hConsoleChild || !terminalVisible) return;
    RECT rcClient;
    GetClientRect(parent, &rcClient);
    int terminalHeight = rcClient.bottom / 2;
    int editorWidth = rcClient.right - toolbarWidth;
    int editorHeight = rcClient.bottom - terminalHeight;
    MoveWindow(hEdit, toolbarWidth, terminalHeight, editorWidth, editorHeight, TRUE);
    // SetWindowPos(hConsoleChild, NULL,toolbarWidth, editorHeight,editorWidth, terminalHeight,SWP_NOZORDER);
}

bool write_text_file(const std::string path, const std::string& text)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;
    file.write(text.data(), text.size());
    return true;
}
void on_file(HWND)  { terminal->append_output("file\n"); }
void on_edit(HWND)  { terminal->append_output("edit\n");}
void on_directory(HWND)  { terminal->append_output("directory\n"); }
void on_save(HWND)
{
    int length = GetWindowTextLengthA(hEdit);
    std::string code(length, '\0');
    if (terminal) {
        terminal->append_output("length: " + std::to_string(length) + "\n");
    }
    GetWindowTextA(hEdit, code.data(), length + 1);
    std::string normalized;
    normalized.reserve(code.size());
    if (terminal) {
        terminal->append_output("saving: " + code + "\n");
    }
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
    write_text_file(output_source_file, normalized);
}
void on_theme(HWND) { terminal->append_output("theme"); }
void on_open_file(HWND)
{
    std::ifstream file(output_source_file);
    if (!file) {
        if (terminal) terminal->append_output("File not found: " + output_source_file + "\n");
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    SetWindowTextA(hEdit, content.c_str());
    if (terminal) terminal->append_output("Loaded file: " + output_source_file + "\n");
}
// void on_open_file(HWND hWnd) {
//     OPENFILENAME ofn;
//     char szFile[MAX_PATH] = { 0 };x
//     ZeroMemory(&ofn, sizeof(ofn));
//     ofn.lStructSize = sizeof(ofn);
//     ofn.hwndOwner = hWnd;
//     ofn.lpstrFilter = "C++ Files\0*.cpp\0All Files\0*.*\0";
//     ofn.lpstrFile = szFile;
//     ofn.nMaxFile = sizeof(szFile);
//     ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
//     ofn.lpstrDefExt = "cpp";
//     if (GetOpenFileName(&ofn)) {
//         current_source_file = szFile;
//         std::ifstream file(current_source_file);
//         if (file) {
//             std::string content((std::istreambuf_iterator<char>(file)),
//                                  std::istreambuf_iterator<char>());
//             SetWindowTextA(hEdit, content.c_str());
//         }
//     }
// }
void on_terminal(HWND hWnd) {
    terminal->append_output("opening terminal...\n");
    // initialize_terminal(hWnd);
}
void on_run(HWND hWnd)
{
    std::thread([hWnd]() {
        on_save(hWnd);
        if (output_exe.empty()) return;
        std::string cmd_kill_process = "taskkill /IM " + output_exe + " /F";
        terminal->execute_command(cmd_kill_process);
        terminal->append_output("Compiling...\n");
        std::string compile_cmd = cpp_executable_path + " " + output_source_file + " -o " + output_exe + " -std=c++17 -Wall";
        int compileResult = system(compile_cmd.c_str());
        if (compileResult != 0) {
            terminal->append_output("Compilation failed!\n");
            return;
        }
        terminal->append_output("Compilation successful: " + output_source_file + " -> " + output_exe + "\n");
        terminal->execute_command(output_exe);
        terminal->append_output("Program finished.\n");
    }).detach();
}

// void on_run(HWND hWnd)
// {
//     std::string compile_cmd = cpp_executable_path +" " + output_source_file + " -o " + output_exe + " -std=c++17 -Wall";
//     on_save(hWnd);
//     if (output_exe.empty()) {
//         return;
//     } else {
//         std::string cmd_kill_process = "taskkill /IM " + output_exe + " /F";
//         terminal->execute_command(cmd_kill_process);
//     }
//     terminal->append_output("Compiling...\n");
//     int compileResult = system(compile_cmd.c_str());
//     if (compileResult != 0) {
//         terminal->append_output("Compilation failed!\n");
//         return;
//     }
//     terminal->append_output("Compilation successful: "+output_source_file+" "+output_exe+".\n\n");
//     terminal->execute_command(output_exe);
//     terminal->append_output("Program finished.\n");
// }
void on_git_push(HWND)  
{ 
    terminal->append_output("pushing to github\n");
    terminal->execute_command("git_push.bat");
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
        { idm_open_file,     on_open_file },
        { idm_run,       on_run  },
        { idm_directory, on_directory },
        { idm_save,      on_save },
        { idm_terminal,  on_terminal },
        { idm_git_push,  on_git_push },
        { idm_exit,      on_exit },
    };
HWND create_button(
    HWND parent,
    HINSTANCE hInstance,
    HFONT font,
    const char* text,
    int id,
    int index
)
{
    HWND b = CreateWindowEx(
        0, "BUTTON", text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, buttonHeight * index, toolbarWidth, buttonHeight,
        parent,
        (HMENU)(INT_PTR)id,
        hInstance,
        NULL
    );
    SendMessage(b, WM_SETFONT, (WPARAM)font, TRUE);
    return b;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
        case WM_CREATE:
        {
            HINSTANCE hInstance = GetModuleHandle(NULL);
            HFONT hToolbarFont = CreateFont(
                18, 0, 0, 0, FW_NORMAL,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                "Audiowide"
            );
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            
            int clientWidth = rcClient.right - rcClient.left;
            int clientHeight = rcClient.bottom - rcClient.top;
            int rightX = toolbarWidth;
            int rightWidth = clientWidth - toolbarWidth;
            int terminalHeight = clientHeight / 2;
            int editorHeight = clientHeight - terminalHeight;

            hEdit = CreateWindowEx(0, "EDIT", "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                rightX, 0,
                rightWidth, editorHeight,
                hWnd, NULL, hInstance, NULL);
                hBackgroundBrush = CreateSolidBrush(RGB(30, 30, 30));
                hEditBrush = CreateSolidBrush(RGB(20, 20, 20));
                hButtonBrush = CreateSolidBrush(RGB(0, 0, 0));
                ButtonInfo buttons[] = {
                    {"file",      idm_file},
                    {"open file", idm_open_file},
                    {"edit",      idm_edit},
                    {"theme",     idm_theme},
                    {"directory", idm_directory},
                    {"run",       idm_run},
                    {"terminal",  idm_terminal},
                    {"git push",  idm_git_push},
                    {"save",      idm_save},
                    {"exit",      idm_exit},
                };
                for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); ++i) {
                    create_button(hWnd, hInstance, hToolbarFont, buttons[i].text, buttons[i].id, i);
                }
            terminal = new embedded_terminal(hWnd, rightX, editorHeight, rightWidth, terminalHeight);
            break;
        }
        case WM_SIZE:
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            int terminalHeight = rc.bottom / 2;
            MoveWindow(hEdit, toolbarWidth, 0, rc.right - toolbarWidth, rc.bottom - terminalHeight, TRUE);
            update_terminal_position(hWnd);
            break;
        }
        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(220, 220, 220));
            SetBkColor(hdc, RGB(20, 20, 20));
            return (LRESULT)hEditBrush;
        }
        // case WM_CTLCOLOREDIT:
        // {
        //     HDC hdc = (HDC)wParam;
        //     HWND hwndCtrl = (HWND)lParam;
        //     if (hwndCtrl == hTerminal)
        //     {
        //         SetTextColor(hdc, RGB(255, 255, 255));
        //         SetBkColor(hdc, RGB(0, 0, 0));
        //         return (LRESULT)hBlackBrush;
        //     }
        //     break;
        // }
        case WM_CTLCOLORBTN:
        {
            // std::cout <<  "WM_CTLCOLORBTN" << std::endl;
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(220, 220, 220));
            SetBkMode(hdc, TRANSPARENT);
            return (INT_PTR)hButtonBrush;
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
        CW_USEDEFAULT, CW_USEDEFAULT, 1100, 700, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // HWND hEdit = CreateWindowEx(0, "EDIT", "",
    //     WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
    //     0, toolbarWidth, 400, 500, hWnd, NULL, hInstance, NULL);
    // HFONT hToolbarFont = CreateFont(
    //     18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
    //     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    //     CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Roboto");
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
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
