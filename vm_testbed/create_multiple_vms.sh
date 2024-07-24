#!/bin/bash

# VM 생성 스크립트
create_vm() {
    VM_NAME=$1
    BASE_IMAGE="/var/lib/libvirt/images/jammy-server-cloudimg-amd64.img"
    VM_DIR="/var/lib/libvirt/images"
    CLOUD_INIT_DIR="/var/lib/libvirt/cloud-init"
    VM_IMAGE="${VM_DIR}/${VM_NAME}.qcow2"
    SEED_IMAGE="${CLOUD_INIT_DIR}/${VM_NAME}-seed.img"

    # Cloud-init 설정 파일 디렉토리 생성
    sudo mkdir -p ${CLOUD_INIT_DIR}
    sudo chown -R $(whoami):$(whoami) ${CLOUD_INIT_DIR}
    sudo chown -R $(whoami):$(whoami) ${VM_DIR}

    # VM 이미지 생성 (10G 크기 지정)
    sudo qemu-img create -f qcow2 -b ${BASE_IMAGE} -F qcow2 ${VM_IMAGE} 10G

    # 클라우드-Init ISO 생성
    sudo cloud-localds ${SEED_IMAGE} user-data meta-data

    # 생성된 ISO 파일의 권한 수정
    sudo chown libvirt-qemu:kvm ${SEED_IMAGE}

    # VM 정의 및 시작
    sudo virt-install --name ${VM_NAME} \
      --ram 2048 \
      --vcpus 2 \
      --disk path=${VM_IMAGE},format=qcow2 \
      --disk path=${SEED_IMAGE},device=cdrom \
      --os-variant ubuntu22.04 \
      --network network=default \
      --graphics none \
      --console pty,target_type=serial \
      --import \
      --noautoconsole
}

# 커맨드 인자로 VM 개수를 받음
NUM_VMS=$1

# 유효한 숫자인지 확인
if ! [[ "$NUM_VMS" =~ ^[0-9]+$ ]]; then
    echo "Usage: $0 <number_of_vms>"
    exit 1
fi

# 지정된 수만큼 VM 생성
for ((i=1; i<=NUM_VMS; i++))
do
    VM_NAME="vm$i"
    create_vm $VM_NAME
done

