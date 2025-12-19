@echo off
C:/msys64/mingw64/bin/g++.exe text_editor.cpp -o text_editor.exe -mwindows -lgdi32 -lcomdlg32

if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    pause
    exit /b 1
)

echo Compilation successful!
text_editor.exe