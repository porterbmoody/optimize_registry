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