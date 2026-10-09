import pandas as pd
import pickle
from sklearn.model_selection import train_test_split

# ========== 1. 读取 NYT ==========
nyt = pd.read_csv('data/nyt.csv')
print("NYT shape:", nyt.shape)

texts = nyt['text'].astype(str).tolist()
labels = nyt['label'].astype(str).tolist()

# ========== 2. 划分 train 80% / temp 20% ==========
train_texts, temp_texts, train_labels, temp_labels = train_test_split(
    texts, labels,
    test_size=0.2,
    random_state=42,
    shuffle=True,
    stratify=labels
)

# ========== 3. temp 再平分 val 10% / test 10% ==========
val_texts, test_texts, val_labels, test_labels = train_test_split(
    temp_texts, temp_labels,
    test_size=0.5,
    random_state=42,
    shuffle=True,
    stratify=temp_labels
)

print("Train:", len(train_texts))
print("Val  :", len(val_texts))
print("Test :", len(test_texts))

# ========== 4. 保存划分结果 ==========
with open('split.pkl', 'wb') as f:
    pickle.dump({
        'train_texts': train_texts,
        'train_labels': train_labels,
        'val_texts': val_texts,
        'val_labels': val_labels,
        'test_texts': test_texts,
        'test_labels': test_labels
    }, f)

print("已保存 split.pkl")