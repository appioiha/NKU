import pickle
import numpy as np
import pandas as pd
from gensim.models import Word2Vec
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, f1_score

# ========== 0. 固定随机种子 ==========
import random
random.seed(42)
np.random.seed(42)

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

# ========== 2. 分词器 ==========
def tokenizer(text):
    return text.lower().split()

# ========== 3. 通用：文档向量 = 所有有效词向量平均 ==========
def document_vector(text, embedding_dict, dim=100):
    tokens = tokenizer(text)
    vectors = []
    for token in tokens:
        if token in embedding_dict:
            vectors.append(embedding_dict[token])
    if len(vectors) == 0:
        return np.zeros(dim)
    return np.mean(vectors, axis=0)

def build_document_vectors(texts, embedding_dict, dim=100):
    return np.array([document_vector(t, embedding_dict, dim) for t in texts])

# ========== 4. 通用：训练 + 评价 ==========
def run_experiment(name, embedding_dict, dim=100):
    print(f"\n===== {name} =====")

    X_train = build_document_vectors(train_texts, embedding_dict, dim)
    X_val = build_document_vectors(val_texts, embedding_dict, dim)
    X_test = build_document_vectors(test_texts, embedding_dict, dim)

    # 统计平均命中率（可选，方便报告里写）
    hit_count = 0
    total_count = 0
    for t in train_texts:
        for tok in tokenizer(t):
            total_count += 1
            if tok in embedding_dict:
                hit_count += 1
    print(f"Train token hit rate: {hit_count / max(total_count, 1):.4f}")

    clf = LogisticRegression(max_iter=1000)
    clf.fit(X_train, train_labels)

    val_pred = clf.predict(X_val)
    val_acc = accuracy_score(val_labels, val_pred)
    val_f1 = f1_score(val_labels, val_pred, average='macro')

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

# ========== 5. 加载 GloVe ==========
def load_glove(path, dim=100):
    emb = {}
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            parts = line.rstrip().split(' ')
            word = parts[0]
            vec = np.asarray(parts[1:], dtype='float32')
            if vec.shape[0] == dim:
                emb[word] = vec
    return emb

print("\n正在加载 GloVe ...")
glove_emb = load_glove('data/glove.6B.100d.txt', dim=100)
print("GloVe 词表大小:", len(glove_emb))

# ========== 6. 用 AG News 训练 Word2Vec ==========
print("\n正在读取 AG News ...")
ag = pd.read_csv('data/ag.csv')
ag_texts = ag['text'].astype(str).tolist()
print("AG 文本数:", len(ag_texts))

print("正在训练 Word2Vec on AG News ...")
ag_sentences = [tokenizer(t) for t in ag_texts]
ag_w2v = Word2Vec(
    sentences=ag_sentences,
    vector_size=100,
    window=5,
    min_count=1,
    workers=4,
    sg=1,
    epochs=10,
    seed=42
)
ag_emb = {w: ag_w2v.wv[w] for w in ag_w2v.wv.index_to_key}
print("AG Word2Vec 词表大小:", len(ag_emb))

# ========== 7. 用 NYT 训练集训练 Word2Vec ==========
print("\n正在训练 Word2Vec on NYT (只用训练集) ...")
nyt_sentences = [tokenizer(t) for t in train_texts]
nyt_w2v = Word2Vec(
    sentences=nyt_sentences,
    vector_size=100,
    window=5,
    min_count=1,
    workers=4,
    sg=1,
    epochs=10,
    seed=42
)
nyt_emb = {w: nyt_w2v.wv[w] for w in nyt_w2v.wv.index_to_key}
print("NYT Word2Vec 词表大小:", len(nyt_emb))

# ========== 8. 三组实验 ==========
results = []

results.append(run_experiment("GloVe (pre-trained)", glove_emb))
results.append(run_experiment("Word2Vec on AG News", ag_emb))
results.append(run_experiment("Word2Vec on NYT", nyt_emb))

# ========== 9. 汇总 ==========
print("\n\n========== Task 2 结果汇总 ==========")
df = pd.DataFrame(results)
print(df.to_string(index=False))
df.to_csv('task2_results.csv', index=False)
print("\n已保存 task2_results.csv")