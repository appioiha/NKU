import pandas as pd

nyt = pd.read_csv('data/nyt.csv')
ag = pd.read_csv('data/ag.csv')

print("NYT shape:", nyt.shape)
print("NYT columns:", nyt.columns.tolist())
print(nyt.head())

print("AG shape:", ag.shape)
print("AG columns:", ag.columns.tolist())
print(ag.head())
