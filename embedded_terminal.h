#pragma once
#include <windows.h>
#include <string>

class embedded_terminal
{
public:
    embedded_terminal(HWND parent, int x, int y, int width, int height);
    ~embedded_terminal();

    void execute_command(const std::string& exePath);
    void append_output(const std::string& text);

private:
    HWND hTerminal;
    HANDLE hReadPipe;
    HANDLE hWritePipe;
    HBRUSH hBlackBrush = CreateSolidBrush(RGB(0,0,0));

    void createPipes();
    void read_output(HANDLE hProcess);
};
