#!/bin/bash

# 첫 번째 인자로 숫자 입력 받기
if [ -z "$1" ]; then
  echo "Usage: $0 <number_of_ips>"
  exit 1
fi

starti=$1
endi=$2
#target=$3
username="root"

# 각 IP 주소에 대해 SSH 접속하여 명령 실행
for ((i=$starti; i<=$endi; i++))
do
    IP_SUFFIX=$((100+$i))
    ip="192.168.122.${IP_SUFFIX}"
    echo "Connecting to $ip..."

    # SSH 접속 및 명령 실행
    ssh -tt -o StrictHostKeyChecking=no "$username@$ip" <<EOF
cd ~/eBPF_syscall/monitor2
./monitor2 30 272 3 > ~/cryptoguard.log 2>&1 &
disown
exit
EOF

done

echo "Commands have been executed."
