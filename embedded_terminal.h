#pragma once
#include <windows.h>
#include <string>

class embedded_terminal
{
public:
    embedded_terminal(HWND parent, int x, int y, int width, int height);
    void runProgram(const std::string& exePath);
    void appendOutput(const std::string& text);
private:
    HWND hTerminal;
    HANDLE hReadPipe;
    HANDLE hWritePipe;
    void createPipes();
    void readOutput(HANDLE hProcess);
};
