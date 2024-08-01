#!/bin/bash

# 현재 경로와 dataset/ 경로 설정
current_path="."
dataset_path="./dataset"

# 현재 경로의 모든 디렉토리 탐색
for dir in "$current_path"/*; do
    if [ -d "$dir" ]; then
        dir_name=$(basename "$dir")

        # dataset/ 경로에 동일한 이름의 디렉토리가 있는지 확인
        if [ -d "$dataset_path/$dir_name" ]; then
            echo "Merging contents of $dir into $dataset_path/$dir_name"
            cp -r "$dir/"* "$dataset_path/$dir_name/"
            rm -r "$dir"
        fi
    fi
done

echo "Merging complete."

