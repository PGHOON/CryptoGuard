#!/bin/bash

# 첫 번째 인자로 숫자 입력 받기
if [ -z "$1" ]; then
  echo "Usage: $0 <number_of_ips>"
  exit 1
fi

count=$1
username="root"

# 각 IP 주소에 대해 SSH 접속하여 명령 실행
for ((i=1; i<=count; i++))
do
    IP_SUFFIX=$((100 + i))
    ip="192.168.122.${IP_SUFFIX}"
    echo "Connecting to $ip..."

    # SSH 접속 및 명령 실행
    mkdir -p vm_datasets/vm${i}_dataset/
    scp -r root@${ip}:/root/eBPF_syscall/monitor2/dataset/* vm_datasets/vm${i}_dataset/
done

echo "Commands have been executed."

