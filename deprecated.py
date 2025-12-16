                # print(subkey_name)
                # print(f"    └─ Contains {second_level_count} subkeys")
            # print(f"\n{'='*60}")
            # print(f"Processing {hive_name}")
            # print(f"{'='*60}")
            # print(f"First-level subkeys: {number_of_subkeys}\n")
                # try:
                    # print(f"[{key_number}] {subkey_name}")
                    # results.append({
                        # 'Hive': hive_name,
                        # 'First_Level_Key': subkey_name,
                        # 'Second_Level_Count': second_level_count
                    # })
                    # subkey.Close()
                    # print(f"    └─ <Permission Denied>")
                    # results.append({
                        # 'Hive': hive_name,
                        # 'First_Level_Key': subkey_name,
                        # 'Second_Level_Count': '<Permission Denied>'
                    # })
                    # results.append({
                        # 'Hive': hive_name,
                        # 'First_Level_Key': subkey_name if 'subkey_name' in locals() else '<Unknown>',
                        # 'Second_Level_Count': f'<Error>'
                    # })
        # return pd.DataFrame(results)


import winreg
import json
import base64

# def convert_value(value, value_type):
#     """Convert registry values into JSON-serializable types."""
#     if value_type == winreg.REG_BINARY:
#         # Convert bytes to base64 string (safe for JSON)
#         return base64.b64encode(value).decode('utf-8')
#     elif value_type == winreg.REG_MULTI_SZ:
#         return list(value)
#     elif value_type in (winreg.REG_SZ, winreg.REG_EXPAND_SZ):
#         return str(value)
#     else:
#         return value

# def read_registry_key(key, path=""):
#     """Recursively read registry key and convert to JSON-friendly dict."""
#     result = {}
#     try:
#         registry_key = winreg.OpenKey(key, path)
#         # read subkeys
#         i = 0
#         while True:
#             try:
#                 subkey_name = winreg.EnumKey(registry_key, i)
#                 result[subkey_name] = read_registry_key(registry_key, subkey_name)
#                 i += 1
#             except OSError:
#                 break
#         # read values
#         j = 0
#         while True:
#             try:
#                 value_name, value_data, value_type = winreg.EnumValue(registry_key, j)
#                 result[value_name] = convert_value(value_data, value_type)
#                 j += 1
#             except OSError:
#                 break
#         winreg.CloseKey(registry_key)
#     except PermissionError:
#         result["ERROR"] = "Access denied"
#     except Exception as e:
#         result["ERROR"] = str(e)
#     return result

# # Example: export HKEY_CURRENT_USER
# registry_json = read_registry_key(winreg.HKEY_CURRENT_USER)

# with open("registry.json", "w", encoding="utf-8") as f:
#     json.dump(registry_json, f, indent=4, ensure_ascii=False)

# Connect to the local machine's HKEY_CURRENT_USER hive


hive = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)
def get_main_subkeys(hive):
    """List the top-level subkeys of a registry hive."""
    subkeys = []
    try:
        i = 0
        while True:
            try:
                subkey_name = winreg.EnumKey(hive, i)
                subkeys.append(subkey_name)
                i += 1
            except OSError:
                break
    except PermissionError:
        print("Access denied to this hive.")
    return subkeys

main_folders = get_main_subkeys(hive)
#%%
import winreg
import socket

def get_subkeys(hive_handle):
    subkeys = []
    with winreg.OpenKey(hive_handle, "") as root:
        i = 0
        while True:
            try:
                subkeys.append(winreg.EnumKey(root, i))
                i += 1
            except OSError:
                break
    return subkeys

hkeys = [
	"LOCAL_MACHINE",
	"USERS",
	"CURRENT_USER",
	"CLASSES_ROOT",
	"CURRENT_CONFIG"
]

for hkey in hkeys:
    print(getattr(winreg, f"HKEY_{hkey}"))

hives = {
	hkey: winreg.ConnectRegistry(str(socket.gethostname()), getattr(winreg, f"HKEY_{hkey}"))
	for hkey in hkeys
}

for hkey in hkeys:
    print(f'{hkey} => {get_subkeys(hives[hkey])}')


#%%

import winreg
import socket

computer = f"\\\\{socket.gethostname()}"  # UNC format
print("Computer:", computer)

# Root hives
hkey_names = [
    "LOCAL_MACHINE",
    "USERS",
    "CURRENT_USER",
    "CLASSES_ROOT",
    "CURRENT_CONFIG"
]
hives = {
    name: winreg.ConnectRegistry(computer, getattr(winreg, f"HKEY_{name}"))
    for name in hkey_names
}
print("Hive handles:", hives)

# Function to get top-level subkeys of a hive
def get_subkeys(hive_handle):
    subkeys = []
    with winreg.OpenKey(hive_handle, "") as root:
        i = 0
        while True:
            try:
                subkeys.append(winreg.EnumKey(root, i))
                i += 1
            except OSError:
                break
    return subkeys

# Example: list subkeys under HKEY_LOCAL_MACHINE
lm_hive = hives["LOCAL_MACHINE"]
top_subkeys = get_subkeys(lm_hive)
print("\nTop-level subkeys under HKEY_LOCAL_MACHINE:")
for key in top_subkeys:
    print(" ", key)


# hives = [winreg.ConnectRegistry(None, hkey) for hkey in hkeys]
# print(get_main_subkeys(hive2))

#%%
winreg.CloseKey(hive_handle)

sys.stdout = open("registry_dump.txt", "w", encoding="utf-8")
# print_registry(hive_handle)
sys.stdout.close()



#%%

import winreg
import socket
import pandas as pd
import matplotlib.pyplot as plt

class Registry:

    def __init__(self):
        self.data = {
            'hkeys' : [name for name in dir(winreg) if name.startswith('HKEY_')],
            'computer' : r"{}".format(socket.gethostname()),
            }
        self.open_hives()
        self.count_first_layer()
        # print(self.data)

    @staticmethod
    def get_subkeys(hive_handle):
        subkeys = []
        try:
            with winreg.OpenKey(hive_handle, "") as root:
                i = 0
                while True:
                    try:
                        subkeys.append(winreg.EnumKey(root, i))
                        i += 1
                    except OSError:
                        break
        except PermissionError:
            subkeys.append("<Permission Denied>")
        return subkeys

    def open_hives(self):
        self.hives = []
        for hkey in self.data['hkeys']:
            # print(hkey)
            if hasattr(winreg, hkey):
                # print(getattr(winreg, f"{hkey}"))
                hkey_attribute = getattr(winreg, f"{hkey}")
                hive = winreg.ConnectRegistry(self.data['computer'], hkey_attribute)
                self.hives.append(hive)
            else:
                print(f"{hkey} not found in winreg")
        print(self.hives)

    @staticmethod
    def count_subkeys(key):
        i = 0
        while True:
            try:
                winreg.EnumKey(key, i)
                i += 1
            except OSError:
                break
        return i
    
    def count_first_layer(self):
        for n, hive in enumerate(self.hives):
            number_of_subkeys = self.count_subkeys(self.hives[n])
            print(f"""{self.data['hkeys'][n]}\n{number_of_subkeys}""")
        data = pd.DataFrame({'folder':[self.data['hkeys']], 'number':number_of_subkeys})
        data.plot()
        plt.show()

registry = Registry()

# %%
# registry.hives[0].OpenKey()
key = winreg.OpenKey(registry.hives[0], '')

i = 0
while True:
	try:
		subkey = winreg.EnumKey(key, i)
		print(subkey)
		i += 1
	except OSError:
		break


#%%

def enum_subkeys(key):
	i = 0
	result = []
	while True:
		try:
			result.append(winreg.EnumKey(key, i))
			i += 1
		except OSError:
			break
	return result

def enum_values(key):
	i = 0
	values = []
	while True:
		try:
			name, value, val_type = winreg.EnumValue(key, i)
			values.append((name, value, val_type))
			i += 1
		except OSError:
			break
	return values

def walk(key, path=""):
	subkeys = enum_subkeys(key)
	values = enum_values(key)

	print("PATH:", path or "(hive root)")
	if values:
		for name, val, _ in values:
			print("   ", name, "=", val)

	for sk in subkeys:
		try:
			with winreg.OpenKey(key, sk) as sub:
				walk(sub, f"{path}\\{sk}" if path else sk)
		except PermissionError:
			print("   SKIPPED (no access):", sk)

walk(key)

// fprintf(log_file, "%llu\n", (unsigned long long)hkey);

// void log_subkeys_recursive(FILE* log_file, HKEY hkey, const std::string& path, int depth, int max_depth) {
// void log_subkeys_recursive(FILE* log_file, HKEY hkey, int depth, int max_depth) {
    // if (depth > max_depth) return;
    // char subkey_name[256];
    // HKEY hkey;
    // bool has_subkeys = false;
    // for (DWORD index = 0; RegEnumKey(hkey, index, name, sizeof(subkey_name)) == ERROR_SUCCESS; index++) {
    //     has_subkeys = true;
    //     std::string full_path = path + "\\" + subkey_name;
    //     HKEY subkey;
    //     if (RegOpenKeyEx(hkey, subkey_name, 0, KEY_READ, &subkey) == ERROR_SUCCESS) {
    //         log_subkeys_recursive(log_file, subkey, full_path, depth + 1, max_depth);
    //         log_subkeys_recursive(log_file, subkey, depth + 1, max_depth);
    //         RegCloseKey(subkey);
        // }
    // }
    // std::cout << hkey << std::endl;
    // if (!has_subkeys || depth == max_depth) {
    //     fprintf(log_file, "%s\n", path.c_str());
    // }
    // RegCloseKey(hkey);
// }

// struct RootKey {
//     HKEY handle;
//     const char* name;
// };

// RootKey roots[] = {
//     {HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT"},
//     {HKEY_CURRENT_USER, "HKEY_CURRENT_USER"},
//     {HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE"},
//     {HKEY_USERS, "HKEY_USERS"},
//     {HKEY_PERFORMANCE_DATA, "HKEY_PERFORMANCE_DATA"},
//     {HKEY_PERFORMANCE_TEXT, "HKEY_PERFORMANCE_TEXT"},
//     {HKEY_PERFORMANCE_NLSTEXT, "HKEY_PERFORMANCE_NLSTEXT"},
//     {HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG"},
//     {HKEY_DYN_DATA, "HKEY_DYN_DATA"},
//     {HKEY_CURRENT_USER_LOCAL_SETTINGS, "HKEY_CURRENT_USER_LOCAL_SETTINGS"},
// };