#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

struct RootKey {
    HKEY handle;
    const char* name;
};

RootKey roots[] = {
    {HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT"},
    {HKEY_CURRENT_USER, "HKEY_CURRENT_USER"},
    {HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE"},
    {HKEY_USERS, "HKEY_USERS"},
    {HKEY_PERFORMANCE_DATA, "HKEY_PERFORMANCE_DATA"},
    {HKEY_PERFORMANCE_TEXT, "HKEY_PERFORMANCE_TEXT"},
    {HKEY_PERFORMANCE_NLSTEXT, "HKEY_PERFORMANCE_NLSTEXT"},
    {HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG"},
    {HKEY_DYN_DATA, "HKEY_DYN_DATA"},
    {HKEY_CURRENT_USER_LOCAL_SETTINGS, "HKEY_CURRENT_USER_LOCAL_SETTINGS"},
};

template<typename T>
void print(const T& value) {
    std::cout << value << std::endl;
}

void log_subkeys(FILE* log_file, HKEY root_key, const char* root_name) {
    char subkey_name[256];
    HKEY hkey;
    if (RegOpenKeyEx(root_key, "", 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
        for (DWORD index = 0; RegEnumKey(hkey, index, subkey_name, sizeof(subkey_name)) == ERROR_SUCCESS; index++) {
            std::cout << root_name << " " << subkey_name << std::endl;
            fprintf(log_file, "%s,%s\n", root_name, subkey_name);
        }
        RegCloseKey(hkey);
    }
}

int main() {
    DWORD dataSize = 1024;
    const char* subkey = "Software\\MyApp";
    const char* valueName = "Version";
    const char* headers = "root,subkey,subkey_count\n";
    const char* log_path = "logs/log.txt";
    FILE* log_file = fopen(log_path, "w");
    if (!log_file) return 1;
    fprintf(log_file, headers);
    for (const auto& root : roots) {
        log_subkeys(log_file, root.handle, root.name);
    }
    fclose(log_file);
	return 0;
}

// g++ access_registry.cpp -o access_registry.exe