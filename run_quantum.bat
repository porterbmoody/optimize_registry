@echo on
echo compiling quantum.cpp
windres resources.rc resources.o
C:/msys64/mingw64/bin/c++.exe quantum.cpp resources.o -o quantum.exe -mwindows
quantum.exe
@REM windres resources.rc resources.res
@REM C:/msys64/mingw64/bin/c++.exe text_editor.cpp resources.res -o text_editor.exe -mwindows -lgdi32 -luser32 -lcomdlg32

echo running quantum.exe