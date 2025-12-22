@echo on
windres resources.rc resources.o
C:\msys64\mingw64\bin\c++.exe log_registry.cpp -o log_registry.exe
log_registry.exe
