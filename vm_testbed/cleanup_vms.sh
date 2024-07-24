#!/bin/bash

NUM_VMS=50
VM_DIR="/var/lib/libvirt/images"
CLOUD_INIT_DIR="/var/lib/libvirt/cloud-init"

# 기존 VM 정의 및 디스크 이미지 삭제 함수
clean_existing_vms_and_disks() {
  for i in $(seq 1 $NUM_VMS); do
    VM_NAME="vm${i}"
    VM_IMAGE="${VM_DIR}/${VM_NAME}.qcow2"
    SEED_IMAGE="${CLOUD_INIT_DIR}/${VM_NAME}-seed.img"
    
    # VM이 존재하는지 확인하고 삭제
    if virsh dominfo ${VM_NAME} &> /dev/null; then
      echo "Destroying and undefining ${VM_NAME}"
      sudo virsh destroy ${VM_NAME} 2>/dev/null
      sudo virsh undefine ${VM_NAME}
    fi

    # VM 디스크 이미지 삭제
    if [ -f ${VM_IMAGE} ]; then
      echo "Removing ${VM_IMAGE}"
      sudo rm -f ${VM_IMAGE}
    fi

    # 클라우드-Init ISO 이미지 삭제
    if [ -f ${SEED_IMAGE} ]; then
      echo "Removing ${SEED_IMAGE}"
      sudo rm -f ${SEED_IMAGE}
    fi
  done
}

# 기존 VM 정의 및 디스크 이미지 삭제
clean_existing_vms_and_disks

