#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <chrono>
#include <regex>
#include <string>

std::vector<HKEY> get_hkeys() {
    std::cout << "C:/msys64/mingw64/include/winreg.h" << std::endl;
    std::ifstream file("C:/msys64/mingw64/include/winreg.h");
    if (!file) {
        std::cerr << "Failed to open winreg.h\n";
        return {};
    }
    std::regex re(R"(#define\s+(HKEY_[A-Z_]+)\s+\(\(HKEY\)\s+\(ULONG_PTR\)\(\(LONG\)(0x[0-9A-Fa-f]+)\)\))");
    std::string line;
    std::vector<HKEY> hkeys = {};
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, re)) {
            std::string hkey_name = match[1].str();
            std::string hex_key = match[2].str();
            ULONG_PTR key_num;
            std::stringstream ss;
            ss << std::hex << hex_key;
            ss >> key_num;
            HKEY hkey = (HKEY)key_num;
            hkeys.push_back(hkey);
            // std::cout << hkey << std::endl;
        }
    }
    return hkeys;
}

template<typename T>
void print(const T& value) {
    std::cout << value << std::endl;
}

void log_subkeys_recursive(FILE* log_file, HKEY hkey, std::string path, int depth, int max_depth) {
    if (depth > max_depth) return;
    char name[256];
    bool has_subkeys = false;
    for (DWORD index = 0; RegEnumKey(hkey, index, name, sizeof(name)) == ERROR_SUCCESS; index++) {
        has_subkeys = true;
        HKEY subkey;
        std::string full_path = path + "\\" + name;

        if (RegOpenKeyEx(hkey, name, 0, KEY_READ, &subkey) == ERROR_SUCCESS) {
            std::cout << full_path << "," <<subkey << std::endl;
            // std::cout << subkey << std::endl;
            log_subkeys_recursive(log_file, subkey, full_path, depth + 1, max_depth);
            RegCloseKey(subkey);
        }
    }
    if (!has_subkeys || depth == max_depth) {
        fprintf(log_file, "%s,%p\n", path.c_str(), (void*)hkey);
    }
}

#include <sstream>

std::string hkey_to_string(HKEY hkey) {
    std::ostringstream oss;
    oss << "0x" << std::hex << reinterpret_cast<ULONG_PTR>(hkey);
    return oss.str();
}

int main() {
    const char* headers = "root,subkey,subsubkey,subsubsubkey\n";
    const char* log_path = "logs/log2txt";
    FILE* log_file = fopen(log_path, "w");
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
        return 1;
    }
    fprintf(log_file, headers);
    auto start = std::chrono::high_resolution_clock::now();
    auto hkeys = get_hkeys();
    // const std::string path;
    // std::string empty_str;
    // print(hkey);
    for (const auto& hkey : hkeys) {
        std::string path = hkey_to_string(hkey);  // use the handle as the starting path
        log_subkeys_recursive(log_file, hkey, path, 0, 1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "\n\nRuntime: " << duration.count() << " seconds" << std::endl;
    fclose(log_file);
    return 0;
}

// g++ access_registry.cpp -o access_registry.exe