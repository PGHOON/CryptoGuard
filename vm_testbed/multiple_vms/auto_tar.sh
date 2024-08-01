#!/bin/bash

# 압축할 최상위 디렉토리를 인자로 받습니다.
if [ -z "$1" ]; then
  echo "사용법: $0 <디렉토리 경로>"
  exit 1
fi

BASE_DIR="$1"

# 디렉토리가 존재하는지 확인합니다.
if [ ! -d "$BASE_DIR" ]; then
  echo "디렉토리 $BASE_DIR 이(가) 존재하지 않습니다."
  exit 1
fi

# BASE_DIR을 순회하면서 서브 디렉토리 별로 압축합니다.
find "$BASE_DIR" -mindepth 1 -maxdepth 1 -type d | while read SUB_DIR; do
  DIR_NAME=$(basename "$SUB_DIR")
  tar -cvzf "${DIR_NAME}.tar.gz" -C "$BASE_DIR" "$DIR_NAME"
  echo "$DIR_NAME.tar.gz 생성 완료."
done

echo "모든 디렉토리 압축 완료."

