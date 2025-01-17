#!/bin/bash

# 기본 설정
BASE_IMAGE="/var/lib/libvirt/images/jammy-server-cloudimg-amd64.img"
VM_DIR="/var/lib/libvirt/images"
CLOUD_INIT_DIR="/var/lib/libvirt/cloud-init"
VM_RAM=2048
VM_VCPUS=2
VM_DISK_SIZE=10G
NETWORK_NAME="default"

# VM 생성 개수 설정
NUM_WORKERS=5

# 디렉토리 생성 및 권한 설정
sudo mkdir -p ${CLOUD_INIT_DIR}
sudo chown -R $USER:$USER ${CLOUD_INIT_DIR}
sudo mkdir -p ${VM_DIR}
sudo chown -R $USER:$USER ${VM_DIR}

# Master 노드 생성
MASTER_NAME="master-new"
MASTER_IMAGE="${VM_DIR}/${MASTER_NAME}.qcow2"
MASTER_SEED_IMAGE="${CLOUD_INIT_DIR}/${MASTER_NAME}-seed.img"

# Master용 디스크 생성
if [ -f "${BASE_IMAGE}" ]; then
  sudo qemu-img create -f qcow2 -b ${BASE_IMAGE} -F qcow2 ${MASTER_IMAGE} ${VM_DISK_SIZE}
else
  echo "Error: Base image ${BASE_IMAGE} not found. Exiting."
  exit 1
fi

# Master user-data 및 meta-data 파일 생성
cat <<EOF > ${CLOUD_INIT_DIR}/user-data-${MASTER_NAME}
#cloud-config
users:
  - name: ubuntu
    ssh-authorized-keys:
      - ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDm9VLlw8+8mArMCOj6123jZ3nk6RLKuqPFadERM8vaOM1OimqjnHAsYi2jTlrGuDi6QBHoZ8WNyEjXaUy9nKuydlJtyd+oDQoOc7yVvXsjzbrze8JQKe8S7H3QZ0UWylmxdk9XiVQCBxqn+gsrajeU4z9NwonoB9o0W666+HtMS3KqLu0Gc382Tq7rposzm9yBTQusR3eg5WTef89GtlvnLwXWo+3QieNrOLnMbPgVsaKZMKiytNLiFepBhISWgnCWIlVj1SDbRJen1QBRHysGvkj0HEB/xYpSjiW3cACDjnU4uD3GNYDsBnM0QI2palObSNC8k6TD+mf6gw5Jrz8v cnslab@cryptoguard
    sudo: ['ALL=(ALL) NOPASSWD:ALL']
    shell: /bin/bash
package_update: true
packages:
  - apt-transport-https
  - ca-certificates
  - curl
  - gnupg
  - containerd
runcmd:
  - sudo mkdir -p -m 755 /etc/apt/keyrings
  - sudo mkdir -p /etc/containerd
  - curl -fsSL https://pkgs.k8s.io/core:/stable:/v1.32/deb/Release.key | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg
  - sudo chmod 644 /etc/apt/keyrings/kubernetes-apt-keyring.gpg
  - echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v1.32/deb/ /" | sudo tee /etc/apt/sources.list.d/kubernetes.list
  - sudo chmod 644 /etc/apt/sources.list.d/kubernetes.list
  - sudo apt-get update
  - sudo apt-get install -y kubelet kubeadm kubectl
  - sudo apt-mark hold kubelet kubeadm kubectl
  - sudo containerd config default | sudo tee /etc/containerd/config.toml
  - sudo sed -i 's/SystemdCgroup = false/SystemdCgroup = true/' /etc/containerd/config.toml
  - sudo systemctl restart containerd
  - sudo systemctl enable containerd
  - sudo systemctl enable kubelet
  - echo "overlay" | sudo tee -a /etc/modules-load.d/k8s.conf
  - echo "br_netfilter" | sudo tee -a /etc/modules-load.d/k8s.conf
  - sudo modprobe overlay
  - sudo modprobe br_netfilter
  - echo "net.bridge.bridge-nf-call-iptables=1" | sudo tee -a /etc/sysctl.conf
  - echo "net.bridge.bridge-nf-call-ip6tables=1" | sudo tee -a /etc/sysctl.conf
  - echo "net.ipv4.ip_forward=1" | sudo tee -a /etc/sysctl.conf
  - sudo sysctl -p
  - sudo kubeadm init --pod-network-cidr=10.244.0.0/16
  - mkdir -p /home/ubuntu/.kube
  - sudo cp -i /etc/kubernetes/admin.conf /home/ubuntu/.kube/config
  - sudo chown ubuntu:ubuntu /home/ubuntu/.kube/config
  - sleep 30
  - kubectl apply -f https://raw.githubusercontent.com/flannel-io/flannel/master/Documentation/kube-flannel.yml
EOF

cat <<EOF > ${CLOUD_INIT_DIR}/meta-data-${MASTER_NAME}
instance-id: ${MASTER_NAME}-instance
local-hostname: ${MASTER_NAME}
EOF

# Master용 Cloud-init ISO 생성
sudo cloud-localds ${MASTER_SEED_IMAGE} ${CLOUD_INIT_DIR}/user-data-${MASTER_NAME} ${CLOUD_INIT_DIR}/meta-data-${MASTER_NAME}

# Master VM 생성
sudo virt-install --name ${MASTER_NAME} \
  --ram ${VM_RAM} \
  --vcpus ${VM_VCPUS} \
  --disk path=${MASTER_IMAGE},format=qcow2 \
  --disk path=${MASTER_SEED_IMAGE},device=cdrom \
  --os-variant ubuntu22.04 \
  --network network=${NETWORK_NAME},mac=52:54:00:00:00:01 \
  --graphics none \
  --console pty,target_type=serial \
  --import \
  --noautoconsole

# Worker 노드 생성 루프
for i in $(seq 1 ${NUM_WORKERS}); do
  WORKER_NAME="worker-${i}"
  WORKER_IMAGE="${VM_DIR}/${WORKER_NAME}.qcow2"
  WORKER_SEED_IMAGE="${CLOUD_INIT_DIR}/${WORKER_NAME}-seed.img"
  WORKER_MAC=$(printf "52:54:00:00:00:%02x" $((i + 1)))

  # Worker용 디스크 생성
  if [ -f "${BASE_IMAGE}" ]; then
    sudo qemu-img create -f qcow2 -b ${BASE_IMAGE} -F qcow2 ${WORKER_IMAGE} ${VM_DISK_SIZE}
  else
    echo "Error: Base image ${BASE_IMAGE} not found for worker ${WORKER_NAME}. Skipping."
    continue
  fi

  # Worker user-data 및 meta-data 파일 생성
  cat <<EOF > ${CLOUD_INIT_DIR}/user-data-${WORKER_NAME}
#cloud-config
users:
  - name: ubuntu
    ssh-authorized-keys:
      - ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDm9VLlw8+8mArMCOj6123jZ3nk6RLKuqPFadERM8vaOM1OimqjnHAsYi2jTlrGuDi6QBHoZ8WNyEjXaUy9nKuydlJtyd+oDQoOc7yVvXsjzbrze8JQKe8S7H3QZ0UWylmxdk9XiVQCBxqn+gsrajeU4z9NwonoB9o0W666+HtMS3KqLu0Gc382Tq7rposzm9yBTQusR3eg5WTef89GtlvnLwXWo+3QieNrOLnMbPgVsaKZMKiytNLiFepBhISWgnCWIlVj1SDbRJen1QBRHysGvkj0HEB/xYpSjiW3cACDjnU4uD3GNYDsBnM0QI2palObSNC8k6TD+mf6gw5Jrz8v cnslab@cryptoguard
    sudo: ['ALL=(ALL) NOPASSWD:ALL']
    shell: /bin/bash
package_update: true
packages:
  - apt-transport-https
  - ca-certificates
  - curl
  - gnupg
  - containerd
runcmd:
  - sudo mkdir -p -m 755 /etc/apt/keyrings
  - sudo mkdir -p /etc/containerd
  - curl -fsSL https://pkgs.k8s.io/core:/stable:/v1.32/deb/Release.key | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg
  - sudo chmod 644 /etc/apt/keyrings/kubernetes-apt-keyring.gpg
  - echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v1.32/deb/ /" | sudo tee /etc/apt/sources.list.d/kubernetes.list
  - sudo chmod 644 /etc/apt/sources.list.d/kubernetes.list
  - sudo apt-get update
  - sudo apt-get install -y kubelet kubeadm kubectl
  - sudo apt-mark hold kubelet kubeadm kubectl
  - sudo containerd config default | sudo tee /etc/containerd/config.toml
  - sudo sed -i 's/SystemdCgroup = false/SystemdCgroup = true/' /etc/containerd/config.toml
  - sudo systemctl restart containerd
  - sudo systemctl enable containerd
  - sudo systemctl enable kubelet
  - echo "overlay" | sudo tee -a /etc/modules-load.d/k8s.conf
  - echo "br_netfilter" | sudo tee -a /etc/modules-load.d/k8s.conf
  - sudo modprobe overlay
  - sudo modprobe br_netfilter
  - echo "net.bridge.bridge-nf-call-iptables=1" | sudo tee -a /etc/sysctl.conf
  - echo "net.bridge.bridge-nf-call-ip6tables=1" | sudo tee -a /etc/sysctl.conf
  - echo "net.ipv4.ip_forward=1" | sudo tee -a /etc/sysctl.conf
  - sudo sysctl -p
EOF

  cat <<EOF > ${CLOUD_INIT_DIR}/meta-data-${WORKER_NAME}
instance-id: ${WORKER_NAME}-instance
local-hostname: ${WORKER_NAME}
EOF

  # Worker용 Cloud-init ISO 생성
  sudo cloud-localds ${WORKER_SEED_IMAGE} ${CLOUD_INIT_DIR}/user-data-${WORKER_NAME} ${CLOUD_INIT_DIR}/meta-data-${WORKER_NAME}

  # Worker VM 생성
  sudo virt-install --name ${WORKER_NAME} \
    --ram ${VM_RAM} \
    --vcpus ${VM_VCPUS} \
    --disk path=${WORKER_IMAGE},format=qcow2 \
    --disk path=${WORKER_SEED_IMAGE},device=cdrom \
    --os-variant ubuntu22.04 \
    --network network=${NETWORK_NAME},mac=${WORKER_MAC} \
    --graphics none \
    --console pty,target_type=serial \
    --import \
    --noautoconsole

done

echo "All VMs have been created successfully."
