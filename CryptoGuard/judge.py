import os
import pandas as pd
import numpy as np
from sklearn.preprocessing import LabelEncoder
from tensorflow.keras.models import load_model

DATASET_DIR = "buf_1/"
WIDTH = 256
HEIGHT = 256
SHAPE = (WIDTH, HEIGHT)
SIZE = WIDTH * HEIGHT * 1

def csvToimage(file_path):
    data = pd.read_csv(file_path)
    data_encoded = LabelEncoder().fit_transform(data['SYSTEM_CALL'])
    image = np.zeros(SHAPE, dtype=np.uint8)
    syscall_nums = min(len(data_encoded), SIZE)
    image.flat[:syscall_nums] = data_encoded[:syscall_nums]
    return image

def load_data(dataset_dir):
    x = []
    y = []
    classes = [class_name for class_name in os.listdir(dataset_dir) if os.path.isdir(os.path.join(dataset_dir, class_name))]
    for class_idx, class_name in enumerate(classes):
        class_dir = os.path.join(dataset_dir, class_name)
        files = [(f, os.path.getsize(os.path.join(class_dir, f))) for f in os.listdir(class_dir) if f.endswith('.csv')]
        sorted_files = sorted(files, key=lambda x: x[1], reverse=True)
        for file_name, _ in sorted_files:
            file_path = os.path.join(class_dir, file_name)
            image = csvToimage(file_path)
            x.append(image)
            y.append(class_idx)
    x = np.array(x).reshape(-1, WIDTH, HEIGHT, 1)
    y = np.array(y)
    return x, y

X, y = load_data(DATASET_DIR)
X = X / 255.0

policy = load_model('policy.h5')

pred = policy.predict(X)

sentence = pd.DataFrame(pred)
sentence.to_csv('SENTENCE.csv', index=False)