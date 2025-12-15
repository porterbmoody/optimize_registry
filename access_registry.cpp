#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <chrono>

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

void log_subkeys_recursive(FILE* log_file, HKEY parent_key, const std::string& path, int depth, int max_depth) {
    if (depth > max_depth) return;
    char subkey_name[256];
    HKEY hkey;
    if (RegOpenKeyEx(parent_key, "", 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
        return;
    }
    bool has_subkeys = false;
    for (DWORD index = 0; RegEnumKey(hkey, index, subkey_name, sizeof(subkey_name)) == ERROR_SUCCESS; index++) {
        has_subkeys = true;
        std::string full_path = path + "\\" + subkey_name;
        HKEY subkey;
        if (RegOpenKeyEx(hkey, subkey_name, 0, KEY_READ, &subkey) == ERROR_SUCCESS) {
            log_subkeys_recursive(log_file, subkey, full_path, depth + 1, max_depth);
            RegCloseKey(subkey);
        }
    }
    if (!has_subkeys || depth == max_depth) {
        fprintf(log_file, "%s\n", path.c_str());
    }
    RegCloseKey(hkey);
}


int main() {
    const char* headers = "root,subkey,subsubkey,subsubsubkey\n";
    const char* log_path = "logs/log.txt";
    FILE* log_file = fopen(log_path, "w");
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
        return 1;
    }
    fprintf(log_file, headers);
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& root : roots) {
        print(root.name);
        log_subkeys_recursive(log_file, root.handle, root.name, 0, 4);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "\n\nRuntime: " << duration.count() << " seconds" << std::endl;
    fclose(log_file);
    return 0;
}

// g++ access_registry.cpp -o access_registry.exe