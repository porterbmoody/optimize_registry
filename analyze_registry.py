import pandas as pd
import csv


# file = '''logs/registry_scan6.log'''
# with open(file, newline='', encoding='utf-8') as csvfile:
    # reader = csv.reader(csvfile)
    # for row in reader:
        # print(row)
# file = 'logs/registry_scan6.log'

# with open(file, 'r', encoding='utf-8') as f:
    # for line in f:
    #     row = line.strip().split('||')
        # print(row)

# data = pd.read_csv(file)
# print(data)
# df = pd.read_csv(
    # file,
    # sep='\|\|',
    # engine='python'
# )

# print(df)
#%%
import pandas as pd

file = 'logs/registry_scan6.log'
rows = []
max_lines = 1000

with open(file, 'r', encoding='utf-8') as f:
    for i, line in enumerate(f):
        if i >= max_lines:
            break
        row = line.strip().split('||')
        print(row)
        # row[2] = int(row[2])
        # row[4] = int(row[4])
        # row[6] = int(row[6])
        # row[8] = int(row[8])
        rows.append(row)

df = pd.DataFrame(rows)

# , columns=[
    # 'root', 'subkey', 'count', 'subsubkey', 'subsubkey_count',
    # 'subsubsubkey', 'subsubsubkey_count', 'subsubsubsubkey', 'subsubsubsubkey_count'
# ]

print(df)

# %%
