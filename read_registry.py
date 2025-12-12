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
hive_handle = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)
print(hive_handle)

REG_PATH = r"Control Panel\Mouse"

def get_value():
    with winreg.OpenKey(hive_handle, REG_PATH, 0, winreg.KEY_READ) as registry_key:
        print(f"Successfully opened key: HKEY_CURRENT_USER\\{REG_PATH}")
        value, reg_type = winreg.QueryValueEx(registry_key, 'MouseSensitivity')
    winreg.CloseKey(hive_handle)
    return value, reg_type
print(get_value())

#%%



import winreg

# Connect to HKEY_CURRENT_USER
# hive_handle = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)

hive = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)
print(hive)
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
print("Main folders in HKEY_CURRENT_USER:")
for folder in main_folders:
    print(f"- {folder}")

winreg.CloseKey(hive)


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

# Connect to local computer
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

# Connect to all root hives
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

# with winreg.OpenKey(hive_handle, r"SOFTWARE\Microsoft\Windows\CurrentVersion") as key:
#     i = 0
#     while True:
#         try:
#             subkey_name = winreg.EnumKey(key, i)
#             print(subkey_name)
#             i += 1
#         except OSError:
#             break

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


#%%

import winreg
import socket
# import matplotlib as plt
import pandas as pd

class Registry:

    def __init__(self):
        self.data = {
            'hkeys': [name for name in dir(winreg) if name.startswith('HKEY_')],
            'computer': socket.gethostname(),
        }
        self.open_hives()
        self.results = self.count_first_layer()
        print(self.count_second_layer())
        # self.results.plot(x='Category', y='Value', kind='bar')
        # plt.title('Bar Chart Example')
        # plt.xlabel('Category')
        # plt.ylabel('Value')
        # plt.show()

    @staticmethod
    def get_subkeys(hive_handle):
        """Get list of subkey names from a registry key handle."""
        subkeys = []
        try:
            i = 0
            while True:
                try:
                    subkeys.append(winreg.EnumKey(hive_handle, i))
                    i += 1
                except OSError:
                    break
        except PermissionError:
            subkeys.append("<Permission Denied>")
        return subkeys
    
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

    @staticmethod
    def query_subkeys(hive_handle):
        """Count the number of subkeys in a registry key."""
        try:
            # Use QueryInfoKey for more efficient counting
            num_subkeys, num_values, last_modified = winreg.QueryInfoKey(hive_handle)
            print(winreg.QueryInfoKey(hive_handle))
            return num_subkeys
        except Exception as e:
            print(f"Error querying key info: {e}")
            return 0

    def open_hives(self):
        """Open connections to all registry hives."""
        self.hives = []
        for hkey_name in self.data['hkeys']:
            try:
                hkey_const = getattr(winreg, hkey_name)
                hive = winreg.ConnectRegistry(None, hkey_const)  # None = local computer
                self.hives.append((hkey_name, hive))
            except AttributeError:
                print(f"{hkey_name} not found in winreg")
            except Exception as e:
                print(f"Error connecting to {hkey_name}: {e}")

    def count_first_layer(self):
        """Count subkeys in the first layer of each hive."""
        results = []
        for hkey_name, hive in self.hives:
            try:
                number_of_subkeys = self.count_subkeys(hive)
                subkey_names = self.get_subkeys(hive)
                results.append({
                    'Hive': hkey_name,
                    'Subkey Count': number_of_subkeys,
                    'Subkeys': subkey_names[:10]
                })
                # print(f"{hkey_name}: {number_of_subkeys} subkeys")
            except Exception as e:
                print(f"Error processing {hkey_name}: {e}")
                results.append({
                    'Hive': hkey_name,
                    'Subkey Count': 0,
                    'Subkeys': []
                })
        
        return pd.DataFrame(results)

    def count_second_layer(self):
        for hive in self.hives:
            number_of_subkeys = self.count_subkeys(hive)
            for sub_key in number_of_subkeys:
                subkey = winreg.OpenKey(hive, )
            print(hive)
            print(number_of_subkeys)
        return number_of_subkeys

    def close_hives(self):
        """Close all open registry hive connections."""
        for hkey_name, hive in self.hives:
            try:
                hive.Close()
            except Exception as e:
                print(f"Error closing {hkey_name}: {e}")
    
    def __del__(self):
        """Cleanup when object is destroyed."""
        self.close_hives()

    def get_summary(self):
        """Return a DataFrame summary of the registry analysis."""
        return self.results

if __name__ == "__main__":
    registry = Registry()
    # print("\nSummary:")
    # print(registry.get_summary())

winreg.EnumKey(registry.hives[0][1], 1)


# %%
key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "Software")

key
winreg.QueryInfoKey(key)
