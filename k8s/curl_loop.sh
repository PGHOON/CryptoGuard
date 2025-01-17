#!/bin/bash

# 사용법: ./script.sh <반복 횟수>
# 보낼 곳 IP도 수정해줘야함.

# 입력 검증
if [ -z "$1" ]; then
  echo "사용법: $0 <반복 횟수>"
  exit 1
fi

# 입력된 반복 횟수
REPEAT=$1

# 성공과 실패 카운트 초기화
SUCCESS=0
FAILURE=0

# 반복 실행
for ((i=1; i<=REPEAT; i++))
do
  curl -s -o /dev/null http://192.168.100.206:31223
  if [ $? -eq 0 ]; then
    SUCCESS=$((SUCCESS + 1))
  else
    FAILURE=$((FAILURE + 1))
  fi
done

# 결과 출력
echo "모든 요청이 완료되었습니다."
echo "성공: $SUCCESS/$REPEAT, 실패: $FAILURE/$REPEAT"
