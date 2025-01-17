sudo virsh list --all

sudo virsh undefine master-new --remove-all-storage
sudo virsh undefine worker-1 --remove-all-storage
sudo virsh undefine worker-2 --remove-all-storage
sudo virsh undefine worker-3 --remove-all-storage
sudo virsh undefine worker-4 --remove-all-storage
sudo virsh undefine worker-5 --remove-all-storage

sudo virsh list --all
