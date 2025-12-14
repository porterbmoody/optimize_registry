import pandas as pd
import csv
import matplotlib
import matplotlib.style

matplotlib.style.use('dark_background')

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
max_lines = 100000

def load_data(file_path, group_columns):
    with open(file_path, 'r', encoding='utf-8') as f:
        for i, line in enumerate(f):
            # if i >= max_lines:
                # break
            row = line.strip().split('||')
            rows.append(row)
    df = pd.DataFrame(rows[1:], columns=rows[0])
    # df = df[df['root'] == 'HKEY_LOCAL_MACHINE']
    df = df.groupby(group_columns).size().reset_index().sort_values(by=0, ascending=False)
    print(df)
    df.columns = ['root', 'subkey', 'subsubkey', 'count']
    df.to_csv('data/subkey_groups.csv', index=False)
    return df

def visualize(df):
    count_min = 5
    print(f"""rows=={len(df)}""")
    df = df[df['count'] > count_min]
    print(f"""rows=={len(df)}""")
    ax = df.plot(x='subsubkey', y='count', kind='bar', title=f'distribution of HKEY_CLASSES_ROOT subkeys (count > {count_min})', xlabel='hive',ylabel='count', figsize=(20, 12))
    fig = ax.get_figure()
    fig.savefig("subkey_groups.png")

group_columns = ['root', 'subkey', 'subsubkey']
df = load_data(file_path, group_columns)
visualize(df)
# print(df.head())


# %%
# self.first_layer = pd.DataFrame(self.results)

