# VM의 IP 주소 확인
VM_NAME=$1
IP_ADDRESS=$(sudo virsh domifaddr ${VM_NAME} | grep -oP '(\d{1,3}\.){3}\d{1,3}' | head -n 1)
echo "IP Address of ${VM_NAME}: ${IP_ADDRESS}"

# SSH 접속
ssh ubuntu@${IP_ADDRESS}

