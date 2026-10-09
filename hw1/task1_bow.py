import pickle
import pandas as pd
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, f1_score

# ========== 1. 加载数据划分 ==========
with open('split.pkl', 'rb') as f:
    split = pickle.load(f)

train_texts = split['train_texts']
train_labels = split['train_labels']
val_texts = split['val_texts']
val_labels = split['val_labels']
test_texts = split['test_texts']
test_labels = split['test_labels']

print("Train:", len(train_texts), "Val:", len(val_texts), "Test:", len(test_texts))

# ========== 2. 定义分词器 ==========
def tokenizer(text):
    return text.lower().split()

# ========== 3. 通用训练 + 评价函数 ==========
def run_experiment(name, vectorizer):
    print(f"\n===== {name} =====")

    X_train = vectorizer.fit_transform(train_texts)
    X_val = vectorizer.transform(val_texts)
    X_test = vectorizer.transform(test_texts)

    print("Vocabulary size:", len(vectorizer.vocabulary_))

    clf = LogisticRegression(max_iter=1000)
    clf.fit(X_train, train_labels)

    # 验证集
    val_pred = clf.predict(X_val)
    val_acc = accuracy_score(val_labels, val_pred)
    val_f1 = f1_score(val_labels, val_pred, average='macro')

    # 测试集
    test_pred = clf.predict(X_test)
    test_acc = accuracy_score(test_labels, test_pred)
    test_f1 = f1_score(test_labels, test_pred, average='macro')

    print(f"Val  Accuracy: {val_acc:.4f}  Macro-F1: {val_f1:.4f}")
    print(f"Test Accuracy: {test_acc:.4f}  Macro-F1: {test_f1:.4f}")

    return {
        'method': name,
        'val_acc': val_acc,
        'val_f1': val_f1,
        'test_acc': test_acc,
        'test_f1': test_f1
    }

# ========== 4. 两种 Bag-of-Words ==========
results = []

# 4.1 Binary Bag of Words
vec_binary = CountVectorizer(
    tokenizer=tokenizer,
    binary=True,
    token_pattern=None
)
results.append(run_experiment("Binary BoW", vec_binary))

# 4.2 Word Frequency
vec_freq = CountVectorizer(
    tokenizer=tokenizer,
    binary=False,
    token_pattern=None
)
results.append(run_experiment("Word Frequency", vec_freq))

# ========== 5. 汇总结果 ==========
print("\n\n========== Task 1 结果汇总 ==========")
df = pd.DataFrame(results)
print(df.to_string(index=False))

df.to_csv('task1_results.csv', index=False)
print("\n已保存 task1_results.csv")