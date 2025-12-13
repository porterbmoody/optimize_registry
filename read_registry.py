
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


#%%

import winreg
import socket
import matplotlib as plt
import matplotlib.style
import pandas as pd
import logging

matplotlib.style.use('dark_background')

logging.basicConfig(
	filename="registry_scan.log",
	level=logging.INFO,
	format="%(asctime)s | %(levelname)s | %(message)s"
)

logger = logging.getLogger(__name__)

class Registry:

    def __init__(self):
        self.data = {
            'hkeys': [name for name in dir(winreg) if name.startswith('HKEY_')],
            'computer': socket.gethostname(),
        }
        self.open_hives()
        self.layer1()
        self.layer2()
        print(self.first_layer)

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

    def layer1(self):
        """Count subkeys in the first layer of each hive."""
        self.results = []
        for hkey_name, hive in self.hives:
            try:
                number_of_subkeys = self.count_subkeys(hive)
                subkey_names = self.get_subkeys(hive)
                self.results.append({
                    'hive': hkey_name,
                    'subkey_count': number_of_subkeys,
                    'subkeys': subkey_names[:10]
                })
                print(f"{hkey_name}: {number_of_subkeys} subkeys")
            except Exception as e:
                print(f"Error processing {hkey_name}: {e}")
                self.results.append({
                    'hive': hkey_name,
                    'subkey_count': 0,
                    'subkeys': []
                })
        self.first_layer = pd.DataFrame(self.results)
        self.first_layer.plot(x='hive', y='subkey_count', kind='bar', title='subkey counts', xlabel='hive',ylabel='count')

    def layer2(self):
        """Count keys within each first-level subkey."""
        for hive_name, hive in self.hives:
            number_of_subkeys = self.count_subkeys(hive)
            logger.info(f"HIVE: {hive_name} | Subkeys: {number_of_subkeys}")
            for key_number in range(number_of_subkeys):
                try:
                    subkey_name = winreg.EnumKey(hive, key_number)
                    subkey = winreg.OpenKey(hive, subkey_name)
                    second_level_count = self.count_subkeys(subkey)
                    logger.info(f"{hive_name}\\{subkey_name} = {second_level_count}")
                    # logger.info(f" └─ Contains {second_level_count} subkeys")
                except PermissionError:
                    logger.warning(f"{hive_name}\\{subkey_name} | Permission denied")
                except Exception as e:
                    logger.error(f"{hive_name}\\{subkey_name} | Error: {e}")

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

#%%

# winreg.EnumKey(registry.hives[0], 0)
# registry.count_subkeys(registry.hives[0][1])
winreg.EnumKey(registry.hives[0][1], 1)

# %%
key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "Software")

key
winreg.QueryInfoKey(key)
