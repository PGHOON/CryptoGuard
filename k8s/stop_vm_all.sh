virsh list --all

sudo virsh shutdown master-new
sudo virsh shutdown worker-1
sudo virsh shutdown worker-2
sudo virsh shutdown worker-3
sudo virsh shutdown worker-4
sudo virsh shutdown worker-5

virsh list --all
