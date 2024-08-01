#!/bin/bash

# scp 명령어
TARGET=$1

SCP_CMD="rsync -av -e 'ssh -i ./id_container -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -p 10926' $TARGET work@proxy2.aitrain.ktcloud.com:~/vm_dataset_phase1_new"

# 무한 루프를 통해 30초마다 scp 명령어 실행
while true
do
    echo "실행 중: $SCP_CMD"
    eval $SCP_CMD
    echo "60초 동안 대기 중..."
    sleep 60
done

