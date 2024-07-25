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
    ip="192.168.122.10$i"
    echo "Connecting to $ip..."

    # SSH 접속 및 명령 실행
    ssh -tt -o StrictHostKeyChecking=no "$username@$ip" <<EOF
cd ~/eBPF_syscall/monitor2
sudo ./monitor2 60 272 3 > ~/cryptoguard.log 2>&1 &
disown
exit
EOF

done

echo "Commands have been executed."

