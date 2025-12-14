
#%%

import winreg
import socket
import matplotlib as plt
import matplotlib.style
import pandas as pd

matplotlib.style.use('dark_background')

class Registry:

    def __init__(self):
        self.data = {
            'hkeys': [name for name in dir(winreg) if name.startswith('HKEY_')],
            'computer': socket.gethostname(),
        }
        self.log_path = 'logs/registry_scan5.log'
        self.log_file = open(self.log_path, "w", encoding="utf-8")
        self.log_file.write("root,subkey,count,subsubkey,subsubkey_count,subsubsubkey\n")
        # self.log_file = open("registry_scan1.log", "w", encoding="utf-8")
        # self.log_file.write("root,subkey,count\n")
        self.open_hives()
        self.layer1()
        # self.layer2()
        self.layer3()
        # print(self.first_layer)

    @staticmethod
    def get_subkeys(hive_handle):
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
        try:
            num_subkeys, num_values, last_modified = winreg.QueryInfoKey(hive_handle)
            return num_subkeys
        except Exception as e:
            return 0

    def visualize(self):
        self.first_layer = pd.DataFrame(self.results)
        ax = self.first_layer.plot(x='hive', y='subkey_count', kind='bar', title='subkey counts', xlabel='hive',ylabel='count', figsize=(10, 6))
        fig = ax.get_figure()
        fig.savefig("subkey_counts.png")

    def open_hives(self):
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
        self.visualize()

    def layer2(self):
        for hive_name, hive in self.hives:
            # self.log_file.write(f"HIVE: {hive_name}")
            number_of_subkeys = self.count_subkeys(hive)
            for key_number in range(number_of_subkeys):
                try:
                    subkey_name = winreg.EnumKey(hive, key_number)
                    subkey = winreg.OpenKey(hive, subkey_name)
                    second_level_count = self.count_subkeys(subkey)
                    self.log_file.write(f"{hive_name},{subkey_name},{second_level_count}\n")
                except PermissionError:
                    self.log_file.write(f"{key_number},Permission denied\n")
                except Exception as e:
                    self.log_file.write(f"{key_number},Permission denied\n")

    def layer3(self):
        for hive in self.hives:
            number_of_subkeys = self.count_subkeys(hive[1])
            print(f'''hive=={hive[0]}\nnumber_of_subkeys=={number_of_subkeys}''')
            for key_number in range(number_of_subkeys):
                try:
                    subkey_name = winreg.EnumKey(hive[1], key_number)
                    subkey = winreg.OpenKey(hive[1], subkey_name)
                    subkey_count = self.count_subkeys(subkey)
                    for subkey_number in range(subkey_count):
                        try:
                            subsubkey_name = winreg.EnumKey(subkey, subkey_number)
                            subsubkey = winreg.OpenKey(subkey, subsubkey_name)
                            subsubkey_count = self.count_subkeys(subsubkey)
                            print(f"{hive[0]},{subkey_name},{subkey_number},{subsubkey_name},{subsubkey_count}")
                            # self.log_file.write(f"{hive[0]},{subkey_name},{subkey_number},{subsubkey_name},{subsubkey_count}\n")
                            for subsubkey_number in range(subsubkey_count):
                                subsubsubkey_name = winreg.EnumKey(subsubkey, subsubkey_number)
                                subsubsubkey = winreg.OpenKey(subsubkey, subsubsubkey_name)
                                subsubsubkey_count = self.count_subkeys(subsubsubkey)
                                # print(f"""{hive[0]} subkey_name=={subkey_name} subsubkey_name=={subsubkey_name} subsubsubkey_name=={subsubsubkey_name} subsubsubkey_count=={subsubsubkey_count}""")
                                self.log_file.write(f"""{hive[0]},{subkey_name},{subsubkey_name},{subsubsubkey_name},{subsubsubkey_count}\n""")
                        except Exception as error:
                            self.log_file.write(f"hive=={hive[0]} error=={error}\n")
                except Exception as error:
                    self.log_file.write(f"""key_number=={key_number} error=={error}\n""")

    def close_hives(self):
        for hkey_name, hive in self.hives:
            try:
                hive.Close()
            except Exception as e:
                print(f"Error closing {hkey_name}: {e}")

    def __del__(self):
        self.close_hives()

    def get_summary(self):
        return self.results

if __name__ == "__main__":
    registry = Registry()

#%%

# winreg.EnumKey(registry.hives[0], 0)
# registry.count_subkeys(registry.hives[0][1])
winreg.EnumKey(registry.hives[0][1], 1)


# %%
key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "Software")

winreg.QueryInfoKey(key)
print(winreg.OpenKey(key, 'Ableton'))
print(key)

# git add .;git commit -m 'changes';git pull;git push;

#%%


import matplotlib.pyplot as plt

plt.plot([1, 2, 3], [4, 5, 6])
plt.savefig("my_plot.png")
plt.show()
