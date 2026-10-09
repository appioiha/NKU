import pickle
import numpy as np
import torch
from torch.utils.data import Dataset
import os
os.environ['HF_ENDPOINT'] = 'https://hf-mirror.com'
os.environ['HF_HUB_DISABLE_SYMLINKS_WARNING'] = '1'

from transformers import (
    BertTokenizer,
    BertForSequenceClassification,
    Trainer,
    TrainingArguments
)
from sklearn.metrics import accuracy_score, f1_score

# ========== 0. 固定随机种子 ==========
import random
random.seed(42)
np.random.seed(42)
torch.manual_seed(42)

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

# ========== 2. 标签转数字 ==========
label_set = sorted(set(train_labels))
label2id = {label: i for i, label in enumerate(label_set)}
id2label = {i: label for label, i in label2id.items()}

train_labels_int = [label2id[l] for l in train_labels]
val_labels_int = [label2id[l] for l in val_labels]
test_labels_int = [label2id[l] for l in test_labels]

num_labels = len(label_set)
print("类别数:", num_labels)

# ========== 3. 加载 tokenizer ==========
tokenizer = BertTokenizer.from_pretrained('bert-base-uncased')

MAX_LEN = 64

train_encodings = tokenizer(
    train_texts,
    truncation=True,
    padding='max_length',
    max_length=MAX_LEN
)
val_encodings = tokenizer(
    val_texts,
    truncation=True,
    padding='max_length',
    max_length=MAX_LEN
)
test_encodings = tokenizer(
    test_texts,
    truncation=True,
    padding='max_length',
    max_length=MAX_LEN
)

# ========== 4. 构造 Dataset ==========
class NewsDataset(Dataset):
    def __init__(self, encodings, labels):
        self.encodings = encodings
        self.labels = labels

    def __getitem__(self, idx):
        item = {k: torch.tensor(v[idx]) for k, v in self.encodings.items()}
        item['labels'] = torch.tensor(self.labels[idx])
        return item

    def __len__(self):
        return len(self.labels)

train_dataset = NewsDataset(train_encodings, train_labels_int)
val_dataset = NewsDataset(val_encodings, val_labels_int)
test_dataset = NewsDataset(test_encodings, test_labels_int)

# ========== 5. 加载模型 ==========
model = BertForSequenceClassification.from_pretrained(
    'bert-base-uncased',
    num_labels=num_labels
)

# ========== 6. 评价函数 ==========
def compute_metrics(eval_pred):
    logits, labels = eval_pred
    preds = np.argmax(logits, axis=-1)
    return {
        'accuracy': accuracy_score(labels, preds),
        'macro_f1': f1_score(labels, preds, average='macro')
    }

# ========== 7. 训练参数（CPU 友好） ==========
training_args = TrainingArguments(
    output_dir='./bert_nyt',
    num_train_epochs=3,
    per_device_train_batch_size=16,     # CPU 上 16 比较稳
    per_device_eval_batch_size=32,
    eval_strategy='epoch',
    save_strategy='epoch',
    logging_steps=100,
    learning_rate=2e-5,
    load_best_model_at_end=True,
    metric_for_best_model='macro_f1',
    save_total_limit=1,
    report_to='none',                  # 不接 wandb
    dataloader_num_workers=0,          # Windows 上建议 0
    seed=42
)

trainer = Trainer(
    model=model,
    args=training_args,
    train_dataset=train_dataset,
    eval_dataset=val_dataset,
    compute_metrics=compute_metrics
)

# ========== 8. 训练 ==========
print("\n开始训练 BERT ...")
trainer.train()

# ========== 9. 测试集评价 ==========
print("\n在 Test Set 上评价 ...")
pred_output = trainer.predict(test_dataset)
preds = np.argmax(pred_output.predictions, axis=-1)

acc = accuracy_score(test_labels_int, preds)
macro_f1 = f1_score(test_labels_int, preds, average='macro')

print(f"\nBERT Test Accuracy: {acc:.4f}")
print(f"BERT Test Macro-F1: {macro_f1:.4f}")

# ========== 10. 保存结果 ==========
import pandas as pd

df = pd.DataFrame([{
    'method': 'BERT-base-uncased',
    'test_acc': acc,
    'test_f1': macro_f1
}])
df.to_csv('task3_results.csv', index=False)
print("\n已保存 task3_results.csv")