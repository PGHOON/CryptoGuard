#!/bin/bash

BASE_IMAGE="/var/lib/libvirt/images/base-vm.qcow2"
VM_DIR="/var/lib/libvirt/images"
CLOUD_INIT_DIR="/var/lib/libvirt/cloud-init"
RAM=2048
VCPUS=2
OS_VARIANT="ubuntu22.04"
NETWORK="default"

META_DATA_TEMPLATE="meta-data-template.yaml"
USER_DATA_TEMPLATE="user-data-template.yaml"
NET_CONFIG_TEMPLATE="net-config-template.yaml"

mkdir -p $CLOUD_INIT_DIR

starti=$1
endi=$2
VM_COUNT=$((endi - starti))

echo $VM_COUNT

# 유효한 가상 머신 개수인지 확인
if [[ -z "$VM_COUNT" || "$VM_COUNT" -le 0 ]]; then
  echo "사용법: $0 <생성할 가상 머신 개수>"
  exit 1
fi

# 파일에서 템플릿을 읽어오는 함수
read_template() {
  local file_path=$1
  if [[ -f "$file_path" ]]; then
    cat "$file_path"
  else
    echo "파일 $file_path을(를) 찾을 수 없습니다."
    exit 1
  fi
}

# 템플릿 파일 내용 읽기
META_DATA_TEMPLATE_CONTENT=$(read_template $META_DATA_TEMPLATE)
USER_DATA_TEMPLATE_CONTENT=$(read_template $USER_DATA_TEMPLATE)
NET_CONFIG_TEMPLATE_CONTENT=$(read_template $NET_CONFIG_TEMPLATE)

# VM_COUNT만큼 가상 머신 생성
for (( i=$starti; i<=$endi; i++ ))
do
  VM_NAME="vm$i"
  VM_IMAGE="$VM_DIR/${VM_NAME}.qcow2"
  CLOUD_INIT_ISO="$CLOUD_INIT_DIR/${VM_NAME}-cloud-init.iso"
  IP_SUFFIX=$((100+$i))
  IP_ADDRESS="192.168.122.$IP_SUFFIX"

  echo $IP_ADDRESS

  sudo qemu-img create -f qcow2 -b ${BASE_IMAGE} -F qcow2 ${VM_IMAGE} 10G

  # 템플릿에서 실제 파일 생성
  NET_CONFIG=$(echo "$NET_CONFIG_TEMPLATE_CONTENT" | sed "s/{{IP_ADDRESS}}/${IP_ADDRESS}/g")
  META_DATA=$(echo "$META_DATA_TEMPLATE_CONTENT" | sed "s/{{VM_NAME}}/${VM_NAME}/g" | sed "s/{{IP_ADDRESS}}/${IP_ADDRESS}/g")
  USER_DATA=$(echo "$USER_DATA_TEMPLATE_CONTENT" | sed "s/{{VM_NAME}}/${VM_NAME}/g" | sed "s/{{IP_ADDRESS}}/${IP_ADDRESS}/g")

  # 실제 파일에 내용 쓰기
  NET_CONFIG_FILE="$CLOUD_INIT_DIR/net-config-$VM_NAME.yaml"
  META_DATA_FILE="$CLOUD_INIT_DIR/meta-data-$VM_NAME.yaml"
  USER_DATA_FILE="$CLOUD_INIT_DIR/user-data-$VM_NAME.yaml"

  echo "$NET_CONFIG" > $NET_CONFIG_FILE
  echo "$META_DATA" > $META_DATA_FILE
  echo "$USER_DATA" > $USER_DATA_FILE

  # cloud-init ISO 생성
  sudo cloud-localds -v --network-config=$NET_CONFIG_FILE $CLOUD_INIT_ISO $USER_DATA_FILE $META_DATA_FILE

  sudo virt-install \
    --name ${VM_NAME} \
    --ram $RAM \
    --vcpus $VCPUS \
    --os-variant $OS_VARIANT \
    --disk path=$VM_IMAGE,format=qcow2 \
    --disk path=$CLOUD_INIT_ISO,device=cdrom \
    --import \
    --network network=default \
    --graphics none \
    --console pty,target_type=serial \
    --noautoconsole
done

