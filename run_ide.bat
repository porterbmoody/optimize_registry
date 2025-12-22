@echo on
echo compiling text_editor.cpp
windres resources.rc resources.o
C:/msys64/mingw64/bin/c++.exe text_editor.cpp resources.o -o text_editor.exe -mwindows
text_editor.exe
@REM windres resources.rc resources.res
@REM C:/msys64/mingw64/bin/c++.exe text_editor.cpp resources.res -o text_editor.exe -mwindows -lgdi32 -luser32 -lcomdlg32


echo running text_editor.exe