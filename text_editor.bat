@echo on
echo compiling text_editor.cpp
C:/msys64/mingw64/bin/windres.exe resources.rc -o resources.o
C:/msys64/mingw64/bin/c++.exe text_editor.cpp resources.o -o text_editor.exe -mwindows -lgdi32 -lcomdlg32
text_editor.exe

echo running text_editor.exe