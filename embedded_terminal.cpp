#include "embedded_terminal.h"
#include <iostream>

embedded_terminal::embedded_terminal(HWND parent, int x, int y, int width, int height)
{
    hTerminal = CreateWindowExA(
        WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        x, y, width, height, parent, (HMENU)200, GetModuleHandle(NULL), NULL
    );
}

void embedded_terminal::append_output(const std::string& text)
{
    int len = GetWindowTextLengthA(hTerminal);
    SendMessageA(hTerminal, EM_SETSEL, len, len);
    SendMessageA(hTerminal, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
    SendMessageA(hTerminal, EM_SCROLLCARET, 0, 0);
}

void embedded_terminal::createPipes()
{
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
        MessageBoxA(NULL, "Failed to create pipe", "Error", MB_OK);

    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
}

void embedded_terminal::read_output(HANDLE hProcess)
{
    char buffer[4096];
    DWORD read;

    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
    {
        buffer[read] = 0;
        append_output(buffer);
    }

    CloseHandle(hReadPipe);
    WaitForSingleObject(hProcess, INFINITE);
    CloseHandle(hProcess);
}

void embedded_terminal::execute_command(const std::string& exePath)
{
    createPipes();

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi{};
    if (!CreateProcessA(NULL, (LPSTR)exePath.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW,
                        NULL, NULL, &si, &pi))
    {
        MessageBoxA(NULL, "Failed to start process", "Error", MB_OK);
        return;
    }

    CloseHandle(hWritePipe);
    read_output(pi.hProcess);
}
