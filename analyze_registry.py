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

file_path = 'logs/registry_scan6.log'
rows = []
max_lines = 10000

def read_csv(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        for i, line in enumerate(f):
            if i >= max_lines:
                break
            row = line.strip().split('||')
            rows.append(row)

    df = pd.DataFrame(rows[1:], columns=rows[0])
    return df

# , columns=[
    # 'root', 'subkey', 'count', 'subsubkey', 'subsubkey_count',
    # 'subsubsubkey', 'subsubsubkey_count', 'subsubsubsubkey', 'subsubsubsubkey_count'
# ]
df = read_csv(file_path)
print(df.head())

# df.groupby(['subkey']).agg('count')
df_groups = df.groupby('subkey').size().reset_index().sort_values(by=0, ascending=False)
print(df_groups)
print(df_groups.to_csv('subkey_groups.csv', index=True))


# %%
