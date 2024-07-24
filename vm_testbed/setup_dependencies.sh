#!/bin/bash

# 시스템 업데이트
sudo apt update

# KVM 및 관련 패키지 설치
sudo apt install -y qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils virt-manager

# libvirt 서비스 활성화 및 시작
sudo systemctl enable libvirtd
sudo systemctl start libvirtd

# NFS 서버 설치
sudo apt install -y nfs-kernel-server

# Cloud-Init 설치 (일반적으로 Ubuntu에 기본 설치되어 있지만 확인 차원에서 설치)
sudo apt install -y cloud-init

# NFS 서버 설정 파일 편집
echo "/home/cnslab/vm/binaries *(rw,sync,no_subtree_check,no_root_squash)" | sudo tee -a /etc/exports

# NFS 서버 재시작
sudo exportfs -a
sudo systemctl restart nfs-kernel-server

# 필요한 디렉토리 생성
sudo mkdir -p /var/lib/libvirt/images
sudo mkdir -p /var/lib/libvirt/cloud-init
sudo mkdir -p /home/cnslab/vm/binaries

# 디렉토리 권한 설정
sudo chown -R $(whoami):$(whoami) /home/cnslab/vm/binaries
sudo chown -R $(whoami):$(whoami) /var/lib/libvirt/images
sudo chown -R $(whoami):$(whoami) /var/lib/libvirt/cloud-init

# cryptojacker binary 파일 압축해제
mkdir binaries
cat binaries.tar.gz.* > binaries.tar.gz
tar -xzvf binaries.tar.gz -C binaries
